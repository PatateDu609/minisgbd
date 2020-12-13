#include <iomanip>
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

	BufferManager::resetInstance();
	DiskManager::resetInstance();
	FileManager::resetInstance();

	return (0);
}
