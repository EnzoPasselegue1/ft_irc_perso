#include "IRC.hpp"

void CommandHandler::handleUser(Client* client, const ParsedCommand& cmd)
{
	if (client->isRegistered())
	{
	    sendError(client, ERR_ALREADYREGISTERED, "", "You may not reregister");
	    return;
	}

	if (cmd.params.size() < 4)
	{
	    sendError(client, ERR_NEEDMOREPARAMS, "USER", "Not enough parameters");
	    return;
	}

	std::string username = cmd.params[0];
	std::string realname = cmd.params[3];

	client->setUsername(username);
	client->setRealname(realname);

	//    (need PASS + NICK + USER)
	if (client->hasPasswordProvided() && !client->getNickname().empty())
	{
	    client->setRegistered(true);
	    sendWelcome(client);
	}
}