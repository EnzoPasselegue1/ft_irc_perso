#include "IRC.hpp"
#include <iostream>
#include <iomanip>

using namespace parser;

void printSeparator(const std::string& title)
{
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  " << std::left << std::setw(56) << title << "  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝\n" << std::endl;
}

void testMessageParser()
{
    printSeparator("MESSAGE PARSER IRC");
    
    // Test messages
    std::string msg1 = ":nick!user@host PRIVMSG #channel :Hello world";
    std::string msg2 = "PING :server.name";
    std::string msg3 = ":server 001 nickname :Welcome to IRC";
    std::string msg4 = "JOIN #channel";
    
    // Test extractPrefix
    std::cout << "--- extractPrefix ---" << std::endl;
    std::cout << "Message: \"" << msg1 << "\"" << std::endl;
    std::cout << "Prefix: \"" << extractPrefix(msg1) << "\"" << std::endl;
    std::cout << "\nMessage: \"" << msg2 << "\"" << std::endl;
    std::cout << "Prefix: \"" << extractPrefix(msg2) << "\"" << std::endl;
    std::cout << "\nMessage: \"" << msg4 << "\"" << std::endl;
    std::cout << "Prefix: \"" << extractPrefix(msg4) << "\"" << std::endl;
    
    // Test extractCommand
    std::cout << "\n--- extractCommand ---" << std::endl;
    std::cout << "Message: \"" << msg1 << "\"" << std::endl;
    std::cout << "Command: \"" << extractCommand(msg1) << "\"" << std::endl;
    std::cout << "\nMessage: \"" << msg2 << "\"" << std::endl;
    std::cout << "Command: \"" << extractCommand(msg2) << "\"" << std::endl;
    std::cout << "\nMessage: \"" << msg4 << "\"" << std::endl;
    std::cout << "Command: \"" << extractCommand(msg4) << "\"" << std::endl;
    
    // Test extractParams
    std::cout << "\n--- extractParams ---" << std::endl;
    std::cout << "Message: \"" << msg1 << "\"" << std::endl;
    std::vector<std::string> params1 = extractParams(msg1);
    std::cout << "Params (" << params1.size() << "): ";
    for (size_t i = 0; i < params1.size(); ++i)
        std::cout << "\"" << params1[i] << "\" ";
    std::cout << std::endl;
    
    std::cout << "\nMessage: \"" << msg3 << "\"" << std::endl;
    std::vector<std::string> params3 = extractParams(msg3);
    std::cout << "Params (" << params3.size() << "): ";
    for (size_t i = 0; i < params3.size(); ++i)
        std::cout << "\"" << params3[i] << "\" ";
    std::cout << std::endl;
    
    // Test extractTrailing
    std::cout << "\n--- extractTrailing ---" << std::endl;
    std::cout << "Message: \"" << msg1 << "\"" << std::endl;
    std::cout << "Trailing: \"" << extractTrailing(msg1) << "\"" << std::endl;
    std::cout << "\nMessage: \"" << msg2 << "\"" << std::endl;
    std::cout << "Trailing: \"" << extractTrailing(msg2) << "\"" << std::endl;
}

void testListParsing()
{
    printSeparator("PARSING DES LISTES");
    
    // Test parseChannelList
    std::cout << "--- parseChannelList ---" << std::endl;
    std::string channels = "#channel1,#channel2,#channel3";
    std::cout << "Input: \"" << channels << "\"" << std::endl;
    std::vector<std::string> channelList = parseChannelList(channels);
    std::cout << "Parsed (" << channelList.size() << "): ";
    for (size_t i = 0; i < channelList.size(); ++i)
        std::cout << "\"" << channelList[i] << "\" ";
    std::cout << std::endl;
    
    // Test parseKeyList
    std::cout << "\n--- parseKeyList ---" << std::endl;
    std::string keys = "key1,key2,key3";
    std::cout << "Input: \"" << keys << "\"" << std::endl;
    std::vector<std::string> keyList = parseKeyList(keys);
    std::cout << "Parsed (" << keyList.size() << "): ";
    for (size_t i = 0; i < keyList.size(); ++i)
        std::cout << "\"" << keyList[i] << "\" ";
    std::cout << std::endl;
    
    // Test parseTargetList
    std::cout << "\n--- parseTargetList ---" << std::endl;
    std::string targets = "user1,user2,#channel";
    std::cout << "Input: \"" << targets << "\"" << std::endl;
    std::vector<std::string> targetList = parseTargetList(targets);
    std::cout << "Parsed (" << targetList.size() << "): ";
    for (size_t i = 0; i < targetList.size(); ++i)
        std::cout << "\"" << targetList[i] << "\" ";
    std::cout << std::endl;
}

