#include "IRC.hpp"

/* ========================================================================== */
/*                       CONSTRUCTOR                                          */
/* ========================================================================== */

Server::Server(int port, const std::string& password)
	: _port(port),
	  _password(password),
	  _serverName(SERVER_NAME),
	  _serverSocket(-1),
	  _running(false),
	  _cmdHandler(NULL)
{
	   _creationDate = std::time(NULL);

	   _cmdHandler = new CommandHandler(*this);

	   _pollFds.reserve(MAX_CLIENTS + 1);  // +1 for socket server
}

// Destructor
Server::~Server()
{
	   for (std::map<int, Client*>::iterator it = _clients.begin();
	        it != _clients.end(); ++it)
	   {
	       close(it->first);      // close the socket
	       delete it->second;     // delete client object
	   }
	   _clients.clear();

	   for (std::map<std::string, Channel*>::iterator it = _channels.begin();
	        it != _channels.end(); ++it)
	   {
	       delete it->second;
	   }
	   _channels.clear();

	   if (_serverSocket != -1)
	       close(_serverSocket);

	   delete _cmdHandler;
}

/* ========================================================================== */
/*                       INITIALIZATION SERVER                               */
/* ========================================================================== */

/*
** Steps:
** 1. Create the socket (socket())
** 2. Configure options (setsockopt())
** 3. Set to non-blocking mode (fcntl())
** 4. Bind to port (bind())
** 5. Set to listen mode (listen())
** 6. Add to poll()
*/
bool Server::init()
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
	{
	    std::cerr << "Error: socket() failed" << std::endl;
	    return false;
	}

	int opt = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
	    std::cerr << "Error: setsockopt() failed" << std::endl;
	    close(_serverSocket);
	    return false;
	}

	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1)
	{
	    std::cerr << "Error: fcntl() failed" << std::endl;
	    close(_serverSocket);
	    return false;
	}

	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;           // IPv4
	serverAddr.sin_addr.s_addr = INADDR_ANY;   // Accept all interfaces
	serverAddr.sin_port = htons(_port);        // Port in network byte order

	if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
	    std::cerr << "Error: bind() failed - port " << _port << " may be in use" << std::endl;
	    close(_serverSocket);
	    return false;
	}

	if (listen(_serverSocket, MAX_CLIENTS) == -1)
	{
	    std::cerr << "Error: listen() failed" << std::endl;
	    close(_serverSocket);
	    return false;
	}

	addToPoll(_serverSocket);

	return true;
}

/* ========================================================================== */
/*                       MAIN LOOP                                            */
/* ========================================================================== */

// 1. Call poll() with a timeout (e.g., 1000ms)
// 2. Loop through all monitored fds
// 3. If it's the server socket, accept new connection
// 4. If it's a client
//    a. Check POLLERR/POLLHUP/POLLNVAL -> mark for removal
//    b. If POLLIN (data to read) -> read and process commands
//    c. If POLLOUT (ready to write) -> send pending data
// 5. Clean up clients marked for disconnection
void Server::run()
{
	_running = true;
	while (_running)
	{
	    
	    int pollResult = poll(&_pollFds[0], _pollFds.size(), 1000);

	    if (pollResult == -1)
	    {
	        if (errno == EINTR) // Interrupted
	            continue;
	        std::cerr << "Error: poll() failed" << std::endl;
	        break;
	    }

	    if (pollResult == 0)
	        continue;

	    for (size_t i = 0; i < _pollFds.size(); ++i)
	    {
	        // No evant on this fd
	        if (_pollFds[i].revents == 0)
	            continue;

	        if (_pollFds[i].fd == _serverSocket)
	        {
	            if (_pollFds[i].revents & POLLIN)
	                acceptNewClient();
	        }
	        else
	        {
	            int clientFd = _pollFds[i].fd;

	            // Error or deconnexion
	            if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
	            {
	                disconnectClient(clientFd);
	                continue;
	            }
	            // Data to read 
	            if (_pollFds[i].revents & POLLIN)
	                handleClientData(clientFd);

	            // Ready to write
	            if (_pollFds[i].revents & POLLOUT)
	                flushClientBuffer(clientFd);
	        }
	    }
        cleanupDisconnectedClients();
	}
}

void Server::stop()
{
    _running = false;
}

/* ========================================================================== */
/*                       CONNECTION MANAGEMENT                                */
/* ========================================================================== */

//accept new client if it's possble
void Server::acceptNewClient()
{
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = accept(_serverSocket, (struct sockaddr*)&clientAddr, &addrLen);

	if (clientFd == -1)
	{
	    std::cerr << "Error: accept() failed" << std::endl;
	    return;
	}

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
	    std::cerr << "Error: fcntl() failed for new client" << std::endl;
	    close(clientFd);
	    return;
	}

	std::string hostname = inet_ntoa(clientAddr.sin_addr);

	Client* client = new Client(clientFd, hostname);
	_clients[clientFd] = client;

	addToPoll(clientFd);

	std::cout << "New client connected: " << hostname << " (fd: " << clientFd << ")" << std::endl;
}

// DisconnectClient
void Server::disconnectClient(int fd)
{
    std::map<int, Client*>::iterator it = _clients.find(fd);
    
    if (it == _clients.end())
	    return;

	Client* client = it->second;

	const std::set<std::string>& channels = client->getChannels();
	for (std::set<std::string>::const_iterator chanIt = channels.begin();
	     chanIt != channels.end(); ++chanIt)
	{
	    Channel* channel = getChannel(*chanIt);
	    if (channel)
	    {
	        // Option : Send message QUIT to other
	        channel->removeMember(client);
	        if (channel->isEmpty())
	            removeChannel(*chanIt);
	    }
	}

	removeFromPoll(fd);

	close(fd);

	delete client;
	_clients.erase(it);

	std::cout << "Client disconnected (fd: " << fd << ")" << std::endl;
}

