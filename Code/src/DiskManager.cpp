#include "DiskManager.hpp"
#include "DBParams.hpp"
#include <fstream>
#include <iostream>
#include <string.h>

DiskManager *DiskManager::INSTANCE = nullptr;

std::string getFilename(int fileIdx){
	return DBParams::DBPath + "Data_" + std::to_string(fileIdx) + ".rf";
}

bool fileExists(std::string name){
	std::fstream file(name, std::ios::in);
	if (!file.is_open())
		return false;
	file.close();
	return true;
}

size_t sizeFile(std::string name){
	std::fstream file(name, std::ios::in | std::ios::ate);
	if (!file.is_open())
		return 0;
	size_t size = file.tellg();
	file.close();
	return size;
}

DiskManager::DiskManager(){

}

DiskManager::~DiskManager(){

}

DiskManager *DiskManager::getInstance(){
	if (!INSTANCE) INSTANCE = new DiskManager();
	return INSTANCE;
}

void DiskManager::resetInstance(){
	if (INSTANCE) delete INSTANCE;
	INSTANCE = NULL;
}

void DiskManager::CreateFile(int fileIdx){
	std::fstream file(getFilename(fileIdx), std::ios::in);

	if (file.is_open())
	{
		std::cerr << "ERREUR : Le fichier Data_" << fileIdx << ".rf existe déjà!" << std::endl;
		file.close();
		return ;
	}
	file.close();
	file.open(getFilename(fileIdx), std::ios::out | std::ios::trunc);
}

PageId DiskManager::AddPage(int fileIdx){
	if (!fileExists(getFilename(fileIdx)))
		return (PageId){ -1, -1 };
	std::fstream file(getFilename(fileIdx), std::ios::out | std::ios::binary | std::ios::ate | std::ios::app);
	std::fstream::pos_type filesize = sizeFile(getFilename(fileIdx));
	char buf[DBParams::pageSize];
	PageId pageId = { .FileIdx = fileIdx, .PageIdx = (int)(filesize / DBParams::pageSize) };

	memset(buf, 0, DBParams::pageSize * sizeof(char));
	file.write(buf, DBParams::pageSize);

	file.close();
	return pageId;
}

void DiskManager::ReadPage(PageId pageId, char buf[]){
	std::fstream file(getFilename(pageId.FileIdx), std::ios::in);

	file.seekg(pageId.PageIdx * DBParams::pageSize);
	file.read(buf, DBParams::pageSize);
	file.close();
}

void DiskManager::WritePage(PageId pageId, char buf[]){
	std::fstream file(getFilename(pageId.FileIdx), std::ios::in);

	if (!file.is_open())
		return ;
	file.close();
	file.open(getFilename(pageId.FileIdx), std::ios::out | std::ios::app);

	file.seekp(pageId.PageIdx * DBParams::pageSize);
	file.write(buf, DBParams::pageSize);
	file.close();
}
