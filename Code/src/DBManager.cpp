#include <iostream>
#include <sstream>
#include "DBManager.hpp"
#include "DBInfo.hpp"
#include "DBParams.hpp"

DBManager* DBManager::INSTANCE = NULL;

int sizeofType(std::string type)
{
	switch (type[0])
	{
	case 's':
		return std::stoi(type.substr(6));
	case 'i':
		return sizeof(int);
	case 'f':
		return sizeof(float);
	default:
		return 0;
	}
}

int getRelInfoSize(const RelationInfo &relInfo)
{
	int sizeBuffered = 0;

	for (std::string type : relInfo.TYPES){
		sizeBuffered += sizeofType(type);
	}
	return sizeBuffered;
}

DBManager::DBManager(){
	DB_INFO = DBInfo::getInstance();

	// Remplissage du dictionnaire de fonctions
	HANDLERS.insert(std::make_pair("CREATEREL", &DBManager::createRelation));
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
	std::string NAME = COMMANDE.substr(0, SPACE);
	std::string ARGS = COMMANDE.substr(SPACE + 1);

	if (HANDLERS.find(NAME) != HANDLERS.end()) // Vérifie que la commande NAME est bien présente dans le dictionnaire de fonctions
		(this->*HANDLERS[NAME])(ARGS); // Appel de la fonction correspondante à NAME dans le dictionnaire de fonctions
	else
		std::cerr << "ERREUR : La commande " << COMMANDE << " n'existe pas !" << std::endl;
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
		std::string type = COL.substr(COL.find(":") + 1, COL.length());

		if (type != "int" && type != "float" && type.find("string") != (size_t)0)
		{
			std::cerr << "ERREUR : type invalide" << std::endl;
			return ;
		}
		if (type.find("string") == (size_t)0)
		{
			std::string size = type.substr(6);
			if (size.empty() || !std::all_of(size.begin(), size.end(), isdigit))
			{
				std::cerr << "ERREUR : type invalide" << std::endl;
				return ;
			}
		}
		rel.TYPES.push_back(type);
	}
	rel.NBRE_COLONNES = rel.NOMS.size();
	rel.recordSize = getRelInfoSize(rel);
	rel.fileIdx = DB_INFO->getCompteur();
	rel.slotCount = DBParams::pageSize / rel.recordSize;
	DB_INFO->addRelation(rel);
}
