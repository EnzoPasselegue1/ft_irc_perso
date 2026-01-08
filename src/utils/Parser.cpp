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
{std::vector<std::string> params;
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







}