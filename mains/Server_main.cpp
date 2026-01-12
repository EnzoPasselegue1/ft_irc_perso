#include "IRC.hpp"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

// Couleurs pour l'affichage
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

class ServerTester
{
private:
    Server* _server;
    int _testPort;
    std::string _testPassword;

    void printTestHeader(const std::string& testName)
    {
        std::cout << "\n" << BLUE << "========================================" << RESET << std::endl;
        std::cout << YELLOW << "TEST: " << testName << RESET << std::endl;
        std::cout << BLUE << "========================================" << RESET << std::endl;
    }

    void printSuccess(const std::string& message)
    {
        std::cout << GREEN << "✓ " << message << RESET << std::endl;
    }

    void printError(const std::string& message)
    {
        std::cout << RED << "✗ " << message << RESET << std::endl;
    }

    void printInfo(const std::string& message)
    {
        std::cout << "  " << message << std::endl;
    }

    // Créer un socket client de test
    int createTestClient()
    {
        int clientFd = socket(AF_INET, SOCK_STREAM, 0);
        if (clientFd == -1)
        {
            printError("Impossible de créer un socket client de test");
            return -1;
        }

        struct sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(_testPort);
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

        if (connect(clientFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        {
            printError("Impossible de connecter le client de test");
            close(clientFd);
            return -1;
        }

        // Attendre un peu pour que le serveur accepte la connexion
        usleep(100000); // 100ms

        return clientFd;
    }

public:
    ServerTester(int port = 6667, const std::string& password = "test123")
        : _testPort(port), _testPassword(password)
    {
        _server = new Server(_testPort, _testPassword);
    }

    ~ServerTester()
    {
        delete _server;
    }

    // Test 1: Initialisation du serveur
    void testServerInit()
    {
        printTestHeader("Initialisation du serveur");
        
        bool initResult = _server->init();
        
        if (initResult)
        {
            printSuccess("Serveur initialisé avec succès");
            printInfo("Port: " + Utils::intToString(_testPort));
            printInfo("Nom du serveur: " + _server->getServerName());
        }
        else
        {
            printError("Échec de l'initialisation du serveur");
        }
        
        assert(initResult && "L'initialisation du serveur doit réussir");
    }

    // Test 2: Getters du serveur
    void testServerGetters()
    {
        printTestHeader("Getters du serveur");
        
        assert(_server->getPassword() == _testPassword);
        printSuccess("getPassword() retourne le bon mot de passe");
        
        assert(!_server->getServerName().empty());
        printSuccess("getServerName() retourne un nom non vide");
        
        assert(_server->getClients().empty());
        printSuccess("getClients() retourne une map vide au départ");
        
        assert(_server->getChannels().empty());
        printSuccess("getChannels() retourne une map vide au départ");
    }

    // Test 3: Gestion des channels
    void testChannelManagement()
    {
        printTestHeader("Gestion des channels");
        
        // Créer un nouveau channel
        Channel* chan1 = _server->getOrCreatChannel("#test");
        assert(chan1 != NULL);
        printSuccess("getOrCreatChannel() crée un nouveau channel");
        
        // Récupérer le même channel
        Channel* chan2 = _server->getOrCreatChannel("#test");
        assert(chan1 == chan2);
        printSuccess("getOrCreatChannel() retourne le channel existant");
        
        // Test insensible à la casse
        Channel* chan3 = _server->getOrCreatChannel("#TEST");
        assert(chan1 == chan3);
        printSuccess("Les noms de channels sont insensibles à la casse");
        
        // Récupérer un channel
        Channel* chan4 = _server->getChannel("#test");
        assert(chan4 == chan1);
        printSuccess("getChannel() retourne le bon channel");
        
        // Channel inexistant
        Channel* chan5 = _server->getChannel("#inexistant");
        assert(chan5 == NULL);
        printSuccess("getChannel() retourne NULL pour un channel inexistant");
        
        // Créer plusieurs channels
        _server->getOrCreatChannel("#general");
        _server->getOrCreatChannel("#random");
        assert(_server->getChannels().size() == 3);
        printSuccess("Plusieurs channels peuvent être créés");
        
        printInfo("Nombre de channels: " + Utils::intToString(_server->getChannels().size()));
    }

    // Test 4: Gestion des clients (simulation)
    void testClientManagement()
    {
        printTestHeader("Gestion des clients");
        
        // Créer des clients simulés
        Client* client1 = new Client(100, "127.0.0.1");
        Client* client2 = new Client(101, "127.0.0.1");
        
        client1->setNickname("Alice");
        client2->setNickname("Bob");
        
        _server->getClients()[100] = client1;
        _server->getClients()[101] = client2;
        
        printSuccess("Clients ajoutés manuellement pour le test");
        
        // Test de recherche par nickname
        Client* found1 = _server->getClientByNickname("Alice");
        assert(found1 == client1);
        printSuccess("getClientByNickname() trouve Alice");
        
        Client* found2 = _server->getClientByNickname("alice"); // Insensible à la casse
        assert(found2 == client1);
        printSuccess("getClientByNickname() est insensible à la casse");
        
        // Test d'existence de nickname
        assert(_server->isNicknameInUse("Alice"));
        printSuccess("isNicknameInUse() détecte les nicknames utilisés");
        
        assert(!_server->isNicknameInUse("Charlie"));
        printSuccess("isNicknameInUse() détecte les nicknames libres");
        
        printInfo("Nombre de clients: " + Utils::intToString(_server->getClients().size()));
    }

    // Test 5: Communication (buffer)
    void testCommunication()
    {
        printTestHeader("Communication (buffers)");
        
        // Utiliser les clients déjà créés
        std::map<int, Client*>& clients = _server->getClients();
        if (clients.empty())
        {
            printError("Aucun client disponible pour le test");
            return;
        }
        
        Client* client = clients.begin()->second;
        int fd = clients.begin()->first;
        
        // Test d'envoi de message
        std::string testMsg = "Hello World";
        _server->sendToClient(fd, testMsg);
        
        std::string& buffer = client->getOutputBuffer();
        assert(!buffer.empty());
        printSuccess("sendToClient() ajoute le message au buffer");
        
        assert(buffer.find(testMsg) != std::string::npos);
        printSuccess("Le message est présent dans le buffer");
        
        printInfo("Taille du buffer: " + Utils::intToString(buffer.size()) + " octets");
    }

    // Test 6: Broadcast sur channel
    void testChannelBroadcast()
    {
        printTestHeader("Broadcast sur channel");
        
        // Créer un channel et y ajouter des clients
        Channel* chan = _server->getOrCreatChannel("#broadcast");
        
        std::map<int, Client*>& clients = _server->getClients();
        for (std::map<int, Client*>::iterator it = clients.begin();
             it != clients.end(); ++it)
        {
            chan->addMember(it->second);
            it->second->joinChannel("#broadcast");
        }
        
        printInfo("Membres du channel: " + Utils::intToString(chan->getMembers().size()));
        
        // Effacer les buffers
        for (std::map<int, Client*>::iterator it = clients.begin();
             it != clients.end(); ++it)
        {
            it->second->getOutputBuffer().clear();
        }
        
        // Broadcast un message
        std::string broadcastMsg = "Broadcast test message";
        _server->broadcastToChannel("#broadcast", broadcastMsg, -1);
        
        // Vérifier que tous les clients ont reçu le message
        int receivedCount = 0;
        for (std::map<int, Client*>::iterator it = clients.begin();
             it != clients.end(); ++it)
        {
            if (!it->second->getOutputBuffer().empty())
                receivedCount++;
        }
        
        assert(receivedCount == static_cast<int>(clients.size()));
        printSuccess("Tous les membres du channel ont reçu le message");
        
        // Test avec exclusion
        for (std::map<int, Client*>::iterator it = clients.begin();
             it != clients.end(); ++it)
        {
            it->second->getOutputBuffer().clear();
        }
        
        int excludeFd = clients.begin()->first;
        _server->broadcastToChannel("#broadcast", "Excluded test", excludeFd);
        
        receivedCount = 0;
        for (std::map<int, Client*>::iterator it = clients.begin();
             it != clients.end(); ++it)
        {
            if (!it->second->getOutputBuffer().empty())
                receivedCount++;
        }
        
        assert(receivedCount == static_cast<int>(clients.size()) - 1);
        printSuccess("L'exclusion fonctionne correctement");
    }

    // Test 7: Suppression de channel
    void testChannelRemoval()
    {
        printTestHeader("Suppression de channel");
        
        _server->getOrCreatChannel("#todelete");
        size_t sizeBefore = _server->getChannels().size();
        
        _server->removeChannel("#todelete");
        size_t sizeAfter = _server->getChannels().size();
        
        assert(sizeAfter == sizeBefore - 1);
        printSuccess("removeChannel() supprime le channel");
        
        Channel* deleted = _server->getChannel("#todelete");
        assert(deleted == NULL);
        printSuccess("Le channel supprimé n'est plus accessible");
    }

    // Test 8: Gestion des buffers client
    void testClientBuffers()
    {
        printTestHeader("Gestion des buffers client");
        
        Client testClient(999, "127.0.0.1");
        
        // Test du buffer d'entrée
        testClient.appendToInputBuffer("NICK test");
        testClient.appendToInputBuffer("\r\n");
        
        std::string& inputBuf = testClient.getInputBuffer();
        assert(inputBuf.find("NICK test") != std::string::npos);
        printSuccess("appendToInputBuffer() fonctionne");
        
        // Test du buffer de sortie
        testClient.appendToOutputBuffer("Welcome message\r\n");
        std::string& outputBuf = testClient.getOutputBuffer();
        assert(!outputBuf.empty());
        printSuccess("appendToOutputBuffer() fonctionne");
        
        size_t sizeBefore = outputBuf.size();
        testClient.trimOutputBuffer(7); // Retirer "Welcome"
        assert(outputBuf.size() == sizeBefore - 7);
        printSuccess("trimOutputBuffer() réduit le buffer");
    }

    // Nettoyer les clients de test
    void cleanup()
    {
        printTestHeader("Nettoyage");
        
        std::map<int, Client*>& clients = _server->getClients();
        for (std::map<int, Client*>::iterator it = clients.begin();
             it != clients.end(); ++it)
        {
            delete it->second;
        }
        clients.clear();
        
        printSuccess("Clients de test nettoyés");
    }

    // Exécuter tous les tests
    void runAllTests()
    {
        std::cout << "\n" << GREEN << "╔════════════════════════════════════════╗" << RESET << std::endl;
        std::cout << GREEN << "║   SUITE DE TESTS - SERVEUR IRC        ║" << RESET << std::endl;
        std::cout << GREEN << "╚════════════════════════════════════════╝" << RESET << std::endl;

        try
        {
            testServerInit();
            testServerGetters();
            testChannelManagement();
            testClientManagement();
            testCommunication();
            testChannelBroadcast();
            testChannelRemoval();
            testClientBuffers();
            cleanup();

            std::cout << "\n" << GREEN << "╔════════════════════════════════════════╗" << RESET << std::endl;
            std::cout << GREEN << "║   TOUS LES TESTS ONT RÉUSSI ! ✓       ║" << RESET << std::endl;
            std::cout << GREEN << "╚════════════════════════════════════════╝" << RESET << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "\n" << RED << "╔════════════════════════════════════════╗" << RESET << std::endl;
            std::cout << RED << "║   ÉCHEC DES TESTS !                   ║" << RESET << std::endl;
            std::cout << RED << "╚════════════════════════════════════════╝" << RESET << std::endl;
            std::cerr << RED << "Exception: " << e.what() << RESET << std::endl;
        }
    }
};

int main()
{
    std::cout << "Démarrage de la suite de tests du serveur IRC..." << std::endl;
    
    ServerTester tester(6667, "testpass123");
    tester.runAllTests();
    
    return 0;
}