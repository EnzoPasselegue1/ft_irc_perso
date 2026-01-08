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

void Client::setPasswordProvided(bool value)
{
	_passwordProvided = value;
}

bool Client::hasPasswordProvided() const
{
	return _passwordProvided;
}

bool Client::isRegistered() const
{
	return _registered;
}

void Client::setRegistered(bool value)
{
	_registered = value;
}

/* ========================================================================== */
/*                    GESTION DES CHANNELS                                    */
/* ========================================================================== */
    

void Client::joinChannel(const std::string& channel)
{
	_channels.insert(Utils::toLower(channel));
}

void Client::leaveChannel(const std::string& channel)
{
	_channels.erase(Utils::toLower(channel));
}

bool Client::isInChannel(const std::string& channel) const
{
	return _channels.find(Utils::toLower(channel)) != _channels.end();
}

const std::set<std::string>& Client::getChannels() const
{
	return _channels;
}

/* ========================================================================== */
/*                    GESTION DES BUFFERS                                     */
/* ========================================================================== */

void Client::appendToInputBuffer(const std::string& data)
{
	_inputBuffer += data;
}

std::string& Client::getInputBuffer()
{
	return _inputBuffer;
}

void Client::clearInputBuffer()
{
	_inputBuffer.clear();
}

void Client::appendToOutputBuffer(const std::string& message)
{
	_outputBuffer += message;
}

std::string& Client::getOutputBuffer()
{
	return _outputBuffer;
}

void Client::trimOutputBuffer(size_t bytes)
{
	if (bytes >= _outputBuffer.size())
	    _outputBuffer.clear();
	else
	    _outputBuffer.erase(0, bytes);
}

bool Client::hasDataToSend() const
{
	return !_outputBuffer.empty();
}

/* ========================================================================== */
/*                         ACCESSEURS                                         */
/* ========================================================================== */

int Client::getFd() const
{
	return _fd;
}

void Client::markForDisconnection()
{
	_shouldDisconnect = true;
}

bool Client::shouldDisconnect() const
{
	return _shouldDisconnect;
}
