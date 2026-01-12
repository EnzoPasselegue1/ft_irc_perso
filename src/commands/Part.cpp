#include "IRC.hpp"

void CommandHandler::handlePart(Client* client, const ParsedCommand& cmd)
{
	if (cmd.params.empty())
	{
	    sendError(client, ERR_NEEDMOREPARAMS, "PART", "Not enough parameters");
	    return;
	}

	std::vector<std::string> channels = parser::parseChannelList(cmd.params[0]);

	std::string reason = "";
	if (cmd.params.size() > 1)
	    reason = cmd.params[1];

	for (size_t i = 0; i < channels.size(); ++i)
	{
	    std::string channelName = channels[i];

	    Channel* channel = _server.getChannel(channelName);
	    if (!channel)
	    {
	        sendError(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
	        continue;
	    }

	    if (!channel->isMember(client))
	    {
	        sendError(client, ERR_NOTONCHANNEL, channelName, "You're not on that channel");
	        continue;
	    }

	    std::string partMsg = ":" + client->getPrefix() + " PART " + channel->getName();
	    if (!reason.empty())
	        partMsg += " :" + reason;

	    _server.broadcastToChannel(channelName, partMsg, -1);

	    channel->removeMember(client);
	    client->leaveChannel(channelName);

	    if (channel->isEmpty())
	        _server.removeChannel(channelName);
	}
}