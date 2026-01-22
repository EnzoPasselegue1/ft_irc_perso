#include "IRC.hpp"

void CommandHandler::handleBot(Client* client, const ParsedCommand& cmd)
{
    if (cmd.params.empty())
    {
        sendError(client, ERR_NEEDMOREPARAMS, "BOT", "Need more parameters");
        return;
    }

    std::string target;
    std::string subject;
    size_t paramStartIndex = 0;

    if (cmd.params[0][0] == '#' || cmd.params[0][0] == '&' || cmd.params[0][0] == '+' || cmd.params[0][0] == '!')
    {
        target = cmd.params[0];
        paramStartIndex = 1;

        Channel* channel = _server.getChannel(target);
        if (!channel)
        {
            sendError(client, ERR_NOSUCHCHANNEL, target, "No such channel");
            return;
        }
        if (!channel->isMember(client))
        {
            sendError(client, ERR_CANNOTSENDTOCHAN, target, "Cannot send to channel");
            return;
        }
        if (cmd.params.size() <= 1)
        {
            sendError(client, ERR_NEEDMOREPARAMS, "BOT", "Need more parameters");
            return;
        }

    }

    for (size_t i = paramStartIndex; i < cmd.params.size(); ++i)
    {
        if (i > paramStartIndex)
            subject += " ";
        subject += cmd.params[i];
    }

    long score = 0;
    for (size_t i = 0; i < subject.size(); ++i)
        score += static_cast<unsigned char>(subject[i]);

    std::string verdict = (score % 2) ? "c'est de droite." : "c'est de gauche.";

    if (target.empty())
    {
        std::string reply = ":" + _server.getServerName() + " NOTICE "
                          + client->getNickname() + " :" + verdict;
        _server.sendToClient(client->getFd(), reply);
    }
    else
    {
        std::string reply = ":" + _server.getServerName() + " NOTICE "
                          + target + " :" + verdict;
        _server.broadcastToChannel(target, reply, -1);
    }
}