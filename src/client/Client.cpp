#include "IRC.hpp"

/* ========================================================================== */
/*                    CONSTRUCTEUR / DESTRUCTEUR                              */
/* ========================================================================== */

Client::Client(int fd, const std::string& hostname)
	: _fd(fd),
	  _nickname(""),
	  _username(""),
	  _realname(""),
	  _hostname(hostname),
	  _passwordProvided(false),
	  _registered(false),
	  _shouldDisconnect(false)
{
    std::cout << "Client created (fd: " << _fd << ")" << std::endl;
}

Client::~Client()
{
    std::cout << "Client destroyed (fd: " << _fd << ")" << std::endl;
}

/* ========================================================================== */
/*                       IDENTIFICATION                                       */
/* ========================================================================== */

void Client::setNickname(const std::string& nickname)
{
	_nickname = nickname;
}

const std::string& Client::getNickname() const
{
	return _nickname;
}

void Client::setUsername(const std::string& username)
{
	_username = username;
}

const std::string& Client::getUsername() const
{
	return _username;
}

void Client::setRealname(const std::string& realname)
{
	_realname = realname;
}

const std::string& Client::getRealname() const
{
	return _realname;
}

const std::string& Client::getHostname() const
{
	return _hostname;
}

std::string Client::getPrefix() const
{
    return _nickname + "!" + _username + "@" + _hostname;
}

/* ========================================================================== */
/*                   ÉTAT D'AUTHENTIFICATION                                  */
/* ========================================================================== */

// Set whether the client has provided a password.
void Client::setPasswordProvided(bool value)
{
	_passwordProvided = value;
}

// Check if the client has provided a password.
bool Client::hasPasswordProvided() const
{
	return _passwordProvided;
}

// Check if the client is registered.
bool Client::isRegistered() const
{
	return _registered;
}

// Set the client's registered status.
void Client::setRegistered(bool value)
{
	_registered = value;
}

/* ========================================================================== */
/*                    GESTION DES CHANNELS                                    */
/* ========================================================================== */
    
// Add the client to a channel.
void Client::joinChannel(const std::string& channel)
{
	_channels.insert(Utils::toLower(channel));
}

// Remove the client from a channel.
void Client::leaveChannel(const std::string& channel)
{
	_channels.erase(Utils::toLower(channel));
}

// Check if the client is in a channel.
bool Client::isInChannel(const std::string& channel) const
{
	return _channels.find(Utils::toLower(channel)) != _channels.end();
}

// Get the set of channels the client is in.
const std::set<std::string>& Client::getChannels() const
{
	return _channels;
}

/* ========================================================================== */
/*                    GESTION DES BUFFERS                                     */
/* ========================================================================== */

// Append data to the client's input buffer.
void Client::appendToInputBuffer(const std::string& data)
{
	_inputBuffer += data;
}

// Get the client's input buffer.
std::string& Client::getInputBuffer()
{
	return _inputBuffer;
}

// Clear the client's input buffer.
void Client::clearInputBuffer()
{
	_inputBuffer.clear();
}

// Append data to the client's output buffer.
void Client::appendToOutputBuffer(const std::string& message)
{
	_outputBuffer += message;
}

// Get the client's output buffer.
std::string& Client::getOutputBuffer()
{
	return _outputBuffer;
}

// Clear a specified number of bytes from the beginning of the output buffer.
void Client::trimOutputBuffer(size_t bytes)
{
	if (bytes >= _outputBuffer.size())
	    _outputBuffer.clear();
	else
	    _outputBuffer.erase(0, bytes);
}

// Check if there is data to send to the client.
bool Client::hasDataToSend() const
{
	return !_outputBuffer.empty();
}

/* ========================================================================== */
/*                         ACCESSEURS                                         */
/* ========================================================================== */

// Get the client's file descriptor.
int Client::getFd() const
{
	return _fd;
}

// Mark the client for disconnection.
void Client::markForDisconnection()
{
	_shouldDisconnect = true;
}

// Check if the client is marked for disconnection.
bool Client::shouldDisconnect() const
{
	return _shouldDisconnect;
}
