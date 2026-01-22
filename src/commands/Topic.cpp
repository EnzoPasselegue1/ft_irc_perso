#include "IRC.hpp"

void CommandHandler::handleTopic(Client* client, const ParsedCommand& cmd)
{
    if (cmd.params.empty())
    {
        sendError(client, ERR_NEEDMOREPARAMS, "TOPIC", "Not enough parameters");
        return;
    }

    std::string channelName = cmd.params[0];
    Channel* channel = _server.getChannel(channelName);
    if (!channel)
    {
        sendError(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
        return;
    }

    if (!channel->isMember(client))
    {
        sendError(client, ERR_NOTONCHANNEL, channelName, "You're not on that channel");
        return;
    }

    if (cmd.params.size() == 1)
    {
        // Get topic
        std::string topic = channel->getTopic();
        if (topic.empty())
        {
            sendReply(client, RPL_NOTOPIC, channelName, "No topic is set");
        }
        else
        {
            sendReply(client, RPL_TOPIC, channelName, topic);
        }
    }
    else
    {
        // Set topic
        if (channel->isTopicRestricted() && !channel->isOperator(client))
        {
            sendError(client, ERR_CHANOPRIVSNEEDED, channelName, "You're not channel operator");
            return;
        }

        std::string newTopic = cmd.params[1];
        channel->setTopic(newTopic, client->getNickname());

        // Notify all members about the topic change
	    std::string topicMsg = ":" + client->getPrefix() + " TOPIC " + channelName;
	    if (!newTopic.empty())
	        topicMsg += " " + newTopic;
	    else
	        topicMsg += "";  // Topic effacé
        
	    _server.broadcastToChannel(channelName, topicMsg, -1);
    }
}