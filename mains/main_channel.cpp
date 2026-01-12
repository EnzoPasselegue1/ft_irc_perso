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
    
    std::cout << "Creating channel '#general'..." << std::endl;
    Channel* channel = new Channel("#general");
    std::cout << "Channel name: " << channel->getName() << std::endl;
    std::cout << "Initial member count: " << channel->getMemberCount() << std::endl;
    std::cout << "Is empty: " << (channel->isEmpty() ? "true" : "false") << std::endl;
    
    std::cout << "\nDeleting channel..." << std::endl;
    delete channel;
}

void testMemberManagement()
{
    printSeparator("GESTION DES MEMBRES");
    
    Channel channel("#test");
    
    // Create test clients
    Client* alice = new Client(1, "host1.com");
    alice->setNickname("Alice");
    
    Client* bob = new Client(2, "host2.com");
    bob->setNickname("Bob");
    
    Client* charlie = new Client(3, "host3.com");
    charlie->setNickname("Charlie");
    
    // Test addMember
    std::cout << "--- Add Members ---" << std::endl;
    std::cout << "Initial member count: " << channel.getMemberCount() << std::endl;
    std::cout << "Is empty: " << (channel.isEmpty() ? "true" : "false") << std::endl;
    
    std::cout << "\nAdding Alice..." << std::endl;
    bool added = channel.addMember(alice);
    std::cout << "  Added: " << (added ? "true" : "false") << std::endl;
    std::cout << "  Member count: " << channel.getMemberCount() << std::endl;
    std::cout << "  Alice is operator: " << (channel.isOperator(alice) ? "true" : "false") 
              << " (first member becomes op)" << std::endl;
    
    std::cout << "\nAdding Alice again (should fail)..." << std::endl;
    added = channel.addMember(alice);
    std::cout << "  Added: " << (added ? "true" : "false") << std::endl;
    std::cout << "  Member count: " << channel.getMemberCount() << std::endl;
    
    std::cout << "\nAdding Bob and Charlie..." << std::endl;
    channel.addMember(bob);
    channel.addMember(charlie);
    std::cout << "  Member count: " << channel.getMemberCount() << std::endl;
    
    // Test isMember
    std::cout << "\n--- Is Member ---" << std::endl;
    std::cout << "Alice is member: " << (channel.isMember(alice) ? "true" : "false") << std::endl;
    std::cout << "Bob is member: " << (channel.isMember(bob) ? "true" : "false") << std::endl;
    
    // Test hasMember (by nickname)
    std::cout << "\n--- Has Member (by nickname) ---" << std::endl;
    std::cout << "Has 'Alice': " << (channel.hasMember("Alice") ? "true" : "false") << std::endl;
    std::cout << "Has 'alice': " << (channel.hasMember("alice") ? "true" : "false") 
              << " (case insensitive)" << std::endl;
    std::cout << "Has 'Dave': " << (channel.hasMember("Dave") ? "true" : "false") << std::endl;
    
    // Test getMemberByNickname
    std::cout << "\n--- Get Member By Nickname ---" << std::endl;
    Client* found = channel.getMemberByNickname("Bob");
    std::cout << "getMemberByNickname('Bob'): " 
              << (found ? found->getNickname() : "NULL") << std::endl;
    
    found = channel.getMemberByNickname("CHARLIE");
    std::cout << "getMemberByNickname('CHARLIE'): " 
              << (found ? found->getNickname() : "NULL") 
              << " (case insensitive)" << std::endl;
    
    found = channel.getMemberByNickname("Unknown");
    std::cout << "getMemberByNickname('Unknown'): " 
              << (found ? found->getNickname() : "NULL") << std::endl;
    
    // Test getMembers
    std::cout << "\n--- Get All Members ---" << std::endl;
    const std::set<Client*>& members = channel.getMembers();
    std::cout << "All members (" << members.size() << "): ";
    for (std::set<Client*>::const_iterator it = members.begin(); 
         it != members.end(); ++it)
    {
        std::cout << (*it)->getNickname() << " ";
    }
    std::cout << std::endl;
    
    // Test removeMember
    std::cout << "\n--- Remove Member ---" << std::endl;
    std::cout << "Removing Bob..." << std::endl;
    channel.removeMember(bob);
    std::cout << "  Member count: " << channel.getMemberCount() << std::endl;
    std::cout << "  Bob is member: " << (channel.isMember(bob) ? "true" : "false") << std::endl;
    
    // Cleanup
    delete alice;
    delete bob;
    delete charlie;
}

