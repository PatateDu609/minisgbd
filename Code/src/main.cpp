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
	// Input *input = Input::getInstance("");

	// DBManager* DBM = DBManager::getInstance();
	// std::string COMMANDE = "";

	// DBM->init();
	// while (COMMANDE != "EXIT"){
	// 	COMMANDE = input->getline();
	// 	input->addCommand(COMMANDE);
	// 	if(COMMANDE.length() && COMMANDE != "EXIT") DBM->processCommand(COMMANDE);
	// }
	// input->saveHistory();
	// DBM->finish();
	// delete input;
	// delete DBM;

	std::vector<char> vec(566, 2);

	std::vector<char> idv(vec.begin(), vec.begin() + 4);
	const int id = *(reinterpret_cast<const int *>(idv.data()));
	std::cout << "id = " << id << std::endl;

	return (0);
}
