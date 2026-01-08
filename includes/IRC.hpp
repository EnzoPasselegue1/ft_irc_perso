#ifndef IRC_HPP
# define IRC_HPP

/* ========================================================================== */
/*                         BIBLIOTHÈQUES STANDARD C++                         */
/* ========================================================================== */

// Conteneurs STL
# include <string>      // std::string pour la manipulation de chaînes
# include <vector>      // std::vector pour les tableaux dynamiques
# include <map>         // std::map pour les associations clé-valeur
# include <set>         // std::set pour les ensembles sans doublons

// Flux d'entrée/sortie
# include <iostream>    // std::cout, std::cerr pour l'affichage
# include <sstream>     // std::stringstream pour la conversion string/nombre
# include <fstream>     // std::ifstream, std::ofstream pour les fichiers

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
# include <fcntl.h>         // fcntl() pour le mode non-bloquant
# include <sys/stat.h>      // fstat() pour les informations sur les fichiers

// Multiplexage I/O
# include <poll.h>          // poll() ou alternative (select, epoll, kqueue)

// Signaux
# include <signal.h>        // signal(), sigaction() pour gérer Ctrl+C, etc.





/* ========================================================================== */


/* ========================================================================== */
/*                        HEADERS OF PROJECT IRC                            */
/* ========================================================================== */

# include "Utils.hpp"
# include "Parser.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "CommandHandler.hpp"
# include "Server.hpp"


#endif