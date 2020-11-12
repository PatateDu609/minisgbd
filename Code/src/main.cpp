#include <iostream>
#include <string>
#include <string.h>
#include "DBManager.hpp"
#include "DBInfo.hpp"
#include "DBParams.hpp"
#include "DiskManager.hpp"
#include "Input.hpp"

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
		std::cout << "YOOOOOOOOOOOOOOOOOOOOOOO" << std::endl;
		if(COMMANDE.length() && COMMANDE != "EXIT") DBM->processCommand(COMMANDE);
	}
	input->saveHistory();
	DBM->finish();
	delete DBM;
	delete input;

	// DiskManager *DM = DiskManager::getInstance();
	// // DM->CreateFile(0);
	// // PageId pageId = DM->AddPage(0);

	// char buf[DBParams::pageSize];
	// memset(buf, 45, DBParams::pageSize);
	// DM->WritePage((PageId){ .FileIdx = 0, .PageIdx = 1 }, buf);

	// DM->ReadPage((PageId){ .FileIdx = 0, .PageIdx = 1}, buf);

	// for (int i = 0; i < DBParams::pageSize; i++)
	// 	printf("%x%c", buf[i] & 0xff, i + 1 < DBParams::pageSize ? ' ' : '\n');
	// delete DM;
}
