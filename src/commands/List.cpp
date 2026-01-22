#include "IRC.hpp"

void CommandHandler::handleList(Client* client, const ParsedCommand& cmd)
{
    std::vector<std::string> targets;

    if (cmd.params.empty())
    {
        std::map<std::string, Channel*>& chans = _server.getChannels();
        for (std::map<std::string, Channel*>::iterator it = chans.begin(); it != chans.end(); ++it)
            targets.push_back(it->second->getName());
    }
    else
    {
        targets = Utils::split(cmd.params[0], ',');
    }

    // 321 RPL_LISTSTART (optionnel mais utile)
    std::string startReply = ":" + _server.getServerName() + " " + RPL_LISTSTART + " "
                           + client->getNickname() + " Channel :Users  Name";
    _server.sendToClient(client->getFd(), startReply);

    for (size_t i = 0; i < targets.size(); ++i)
    {
        Channel* channel = _server.getChannel(targets[i]);
        if (!channel)
            continue;

        std::string userCount = Utils::intToString(channel->getMemberCount());
        std::string topic = channel->hasTopic() ? channel->getTopic() : "";

        std::string reply = ":" + _server.getServerName() + " " + RPL_LIST + " "
                          + client->getNickname() + " " + channel->getName()
                          + " " + userCount + " :" + topic;
        _server.sendToClient(client->getFd(), reply);
    }

    // 323 RPL_LISTEND
    std::string endReply = ":" + _server.getServerName() + " " + RPL_LISTEND + " "
                         + client->getNickname() + " :End of /LIST";
    _server.sendToClient(client->getFd(), endReply);
}