void testModeParsing()
{
    printSeparator("PARSING MODE");
    
    std::cout << "--- parseModeString ---" << std::endl;
    
    // Test 1: Simple mode changes
    std::string modeStr1 = "+it-k";
    std::vector<std::string> params1;
    params1.push_back("oldkey");
    
    std::cout << "Mode: \"" << modeStr1 << "\"" << std::endl;
    std::cout << "Params: ";
    for (size_t i = 0; i < params1.size(); ++i)
        std::cout << "\"" << params1[i] << "\" ";
    std::cout << std::endl;
    
    std::vector<ModeChange> changes1 = parseModeString(modeStr1, params1);
    std::cout << "Parsed changes (" << changes1.size() << "):" << std::endl;
    for (size_t i = 0; i < changes1.size(); ++i)
    {
        std::cout << "  " << (changes1[i].adding ? "+" : "-") << changes1[i].mode;
        if (!changes1[i].param.empty())
            std::cout << " (param: \"" << changes1[i].param << "\")";
        std::cout << std::endl;
    }
    
    // Test 2: Mode with operators
    std::cout << "\nMode: \"+o-o+l\"" << std::endl;
    std::vector<std::string> params2;
    params2.push_back("user1");
    params2.push_back("user2");
    params2.push_back("10");
    std::cout << "Params: ";
    for (size_t i = 0; i < params2.size(); ++i)
        std::cout << "\"" << params2[i] << "\" ";
    std::cout << std::endl;
    
    std::vector<ModeChange> changes2 = parseModeString("+o-o+l", params2);
    std::cout << "Parsed changes (" << changes2.size() << "):" << std::endl;
    for (size_t i = 0; i < changes2.size(); ++i)
    {
        std::cout << "  " << (changes2[i].adding ? "+" : "-") << changes2[i].mode;
        if (!changes2[i].param.empty())
            std::cout << " (param: \"" << changes2[i].param << "\")";
        std::cout << std::endl;
    }
}

void testValidation()
{
    printSeparator("VALIDATION");
    
    // Test isValidChannelName
    std::cout << "--- isValidChannelName ---" << std::endl;
    std::string testChannels[] = {
        "#channel",
        "&localchan",
        "#valid_channel",
        "invalid",
        "#with space",
        "#with,comma",
        ""
    };
    
    for (size_t i = 0; i < 7; ++i)
    {
        std::cout << "\"" << testChannels[i] << "\" -> " 
                  << (isValidChannelName(testChannels[i]) ? "VALID" : "INVALID") 
                  << std::endl;
    }
    
    // Test isValidNickname
    std::cout << "\n--- isValidNickname ---" << std::endl;
    std::string testNicks[] = {
        "ValidNick",
        "nick123",
        "nick_name",
        "[special]",
        "1invalid",
        "nick with space",
        "a",
        ""
    };
    
    for (size_t i = 0; i < 8; ++i)
    {
        std::cout << "\"" << testNicks[i] << "\" -> " 
                  << (isValidNickname(testNicks[i]) ? "VALID" : "INVALID") 
                  << std::endl;
    }
    
    // Test isValidMode
    std::cout << "\n--- isValidMode ---" << std::endl;
    char testModes[] = {'i', 't', 'k', 'o', 'l', 'x', 'z'};
    for (size_t i = 0; i < 7; ++i)
    {
        std::cout << "'" << testModes[i] << "' -> " 
                  << (isValidMode(testModes[i]) ? "VALID" : "INVALID") 
                  << std::endl;
    }
    
    // Test modeNeedsParam
    std::cout << "\n--- modeNeedsParam ---" << std::endl;
    char paramModes[] = {'k', 'o', 'l', 'i', 't'};
    for (size_t i = 0; i < 5; ++i)
    {
        std::cout << "'" << paramModes[i] << "' adding=true -> " 
                  << (modeNeedsParam(paramModes[i], true) ? "NEEDS PARAM" : "NO PARAM") 
                  << std::endl;
        std::cout << "'" << paramModes[i] << "' adding=false -> " 
                  << (modeNeedsParam(paramModes[i], false) ? "NEEDS PARAM" : "NO PARAM") 
                  << std::endl;
    }
}

void testPrefixParsing()
{
    printSeparator("PARSING DU PRÉFIXE");
    
    std::string testPrefixes[] = {
        "nick!user@host.com",
        "nickname",
        "server.name",
        "bot!~bot@192.168.1.1"
    };
    
    for (size_t i = 0; i < 4; ++i)
    {
        std::cout << "Prefix: \"" << testPrefixes[i] << "\"" << std::endl;
        std::cout << "  Nick: \"" << getNickFromPrefix(testPrefixes[i]) << "\"" << std::endl;
        std::cout << "  User: \"" << getUserFromPrefix(testPrefixes[i]) << "\"" << std::endl;
        std::cout << "  Host: \"" << getHostFromPrefix(testPrefixes[i]) << "\"" << std::endl;
        std::cout << std::endl;
    }
}

int main()
{
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           IRC PARSER TEST SUITE                            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    
    try
    {
        testMessageParser();
        testListParsing();
        testModeParsing();
        testValidation();
        testPrefixParsing();
        
        std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║           ALL TESTS COMPLETED                              ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════╝\n" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}