#ifndef DBMANAGER_HPP
#define DBMANAGER_HPP
#include "DBInfo.hpp"
#include <string>
#include <vector>
#include <map>

class DBManager
{
private:
	// Typedef sur le type de fonction de gestion des commandes
	typedef void (DBManager::*CommandHandler)(std::string);

	static DBManager* INSTANCE;
	DBInfo* DB_INFO;
	// Dictionnaire de fonctions de gestion des commandes
	std::map<std::string, CommandHandler> HANDLERS;

	// Fonctions à stocker dans le dictionnaire de fonctions des commandes
	void createRelation(std::string args);

	DBManager();
public:
	// Suppresion du constructeur de copie (impossible de construire un DBManager en utilisant une autre instance de DBManager)
	DBManager(const DBManager&) = delete;
	// Suppresion de l'opérateur de copie (impossibilité de faire DBManager a = b, où b est un DBManager)
	DBManager& operator=(const DBManager&) = delete;

	~DBManager();
	static DBManager* getInstance();
	void init();
	void finish();
	void processCommand(std::string COMMANDE);
};

#endif
