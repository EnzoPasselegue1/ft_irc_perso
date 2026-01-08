#include "Utils.hpp"
#include <iostream>
#include <cassert>

using namespace Utils;

void testStringHandling()
{
    std::cout << "=== Testing String Handling ===" << std::endl;
    
    // Test trim
    std::cout << "trim(\"  hello  \") = \"" << trim("  hello  ") << "\"" << std::endl;
    std::cout << "trim(\"\\t\\ntest\\n\") = \"" << trim("\t\ntest\n") << "\"" << std::endl;
    
    // Test split (char delimiter)
    std::vector<std::string> tokens = split("one,two,three", ',');
    std::cout << "split(\"one,two,three\", ',') = ";
    for (size_t i = 0; i < tokens.size(); ++i)
        std::cout << "\"" << tokens[i] << "\" ";
    std::cout << std::endl;
    
    // Test split (string delimiter)
    tokens = split("one::two::three", "::");
    std::cout << "split(\"one::two::three\", \"::\") = ";
    for (size_t i = 0; i < tokens.size(); ++i)
        std::cout << "\"" << tokens[i] << "\" ";
    std::cout << std::endl;
    
    // Test toLower
    std::cout << "toLower(\"HeLLo WoRLd\") = \"" << toLower("HeLLo WoRLd") << "\"" << std::endl;
    
    // Test toUpper
    std::cout << "toUpper(\"HeLLo WoRLd\") = \"" << toUpper("HeLLo WoRLd") << "\"" << std::endl;
    
    // Test startsWith
    std::cout << "startsWith(\"hello world\", \"hello\") = " 
              << (startsWith("hello world", "hello") ? "true" : "false") << std::endl;
    std::cout << "startsWith(\"hello world\", \"world\") = " 
              << (startsWith("hello world", "world") ? "true" : "false") << std::endl;
    
    // Test endsWith
    std::cout << "endsWith(\"hello world\", \"world\") = " 
              << (endsWith("hello world", "world") ? "true" : "false") << std::endl;
    std::cout << "endsWith(\"hello world\", \"hello\") = " 
              << (endsWith("hello world", "hello") ? "true" : "false") << std::endl;
    
    // Test replaceAll
    std::cout << "replaceAll(\"hello world world\", \"world\", \"universe\") = \"" 
              << replaceAll("hello world world", "world", "universe") << "\"" << std::endl;
    
    std::cout << std::endl;
}

void testTypeConversion()
{
    std::cout << "=== Testing Type Conversion ===" << std::endl;
    
    // Test stringToInt
    std::cout << "stringToInt(\"42\") = " << stringToInt("42") << std::endl;
    std::cout << "stringToInt(\"-123\") = " << stringToInt("-123") << std::endl;
    
    // Test intToString
    std::cout << "intToString(42) = \"" << intToString(42) << "\"" << std::endl;
    std::cout << "intToString(-123) = \"" << intToString(-123) << "\"" << std::endl;
    
    // Test isNumber
    std::cout << "isNumber(\"123\") = " << (isNumber("123") ? "true" : "false") << std::endl;
    std::cout << "isNumber(\"-456\") = " << (isNumber("-456") ? "true" : "false") << std::endl;
    std::cout << "isNumber(\"abc\") = " << (isNumber("abc") ? "true" : "false") << std::endl;
    std::cout << "isNumber(\"12a\") = " << (isNumber("12a") ? "true" : "false") << std::endl;
    
    // Test isPositiveNumber
    std::cout << "isPositiveNumber(\"123\") = " << (isPositiveNumber("123") ? "true" : "false") << std::endl;
    std::cout << "isPositiveNumber(\"-456\") = " << (isPositiveNumber("-456") ? "true" : "false") << std::endl;
    std::cout << "isPositiveNumber(\"0\") = " << (isPositiveNumber("0") ? "true" : "false") << std::endl;
    
    std::cout << std::endl;
}

void testIRCFormat()
{
    std::cout << "=== Testing IRC Format ===" << std::endl;
    
    // Test formatServerReply
    std::string reply = formatServerReply("irc.server.com", "001", "nickname", "Welcome to IRC");
    std::cout << "formatServerReply() = \"" << reply << "\"" << std::endl;
    
    // Test formatUserMessage
    std::string msg = formatUserMessage("nick!user@host", "PRIVMSG", "#channel :Hello everyone");
    std::cout << "formatUserMessage() = \"" << msg << "\"" << std::endl;
    
    std::cout << std::endl;
}

void testMiscellaneous()
{
    std::cout << "=== Testing Miscellaneous ===" << std::endl;
    
    // Test getCurrentTimestamp
    std::cout << "getCurrentTimestamp() = \"" << getCurrentTimestamp() << "\"" << std::endl;
    
    // Test getUnixTimestamp
    long timestamp = getUnixTimestamp();
    std::cout << "getUnixTimestamp() = " << timestamp << std::endl;
    
    // Test timestampToString
    std::cout << "timestampToString(" << timestamp << ") = \"" 
              << timestampToString(timestamp) << "\"" << std::endl;
    
    std::cout << std::endl;
}

int main()
{
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║     UTILS LIBRARY TEST SUITE          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝\n" << std::endl;
    
    try
    {
        testStringHandling();
        testTypeConversion();
        testIRCFormat();
        testMiscellaneous();
        
        std::cout << "╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║     ALL TESTS COMPLETED               ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}