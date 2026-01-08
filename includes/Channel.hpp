#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

class Client;
class Channel
{
private:
        /* ================================================================== */
		/*                    ATTRIBUTS PRIVÉS                                */
		/* ================================================================== */
		
		// Identification
		std::string         _name;
		
		// Topic
		std::string         _topic;
		std::string         _topicSetter;
		time_t              _topicTime;
		
		// Modes
		bool                _inviteOnly;
		bool                _topicRestricted;
		std::string         _key;
		size_t              _userLimit;
		
		// Membres et opérateurs
		std::set<Client*>   _members;
		std::set<Client*>   _operators;
		
		// Invitations
		std::set<std::string> _invitedUsers;

		/* ================================================================== */
		/*                CONSTRUCTEURS INTERDITS                             */
		/* ================================================================== */
		
		Channel();
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
    
public:
    Channel(const std::string& name);
    ~Channel();

    /* ========================================================================== */
    /*                    GESTION DES MEMBRES                                     */
    /* ========================================================================== */

    bool addMember(Client* client);
    void removeMember(Client* client);
    bool isMember(Client* client) const;
    bool hasMember(const std::string& nickname) const;
    Client* getMemberByNickname(const std::string& nickname) const;
    const std::set<Client*>& getMembers() const;
    size_t getMemberCount() const;
    bool isEmpty() const;

    /* ========================================================================== */
    /*                    GESTION DES OPÉRATEURS                                  */
    /* ========================================================================== */
    bool addOperator(Client* client);
    void removeOperator(Client* client);
    bool isOperator(Client* client) const;
    bool isOperator(const std::string& nickname) const;

    /* ========================================================================== */
    /*                    GESTION DES MODES                                       */
    /* ========================================================================== */
    void setInviteOnly(bool enabled);
    bool isInviteOnly() const;

    void setTopicRestricted(bool enabled);
    bool isTopicRestricted() const;

    void setKey(const std::string& key);
    const std::string& getKey() const;
    bool hasKey() const;
    bool checkKey(const std::string& key) const;

    void setUserLimit(size_t limit);
    size_t getUserLimit() const;
    bool hasUserLimit() const;
    bool isFull() const;
    std::string getModeString() const;
    std::string getModeStringWithParams() const;

};

#endif