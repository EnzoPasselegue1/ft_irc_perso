#include "IRC.hpp"
#include <iostream>
#include <iomanip>

void printSeparator(const std::string& title)
{
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  " << std::left << std::setw(56) << title << "  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝\n" << std::endl;
}

void testConstructorDestructor()
{
    printSeparator("CONSTRUCTEUR / DESTRUCTEUR");
    
    std::cout << "Creating client with fd=5 and hostname='localhost'..." << std::endl;
    Client* client = new Client(5, "localhost");
    std::cout << "Client fd: " << client->getFd() << std::endl;
    std::cout << "Client hostname: " << client->getHostname() << std::endl;
    
    std::cout << "\nDeleting client..." << std::endl;
    delete client;
}

void testIdentification()
{
    printSeparator("IDENTIFICATION");
    
    Client client(10, "irc.server.com");
    
    // Test nickname
    std::cout << "--- Nickname ---" << std::endl;
    std::cout << "Initial nickname: \"" << client.getNickname() << "\" (should be empty)" << std::endl;
    client.setNickname("JohnDoe");
    std::cout << "After setNickname('JohnDoe'): \"" << client.getNickname() << "\"" << std::endl;
    
    // Test username
    std::cout << "\n--- Username ---" << std::endl;
    std::cout << "Initial username: \"" << client.getUsername() << "\" (should be empty)" << std::endl;
    client.setUsername("john");
    std::cout << "After setUsername('john'): \"" << client.getUsername() << "\"" << std::endl;
    
    // Test realname
    std::cout << "\n--- Realname ---" << std::endl;
    std::cout << "Initial realname: \"" << client.getRealname() << "\" (should be empty)" << std::endl;
    client.setRealname("John Doe");
    std::cout << "After setRealname('John Doe'): \"" << client.getRealname() << "\"" << std::endl;
    
    // Test hostname
    std::cout << "\n--- Hostname ---" << std::endl;
    std::cout << "Hostname: \"" << client.getHostname() << "\"" << std::endl;
    
    // Test prefix
    std::cout << "\n--- Prefix ---" << std::endl;
    std::cout << "Full prefix: \"" << client.getPrefix() << "\"" << std::endl;
    std::cout << "Expected format: nickname!username@hostname" << std::endl;
}

void testAuthentication()
{
    printSeparator("ÉTAT D'AUTHENTIFICATION");
    
    Client client(15, "192.168.1.1");
    
    // Test password
    std::cout << "--- Password Status ---" << std::endl;
    std::cout << "Initial hasPasswordProvided: " 
              << (client.hasPasswordProvided() ? "true" : "false") 
              << " (should be false)" << std::endl;
    
    client.setPasswordProvided(true);
    std::cout << "After setPasswordProvided(true): " 
              << (client.hasPasswordProvided() ? "true" : "false") << std::endl;
    
    client.setPasswordProvided(false);
    std::cout << "After setPasswordProvided(false): " 
              << (client.hasPasswordProvided() ? "true" : "false") << std::endl;
    
    // Test registration
    std::cout << "\n--- Registration Status ---" << std::endl;
    std::cout << "Initial isRegistered: " 
              << (client.isRegistered() ? "true" : "false") 
              << " (should be false)" << std::endl;
    
    client.setRegistered(true);
    std::cout << "After setRegistered(true): " 
              << (client.isRegistered() ? "true" : "false") << std::endl;
    
    client.setRegistered(false);
    std::cout << "After setRegistered(false): " 
              << (client.isRegistered() ? "true" : "false") << std::endl;
}

