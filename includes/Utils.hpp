#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <sstream>

namespace Utils
{
/* ========================================================================== */
/*                              CHAIN HANDLING                                */
/* ========================================================================== */
    
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

/* ========================================================================== */
/*                              TYPES CONVERSION                              */
/* ========================================================================== */

    int stringToInt(const std::string& str);
    std::string intToString(int number);
    bool isNumber(const std::string& str);
    bool isPositiveNumber(const std::string& str);

/* ========================================================================== */
/*                              IRC FORMAT                                    */
/* ========================================================================== */

    std::string formatServerReply(const std::string& serverName,
                                    const std::string& code,
                                    const std::string& target,
                                    const std::string& message);
    std::string formatUserMessage(const std::string& prefix,
                                    const std::string& command,
                                    const std::string& params);

/* ========================================================================== */
/*                    UTILITAIRES DIVERS                                      */
/* ========================================================================== */

    std::string getCurrentTimestamp();
    long getUnixTimestamp();
    std::string timestampToString(long timestamp);
}

#endif