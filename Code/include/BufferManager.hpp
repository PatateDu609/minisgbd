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
	std::map<PageId, Frame*> FRAMES;
	static BufferManager* INSTANCE;

	DiskManager *DM;

	BufferManager();
public:
	~BufferManager();
	BufferManager(const BufferManager& other) = delete;
	BufferManager& operator=(const BufferManager& other) = delete;
	std::map<PageId, Frame*> getFrames() const;
	static BufferManager* getInstance();
	static void resetInstance();
	std::vector<char>* GetPage(const PageId& pageId);
	void FreePage(const PageId& pageId, bool valdirty);
	void FlushAll();
};

#endif