Client* Server::getClientByNickname(const std::string& nickname)
{
	std::string lowerNick = Utils::toLower(nickname);
	for (std::map<int, Client*>::iterator it = _clients.begin();
	     it != _clients.end(); ++it)
	{
	    if (Utils::toLower(it->second->getNickname()) == lowerNick)
	        return it->second;
	}
	return NULL;
}

// Verify if the nickname is already used
bool Server::isNicknameInUse(const std::string& nickname)
{
	return (getClientByNickname(nickname) != NULL);
}

/* ========================================================================== */
/*                       CHANNEL MANAGEMENT                                    */
/* ========================================================================== */

Channel* Server::getOrCreatChannel(const std::string& name)
{

	std::string lowerName = Utils::toLower(name);
	std::map<std::string, Channel*>::iterator it = _channels.find(lowerName);

	if (it != _channels.end())
	    return it->second;

	// if doesn't exit, creat channel
	Channel* channel = new Channel(name);
	_channels[lowerName] = channel;
	return channel;
}

Channel* Server::getChannel(const std::string& name)
{
    std::string lowerName = Utils::toLower(name);
    std::map<std::string, Channel*>::iterator it = _channels.find(lowerName);

    if (it != _channels.end())
        return it->second;
    return NULL;
}

Channel* Server::removeChannel(const std::string& name)
{
    std::string lowerName = Utils::toLower(name);
    std::map<std::string, Channel*>::iterator it = _channels.find(lowerName);

    if (it != _channels.end())
    {
        delete it->second;
        _channels.erase(it);
    }
    return NULL;
}


/* ========================================================================== */
/*                       COMMUNICATION                                       */
/* ========================================================================== */

void Server::sendToClient(int fd, const std::string& message)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
    {
	    it->second->appendToOutputBuffer(message + CRLF);

	    for (size_t i = 0; i < _pollFds.size(); ++i)
	    {
	        if (_pollFds[i].fd == fd)
	        {
	            _pollFds[i].events |= POLLOUT;
	            break;
	        }
	    }
	}
}

void Server::broadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd)
{
	Channel* channel = getChannel(channelName);
	if (!channel)
	    return;

	const std::set<Client*>& members = channel->getMembers();
	for (std::set<Client*>::const_iterator it = members.begin();
	     it != members.end(); ++it)
	{
	    if ((*it)->getFd() != excludeFd)
	        sendToClient((*it)->getFd(), message);
	}
}

/* ========================================================================== */
/*                       GETTEURS                                             */
/* ========================================================================== */

const std::string& Server::getPassword() const
{
    return _password;
}

const std::string& Server::getServerName() const
{
    return _serverName;
}

std::map<int, Client*>& Server::getClients()
{
    return _clients;
}

std::map<std::string, Channel*>& Server::getChannels()
{
    return _channels;
}

/* ========================================================================== */
/*                       PRIVATE METHODS                                      */
/* ========================================================================== */

void Server::handleClientData(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
	    return;
	Client* client = it->second;

    char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);
	ssize_t bytesRead = recv(fd, buffer, BUFFER_SIZE - 1, 0);

	if (bytesRead <= 0) // 0 = diconnect , -1 = error
	{
	    client->markForDisconnection();
	    return;
	}

    client->appendToInputBuffer(std::string(buffer, bytesRead));

    std::string& inputBuffer = client->getInputBuffer();
	size_t pos;
	while ((pos = inputBuffer.find(CRLF)) != std::string::npos)
	{
	    // Extract command
	    std::string command = inputBuffer.substr(0, pos);
	    inputBuffer.erase(0, pos + 2);  // +2 for \r\n

	    // Ignore empty lines 
	    if (!command.empty())
	        processCommand(client, command);
	}
}

void Server::processCommand(Client* client, const std::string& command)
{
    if (_cmdHandler)
        _cmdHandler->handleCommand(client, command);
}

void Server::flushClientBuffer(int fd)
{
    std::map<int, Client*>::iterator it = _clients.find(fd);
    
    if (it == _clients.end())
        return ;
    Client* client = it->second;
    std::string& outputBuffer = client->getOutputBuffer();
    if (outputBuffer.empty())
    {
        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            if (_pollFds[i].fd == fd)
            {
                _pollFds[i].events &= ~POLLOUT;
                break;
            }
        }
        return ;
    }

	ssize_t bytesSent = send(fd, outputBuffer.c_str(), outputBuffer.size(), 0);

    if (bytesSent == -1)
	{
	    if (errno != EAGAIN && errno != EWOULDBLOCK)
	        client->markForDisconnection();
	    return;
	}

	client->trimOutputBuffer(bytesSent);
}

void Server::addToPoll(int fd)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollFds.push_back(pfd);
}

//Remove fd from poll
void Server::removeFromPoll(int fd)
{
	for (std::vector<pollfd>::iterator it = _pollFds.begin();
	     it != _pollFds.end(); ++it)
	{
	    if (it->fd == fd)
	    {
	        _pollFds.erase(it);
	        break;
	    }
	}
}

void Server::cleanupDisconnectedClients()
{
    std::vector<int> fdsToRemove;
    
    for (std::map<int, Client*>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
		if (it->second->isMarkedForDisconnection())
        {
            fdsToRemove.push_back(it->first);
        }
    }
    
    for (std::vector<int>::iterator it = fdsToRemove.begin();
         it != fdsToRemove.end(); ++it)
    {
        disconnectClient(*it);
    }
}

