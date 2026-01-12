#include "IRC.hpp"

void CommandHandler::handleNick(Client* client, const ParsedCommand& cmd)
{
	if (cmd.params.empty())
	{
	    sendError(client, ERR_NONICKNAMEGIVEN, "", "No nickname given");
	    return;
	}

	std::string newNick = cmd.params[0];

	if (!parser::isValidNickname(newNick))
	{
	    sendError(client, ERR_ERRONEUSNICKNAME, newNick, "Erroneous nickname");
	    return;
	}

	Client* existing = _server.getClientByNickname(newNick);
	if (existing && existing != client)
	{
	    sendError(client, ERR_NICKNAMEINUSE, newNick, "Nickname is already in use");
	    return;
	}

	std::string oldNick = client->getNickname();

	client->setNickname(newNick);

	if (client->isRegistered())
	{
	    std::string oldPrefix = oldNick + "!" + client->getUsername() + "@" + client->getHostname();
	    std::string message = ":" + oldPrefix + " NICK :" + newNick;

	    _server.sendToClient(client->getFd(), message);

	    const std::set<std::string>& channels = client->getChannels();
	    for (std::set<std::string>::const_iterator it = channels.begin();
	         it != channels.end(); ++it)
	    {
	        _server.broadcastToChannel(*it, message, client->getFd());
	    }
	}
	else
	{
	    checkRegistration(client);
	}
}

 void CommandHandler::checkRegistration(Client* client)
 {
     if (!client->hasPasswordProvided())
         return;
     if (client->getNickname().empty())
         return;
     if (client->getUsername().empty())
         return;

     client->setRegistered(true);
     sendWelcome(client);
 }