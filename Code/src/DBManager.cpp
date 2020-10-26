#include <iostream>
#include <sstream>
#include "DBManager.hpp"
#include "DBInfo.hpp"

DBManager* DBManager::INSTANCE = NULL;

DBManager::DBManager(){
	DB_INFO = new DBInfo();

	// Remplissage du dictionnaire de fonctions
	HANDLERS.insert(std::make_pair("CREATEREL", DBManager::createRelation));
}

DBManager::~DBManager(){
	delete DB_INFO;
}

DBManager* DBManager::getInstance() {
	if (INSTANCE == NULL)
		INSTANCE = new DBManager();
	return INSTANCE;
}

void DBManager::init(){
	DB_INFO->init();
}

void DBManager::finish(){
	DB_INFO->finish();
}

void DBManager::processCommand(std::string COMMANDE){
	std::size_t SPACE = COMMANDE.find(' ');
	std::string NAME = COMMANDE.substr(0, COMMANDE.find(' '));
	std::string ARGS = COMMANDE.substr(SPACE + 1, COMMANDE.length());

	if (HANDLERS.find(NAME) != HANDLERS.end()) // Vérifie que la commande NAME est bien présente dans le dictionnaire de fonctions
		HANDLERS[NAME](ARGS); // Appel de la fonction correspondante à NAME dans le dictionnaire de fonctions
	else
		std::cout << "La commande " << COMMANDE << " n'existe pas !" << std::endl;
}

void DBManager::createRelation(std::string ARGS){
	RelationInfo rel;
	std::string COL;
	std::istringstream ISS(ARGS);

	ISS >> rel.NOM_RELATION;
	while (!ISS.eof())
	{
		ISS >> COL;
		rel.NOMS.push_back(COL.substr(0, COL.find(":")));
		rel.TYPES.push_back(COL.substr(COL.find(":") + 1, COL.length()));
	}
	rel.NBRE_COLONNES = rel.NOMS.size();
	std::cout << "Relation(" << rel.NOM_RELATION << "): ";
	for (int i = 0; i < rel.NBRE_COLONNES; i++)
		std::cout << rel.NOMS[i] << "(" << rel.TYPES[i] << ")" << (i + 1 < rel.NBRE_COLONNES ? ", " : "\n");
}
