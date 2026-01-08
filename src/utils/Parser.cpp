#include "IRC.hpp"

namespace parser {
/* ========================================================================== */
/*                          MESSAGE PARSER IRC                                */
/* ========================================================================== */

// Extracts the prefix from an IRC message.
std::string extractPrefix(const std::string& message)
{
	if (message.empty() || message[0] != ':')
	    return "";

	size_t spacePos = message.find(' ');
	if (spacePos == std::string::npos)
	    return message.substr(1);

	return message.substr(1, spacePos - 1);
}

// Extracts the command from an IRC message.
std::string extractCommand(const std::string& message)
{
	std::string line = message;

	if (!line.empty() && line[0] == ':')
	{
	    size_t spacePos = line.find(' ');
	    if (spacePos == std::string::npos)
	        return "";
	    line = line.substr(spacePos + 1);
	}

	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos)
	    return Utils::toUpper(line);

	return Utils::toUpper(line.substr(0, spacePos));
}

// Extracts the parameters from an IRC message.
std::vector<std::string> extractParams(const std::string& message)
{
    std::vector<std::string> params;
	std::string line = message;

	if (!line.empty() && line[0] == ':')
	{
	    size_t spacePos = line.find(' ');
	    if (spacePos == std::string::npos)
	        return params;
	    line = line.substr(spacePos + 1);
	}

	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos)
	    return params;
	line = line.substr(spacePos + 1);

	std::string trailing = "";
	size_t trailingPos = line.find(" :");
	if (trailingPos != std::string::npos)
	{
	    trailing = line.substr(trailingPos + 2);
	    line = line.substr(0, trailingPos);
	}

	params = Utils::split(line, ' ');

	if (!trailing.empty())
	    params.push_back(trailing);

	return params;
}

// Extracts the trailing part from an IRC message.
std::string extractTrailing(const std::string& message)
{
    size_t pos = message.find(" :");
    if (pos != std::string::npos && pos > 0)
        return message.substr(pos + 2);
    return "";
}

/* ========================================================================== */
/*                    PARSING DES LISTES                                      */
/* ========================================================================== */

// Splits a comma-separated list of channels into a vector of strings.
std::vector<std::string> parseChannelList(const std::string& channelList)
{
    return Utils::split(channelList, ',');
}

// Splits a comma-separated list of keys into a vector of strings.
std::vector<std::string> parseKeyList(const std::string& keyList)
{
	std::vector<std::string> keys;
	std::stringstream ss(keyList);
	std::string key;
	while (std::getline(ss, key, ','))
	    keys.push_back(key);
	return keys;
}


// Splits a comma-separated list of targets into a vector of strings.
std::vector<std::string> parseTargetList(const std::string& targetList)
{
	return Utils::split(targetList, ',');
}

/* ========================================================================== */
/*                              PARSING MODE                                  */
/* ========================================================================== */

// Parses a mode string and its parameters into a vector of ModeChange structures.
std::vector<ModeChange> parseModeString(const std::string& modeString,
										const std::vector<std::string>& params)
{
	std::vector<ModeChange> changes;
	bool adding = true;
	size_t paramIndex = 0;

	for (size_t i = 0; i < modeString.length(); ++i)
	{
	    char c = modeString[i];

	    if (c == '+')
	    {
	        adding = true;
	        continue;
	    }
	    if (c == '-')
	    {
	        adding = false;
	        continue;
	    }

	    ModeChange change;
	    change.adding = adding;
	    change.mode = c;
	    change.param = "";

	    if (modeNeedsParam(c, adding) && paramIndex < params.size())
	    {
	        change.param = params[paramIndex++];
	    }

	    changes.push_back(change);
	}
	return changes;
}

/* ========================================================================== */
/*                    VALIDATION                                              */
/* ========================================================================== */

// Validates a channel name according to IRC rules.
bool isValidChannelName(const std::string& name)
{
	if (name.empty() || name.length() > MAX_CHANNEL_LENGTH)
	    return false;

	if (name[0] != '#' && name[0] != '&')
	    return false;

	for (size_t i = 1; i < name.length(); ++i)
	{
	    char c = name[i];
	    if (c == ' ' || c == ',' || c == '\x07' || c == '\0' || c == '\r' || c == '\n')
	        return false;
	}
	return true;
	(void)name;
	return false;
}

// Validates a nickname according to IRC rules.
bool isValidNickname(const std::string& nickname)
{
	if (nickname.empty() || nickname.length() > MAX_NICK_LENGTH)
	    return false;

	char first = nickname[0];
	if (!std::isalpha(first) &&
	    first != '[' && first != ']' && first != '\\' &&
	    first != '`' && first != '^' && first != '{' &&
	    first != '}' && first != '_')
	    return false;

	for (size_t i = 1; i < nickname.length(); ++i)
	{
	    char c = nickname[i];
	    if (!std::isalnum(c) &&
	        c != '[' && c != ']' && c != '\\' &&
	        c != '`' && c != '^' && c != '{' &&
	        c != '}' && c != '-' && c != '_')
	        return false;
	}

	return true;
}

// Validates if a mode character is recognized.
bool isValidMode(char mode)
{
	return (mode == 'i' || mode == 't' || mode == 'k' ||
	        mode == 'o' || mode == 'l');
}

// Determines if a mode change requires a parameter.
bool modeNeedsParam(char mode, bool adding)
{
	switch (mode)
	{
	    case 'k':  // Key always needs a param for +
	        return adding;
	    case 'o':  // Operator always needs a param
	        return true;
	    case 'l':  // Limit only for +
	        return adding;
	    default:
	        return false;
	}

}
}