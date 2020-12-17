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
#include "parser.hpp"

std::string DBParams::DBPath = "DB/";
int DBParams::pageSize = 4096;
int DBParams::frameCount = 2;

int main(int ac, char **av)
{
	Input *input = Input::getInstance("> ");

	if (ac > 1)
		DBParams::DBPath = av[1];

	DBManager *DBM = DBManager::getInstance();
	std::string COMMANDE = "";

	DBM->init();
	while (COMMANDE != "EXIT")
	{
		COMMANDE = input->getline();
		input->addCommand(COMMANDE);
		if (COMMANDE.length() && COMMANDE != "EXIT")
			DBM->processCommand(COMMANDE);
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
