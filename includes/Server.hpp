#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <map>
# include <poll.h>

// Forward declarations pour éviter les inclusions circulaires
class Client;
class Channel;
class CommandHandler;

class Server
{
    private:
        /* ================================================================== */
        /*                    ATTRIBUTS PRIVÉS                                */
        /* ================================================================== */
        int                             _port;
        std::string                     _password;
        std::string                     _serverName;
        time_t                          _creationDate;

        int                             _serverSocket;
        bool                            _running;

        std::map<int, Client*>          _clients;   // Map des clients connectés (clé: fd)
        std::map<std::string, Channel*>  _channels;  // Map des channels (clé: nom du channel)

        std::vector<struct pollfd>      _pollFds;   // Liste des descripteurs pour poll()

        CommandHandler*                 _cmdHandler;

		int                             _serverSocket;  // Socket d'écoute principal

        /* ================================================================== */
        /*                CONSTRUCTEURS INTERDITS                             */
        /* ================================================================== */
        Server();
        Server(const Server& other);
        Server& operator=(const Server& other);

    public:
        Server(int port, const std::string& password);
        ~Server();

        bool Server::init();
        void Server::run();
        
};
