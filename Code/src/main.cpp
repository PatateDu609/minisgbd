#include <iostream>
#include <string>
#include <string.h>
#include "DBManager.hpp"
#include "DBInfo.hpp"
#include "DBParams.hpp"
#include "DiskManager.hpp"
#include "Input.hpp"
#include "Record.hpp"

std::string DBParams::DBPath = "DB/";
int DBParams::pageSize = 4096;
int DBParams::frameCount =2;

int main()
{
	Input *input = Input::getInstance("");

	DBManager* DBM = DBManager::getInstance();
	std::string COMMANDE = "";

	DBM->init();
	while (COMMANDE != "EXIT"){
		COMMANDE = input->getline();
		input->addCommand(COMMANDE);
		if(COMMANDE.length() && COMMANDE != "EXIT") DBM->processCommand(COMMANDE);
	}
	input->saveHistory();
	DBM->finish();
	delete input;
	delete DBM;

	// RelationInfo rel;
	// rel.NOM_RELATION = "R1";
	// rel.NBRE_COLONNES = 5;
	// rel.NOMS.push_back("C1"); rel.TYPES.push_back("string11");
	// rel.NOMS.push_back("C2"); rel.TYPES.push_back("int");
	// rel.NOMS.push_back("C3"); rel.TYPES.push_back("float");
	// rel.NOMS.push_back("C4"); rel.TYPES.push_back("int");
	// rel.NOMS.push_back("C5"); rel.TYPES.push_back("string17");
	// Record rec(rel);
}
