#ifndef DISKMANAGER_HPP
#define DISKMANAGER_HPP

#include "PageId.hpp"
#include <string>

class DiskManager
{
private:
	static DiskManager *INSTANCE;

	DiskManager();
public:
	DiskManager(const DiskManager &) = delete;
	DiskManager& operator=(const DiskManager &) = delete;

	~DiskManager();

	static DiskManager *getInstance();
	static void resetInstance();

	void CreateFile(int fileIdx);
	PageId AddPage(int fileIdx);
	void ReadPage(PageId pageId, char buf[]);
	void WritePage(PageId pageId, char buf[]);
};

std::string getFilename(int fileIdx);
size_t sizeFile(std::string name);
bool fileExists(std::string name);

#endif
