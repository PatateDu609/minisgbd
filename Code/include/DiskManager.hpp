#ifndef DISKMANAGER_HPP
#define DISKMANAGER_HPP

#include "PageId.hpp"

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

	void CreateFile(int fileIdx);
	PageId AddPage(int fileIdx);
	void ReadPage(PageId pageId, char buf[]);
	void WritePage(PageId pageId, char buf[]);
};

#endif