void testOperatorManagement()
{
    printSeparator("GESTION DES OPÉRATEURS");
    
    Channel channel("#ops");
    
    Client* alice = new Client(1, "host1.com");
    alice->setNickname("Alice");
    
    Client* bob = new Client(2, "host2.com");
    bob->setNickname("Bob");
    
    Client* charlie = new Client(3, "host3.com");
    charlie->setNickname("Charlie");
    
    // Add members
    channel.addMember(alice);  // Becomes op automatically
    channel.addMember(bob);
    channel.addMember(charlie);
    
    std::cout << "--- Initial State ---" << std::endl;
    std::cout << "Alice is operator: " << (channel.isOperator(alice) ? "true" : "false") 
              << " (first member)" << std::endl;
    std::cout << "Bob is operator: " << (channel.isOperator(bob) ? "true" : "false") << std::endl;
    
    // Test addOperator
    std::cout << "\n--- Add Operator ---" << std::endl;
    std::cout << "Making Bob an operator..." << std::endl;
    bool added = channel.addOperator(bob);
    std::cout << "  Added: " << (added ? "true" : "false") << std::endl;
    std::cout << "  Bob is operator: " << (channel.isOperator(bob) ? "true" : "false") << std::endl;
    
    // Test addOperator on non-member
    std::cout << "\n--- Add Operator (non-member) ---" << std::endl;
    Client* dave = new Client(4, "host4.com");
    dave->setNickname("Dave");
    
    std::cout << "Trying to make Dave (non-member) an operator..." << std::endl;
    added = channel.addOperator(dave);
    std::cout << "  Added: " << (added ? "true" : "false") 
              << " (should fail)" << std::endl;
    
    // Test isOperator by nickname
    std::cout << "\n--- Is Operator (by nickname) ---" << std::endl;
    std::cout << "isOperator('Alice'): " << (channel.isOperator("Alice") ? "true" : "false") << std::endl;
    std::cout << "isOperator('bob'): " << (channel.isOperator("bob") ? "true" : "false") 
              << " (case insensitive)" << std::endl;
    std::cout << "isOperator('Charlie'): " << (channel.isOperator("Charlie") ? "true" : "false") << std::endl;
    
    // Test removeOperator
    std::cout << "\n--- Remove Operator ---" << std::endl;
    std::cout << "Removing Bob from operators..." << std::endl;
    channel.removeOperator(bob);
    std::cout << "  Bob is operator: " << (channel.isOperator(bob) ? "true" : "false") << std::endl;
    std::cout << "  Bob is still member: " << (channel.isMember(bob) ? "true" : "false") << std::endl;
    
    // Cleanup
    delete alice;
    delete bob;
    delete charlie;
    delete dave;
}

