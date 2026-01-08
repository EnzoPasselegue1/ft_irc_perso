#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
# include <vector>

namespace parser {

/* ========================================================================== */
/*                          MESSAGE PARSER IRC                                */
/* ========================================================================== */
std::string extractPrefix(const std::string& message);
std::string extractCommand(const std::string& message);
std::vector<std::string> extractParams(const std::string& message);
std::string extractTrailing(const std::string& message);


/* ========================================================================== */
/*                    PARSING DES LISTES                                      */
/* ========================================================================== */
std::vector<std::string> parseChannelList(const std::string& channelList);
std::vector<std::string> parseKeyList(const std::string& keyList);
std::vector<std::string> parseTargetList(const std::string& targetList);

/* ========================================================================== */
/*                              PARSING MODE                                  */
/* ========================================================================== */
struct ModeChange
	{
		bool        adding;     // true for +, false for -
		char        mode;       // Mode character (i, t, k, o, l)
		std::string param;      // Parameter if necessary (for k, o, l)
	};
    std::vector<ModeChange> parseModeString(const std::string& modeString,
        const std::vector<std::string>& params);

/* ========================================================================== */
/*                          VALIDATION                                        */
/* ========================================================================== */
bool isValidChannelName(const std::string& name);
bool isValidNickname(const std::string& nickname);
bool isValidMode(char mode);
bool modeNeedsParam(char mode, bool adding);
        
/* ========================================================================== */
/*                       PREFIXE PARSING                                      */
/* ========================================================================== */
std::string getNickFromPrefix(const std::string& prefix);
std::string getUserFromPrefix(const std::string& prefix);
std::string getHostFromPrefix(const std::string& prefix);
}
#endif