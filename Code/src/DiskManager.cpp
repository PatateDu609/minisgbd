#include "DiskManager.hpp"
#include "DBParams.hpp"
#include <fstream>
#include <iostream>
#include <string.h>

DiskManager *DiskManager::INSTANCE = nullptr;

std::string getFileName(int fileIdx)
{
	return DBParams::DBPath + "Data_" + std::to_string(fileIdx) + ".rf";
}

DiskManager::DiskManager(){

}

DiskManager::~DiskManager(){

}

DiskManager *DiskManager::getInstance() {
	if (!INSTANCE) INSTANCE = new DiskManager();
	return INSTANCE;
}

void DiskManager::CreateFile(int fileIdx)
{
	std::fstream file(getFileName(fileIdx), std::ios::out | std::ios::in | std::ios::trunc);
	file.close();
}

PageId DiskManager::AddPage(int fileIdx)
{
	std::fstream file(getFileName(fileIdx), std::ios::in);

	if (!file.is_open())
		return ((PageId){ .FileIdx = -1, .PageIdx = -1});
	file.close();
	file.open(getFileName(fileIdx), std::ios::out | std::ios::binary | std::ios::ate | std::ios::app);
	std::fstream::pos_type filesize = file.tellg();
	char buf[DBParams::pageSize];
	PageId pageId = { .FileIdx = fileIdx, .PageIdx = (int)(filesize / 4096) };

	memset(buf, 0, DBParams::pageSize * sizeof(char));
	file.write(buf, DBParams::pageSize);

	file.seekg(0, std::ios::end);
	file.close();
	return pageId;
}

void DiskManager::ReadPage(PageId pageId, char buf[])
{
	std::fstream file(getFileName(pageId.FileIdx), std::ios::in);

	file.seekg(pageId.PageIdx * DBParams::pageSize);
	file.read(buf, DBParams::pageSize);
	file.close();
}

void DiskManager::WritePage(PageId pageId, char buf[])
{
	std::fstream file(getFileName(pageId.FileIdx), std::ios::in);

	if (!file.is_open())
		return ;
	file.close();
	file.open(getFileName(pageId.FileIdx), std::ios::out | std::ios::app);

	file.seekp(pageId.PageIdx * DBParams::pageSize);
	file.write(buf, DBParams::pageSize);
	file.close();
}