void testModes()
{
    printSeparator("GESTION DES MODES");
    
    Channel channel("#modes");
    
    // Test invite-only mode
    std::cout << "--- Mode i (Invite-only) ---" << std::endl;
    std::cout << "Initial isInviteOnly: " << (channel.isInviteOnly() ? "true" : "false") << std::endl;
    
    channel.setInviteOnly(true);
    std::cout << "After setInviteOnly(true): " << (channel.isInviteOnly() ? "true" : "false") << std::endl;
    
    channel.setInviteOnly(false);
    std::cout << "After setInviteOnly(false): " << (channel.isInviteOnly() ? "true" : "false") << std::endl;
    
    // Test topic restricted mode
    std::cout << "\n--- Mode t (Topic restricted) ---" << std::endl;
    std::cout << "Initial isTopicRestricted: " << (channel.isTopicRestricted() ? "true" : "false") << std::endl;
    
    channel.setTopicRestricted(true);
    std::cout << "After setTopicRestricted(true): " << (channel.isTopicRestricted() ? "true" : "false") << std::endl;
    
    // Test key mode
    std::cout << "\n--- Mode k (Channel key) ---" << std::endl;
    std::cout << "Initial hasKey: " << (channel.hasKey() ? "true" : "false") << std::endl;
    std::cout << "Initial key: \"" << channel.getKey() << "\"" << std::endl;
    
    channel.setKey("secret123");
    std::cout << "\nAfter setKey('secret123'):" << std::endl;
    std::cout << "  hasKey: " << (channel.hasKey() ? "true" : "false") << std::endl;
    std::cout << "  getKey: \"" << channel.getKey() << "\"" << std::endl;
    std::cout << "  checkKey('secret123'): " << (channel.checkKey("secret123") ? "true" : "false") << std::endl;
    std::cout << "  checkKey('wrong'): " << (channel.checkKey("wrong") ? "true" : "false") << std::endl;
    
    channel.setKey("");
    std::cout << "\nAfter setKey(''):" << std::endl;
    std::cout << "  hasKey: " << (channel.hasKey() ? "true" : "false") << std::endl;
    
    // Test user limit mode
    std::cout << "\n--- Mode l (User limit) ---" << std::endl;
    std::cout << "Initial hasUserLimit: " << (channel.hasUserLimit() ? "true" : "false") << std::endl;
    std::cout << "Initial getUserLimit: " << channel.getUserLimit() << std::endl;
    
    channel.setUserLimit(5);
    std::cout << "\nAfter setUserLimit(5):" << std::endl;
    std::cout << "  hasUserLimit: " << (channel.hasUserLimit() ? "true" : "false") << std::endl;
    std::cout << "  getUserLimit: " << channel.getUserLimit() << std::endl;
    
    // Test isFull
    std::cout << "\n--- Is Full ---" << std::endl;
    Client* clients[6];
    for (int i = 0; i < 6; i++)
    {
        clients[i] = new Client(i + 10, "host.com");
        clients[i]->setNickname("User" + Utils::intToString(i + 1));
    }
    
    std::cout << "Adding members with limit=5:" << std::endl;
    for (int i = 0; i < 6; i++)
    {
        channel.addMember(clients[i]);
        std::cout << "  Member " << (i + 1) << " added. isFull: " 
                  << (channel.isFull() ? "true" : "false") << std::endl;
        if (channel.isFull())
            break;
    }
    
    channel.setUserLimit(0);
    std::cout << "\nAfter setUserLimit(0) (no limit):" << std::endl;
    std::cout << "  hasUserLimit: " << (channel.hasUserLimit() ? "true" : "false") << std::endl;
    std::cout << "  isFull: " << (channel.isFull() ? "true" : "false") << std::endl;
    
    // Test mode strings
    std::cout << "\n--- Mode Strings ---" << std::endl;
    Channel channel2("#modes2");
    
    std::cout << "No modes set:" << std::endl;
    std::cout << "  getModeString: \"" << channel2.getModeString() << "\"" << std::endl;
    
    channel2.setInviteOnly(true);
    channel2.setTopicRestricted(true);
    std::cout << "\nWith +i +t:" << std::endl;
    std::cout << "  getModeString: \"" << channel2.getModeString() << "\"" << std::endl;
    
    channel2.setKey("pass123");
    channel2.setUserLimit(10);
    std::cout << "\nWith +i +t +k +l:" << std::endl;
    std::cout << "  getModeString: \"" << channel2.getModeString() << "\"" << std::endl;
    std::cout << "  getModeStringWithParams: \"" << channel2.getModeStringWithParams() << "\"" << std::endl;
    
    // Cleanup
    for (int i = 0; i < 6; i++)
        delete clients[i];
}

void testTopic()
{
    printSeparator("GESTION DU TOPIC");
    
    Channel channel("#topic");
    
    std::cout << "--- Initial State ---" << std::endl;
    std::cout << "hasTopic: " << (channel.hasTopic() ? "true" : "false") << std::endl;
    std::cout << "getTopic: \"" << channel.getTopic() << "\"" << std::endl;
    std::cout << "getTopicSetter: \"" << channel.getTopicSetter() << "\"" << std::endl;
    std::cout << "getTopicTime: " << channel.getTopicTime() << std::endl;
    
    std::cout << "\n--- Set Topic ---" << std::endl;
    std::cout << "Setting topic 'Welcome to the channel!' by Alice..." << std::endl;
    channel.setTopic("Welcome to the channel!", "Alice");
    
    std::cout << "  hasTopic: " << (channel.hasTopic() ? "true" : "false") << std::endl;
    std::cout << "  getTopic: \"" << channel.getTopic() << "\"" << std::endl;
    std::cout << "  getTopicSetter: \"" << channel.getTopicSetter() << "\"" << std::endl;
    std::cout << "  getTopicTime: " << channel.getTopicTime() << " (unix timestamp)" << std::endl;
    
    std::cout << "\n--- Change Topic ---" << std::endl;
    std::cout << "Changing topic to 'New topic here' by Bob..." << std::endl;
    time_t oldTime = channel.getTopicTime();
    channel.setTopic("New topic here", "Bob");
    
    std::cout << "  getTopic: \"" << channel.getTopic() << "\"" << std::endl;
    std::cout << "  getTopicSetter: \"" << channel.getTopicSetter() << "\"" << std::endl;
    std::cout << "  Time changed: " << (channel.getTopicTime() > oldTime ? "true" : "false") << std::endl;
    
    std::cout << "\n--- Clear Topic ---" << std::endl;
    std::cout << "Setting empty topic..." << std::endl;
    channel.setTopic("", "Charlie");
    std::cout << "  hasTopic: " << (channel.hasTopic() ? "true" : "false") << std::endl;
    std::cout << "  getTopicSetter: \"" << channel.getTopicSetter() << "\" (still set)" << std::endl;
}