void testChannelManagement()
{
    printSeparator("GESTION DES CHANNELS");
    
    Client client(20, "client.host.com");
    
    std::cout << "--- Initial State ---" << std::endl;
    std::cout << "Channels count: " << client.getChannels().size() 
              << " (should be 0)" << std::endl;
    
    // Test joinChannel
    std::cout << "\n--- Join Channels ---" << std::endl;
    client.joinChannel("#general");
    std::cout << "After joining #general:" << std::endl;
    std::cout << "  Channels count: " << client.getChannels().size() << std::endl;
    std::cout << "  Is in #general: " 
              << (client.isInChannel("#general") ? "true" : "false") << std::endl;
    std::cout << "  Is in #GENERAL: " 
              << (client.isInChannel("#GENERAL") ? "true" : "false") 
              << " (case insensitive)" << std::endl;
    
    client.joinChannel("#random");
    client.joinChannel("#help");
    std::cout << "\nAfter joining #random and #help:" << std::endl;
    std::cout << "  Channels count: " << client.getChannels().size() << std::endl;
    
    // Display all channels
    std::cout << "  All channels: ";
    const std::set<std::string>& channels = client.getChannels();
    for (std::set<std::string>::const_iterator it = channels.begin(); 
         it != channels.end(); ++it)
    {
        std::cout << "\"" << *it << "\" ";
    }
    std::cout << std::endl;
    
    // Test leaveChannel
    std::cout << "\n--- Leave Channel ---" << std::endl;
    client.leaveChannel("#random");
    std::cout << "After leaving #random:" << std::endl;
    std::cout << "  Channels count: " << client.getChannels().size() << std::endl;
    std::cout << "  Is in #random: " 
              << (client.isInChannel("#random") ? "true" : "false") << std::endl;
    std::cout << "  Is in #general: " 
              << (client.isInChannel("#general") ? "true" : "false") << std::endl;
    
    // Test case insensitivity
    std::cout << "\n--- Case Insensitivity Test ---" << std::endl;
    client.leaveChannel("#GENERAL");
    std::cout << "After leaving #GENERAL (uppercase):" << std::endl;
    std::cout << "  Is in #general: " 
              << (client.isInChannel("#general") ? "true" : "false") 
              << " (should be false)" << std::endl;
}

void testBufferManagement()
{
    printSeparator("GESTION DES BUFFERS");
    
    Client client(25, "buffer.test.com");
    
    // Test input buffer
    std::cout << "--- Input Buffer ---" << std::endl;
    std::cout << "Initial input buffer: \"" << client.getInputBuffer() 
              << "\" (should be empty)" << std::endl;
    
    client.appendToInputBuffer("NICK ");
    std::cout << "After appending 'NICK ': \"" << client.getInputBuffer() << "\"" << std::endl;
    
    client.appendToInputBuffer("JohnDoe\r\n");
    std::cout << "After appending 'JohnDoe\\r\\n': \"" << client.getInputBuffer() << "\"" << std::endl;
    
    client.clearInputBuffer();
    std::cout << "After clearInputBuffer(): \"" << client.getInputBuffer() 
              << "\" (should be empty)" << std::endl;
    
    // Test output buffer
    std::cout << "\n--- Output Buffer ---" << std::endl;
    std::cout << "Initial hasDataToSend: " 
              << (client.hasDataToSend() ? "true" : "false") 
              << " (should be false)" << std::endl;
    
    client.appendToOutputBuffer(":server 001 user :Welcome\r\n");
    std::cout << "After appending message:" << std::endl;
    std::cout << "  hasDataToSend: " 
              << (client.hasDataToSend() ? "true" : "false") << std::endl;
    std::cout << "  Buffer content: \"" << client.getOutputBuffer() << "\"" << std::endl;
    std::cout << "  Buffer size: " << client.getOutputBuffer().size() << " bytes" << std::endl;
    
    client.appendToOutputBuffer(":server 002 user :Your host\r\n");
    std::cout << "\nAfter appending second message:" << std::endl;
    std::cout << "  Buffer size: " << client.getOutputBuffer().size() << " bytes" << std::endl;
    
    // Test trimOutputBuffer
    std::cout << "\n--- Trim Output Buffer ---" << std::endl;
    size_t firstMessageSize = 28; // ":server 001 user :Welcome\r\n"
    std::cout << "Trimming " << firstMessageSize << " bytes..." << std::endl;
    client.trimOutputBuffer(firstMessageSize);
    std::cout << "  Remaining buffer: \"" << client.getOutputBuffer() << "\"" << std::endl;
    std::cout << "  Buffer size: " << client.getOutputBuffer().size() << " bytes" << std::endl;
    
    // Test trimming more than buffer size
    std::cout << "\nTrimming 1000 bytes (more than buffer size)..." << std::endl;
    client.trimOutputBuffer(1000);
    std::cout << "  Buffer is now empty: " 
              << (client.getOutputBuffer().empty() ? "true" : "false") << std::endl;
    std::cout << "  hasDataToSend: " 
              << (client.hasDataToSend() ? "true" : "false") << std::endl;
}

