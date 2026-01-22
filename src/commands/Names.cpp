#include "IRC.hpp"

void CommandHandler::handleNames(Client* client, const ParsedCommand& cmd)
{
    std::vector<std::string> targets;

    if (cmd.params.empty())
    {
        std::map<std::string, Channel*>& chans = _server.getChannels();
        for (std::map<std::string, Channel*>::iterator it = chans.begin();
             it != chans.end(); ++it)
            targets.push_back(it->second->getName());
    }
    else
    {
        targets = Utils::split(cmd.params[0], ',');
    }

    if (targets.empty())
    {
        std::string endReply = ":" + _server.getServerName() + " " + RPL_ENDOFNAMES
                             + " " + client->getNickname() + " * :End of /NAMES list";
        _server.sendToClient(client->getFd(), endReply);
        return;
    }

    for (size_t i = 0; i < targets.size(); ++i)
    {
        Channel* channel = _server.getChannel(targets[i]);
        if (!channel)
            continue;

        std::string names = channel->getNamesList(); // "@op user1 user2"
        std::string reply = ":" + _server.getServerName() + " " + RPL_NAMREPLY + " "
                          + client->getNickname() + " = " + channel->getName()
                          + " :" + names;
        _server.sendToClient(client->getFd(), reply);

        std::string endReply = ":" + _server.getServerName() + " " + RPL_ENDOFNAMES
                             + " " + client->getNickname() + " " + channel->getName()
                             + " :End of /NAMES list";
        _server.sendToClient(client->getFd(), endReply);
    }
}