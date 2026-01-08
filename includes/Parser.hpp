#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
# include <vector>

namespace parser {
std::string extractPrefix(const std::string& message);
std::string extractCommand(const std::string& message);
std::vector<std::string> extractParams(const std::string& message);
std::string extractTrailing(const std::string& message);


struct ModeChange
	{
		bool        adding;     // true for +, false for -
		char        mode;       // Mode character (i, t, k, o, l)
		std::string param;      // Parameter if necessary (for k, o, l)
	};
}


#endif