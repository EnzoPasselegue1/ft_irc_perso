#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <map>
# include <poll.h>

// Forward declarations for circular inclusion
class Client;
class Channel;
class CommandHandler;

class Server
{
    private:
        int                             _port;
        std::string                     _password;
        std::string                     _serverName;
        time_t                          _creationDate;

        int                             _serverSocket;
        bool                            _running;

        std::map<int, Client*>          _clients;   // Map of clients connected (key: fd)
        std::map<std::string, Channel*>  _channels;  // map of channel (clé: nom du channel)

        std::vector<struct pollfd>      _pollFds;   // list the descripteur for poll

        CommandHandler*                 _cmdHandler;

		int                             _serverSocket;  // Principal socket listener

        /* ================================================================== */
        /*                CONSTRUCTEURS INTERDICTION                          */
        /* ================================================================== */
        Server();
        Server(const Server& other);
        Server& operator=(const Server& other);

    public:
        Server(int port, const std::string& password);
        ~Server();

        bool init();
        void run();
        void stop();
        void acceptNewClient();
        void disconnectClient(int fd);
        Client* getClientByNickname(const std::string& nickname);
        bool isNicknameInUse(const std::string& nickname);
        Channel* getOrCreatChannel(const std::string& name);
        Channel* getChannel(const std::string& name);
        Channel* removeChannel(const std::string& name);
        void sendToClient(int fd, const std::string& message);
        void broadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd);
        const std::string& getPassword() const;
        const std::string& getServerName() const;
        std::map<int, Client*>& getClients();
        std::map<std::string, Channel*>& getChannels();


        void handleClientData(int fd);
        void processCommand(Client* client, const std::string& command);
        void flushClientBuffer(int fd);
        void addToPoll(int fd);
        void removeFromPoll(int fd);

};

#endif