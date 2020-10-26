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
	typedef void (*CommandHandler)(std::string);

	static DBManager* INSTANCE;
	DBInfo* DB_INFO;
	// Dictionnaire de fonctions de gestion des commandes
	std::map<std::string, CommandHandler> HANDLERS;

	// Fonctions à stocker dans le dictionnaire de fonctions des commandes
	static void createRelation(std::string args);

public:
	DBManager(/* args */);
	~DBManager();
	static DBManager* getInstance();
	void init();
	void finish();
	void processCommand(std::string COMMANDE);
};

#endif
