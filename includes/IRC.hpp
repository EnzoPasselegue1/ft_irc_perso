#ifndef IRC_HPP
# define IRC_HPP

/* ========================================================================== */
/*                         STANDARD C++ LIBRARIES                         */
/* ========================================================================== */

// Conteneurs STL
# include <string>      // std::string for string manipulation
# include <vector>      // std::vector for dynamic arrays
# include <map>         // std::map for key-value associations
# include <set>         // std::set for sets without duplicates

// Input/output streams
# include <iostream>    // std::cout, std::cerr for output
# include <sstream>     // std::stringstream for string/number conversion
# include <fstream>     // std::ifstream, std::ofstream for files

// Utilitaires
# include <algorithm>   // std::find, std::remove, etc.
# include <cstdlib>     // atoi, exit, etc.
# include <cstring>     // memset, strlen (versions C++ de string.h)
# include <cerrno>      // errno pour les codes d'erreur système
# include <ctime>       // time() pour les timestamps

/* ========================================================================== */
/*                        BIBLIOTHÈQUES SYSTÈME (POSIX)                       */
/* ========================================================================== */

// Sockets et réseau
# include <sys/socket.h>    // socket(), bind(), listen(), accept(), send(), recv()
# include <sys/types.h>     // Types de données système (size_t, ssize_t, etc.)
# include <netinet/in.h>    // Structures sockaddr_in pour IPv4
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
/*                           CONSTANTES DU PROJET                             */
/* ========================================================================== */

// Configuration du serveur
# define SERVER_NAME        "ft_irc"                // Nom affiché du serveur
# define SERVER_VERSION     "1.0"                   // Version du serveur
# define MAX_CLIENTS        100                     // Nombre max de clients simultanés
# define MAX_CHANNELS       50                      // Nombre max de channels
# define BUFFER_SIZE        512                     // Taille du buffer de réception (RFC 2812)
# define MAX_NICK_LENGTH    9                       // Longueur max d'un nickname
# define MAX_CHANNEL_LENGTH 50                      // Longueur max d'un nom de channel
# define MAX_TOPIC_LENGTH   390                     // Longueur max d'un topic

// Codes de retour
# define SUCCESS            0
# define FAILURE            -1

// Caractères spéciaux IRC
# define CRLF               "\r\n"                  // Fin de ligne IRC (Carriage Return + Line Feed)

/* ========================================================================== */
/*                    CODES DE RÉPONSE IRC (NUMERIC REPLIES)                  */
/* ========================================================================== */

// Réponses de bienvenue (connexion réussie)
# define RPL_WELCOME        "001"   // Message de bienvenue après authentification
# define RPL_YOURHOST       "002"   // Informations sur le serveur
# define RPL_CREATED        "003"   // Date de création du serveur
# define RPL_MYINFO         "004"   // Informations techniques du serveur

// Réponses aux commandes
# define RPL_CHANNELMODEIS  "324"   // Mode actuel d'un channel
# define RPL_NOTOPIC        "331"   // Pas de topic défini
# define RPL_TOPIC          "332"   // Topic du channel
# define RPL_TOPICWHOTIME   "333"   // Qui a défini le topic et quand
# define RPL_INVITING       "341"   // Confirmation d'invitation
# define RPL_NAMREPLY       "353"   // Liste des utilisateurs d'un channel
# define RPL_ENDOFNAMES     "366"   // Fin de la liste des utilisateurs

// Erreurs de commande
# define ERR_NOSUCHNICK     "401"   // Nickname inexistant
# define ERR_NOSUCHSERVER   "402"   // Serveur inexistant
# define ERR_NOSUCHCHANNEL  "403"   // Channel inexistant
# define ERR_CANNOTSENDTOCHAN "404" // Impossible d'envoyer au channel
# define ERR_TOOMANYCHANNELS "405"  // Trop de channels joints
# define ERR_NORECIPIENT    "411"   // Pas de destinataire
# define ERR_NOTEXTTOSEND   "412"   // Pas de texte à envoyer
# define ERR_UNKNOWNCOMMAND "421"   // Commande inconnue
# define ERR_NONICKNAMEGIVEN "431"  // Pas de nickname fourni
# define ERR_ERRONEUSNICKNAME "432" // Nickname invalide
# define ERR_NICKNAMEINUSE  "433"   // Nickname déjà utilisé
# define ERR_USERNOTINCHANNEL "441" // User pas dans le channel
# define ERR_NOTONCHANNEL   "442"   // Vous n'êtes pas dans ce channel
# define ERR_USERONCHANNEL  "443"   // User déjà dans le channel
# define ERR_NOTREGISTERED  "451"   // Pas encore authentifié
# define ERR_NEEDMOREPARAMS "461"   // Paramètres manquants
# define ERR_ALREADYREGISTERED "462"// Déjà authentifié
# define ERR_PASSWDMISMATCH "464"   // Mot de passe incorrect
# define ERR_KEYSET         "467"   // Clé du channel déjà définie
# define ERR_CHANNELISFULL  "471"   // Channel plein (limite atteinte)
# define ERR_UNKNOWNMODE    "472"   // Mode inconnu
# define ERR_INVITEONLYCHAN "473"   // Channel sur invitation uniquement
# define ERR_BADCHANNELKEY  "475"   // Mauvaise clé de channel
# define ERR_CHANOPRIVSNEEDED "482" // Privilèges opérateur requis
# define ERR_UMODEUNKNOWNFLAG "501" // Flag de mode inconnu


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