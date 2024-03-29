#include "DiskManager.hpp"
#include "DBParams.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>

DiskManager *DiskManager::INSTANCE = nullptr;

std::string getFilename(int fileIdx){
	return DBParams::DBPath + "Data_" + std::to_string(fileIdx) + ".rf";
}

bool fileExists(std::string name){
	std::fstream file(name, std::ios::in | std::ios::binary);
	if (!file.is_open())
		return false;
	file.close();
	return true;
}

size_t sizeFile(std::string name){
	std::fstream file(name, std::ios::in | std::ios::ate | std::ios::binary);
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
	file.close();
}

PageId DiskManager::AddPage(int fileIdx){
	if (!fileExists(getFilename(fileIdx)))
		return (PageId){ -1, -1 };
	std::fstream file(getFilename(fileIdx), std::ios::out | std::ios::ate | std::ios::app);
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

	if (!file.is_open())
	{
		std::cerr << "ERREUR : Le fichier Data_" << pageId.FileIdx << ".rf n'a pas pu être ouvert !" << std::endl;
		return ;
	}
	int filesize = (int)sizeFile(getFilename(pageId.FileIdx));

	if (filesize <= pageId.PageIdx * DBParams::pageSize)
	{
		std::cerr << "ERREUR : La page demandée n'existe pas dans le fichier Data_" << pageId.FileIdx << ".rf !" << std::endl;
		file.close();
		return ;
	}
	file.seekg(pageId.PageIdx * DBParams::pageSize);
	file.read(buf, DBParams::pageSize);
	file.close();
}

bool DiskManager::PageExists(PageId pageId) const{
	if(fileExists(getFilename(pageId.FileIdx))) {
		size_t fileSize = sizeFile(getFilename(pageId.FileIdx));
		return (int)(fileSize/DBParams::pageSize) > pageId.PageIdx;
	} else return false;
}

void DiskManager::WritePage(PageId pageId, char buf[]){
	std::fstream file(getFilename(pageId.FileIdx), std::ios::in);

	if (!file.is_open())
	{
		std::cerr << "ERREUR : Le fichier Data_" << pageId.FileIdx << ".rf n'a pas pu être ouvert !" << std::endl;
		return ;
	}
	file.close();

	int filesize = (int)sizeFile(getFilename(pageId.FileIdx));
	if (filesize <= pageId.PageIdx * DBParams::pageSize)
	{
		std::cerr << "ERREUR : La page demandée n'existe pas dans le fichier Data_" << pageId.FileIdx << ".rf !" << std::endl;
		file.close();
		return ;
	}
	file.open(getFilename(pageId.FileIdx), std::ios::in);

	char BUFFER[filesize];
	file.read(BUFFER, filesize);
	memcpy(BUFFER + pageId.PageIdx * DBParams::pageSize, buf, DBParams::pageSize);
	file.close();
	file.open(getFilename(pageId.FileIdx), std::ios::trunc | std::ios::out);
	file.write(BUFFER, filesize);
	file.close();
}
