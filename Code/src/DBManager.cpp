#include <iostream>
#include <sstream>
#include "DBManager.hpp"
#include "DBInfo.hpp"
#include "DBParams.hpp"
#include "parser.hpp"

DBManager *DBManager::INSTANCE = NULL;

DBManager::DBManager()
{
	DB_INFO = DBInfo::getInstance();
	fileManager = FileManager::getInstance();

	// Remplissage du dictionnaire de fonctions
	HANDLERS.insert(std::make_pair("CREATEREL", &DBManager::createRelation));

	HANDLERS.insert(std::make_pair("CLEAN", &DBManager::clean));
	HANDLERS.insert(std::make_pair("RESET", &DBManager::reset));
	HANDLERS.insert(std::make_pair("INSERT", &DBManager::insert));
	// HANDLERS.insert(std::make_pair("BATCHINSERT", ));
	// HANDLERS.insert(std::make_pair("SELECTALL", ));
	// HANDLERS.insert(std::make_pair("SELECTS", ));

	// HANDLERS.insert(std::make_pair("SELECTC", ));
	// HANDLERS.insert(std::make_pair("UPDATE", ));
}

DBManager::~DBManager()
{
	delete DB_INFO;
}

DBManager *DBManager::getInstance()
{
	if (INSTANCE == NULL)
		INSTANCE = new DBManager();
	return INSTANCE;
}

void DBManager::init()
{
	DB_INFO->init();
	fileManager->Init();
}

void DBManager::finish()
{
	BufferManager::getInstance()->FlushAll();
	DB_INFO->finish();
}

void DBManager::processCommand(std::string COMMANDE)
{
	std::size_t SPACE = COMMANDE.find(' ');
	std::string NAME = COMMANDE.substr(0, SPACE);
	std::string ARGS = COMMANDE.substr(SPACE + 1);

	if (HANDLERS.find(NAME) != HANDLERS.end()) // Vérifie que la commande NAME est bien présente dans le dictionnaire de fonctions
		(this->*HANDLERS[NAME])(ARGS);		   // Appel de la fonction correspondante à NAME dans le dictionnaire de fonctions
	else
		std::cerr << "ERREUR : La commande " << COMMANDE << " n'existe pas !" << std::endl;
}

void DBManager::createRelation(std::string ARGS)
{
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
			return;
		}
		if (type.find("string") == (size_t)0)
		{
			std::string size = type.substr(6);
			if (size.empty() || !std::all_of(size.begin(), size.end(), isdigit))
			{
				std::cerr << "ERREUR : type invalide" << std::endl;
				return;
			}
		}
		rel.TYPES.push_back(type);
	}
	rel.NBRE_COLONNES = rel.NOMS.size();
	rel.recordSize = getRelInfoSize(rel);
	rel.fileIdx = DB_INFO->getCompteur();
	rel.slotCount = DBParams::pageSize / rel.recordSize;
	DB_INFO->addRelation(rel);
	fileManager->CreateRelationFile(rel);
}

void DBManager::reset(std::string args)
{
	(void)args;

	BufferManager::getInstance()->FlushAll();
	fileManager->reset();
	DB_INFO->reset();

	remove((DBParams::DBPath + "Catalog.def").c_str());

	BufferManager::resetInstance();
	BufferManager::getInstance();
	FileManager::resetInstance();
	fileManager = FileManager::getInstance();
	DBInfo::getInstance();
	DB_INFO = DBInfo::getInstance();
}

void DBManager::clean(std::string args)
{
	(void)args;
	fileManager->clean();
}

void DBManager::insert(std::string args)
{
	std::map<std::string, std::string> parsed = parse(args, {"INTO", "RECORD"});
	std::vector<std::string> values = parseValues(parsed["RECORD"]);

	std::vector<RelationInfo> info = DB_INFO->getInfo();
	RelationInfo rel = *std::find(info.begin(), info.end(), parsed["INTO"]);
	Record rc(rel);
	rc.setValues(values);
	fileManager->InsertRecordInRelation(rc, rel);
}
