#include "IRC.hpp"

/* ========================================================================== */
/*                    CONSTRUCTEUR / DESTRUCTEUR                              */
/* ========================================================================== */

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

bool Channel::addMember(Client* client)
{
	if (isMember(client))
	    return false;

	_members.insert(client);

	if (_members.size() == 1)
	    addOperator(client);

	return true;
}

void Channel::removeMember(Client* client)
{
	_members.erase(client);
	_operators.erase(client);

}

bool Channel::isMember(Client* client) const
{
	return _members.find(client) != _members.end();
}

bool Channel::hasMember(const std::string& nickname) const
{
	return (getMemberByNickname(nickname) != NULL);
}

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

const std::set<Client*>& Channel::getMembers() const
{
	return _members;
}

size_t Channel::getMemberCount() const
{
	return _members.size();
}

bool Channel::isEmpty() const
{
	return _members.empty();
}

/* ========================================================================== */
/*                    GESTION DES OPÉRATEURS                                  */
/* ========================================================================== */

bool Channel::addOperator(Client* client)
{
	if (isMember(client))
	{
		_operators.insert(client);
		return true;
	}
	return false;
}

void Channel::removeOperator(Client* client)
{
	_operators.erase(client);
}

bool Channel::isOperator(Client* client) const
{
	return _operators.find(client) != _operators.end();
}

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