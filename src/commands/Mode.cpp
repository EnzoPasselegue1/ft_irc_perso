#include "IRC.hpp"

void CommandHandler::handleMode(Client* client, const ParsedCommand& cmd)
{
    if (cmd.params.empty())
    {
        sendError(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters");
        return;
    }
    
    std::string target = cmd.params[0];

    if (target[0] != '#' && target[0] != '&' && target[0] != '+' && target[0] != '!')
    { 
        sendError(client, ERR_UMODEUNKNOWNFLAG, target, "Unknown user mode flag");
        return;
    }

    Channel* channel = _server.getChannel(target);
    if (!channel)
    {
        sendError(client, ERR_NOSUCHCHANNEL, target, "No such channel");
        return;
    }

    if (cmd.params.size() == 1)
    {
        // Just return the current modes
        std::string modes = channel->getModeStringWithParams();
        sendReply(client, RPL_CHANNELMODEIS, target, modes);
        return;
    }

    if (!channel->isOperator(client))
    {
        sendError(client, ERR_CHANOPRIVSNEEDED, target, "You're not channel operator");
        return;
    }

    std::string modeString = cmd.params[1];
    std::vector<std::string> modeParams;
    for (size_t i = 2; i < cmd.params.size(); ++i)
    {
        modeParams.push_back(cmd.params[i]);
    }
    applyModeChanges(client, channel, modeString, modeParams);
}

void::CommandHandler::applyModeChanges(Client* client, Channel* channel,
                                     const std::string& modeString,
                                     const std::vector<std::string>& modeParams)
{
    bool adding = true;
    size_t paramIndex = 0;
    std::string appliedModes = "";
    std::string modeParamsStr = "";

    for (size_t i = 0; i < modeString.size(); ++i)
    {
        char modeChar = modeString[i];
        if (modeChar == '+')
        {
            adding = true;
            appliedModes += "+";
            continue;
        }
        if (modeChar == '-')
        {
            adding = false;
            appliedModes += "-";
            continue;
        }
        
        switch (modeChar)
        {
            case 'i':
                channel->setInviteOnly(adding);
                appliedModes += "i";
                break;
            case 't':
                channel->setTopicRestricted(adding);
                appliedModes += "t";
                break;
            case 'k':
                if (adding)
                {
                    if (paramIndex >= modeParams.size())
                    {
                        sendError(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for +k");
                        return;
                    }
                    channel->setKey(modeParams[paramIndex]);
                    modeParamsStr += " " + modeParams[paramIndex];
                    paramIndex++;
                }
                else
                {
                    channel->setKey("");
                }
                appliedModes += "k";
                break;
            case 'o':
                if (paramIndex >= modeParams.size())
                {
                    sendError(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for o");
                    return;
                }
                {
                    Client* targetClient = _server.getClientByNickname(modeParams[paramIndex]);
                    if (!targetClient)
                    {
                        sendError(client, ERR_NOSUCHNICK, modeParams[paramIndex], "No such nick/channel");
                        return;
                    }
                    if (adding)
                        channel->addOperator(targetClient);
                    else
                        channel->removeOperator(targetClient);
                    modeParamsStr += " " + modeParams[paramIndex];
                    paramIndex++;
                }
                appliedModes += "o";
                break;
            case 'l':
                if (adding)
                {
                    if (paramIndex >= modeParams.size())
                    {
                        sendError(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for +l");
                        return;
                    }
                    int limit = Utils::stringToInt(modeParams[paramIndex]);
                    if (limit <= 0)
                    {
                        continue;
                    }
                    channel->setUserLimit(static_cast<size_t>(limit));
                    appliedModes += " " + Utils::intToString(limit);
                }
                else 
                {
                    channel->setUserLimit(0);
                }
                appliedModes += "l";
                break;
            
            default:
                sendError(client, ERR_UNKNOWNMODE, std::string(1, modeChar), "is unknown mode char to me");
                break;
        }
    }
    if (!appliedModes.empty() && appliedModes != "+" && appliedModes != "-")
    {
        std::string modeMsg = ":" + client->getPrefix() + " MODE " +
                              channel->getName() + " " + appliedModes + modeParamsStr;
        _server.broadcastToChannel(channel->getName(), modeMsg, -1);
    }
}