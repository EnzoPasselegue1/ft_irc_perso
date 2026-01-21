#include "IRC.hpp"

void CommandHandler::handleWho(Client* client, const ParsedCommand& cmd)
{
    if (cmd.params.empty())
    {
        sendError(client, ERR_NEEDMOREPARAMS, "WHO", "Not enough parameters");
        return;
    }

    std::string target = cmd.params[0];
    bool isChannel = (target[0] == '#' || target[0] == '&' || target[0] == '+' || target[0] == '!');

    if (isChannel)
    {
        Channel* channel = _server.getChannel(target);
        if (!channel)
        {
            sendReply(client, RPL_ENDOFWHO, target,"End of WHO list");
            return;
        }

        if (!channel->isMember(client))
        {
            sendReply(client, RPL_ENDOFWHO, target, "You are not member end of WHO list");
            return;
        }

        const std::set<Client*>& memberSet = channel->getMembers();
        std::vector<Client*> members(memberSet.begin(), memberSet.end());
        for (size_t i = 0; i < members.size(); ++i)
        {
            Client* member = members[i];
            sendWhoReply(client, channel, member);
        }

        sendReply(client, RPL_ENDOFWHO, target, "End of WHO list");
    }
    else if (target != "*" && !target.empty())
    {
        Client* targetClient = _server.getClientByNickname(target);
        if (targetClient)
        {
            sendWhoReply(client, NULL, targetClient);
        }
        sendReply(client, RPL_ENDOFWHO, target, "End of WHO list");
    }
    else
    {
        std::map<int, Client*>& clients = _server.getClients();
        for (std::map<int, Client*>::iterator it = clients.begin();
             it != clients.end(); ++it)
        {
            Client* targetClient = it->second;
            sendWhoReply(client, NULL, targetClient);
        }
        sendReply(client, RPL_ENDOFWHO, target, "End of WHO list");
    }
}

void CommandHandler::sendWhoReply(Client* client, Channel* channel, Client* targetClient)
{
    std::string channelName = "*";
    if (channel && channel->isMember(targetClient))
        channelName = channel->getName();

    std::string flags = "H";
    if (channel && channel->isOperator(targetClient))
        flags += "@";
    std::string replyParams = channelName + " " + targetClient->getUsername() + " "
                            + targetClient->getHostname() + " "
                            + _server.getServerName() + " "
                            + targetClient->getNickname() + " "
                            + flags + " :0 ";

    sendReply(client, RPL_WHOREPLY, replyParams, targetClient->getRealname());
}