# ft_irc - Internet Relay Chat Server

## Vue d'ensemble du projet

Ce projet consiste à implémenter un serveur IRC (Internet Relay Chat) en C++98.
Le serveur doit être capable de gérer plusieurs clients simultanément et de
supporter les fonctionnalités de base d'IRC.

## Architecture du projet

```
ft_irc/
├── Makefile                 # Compilation du projet
├── README.md                # Ce fichier
├── includes/                # Headers (.hpp)
│   ├── IRC.hpp              # Header principal (includes + constantes)
│   ├── Server.hpp           # Classe serveur
│   ├── Client.hpp           # Classe client
│   ├── Channel.hpp          # Classe channel
│   ├── CommandHandler.hpp   # Gestionnaire de commandes
│   ├── Utils.hpp            # Fonctions utilitaires
│   └── Parser.hpp           # Parsing IRC
│
└── src/                     # Sources (.cpp)
    ├── main.cpp             # Point d'entrée
    ├── server/
    │   ├── Server.cpp       # Implémentation serveur
    │   └── Socket.cpp       # Utilitaires socket (optionnel)
    ├── client/
    │   └── Client.cpp       # Implémentation client
    ├── channel/
    │   └── Channel.cpp      # Implémentation channel
    ├── commands/
    │   ├── CommandHandler.cpp  # Dispatcher de commandes
    │   ├── Pass.cpp         # Commande PASS
    │   ├── Nick.cpp         # Commande NICK
    │   ├── User.cpp         # Commande USER
    │   ├── Join.cpp         # Commande JOIN
    │   ├── Part.cpp         # Commande PART
    │   ├── Privmsg.cpp      # Commande PRIVMSG
    │   ├── Kick.cpp         # Commande KICK
    │   ├── Invite.cpp       # Commande INVITE
    │   ├── Topic.cpp        # Commande TOPIC
    │   ├── Mode.cpp         # Commande MODE
    │   ├── Quit.cpp         # Commande QUIT
    │   └── Ping.cpp         # Commande PING
    └── utils/
        ├── Utils.cpp        # Fonctions utilitaires
        └── Parser.cpp       # Parsing IRC
```

## Ordre de développement recommandé

### Phase 1 : Fondations (Utils et parsing)
1. **Utils.cpp** - Implémenter les fonctions de base
   - `trim()`, `split()`, `toLower()`, `toUpper()`
   - `intToString()`, `stringToInt()`
   - Tester chaque fonction individuellement

2. **Parser.cpp** - Implémenter le parsing IRC
   - `parseChannelList()`, `parseKeyList()`
   - `isValidChannelName()`, `isValidNickname()`

### Phase 2 : Classes de base (Client, Channel)
3. **Client.cpp** - Implémenter la classe Client
   - Constructeur, getters, setters
   - Gestion des buffers
   - Gestion des channels

4. **Channel.cpp** - Implémenter la classe Channel
   - Gestion des membres
   - Gestion des opérateurs
   - Gestion des modes (i, t, k, l)
   - Gestion du topic

### Phase 3 : Serveur et réseau
5. **Server.cpp** - Implémenter le serveur
   - `init()` : création du socket, bind, listen
   - `run()` : boucle principale avec poll()
   - `acceptNewClient()` : accepter les connexions
   - `handleClientData()` : lecture des données
   - `sendToClient()`, `broadcastToChannel()`
   - `disconnectClient()` : déconnexion propre

6. **main.cpp** - Point d'entrée
   - Validation des arguments
   - Gestion des signaux
   - Création et lancement du serveur

### Phase 4 : Commandes d'authentification
7. **Pass.cpp** - Commande PASS
8. **Nick.cpp** - Commande NICK
9. **User.cpp** - Commande USER

À ce stade, tu devrais pouvoir te connecter avec un client IRC !

### Phase 5 : Commandes de base
10. **Ping.cpp** - Commande PING (simple)
11. **Quit.cpp** - Commande QUIT
12. **Join.cpp** - Commande JOIN
13. **Part.cpp** - Commande PART
14. **Privmsg.cpp** - Commande PRIVMSG

### Phase 6 : Commandes opérateur
15. **Topic.cpp** - Commande TOPIC
16. **Kick.cpp** - Commande KICK
17. **Invite.cpp** - Commande INVITE
18. **Mode.cpp** - Commande MODE (la plus complexe)

## Compilation

```bash
make        # Compile le projet
make clean  # Supprime les fichiers objets
make fclean # Supprime tout (objets + exécutable)
make re     # Recompile tout
```

## Utilisation

```bash
./ircserv <port> <password>

# Exemple
./ircserv 6667 mypassword
```

## Tests

### Test avec netcat
```bash
nc -C 127.0.0.1 6667
PASS mypassword
NICK testuser
USER testuser 0 * :Test User
JOIN #test
PRIVMSG #test :Hello World!
QUIT
```

### Test avec irssi
```bash
irssi -c 127.0.0.1 -p 6667 -w mypassword
```

### Test avec HexChat/WeeChat
Configurer le serveur avec :
- Adresse : 127.0.0.1
- Port : 6667
- Mot de passe : mypassword

## Clients IRC recommandés pour les tests

1. **irssi** (terminal) - `apt install irssi`
2. **WeeChat** (terminal) - `apt install weechat`
3. **HexChat** (GUI) - `apt install hexchat`
4. **LimeChat** (macOS)

## Ressources utiles

- [RFC 2812](https://tools.ietf.org/html/rfc2812) - IRC Protocol
- [RFC 1459](https://tools.ietf.org/html/rfc1459) - IRC Original
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/)

## Conseils de développement

1. **Compiler souvent** : Compile après chaque fonction implémentée
2. **Tester progressivement** : Teste chaque commande individuellement
3. **Utiliser les logs** : Ajoute des std::cout pour débugger
4. **Lire les RFCs** : Les messages d'erreur sont standardisés
5. **Utiliser netcat** : Parfait pour envoyer des commandes brutes

## Points d'attention

- **Non-bloquant** : Tous les sockets doivent être non-bloquants
- **poll()** : Une seule instance pour gérer tous les fd
- **Buffers** : Gérer les commandes fragmentées (partial data)
- **CRLF** : Les messages IRC se terminent par `\r\n`
- **Majuscules** : Les commandes sont insensibles à la casse
