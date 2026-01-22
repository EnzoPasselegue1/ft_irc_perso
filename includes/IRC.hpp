#ifndef IRC_HPP
# define IRC_HPP

/* ========================================================================== */
/*                         STANDARD C++ LIBRARIES                         */
/* ========================================================================== */

// Container STL
# include <string>      // std::string for string manipulation
# include <vector>      // std::vector for dynamic arrays
# include <map>         // std::map for key-value associations
# include <set>         // std::set for sets without duplicates

// Input/output streams
# include <iostream>    // std::cout, std::cerr for output
# include <sstream>     // std::stringstream for string/number conversion
# include <fstream>     // std::ifstream, std::ofstream for files

// Utilities
# include <algorithm>   // std::find, std::remove, etc.
# include <cstdlib>     // atoi, exit, etc.
# include <cstring>     // memset, strlen (C++ versions of string.h)
# include <cerrno>      // errno for system error codes
# include <ctime>       // time() for timestamps

/* ========================================================================== */
/*                        BIBLIOTHÈQUES SYSTÈME (POSIX)                       */
/* ========================================================================== */

// Sockets et réseau
# include <sys/socket.h>    // socket(), bind(), listen(), accept(), send(), recv()
# include <sys/types.h>     // System data types (size_t, ssize_t, etc.)
# include <netinet/in.h>    // sockaddr_in structures for IPv4
# include <arpa/inet.h>     // inet_addr(), inet_ntoa(), htons(), ntohs()
# include <netdb.h>         // getaddrinfo(), gethostbyname(), getprotobyname()

// Gestion des fichiers et I/O
# include <unistd.h>        // close(), read(), write()
# include <fcntl.h>         // fcntl() for non-blocking mode
# include <sys/stat.h>      // fstat() for file information

// Multiplexage I/O
# include <poll.h>          // poll() or alternative (select, epoll, kqueue)

// Signaux
# include <signal.h>        // signal(), sigaction() for handling Ctrl+C, etc.

/* ========================================================================== */
/*                           PROJECT CONSTANTS                                */
/* ========================================================================== */

// Server configuration
# define SERVER_NAME        "ft_irc"                // Server display name
# define SERVER_VERSION     "1.0"                   // Server version
# define MAX_CLIENTS        100                     // Maximum number of simultaneous clients
# define MAX_CHANNELS       50                      // Maximum number of channels
# define BUFFER_SIZE        512                     // Reception buffer size (RFC 2812)
# define MAX_NICK_LENGTH    9                       // Maximum length of a nickname
# define MAX_CHANNEL_LENGTH 50                      // Maximum length of a channel name
# define MAX_TOPIC_LENGTH   390                     // Maximum length of a topic

// Return codes
# define SUCCESS            0
# define FAILURE            -1

// IRC special characters
# define CRLF               "\r\n"                  // IRC line ending (Carriage Return + Line Feed)

/* ========================================================================== */
/*                    IRC NUMERIC REPLIES                                     */
/* ========================================================================== */

// Respon valid (connexion réussie)
# define RPL_WELCOME        "001"   // Welcome message after authentication
# define RPL_YOURHOST       "002"   // Server information
# define RPL_CREATED        "003"   // Server creation date
# define RPL_MYINFO         "004"   // Server technical information

// Return codes for commands
# define RPL_ENDOFWHO        "315"   // End of WHO response
# define RPL_CHANNELMODEIS  "324"   // Current mode of a channel
# define RPL_NOTOPIC        "331"   // No topic set
# define RPL_TOPIC          "332"   // Channel topic
# define RPL_TOPICWHOTIME   "333"   // Who set the topic and when
# define RPL_INVITING       "341"   // Invitation confirmation
# define RPL_WHOREPLY       "352"   // WHO command response
# define RPL_NAMREPLY       "353"   // List of users in a channel
# define RPL_ENDOFNAMES     "366"   // End of user list
# define RPL_ENDOF
# define RPL_LISTSTART      "321"   // Start of channel list
# define RPL_LIST           "322"   // Channel list entry
# define RPL_LISTEND        "323"   // End of channel list

// Command errors
# define ERR_NOSUCHNICK     "401"   // Nickname does not exist
# define ERR_NOSUCHSERVER   "402"   // Server does not exist
# define ERR_NOSUCHCHANNEL  "403"   // Channel does not exist
# define ERR_CANNOTSENDTOCHAN "404" // Cannot send to channel
# define ERR_TOOMANYCHANNELS "405"  // Too many channels joined
# define ERR_NORECIPIENT    "411"   // No recipient
# define ERR_NOTEXTTOSEND   "412"   // No text to send
# define ERR_UNKNOWNCOMMAND "421"   // Unknown command
# define ERR_NONICKNAMEGIVEN "431"  // No nickname given
# define ERR_ERRONEUSNICKNAME "432" // Invalid nickname
# define ERR_NICKNAMEINUSE  "433"   // Nickname already in use
# define ERR_USERNOTINCHANNEL "441" // User not in channel
# define ERR_NOTONCHANNEL   "442"   // You are not on that channel
# define ERR_USERONCHANNEL  "443"   // User already in channel
# define ERR_NOTREGISTERED  "451"   // Not registered
# define ERR_NEEDMOREPARAMS "461"   // Missing parameters
# define ERR_ALREADYREGISTERED "462"// Already registered
# define ERR_PASSWDMISMATCH "464"   // Incorrect password
# define ERR_KEYSET         "467"   // Channel key already set
# define ERR_CHANNELISFULL  "471"   // Channel full (limit reached)
# define ERR_UNKNOWNMODE    "472"   // Unknown mode
# define ERR_INVITEONLYCHAN "473"   // Invite-only channel
# define ERR_BADCHANNELKEY  "475"   // Bad channel key
# define ERR_CHANOPRIVSNEEDED "482" // Operator privileges needed
# define ERR_UMODEUNKNOWNFLAG "501" // Unknown mode flag

/* ========================================================================== */
/*                          FORWARD DECLARATIONS                              */
/* ========================================================================== */

class Client;
class Channel;
class Server;
class CommandHandler;

/* ========================================================================== */
/*                        HEADERS OF PROJECT IRC                              */
/* ========================================================================== */

# include "Utils.hpp"
# include "Parser.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "CommandHandler.hpp"
# include "Server.hpp"


#endif