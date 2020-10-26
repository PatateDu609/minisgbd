#include <iostream>
#include "DBManager.hpp"
#include "DBInfo.hpp"

DBManager* DBManager::INSTANCE = NULL;

DBManager::DBManager(){
	DB_INFO = new DBInfo();
}

DBManager::~DBManager(){
	delete DB_INFO;
}

DBManager* DBManager::getInstance() {
	if (INSTANCE == NULL) INSTANCE = new DBManager();
	return INSTANCE;
}

void DBManager::init(){
	DB_INFO->init();
}

void DBManager::finish(){
	DB_INFO->finish();
}

void DBManager::processCommand(std::string COMMANDE){
	std::cout << "OH c'est cool ça : " << COMMANDE << std::endl;
}

void createRelation(std::string NOM_REL, int NBRE, std::vector<std::string> TYPES, std::vector<std::string> NOM_COL){

}