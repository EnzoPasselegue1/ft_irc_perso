#include "IRC.hpp"

CommandHandler::CommandHandler(Server& server) : _server(server)
{
}

CommandHandler::~CommandHandler()
{}

void CommandHandler::handleCommand(Client* client, const std::string& rawCommand)
{
	ParsedCommand cmd = parseCommand(rawCommand);

	if (cmd.command.empty())
	    return;

	std::string upperCmd = Utils::toUpper(cmd.command);

	//    PASS, NICK, USER, QUIT ne nécessitent pas d'être enregistré
	//    Les autres commandes nécessitent l'enregistrement
	if (!client->isRegistered() &&
	    upperCmd != "PASS" && upperCmd != "NICK" &&
	    upperCmd != "USER" && upperCmd != "QUIT" && upperCmd != "CAP")
	{
	    sendError(client, ERR_NOTREGISTERED, "*", "You have not registered");
	    return;
	}

	if (upperCmd == "PASS")
	    handlePass(client, cmd);
	else if (upperCmd == "NICK")
	    handleNick(client, cmd);
	else if (upperCmd == "USER")
	    handleUser(client, cmd);
	else if (upperCmd == "JOIN")
	    handleJoin(client, cmd);
	else if (upperCmd == "PART")
	    handlePart(client, cmd);
	else if (upperCmd == "PRIVMSG")
	    handlePrivmsg(client, cmd);
	//else if (upperCmd == "KICK")
	//    handleKick(client, cmd);
	//else if (upperCmd == "INVITE")
	//    handleInvite(client, cmd);
	else if (upperCmd == "TOPIC")
	    handleTopic(client, cmd);
	//else if (upperCmd == "MODE")
	//    handleMode(client, cmd);
	else if (upperCmd == "QUIT")
	    handleQuit(client, cmd);
	else if (upperCmd == "PING")
	    handlePing(client, cmd);
	//else if (upperCmd == "CAP")
	//    ; // Ignorer CAP (utilisé par certains clients modernes)
	//else if (upperCmd == "WHO" || upperCmd == "WHOIS")
	//    ; // Optionnel : implémenter WHO/WHOIS
	else
	    sendError(client, ERR_UNKNOWNCOMMAND, upperCmd, "Unknown command");
}

ParsedCommand CommandHandler::parseCommand(const std::string& rawCommand)
{
	ParsedCommand cmd;
	std::string line = rawCommand;

	// 1. Extraire le préfixe (optionnel, commence par ':')
	if (!line.empty() && line[0] == ':')
	{
	    size_t spacePos = line.find(' ');
	    if (spacePos != std::string::npos)
	    {
	        cmd.prefix = line.substr(1, spacePos - 1);
	        line = line.substr(spacePos + 1);
	    }
	    else
	    {
	        cmd.prefix = line.substr(1);
	        return cmd;
	    }
	}

	// 2. Trouver le trailing (partie après " :")
	std::string trailing;
	size_t trailingPos = line.find(" :");
	if (trailingPos != std::string::npos)
	{
	    trailing = line.substr(trailingPos + 2);
	    line = line.substr(0, trailingPos);
	}

	// 3. Extraire la commande et les paramètres
	std::vector<std::string> parts = Utils::split(line, ' ');
	if (!parts.empty())
	{
	    cmd.command = parts[0];
	    for (size_t i = 1; i < parts.size(); ++i)
	    {
	        if (!parts[i].empty())
	            cmd.params.push_back(parts[i]);
	    }
	}

	// 4. Ajouter le trailing comme dernier paramètre
	if (!trailing.empty())
	    cmd.params.push_back(trailing);

	return cmd;
}

/* ========================================================================== */
/*                    UTILS                                                   */
/* ========================================================================== */


void CommandHandler::sendError(Client* client, const std::string& errorCode,
							   const std::string& target, const std::string& message)
{
	std::string nick = client->getNickname();
	if (nick.empty())
	    nick = "*";

	std::string reply = ":" + _server.getServerName() + " " + errorCode + " "
	                  + nick + " " + target + " :" + message;

	_server.sendToClient(client->getFd(), reply);
}

void CommandHandler::sendReply(Client* client, const std::string& replyCode,
							   const std::string& params, const std::string& message)
{
	std::string nick = client->getNickname();
	if (nick.empty())
	    nick = "*";

	std::string reply = ":" + _server.getServerName() + " " + replyCode + " " + nick;

	if (!params.empty())
	    reply += " " + params;

	if (!message.empty())
	    reply += " :" + message;

	_server.sendToClient(client->getFd(), reply);
}


void CommandHandler::sendWelcome(Client* client)
{
	std::string nick = client->getNickname();
	std::string prefix = client->getPrefix();

	// 001 RPL_WELCOME
	sendReply(client, RPL_WELCOME, "",
	          "Welcome to the Internet Relay Network " + prefix);

	// 002 RPL_YOURHOST
	sendReply(client, RPL_YOURHOST, "",
	          "Your host is " + _server.getServerName() +
	          ", running version " + SERVER_VERSION);

	// 003 RPL_CREATED
	sendReply(client, RPL_CREATED, "",
	          "This server was created ");

	// 004 RPL_MYINFO
	sendReply(client, RPL_MYINFO,
	          _server.getServerName() + " " + SERVER_VERSION + " o itkol",
	          "");
}