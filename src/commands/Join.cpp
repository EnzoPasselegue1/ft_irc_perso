#include "IRC.hpp"

void CommandHandler::handleJoin(Client* client, const ParsedCommand& cmd)
{
	if (cmd.params.empty())
	{
	    sendError(client, ERR_NEEDMOREPARAMS, "JOIN", "Not enough parameters");
	    return;
	}

    if (cmd.params[0] == "0")
    {
        // Leave all channels
        // copy channels to avoid iterator invalidation if handlePart modifies client's channel set
        std::vector<std::string> channels(client->getChannels().begin(), client->getChannels().end());
        for (std::vector<std::string>::const_iterator it = channels.begin(); it != channels.end(); ++it)
        {
            ParsedCommand partCmd;
            partCmd.command = "PART";
            partCmd.params = std::vector<std::string>(1, *it);
            handlePart(client, partCmd);
        }
        return;
    }

	std::vector<std::string> channels = parser::parseChannelList(cmd.params[0]);
	std::vector<std::string> keys;
	if (cmd.params.size() > 1)
	    keys = parser::parseKeyList(cmd.params[1]);

	for (size_t i = 0; i < channels.size(); ++i)
	{
	    std::string channelName = channels[i];
	    std::string key = (i < keys.size()) ? keys[i] : "";

	    joinSingleChannel(client, channelName, key);
	}
}

void CommandHandler::joinSingleChannel(Client* client, const std::string& channelName, const std::string& key)
 {
     if (!parser::isValidChannelName(channelName))
     {
         sendError(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
         return;
     }

     Channel* channel = _server.getOrCreatChannel(channelName);
     if (!channel)
     {
         sendError(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
         return;
     }

     if (channel->isMember(client))
         return;


     if (channel->isInviteOnly() && !channel->isInvited(client->getNickname()))
     {
         sendError(client, ERR_INVITEONLYCHAN, channelName, "Cannot join channel (+i)");
         return;
     }

     if (channel->hasKey() && !channel->checkKey(key))
     {
         sendError(client, ERR_BADCHANNELKEY, channelName, "Cannot join channel (+k)");
         return;
     }

     if (channel->isFull())
     {
         sendError(client, ERR_CHANNELISFULL, channelName, "Cannot join channel (+l)");
         return;
     }

     channel->addMember(client);
     client->joinChannel(channelName);

     channel->removeInvite(client->getNickname());

     std::string joinMsg = ":" + client->getPrefix() + " JOIN " + channel->getName();
     _server.broadcastToChannel(channelName, joinMsg, -1);  // -1 = pas d'exclusion

     if (channel->hasTopic())
     {
         sendReply(client, RPL_TOPIC, channel->getName(), channel->getTopic());
     }

     sendReply(client, RPL_NAMREPLY, "= " + channel->getName(), channel->getNamesList());
     sendReply(client, RPL_ENDOFNAMES, channel->getName(), "End of /NAMES list");
 }
