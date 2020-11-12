#ifndef BUFFERMANAGER_HPP
#define BUFFERMANAGER_HPP

#include "Frame.hpp"
#include "DBParams.hpp"
#include "DiskManager.hpp"
#include <vector>
#include <algorithm>
#include <map>

class BufferManager
{
private:
	std::map<PageId&, Frame*> FRAMES;
	static BufferManager* INSTANCE;

	DiskManager *DM;

	BufferManager();
public:
	BufferManager(const BufferManager& other) = delete;
	BufferManager& operator=(const BufferManager& other) = delete;
	
	BufferManager* getInstance();

	std::vector<char>& GetPage(PageId& pageId);
	void FreePage(PageId& pageId, bool valdirty);
	void FlushAll();
};

#endif