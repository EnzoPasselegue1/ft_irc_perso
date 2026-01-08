#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
# include <vector>

namespace parser {
std::string extractPrefix(const std::string& message);
std::string extractCommand(const std::string& message);
std::vector<std::string> extractParams(const std::string& message);
std::string extractTrailing(const std::string& message);

}


#endif