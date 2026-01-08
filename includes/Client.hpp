#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <set>

class Client
{
private:
	/* ================================================================== */
	/*                    ATTRIBUTS PRIVÉS                                */
	/* ================================================================== */
	
	// Identifiant socket
	int         _fd;
	
	// Informations d'identification
	std::string _nickname;
	std::string _username;
	std::string _realname;
	std::string _hostname;
	
	// État
	bool        _passwordProvided;
	bool        _registered;
	bool        _shouldDisconnect;
	
	// Channels
	std::set<std::string> _channels;
	
	std::string _inputBuffer;
	std::string _outputBuffer;

	/* ================================================================== */
	/*                CONSTRUCTEURS INTERDITS                             */
	/* ================================================================== */
	Client();
	Client(const Client& other);
	Client& operator=(const Client& other);

public:
	/* ================================================================== */
	/*                    CONSTRUCTEUR / DESTRUCTEUR                      */
	/* ================================================================== */
	
    Client(int fd, const std::string& hostname);
    ~Client();

    /* ================================================================== */
    /*                        IDENTIFICATION                              */
    /* ================================================================== */
    void setNickname(const std::string& nickname);
    const std::string& getNickname() const;
    void setUsername(const std::string& username);
    const std::string& getUsername() const;
    void setRealname(const std::string& realname);
    const std::string& getRealname() const;
    const std::string& getHostname() const;
    std::string getPrefix() const;

    /* ========================================================================== */
    /*                   ÉTAT D'AUTHENTIFICATION                                  */
    /* ========================================================================== */
    void setPasswordProvided(bool provided);
    bool hasPasswordProvided() const;
    bool isPasswordProvided() const;
    void setRegistered(bool registered);
    bool isRegistered() const;
    void markForDisconnect();
    bool shouldDisconnect() const;


    /* ========================================================================== */
    /*                    GESTION DES CHANNELS                                    */
    /* ========================================================================== */
    void joinChannel(const std::string& channelName);
    void leaveChannel(const std::string& channelName);
    bool isInChannel(const std::string& channelName) const;
    const std::set<std::string>& getChannels() const;

    /* ========================================================================== */
    /*                    GESTION DES BUFFERS                                     */
    /* ========================================================================== */
    void appendToInputBuffer(const std::string& data);
    std::string& getInputBuffer();
    void appendToOutputBuffer(const std::string& data);
    std::string& getOutputBuffer();
    void clearInputBuffer();
    void trimOutputBuffer(size_t bytes);
    bool hasDataToSend() const;

    /* ========================================================================== */
    /*                         ACCESSEURS                                         */
    /* ========================================================================== */
    int getFd() const;
    void markForDisconnection();
    bool shouldDisconnect() const;
};

#endif