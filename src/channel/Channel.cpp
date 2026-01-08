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

// Destructor
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