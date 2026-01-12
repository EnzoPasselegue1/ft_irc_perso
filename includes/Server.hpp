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

        bool Server::init();
        void Server::run();
        void Server::stop();
        void Server::disconnectClient(int fd);
        Client* Server::getClientByNickname(const std::string& nickname);
        bool Server::isNicknameInUse(const std::string& nickname);
        Channel* Server::getOrCreatChannel(const std::string& name);
        Channel* Server::getChannel(const std::string& name);
        Channel* Server::RemoveChannel(const std::string& name);
        void Server::sendToClient(int fd, const std::string& message);
        void Server::broadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd);
        const std::string& Server::getPassword() const;
        const std::string& Server::getServerName() const;
        std::map<int, Client*>& Server::getClients();
        std::map<std::string, Channel*>& Server::getChannels();
};
