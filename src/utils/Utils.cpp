#include "IRC.hpp"

namespace Utils
{

/* ========================================================================== */
/*                              CHAIN HANDLING                                */
/* ========================================================================== */

//The trim function removes leading and trailing whitespace characters (spaces, tabs, newlines) from a string.
std::string trim(const std::string& str) 
{
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));    
}

//First split function: Splits a string into substrings using a single character as delimiter and returns them in a vector.
std::vector<std::string> split(const std::string& str, char delimiter) 
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) 
    {
        tokens.push_back(token);
    }
    return tokens;
}

//Second split function: Splits a string into substrings using a multi-character string as delimiter and returns them in a vector.
std::vector<std::string> split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> parts;
    size_t pos = 0;
    size_t prevPos = 0;
        while ((pos = str.find(delimiter, prevPos)) != std::string::npos)
    {
        parts.push_back(str.substr(prevPos, pos - prevPos));
        prevPos = pos + delimiter.length();
    }
     if (prevPos < str.length())
        parts.push_back(str.substr(prevPos));
     return parts;
}

//toLower: Converts all characters in a string to lowercase and returns the result.
std::string toLower(const std::string& str)
{
     std::string result = str;
     for (size_t i = 0; i < result.length(); ++i)
         result[i] = std::tolower(result[i]);
     return result;

}

//toUpper: Converts all characters in a string to uppercase and returns the result.
std::string toUpper(const std::string& str)
{
	std::string result = str;
	for (size_t i = 0; i < result.length(); ++i)
	    result[i] = std::toupper(result[i]);
	return result;
}

//startsWith: Checks if a string starts with a given prefix.
bool startsWith(const std::string& str, const std::string& prefix)
{
	 if (prefix.length() > str.length())
	     return false;
	 return str.compare(0, prefix.length(), prefix) == 0;
	
}

//endsWith: Checks if a string ends with a given suffix.
bool endsWith(const std::string& str, const std::string& suffix)
{    
    if (suffix.length() > str.length())
        return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0; 
}

//replaceAll: Replaces all occurrences of a substring with another substring in a given string.
std::string replaceAll(const std::string& str, const std::string& from, const std::string& to)
{
	std::string result = str;
	size_t pos = 0;
	while ((pos = result.find(from, pos)) != std::string::npos)
	{
	    result.replace(pos, from.length(), to);
	    pos += to.length();
	}
	return result;
}
















} // namespace Utils
