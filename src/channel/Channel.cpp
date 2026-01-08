#include "IRC.hpp"

/* ========================================================================== */
/*                    CONSTRUCTEUR / DESTRUCTEUR                              */
/* ========================================================================== */

// Constructor initializing the channel with a name and default values for other attributes.
Channel::Channel(const std::string& name)
	: _name(name),
	  _topic(""),
	  _topicSetter(""),
	  _topicTime(0),
	  _inviteOnly(false),
	  _topicRestricted(false),
	  _key(""),
	  _userLimit(0)
      {}

Channel::~Channel() {}

/* ========================================================================== */
/*                    GESTION DES MEMBRES                                     */
/* ========================================================================== */

// Add a member to the channel. Returns true if the member was added, false if they were already a member.
bool Channel::addMember(Client* client)
{
	if (isMember(client))
	    return false;

	_members.insert(client);

	if (_members.size() == 1)
	    addOperator(client);

	return true;
}

// Remove a member from the channel.
void Channel::removeMember(Client* client)
{
	_members.erase(client);
	_operators.erase(client);

}

// Check if a client is a member of the channel.
bool Channel::isMember(Client* client) const
{
	return _members.find(client) != _members.end();
}

// Check if a member with the given nickname is in the channel.
bool Channel::hasMember(const std::string& nickname) const
{
	return (getMemberByNickname(nickname) != NULL);
}

// Get a member by their nickname. Returns NULL if not found.
Client* Channel::getMemberByNickname(const std::string &nickname) const
{
	std::string lowerNick = Utils::toLower(nickname);
	for (std::set<Client*>::iterator it = _members.begin();
	     it != _members.end(); ++it)
	{
	    if (Utils::toLower((*it)->getNickname()) == lowerNick)
	        return *it;
	}
	return NULL;
}

// Get the set of members in the channel.
const std::set<Client*>& Channel::getMembers() const
{
	return _members;
}

// Get the number of members in the channel.
size_t Channel::getMemberCount() const
{
	return _members.size();
}

// Check if the channel is empty.
bool Channel::isEmpty() const
{
	return _members.empty();
}

/* ========================================================================== */
/*                    GESTION DES OPÉRATEURS                                  */
/* ========================================================================== */

// Add an operator to the channel. Returns true if the operator was added, false if the client is not a member.
bool Channel::addOperator(Client* client)
{
	if (isMember(client))
	{
		_operators.insert(client);
		return true;
	}
	return false;
}

// Remove an operator from the channel.
void Channel::removeOperator(Client* client)
{
	_operators.erase(client);
}

// Check if a client is an operator of the channel.
bool Channel::isOperator(Client* client) const
{
	return _operators.find(client) != _operators.end();
}

// Check if a nickname belongs to an operator of the channel.
bool Channel::isOperator(const std::string& nickname) const
{
	std::string lowerNick = Utils::toLower(nickname);
	for (std::set<Client*>::const_iterator it = _operators.begin();
	     it != _operators.end(); ++it)
	{
	    if (Utils::toLower((*it)->getNickname()) == lowerNick)
	        return true;
	}
	return false;
}

/* ========================================================================== */
/*                    GESTION DES MODES                                       */
/* ========================================================================== */

/* --- Mode i : Invite-only --- */

void Channel::setInviteOnly(bool enabled)
{
	_inviteOnly = enabled;
}

bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

/* --- Mode t : Topic protégé --- */

void Channel::setTopicRestricted(bool enabled)
{
	_topicRestricted = enabled;
}

bool Channel::isTopicRestricted() const
{
	return _topicRestricted;
}

/* --- Mode k : Channel key (password) --- */

void Channel::setKey(const std::string& key)
{
	_key = key;
}

const std::string& Channel::getKey() const
{
	return _key;
}

bool Channel::hasKey() const
{
	return !_key.empty();
}

bool Channel::checkKey(const std::string& key) const
{
	return _key == key;
}

/* --- Mode l : Limite d'utilisateurs --- */

void Channel::setUserLimit(size_t limit)
{
	_userLimit = limit;
}

size_t Channel::getUserLimit() const
{
	return _userLimit;
}

bool Channel::hasUserLimit() const
{
	return _userLimit > 0;
}

bool Channel::isFull() const
{
	if (!hasUserLimit())
	    return false;
	return _members.size() >= _userLimit;
}

/* --- Génération de la chaîne de modes --- */

std::string Channel::getModeString() const
{
	std::string modes = "+";

	if (_inviteOnly)
	    modes += "i";
	if (_topicRestricted)
	    modes += "t";
	if (hasKey())
	    modes += "k";
	if (hasUserLimit())
	    modes += "l";

	if (modes == "+")
	    return "";
	return modes;
}

std::string Channel::getModeStringWithParams() const
{
	std::string modes = getModeString();
	std::string params = "";

	if (hasKey())
	    params += " " + _key;
	if (hasUserLimit())
	    params += " " + Utils::intToString(_userLimit);

	return modes + params;
}

/* ========================================================================== */
/*                    GESTION DU TOPIC                                        */
/* ========================================================================== */

// Set the topic of the channel along with the setter's nickname and the current timestamp.
void Channel::setTopic(const std::string& topic, const std::string& setterNick)
{

	_topic = topic;
	_topicSetter = setterNick;
	_topicTime = std::time(NULL);  // Timestamp actuel

}

// Get the current topic of the channel.
const std::string& Channel::getTopic() const
{
	return _topic;
}

// Check if the channel has a topic set.
bool Channel::hasTopic() const
{
	return !_topic.empty();
}

// Get the nickname of the user who set the topic.
const std::string& Channel::getTopicSetter() const
{
	return _topicSetter;
}

// Get the timestamp of when the topic was set.
time_t Channel::getTopicTime() const
{
	return _topicTime;
}

/* ========================================================================== */
/*                    GESTION DES INVITATIONS                                 */
/* ========================================================================== */

void Channel::addInvite(const std::string& nickname)
{
	_invitedUsers.insert(Utils::toLower(nickname));
}

void Channel::removeInvite(const std::string& nickname)
{
	_invitedUsers.erase(Utils::toLower(nickname));
}

bool Channel::isInvited(const std::string& nickname) const
{
	return _invitedUsers.find(Utils::toLower(nickname)) != _invitedUsers.end();
}

/* ========================================================================== */
/*                         ACCESSEURS                                         */
/* ========================================================================== */

const std::string& Channel::getName() const
{
	return _name;
}

//Format : "@op1 user1 @op2 user2"
std::string Channel::getNamesList() const
{
	std::string list;

	for (std::set<Client*>::const_iterator it = _members.begin();
	     it != _members.end(); ++it)
	{
	    if (!list.empty())
	        list += " ";

	    if (isOperator(*it))
	        list += "@";

	    list += (*it)->getNickname();
	}

	return list;
}