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
        void sendWhoReply(Client* client, Channel* channel, Client* targetClient);
    

    public:
        CommandHandler(Server& server);
        ~CommandHandler();

/* ========================================================================== */
/*                         CONNEXION                                          */
/* ========================================================================== */

        void handleCommand(Client* client, const std::string& rawCommand);
        void handleNick(Client* client, const ParsedCommand& cmd);
        void checkRegistration(Client* client);
        void handleUser(Client* client, const ParsedCommand& cmd);

/* ========================================================================== */
/*                         BASICS COMMANDS                                   */
/* ========================================================================== */
        void handlePing(Client* client, const ParsedCommand& cmd);
        void handleQuit(Client* client, const ParsedCommand& cmd);
        void handleJoin(Client* client, const ParsedCommand& cmd);
        void joinSingleChannel(Client* client, const std::string& channelName,
                             const std::string& key);
        void handlePart(Client* client, const ParsedCommand& cmd);
        void handlePrivmsg(Client* client, const ParsedCommand& cmd);
        void handleNotice(Client* client, const ParsedCommand& cmd);

/* ========================================================================== */
/*                         OPERATOR COMMANDS                                  */
/* ========================================================================== */

        void handleTopic(Client* client, const ParsedCommand& cmd);
        void handleKick(Client* client, const ParsedCommand& cmd);
        void handleInvite(Client* client, const ParsedCommand& cmd);
        void handleMode(Client* client, const ParsedCommand& cmd);
        void applyModeChanges(Client* client, Channel* channel,
                              const std::string& modeString,
                              const std::vector<std::string>& modeParams);
        void handleWho(Client* client, const ParsedCommand& cmd);
        void handleNames(Client* client, const ParsedCommand& cmd);
        void handleList(Client* client, const ParsedCommand& cmd);
        void handleBot(Client* client, const ParsedCommand& cmd);

/* ========================================================================== */
/*                         UTILS                                              */
/* ========================================================================== */

        ParsedCommand parseCommand(const std::string& rawCommand);
        void sendError(Client* client, const std::string& errorCode,
                     const std::string& target, const std::string& message);
        void sendReply(Client* client, const std::string& replyCode,
                     const std::string& params, const std::string& message);
        void sendWelcome(Client* client);

};

#endif