void testAccessors()
{
    printSeparator("ACCESSEURS ET DÉCONNEXION");
    
    Client client(30, "test.server.com");
    
    // Test getFd
    std::cout << "--- File Descriptor ---" << std::endl;
    std::cout << "getFd(): " << client.getFd() << std::endl;
    
    // Test disconnection flag
    std::cout << "\n--- Disconnection Flag ---" << std::endl;
    std::cout << "Initial shouldDisconnect: " 
              << (client.shouldDisconnect() ? "true" : "false") 
              << " (should be false)" << std::endl;
    
    client.markForDisconnection();
    std::cout << "After markForDisconnection():" << std::endl;
    std::cout << "  shouldDisconnect: " 
              << (client.shouldDisconnect() ? "true" : "false") 
              << " (should be true)" << std::endl;
}

void testCompleteScenario()
{
    printSeparator("SCÉNARIO COMPLET D'UTILISATION");
    
    std::cout << "Simulating a complete client connection scenario...\n" << std::endl;
    
    // Create client
    std::cout << "1. Client connects from 192.168.1.100" << std::endl;
    Client client(42, "192.168.1.100");
    
    // Authentication
    std::cout << "\n2. Client sends PASS command" << std::endl;
    client.setPasswordProvided(true);
    std::cout << "   Password provided: " 
              << (client.hasPasswordProvided() ? "✓" : "✗") << std::endl;
    
    std::cout << "\n3. Client sends NICK command" << std::endl;
    client.setNickname("Alice");
    std::cout << "   Nickname: " << client.getNickname() << std::endl;
    
    std::cout << "\n4. Client sends USER command" << std::endl;
    client.setUsername("alice");
    client.setRealname("Alice Wonderland");
    std::cout << "   Username: " << client.getUsername() << std::endl;
    std::cout << "   Realname: " << client.getRealname() << std::endl;
    
    std::cout << "\n5. Client is now registered" << std::endl;
    client.setRegistered(true);
    std::cout << "   Registered: " 
              << (client.isRegistered() ? "✓" : "✗") << std::endl;
    std::cout << "   Full prefix: " << client.getPrefix() << std::endl;
    
    // Join channels
    std::cout << "\n6. Client joins channels" << std::endl;
    client.joinChannel("#general");
    client.joinChannel("#random");
    client.joinChannel("#help");
    std::cout << "   Channels (" << client.getChannels().size() << "): ";
    const std::set<std::string>& channels = client.getChannels();
    for (std::set<std::string>::const_iterator it = channels.begin(); 
         it != channels.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // Send message
    std::cout << "\n7. Client sends message to #general" << std::endl;
    std::string message = "PRIVMSG #general :Hello everyone!\r\n";
    client.appendToInputBuffer(message);
    std::cout << "   Input buffer: \"" << client.getInputBuffer() << "\"" << std::endl;
    
    // Server response
    std::cout << "\n8. Server prepares response" << std::endl;
    std::string response = ":server 353 Alice = #general :Alice Bob Charlie\r\n";
    client.appendToOutputBuffer(response);
    std::cout << "   Has data to send: " 
              << (client.hasDataToSend() ? "yes" : "no") << std::endl;
    
    // Leave channel
    std::cout << "\n9. Client leaves #random" << std::endl;
    client.leaveChannel("#random");
    std::cout << "   Remaining channels: " << client.getChannels().size() << std::endl;
    
    // Disconnect
    std::cout << "\n10. Client disconnects" << std::endl;
    client.markForDisconnection();
    std::cout << "   Should disconnect: " 
              << (client.shouldDisconnect() ? "yes" : "no") << std::endl;
}

int main()
{
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              CLIENT CLASS TEST SUITE                       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    
    try
    {
        testConstructorDestructor();
        testIdentification();
        testAuthentication();
        testChannelManagement();
        testBufferManagement();
        testAccessors();
        testCompleteScenario();
        
        std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║              ALL TESTS COMPLETED                           ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════╝\n" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}