void testInvitations()
{
    printSeparator("GESTION DES INVITATIONS");
    
    Channel channel("#invite");
    
    std::cout << "--- Initial State ---" << std::endl;
    std::cout << "isInvited('Alice'): " << (channel.isInvited("Alice") ? "true" : "false") << std::endl;
    
    std::cout << "\n--- Add Invitations ---" << std::endl;
    std::cout << "Adding invitation for Alice..." << std::endl;
    channel.addInvite("Alice");
    std::cout << "  isInvited('Alice'): " << (channel.isInvited("Alice") ? "true" : "false") << std::endl;
    std::cout << "  isInvited('alice'): " << (channel.isInvited("alice") ? "true" : "false") 
              << " (case insensitive)" << std::endl;
    
    std::cout << "\nAdding invitations for Bob and Charlie..." << std::endl;
    channel.addInvite("Bob");
    channel.addInvite("Charlie");
    std::cout << "  isInvited('Bob'): " << (channel.isInvited("Bob") ? "true" : "false") << std::endl;
    std::cout << "  isInvited('Charlie'): " << (channel.isInvited("Charlie") ? "true" : "false") << std::endl;
    std::cout << "  isInvited('Dave'): " << (channel.isInvited("Dave") ? "true" : "false") << std::endl;
    
    std::cout << "\n--- Remove Invitation ---" << std::endl;
    std::cout << "Removing invitation for Bob..." << std::endl;
    channel.removeInvite("Bob");
    std::cout << "  isInvited('Bob'): " << (channel.isInvited("Bob") ? "true" : "false") << std::endl;
    std::cout << "  isInvited('Alice'): " << (channel.isInvited("Alice") ? "true" : "false") 
              << " (still invited)" << std::endl;
    
    std::cout << "\n--- Case Insensitivity Test ---" << std::endl;
    std::cout << "Removing 'CHARLIE' (uppercase)..." << std::endl;
    channel.removeInvite("CHARLIE");
    std::cout << "  isInvited('charlie'): " << (channel.isInvited("charlie") ? "true" : "false") << std::endl;
}

void testAccessors()
{
    printSeparator("GETTERS");
    
    Channel channel("#test");
    
    Client* alice = new Client(1, "host1.com");
    alice->setNickname("Alice");
    
    Client* bob = new Client(2, "host2.com");
    bob->setNickname("Bob");
    
    Client* charlie = new Client(3, "host3.com");
    charlie->setNickname("Charlie");
    
    channel.addMember(alice);  // Becomes op
    channel.addMember(bob);
    channel.addMember(charlie);
    channel.addOperator(charlie);
    
    std::cout << "--- getName ---" << std::endl;
    std::cout << "Channel name: \"" << channel.getName() << "\"" << std::endl;
    
    std::cout << "\n--- getNamesList ---" << std::endl;
    std::cout << "Names list: \"" << channel.getNamesList() << "\"" << std::endl;
    std::cout << "Format: @op1 user1 @op2 user2 ..." << std::endl;
    std::cout << "(@ prefix indicates operators)" << std::endl;
    
    delete alice;
    delete bob;
    delete charlie;
}

