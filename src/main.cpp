#include "IRC.hpp"
#include <csignal>
#include <cstdlib>

Server* g_server = NULL;

void signalHandler(int signum)
{
	const char* msg = "\nSignal received, shutting down server...\n";
	write(STDOUT_FILENO, msg, 45);
	
	if (g_server != NULL)
		g_server->stop();
	
	(void)signum;
}

void setupSignalHandlers()
{
	struct sigaction sa;
	
	// Configuration pour SIGINT et SIGTERM
	sa.sa_handler = signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		std::cerr << "Error: Failed to set SIGINT handler" << std::endl;
		exit(1);
	}
	
	if (sigaction(SIGTERM, &sa, NULL) == -1)
	{
		std::cerr << "Error: Failed to set SIGTERM handler" << std::endl;
		exit(1);
	}
	
	signal(SIGPIPE, SIG_IGN);
}

bool validateArguments(int argc, char** argv)
{
	// Vérifier le nombre d'arguments
	if (argc != 3)
	{
		std::cerr << "Error: Invalid number of arguments" << std::endl;
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		std::cerr << "Example: " << argv[0] << " 6667 mypassword" << std::endl;
		return false;
	}
	
	// Vérifier que le port est un nombre valide
	std::string portStr = argv[1];
	for (size_t i = 0; i < portStr.length(); i++)
	{
		if (!std::isdigit(portStr[i]))
		{
			std::cerr << "Error: Port must be a number" << std::endl;
			return false;
		}
	}
	
	// Convertir et valider le port
	long port = std::atol(argv[1]);
	
	if (port < 1 || port > 65535)
	{
		std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
		return false;
	}
	
	if (port < 1024)
	{
		std::cerr << "Warning: Ports below 1024 require root privileges" << std::endl;
	}
	
	// Vérifier que le password n'est pas vide
	std::string password = argv[2];
	if (password.empty())
	{
		std::cerr << "Error: Password cannot be empty" << std::endl;
		return false;
	}
	
	return true;
}

int main(int argc, char** argv)
{
	// 1. Valider les arguments
	if (!validateArguments(argc, argv))
		return 1;
	
	// 2. Parser les arguments
	int port = std::atoi(argv[1]);
	std::string password = argv[2];
	
	// 3. Configurer les gestionnaires de signaux
	setupSignalHandlers();
	
	// 4. Créer le serveur
	Server server(port, password);
	g_server = &server;  // Pour le gestionnaire de signaux
	
	// 5. Initialiser le serveur
	if (!server.init())
	{
		std::cerr << "Failed to initialize server" << std::endl;
		g_server = NULL;
		return 1;
	}
	
	// 6. Afficher un message de démarrage
	std::cout << "========================================" << std::endl;
	std::cout << "ft_irc server started successfully!" << std::endl;
	std::cout << "Port: " << port << std::endl;
	std::cout << "Server name: " << server.getServerName() << std::endl;
	std::cout << "Press Ctrl+C to stop the server" << std::endl;
	std::cout << "========================================" << std::endl;
	
	// 7. Lancer la boucle principale
	server.run();  // Bloque jusqu'à arrêt du serveur
	
	// 8. Nettoyer et quitter
	std::cout << "\n========================================" << std::endl;
	std::cout << "Server stopped cleanly" << std::endl;
	std::cout << "========================================" << std::endl;
	
	g_server = NULL;
	
	return 0;
}