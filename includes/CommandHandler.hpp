#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

#include <string>
#include <vector>
#include <map>

class Client;
class Server;
class Channel;

struct ParsedCommand
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};

class CommandHandler
{
    private:
        Server& _server;
        void handlePass(Client* client, const ParsedCommand& cmd);
    

    public:
        CommandHandler(Server& server);
        ~CommandHandler();

        void handleCommand(Client* client, const std::string& rawCommand);
        void handleNick(Client* client, const ParsedCommand& cmd);
        void checkRegistration(Client* client);
        void handleUser(Client* client, const ParsedCommand& cmd);

        void handlePing(Client* client, const ParsedCommand& cmd);
        void handleQuit(Client* client, const ParsedCommand& cmd);
        void handleJoin(Client* client, const ParsedCommand& cmd);
        void joinSingleChannel(Client* client, const std::string& channelName, const std::string& key);
        void handlePart(Client* client, const ParsedCommand& cmd);
        void handlePrivmsg(Client* client, const ParsedCommand& cmd);

        void handleTopic(Client* client, const ParsedCommand& cmd);

        ParsedCommand parseCommand(const std::string& rawCommand);
        void sendError(Client* client, const std::string& errorCode, const std::string& target, const std::string& message);
        void sendReply(Client* client, const std::string& replyCode, const std::string& params, const std::string& message);
        void sendWelcome(Client* client);

};

#endif