void testCompleteScenario()
{
    printSeparator("SCÉNARIO COMPLET D'UTILISATION");
    
    std::cout << "Simulating a complete IRC channel scenario...\n" << std::endl;
    
    // Create channel
    std::cout << "1. Creating channel #lobby" << std::endl;
    Channel channel("#lobby");
    std::cout << "   Channel: " << channel.getName() << std::endl;
    
    // Create clients
    Client* alice = new Client(1, "192.168.1.10");
    alice->setNickname("Alice");
    
    Client* bob = new Client(2, "192.168.1.20");
    bob->setNickname("Bob");
    
    Client* charlie = new Client(3, "192.168.1.30");
    charlie->setNickname("Charlie");
    
    // Alice joins first
    std::cout << "\n2. Alice joins the channel (becomes operator)" << std::endl;
    channel.addMember(alice);
    std::cout << "   Members: " << channel.getMemberCount() << std::endl;
    std::cout << "   Alice is operator: " << (channel.isOperator(alice) ? "✓" : "✗") << std::endl;
    
    // Set channel modes
    std::cout << "\n3. Alice sets channel modes: +itk secret123" << std::endl;
    channel.setInviteOnly(true);
    channel.setTopicRestricted(true);
    channel.setKey("secret123");
    std::cout << "   Mode string: " << channel.getModeStringWithParams() << std::endl;
    
    // Set topic
    std::cout << "\n4. Alice sets channel topic" << std::endl;
    channel.setTopic("Welcome to #lobby!", "Alice");
    std::cout << "   Topic: \"" << channel.getTopic() << "\"" << std::endl;
    std::cout << "   Set by: " << channel.getTopicSetter() << std::endl;
    
    // Alice invites Bob
    std::cout << "\n5. Alice invites Bob to the channel" << std::endl;
    channel.addInvite("Bob");
    std::cout << "   Bob is invited: " << (channel.isInvited("Bob") ? "✓" : "✗") << std::endl;
    
    // Bob joins
    std::cout << "\n6. Bob joins the channel" << std::endl;
    channel.addMember(bob);
    std::cout << "   Members: " << channel.getMemberCount() << std::endl;
    std::cout << "   Names list: " << channel.getNamesList() << std::endl;
    
    // Alice gives Bob operator status
    std::cout << "\n7. Alice gives Bob operator status" << std::endl;
    channel.addOperator(bob);
    std::cout << "   Bob is operator: " << (channel.isOperator(bob) ? "✓" : "✗") << std::endl;
    std::cout << "   Names list: " << channel.getNamesList() << std::endl;
    
    // Charlie tries to join (invite-only)
    std::cout << "\n8. Charlie tries to join (channel is invite-only)" << std::endl;
    std::cout << "   Charlie is invited: " << (channel.isInvited("Charlie") ? "✓" : "✗") << std::endl;
    std::cout << "   Channel is invite-only: " << (channel.isInviteOnly() ? "✓" : "✗") << std::endl;
    
    // Bob invites Charlie
    std::cout << "\n9. Bob invites Charlie" << std::endl;
    channel.addInvite("Charlie");
    channel.addMember(charlie);
    std::cout << "   Members: " << channel.getMemberCount() << std::endl;
    std::cout << "   Names list: " << channel.getNamesList() << std::endl;
    
    // Set user limit
    std::cout << "\n10. Setting user limit to 3" << std::endl;
    channel.setUserLimit(3);
    std::cout << "   User limit: " << channel.getUserLimit() << std::endl;
    std::cout << "   Channel is full: " << (channel.isFull() ? "✓" : "✗") << std::endl;
    
    // Bob leaves
    std::cout << "\n11. Bob leaves the channel" << std::endl;
    channel.removeMember(bob);
    std::cout << "   Members: " << channel.getMemberCount() << std::endl;
    std::cout << "   Channel is full: " << (channel.isFull() ? "✓" : "✗") << std::endl;
    std::cout << "   Names list: " << channel.getNamesList() << std::endl;
    
    // Display final state
    std::cout << "\n12. Final channel state:" << std::endl;
    std::cout << "   Name: " << channel.getName() << std::endl;
    std::cout << "   Members: " << channel.getMemberCount() << std::endl;
    std::cout << "   Modes: " << channel.getModeStringWithParams() << std::endl;
    std::cout << "   Topic: \"" << channel.getTopic() << "\"" << std::endl;
    std::cout << "   Is empty: " << (channel.isEmpty() ? "yes" : "no") << std::endl;
    
    // Cleanup
    delete alice;
    delete bob;
    delete charlie;
}

int main()
{
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║             CHANNEL CLASS TEST SUITE                       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    
    try
    {
        testConstructorDestructor();
        testMemberManagement();
        testOperatorManagement();
        testModes();
        testTopic();
        testInvitations();
        testAccessors();
        testCompleteScenario();
        
        std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║             ALL TESTS COMPLETED                            ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════╝\n" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}