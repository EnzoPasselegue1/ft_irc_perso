#include "IRC.hpp"


Server::Server(int port, const std::string& password)
	: _port(port),
	  _password(password),
	  _serverName(SERVER_NAME),
	  _serverSocket(-1),
	  _running(false),
	  _cmdHandler(NULL)
{
	   _creationDate = Utils::getCurrentTimestamp();

	   _cmdHandler = new CommandHandler(*this);

	   _pollFds.reserve(MAX_CLIENTS + 1);  // +1 pour le socket serveur
}

// Destructeur
Server::~Server()
{
	   for (std::map<int, Client*>::iterator it = _clients.begin();
	        it != _clients.end(); ++it)
	   {
	       close(it->first);      // Ferme le socket
	       delete it->second;     // Supprime l'objet Client
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
/*                       INITIALISATION DU SERVEUR                            */
/* ========================================================================== */

/*
** Étapes :
** 1. Créer le socket (socket())
** 2. Configurer les options (setsockopt())
** 3. Mettre en mode non-bloquant (fcntl())
** 4. Lier au port (bind())
** 5. Mettre en écoute (listen())
** 6. Ajouter à poll()
*/
bool Server::init()
{
	// 1. Créer le socket
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
	{
	    std::cerr << "Error: socket() failed" << std::endl;
	    return false;
	}

	// 2. Configurer SO_REUSEADDR pour pouvoir réutiliser le port
	//    immédiatement après fermeture du serveur
	int opt = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
	    std::cerr << "Error: setsockopt() failed" << std::endl;
	    close(_serverSocket);
	    return false;
	}

	// 3. Mettre le socket en mode non-bloquant
	//    IMPORTANT : Requis par le sujet, obligatoire
	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1)
	{
	    std::cerr << "Error: fcntl() failed" << std::endl;
	    close(_serverSocket);
	    return false;
	}

	// 4. Configurer l'adresse
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;           // IPv4
	serverAddr.sin_addr.s_addr = INADDR_ANY;   // Accepte toutes les interfaces
	serverAddr.sin_port = htons(_port);        // Port en network byte order

	// 5. Lier le socket au port
	if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
	    std::cerr << "Error: bind() failed - port " << _port << " may be in use" << std::endl;
	    close(_serverSocket);
	    return false;
	}

	// 6. Mettre en écoute
	if (listen(_serverSocket, MAX_CLIENTS) == -1)
	{
	    std::cerr << "Error: listen() failed" << std::endl;
	    close(_serverSocket);
	    return false;
	}

	// 7. Ajouter le socket serveur à poll
	addToPoll(_serverSocket);

	return true;
}