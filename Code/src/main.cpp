#include <iostream>
#include <string>
#include "DBManager.hpp"
#include "DBInfo.hpp"

int main()
{
	DBManager* DBM = DBManager::getInstance();
	std::string COMMANDE = "";

	DBM->init();
	while (COMMANDE != "EXIT"){
		 std::getline(std::cin, COMMANDE);
		if(COMMANDE != "EXIT") DBM->processCommand(COMMANDE);
	}
	DBM->finish();
	delete DBM;
}
