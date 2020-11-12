#include "BufferManager.hpp"
#include <iostream>

BufferManager* BufferManager :: INSTANCE = NULL;

BufferManager::BufferManager() : DM(NULL)
{
	DM = DiskManager::getInstance();
}

BufferManager* BufferManager::getInstance()
{
	if (!INSTANCE) INSTANCE = new BufferManager();
	return INSTANCE;
}

std::vector<char>& BufferManager::GetPage(PageId& pageId)
{
	if(FRAMES.size() < DBParams::frameCount) {
		FRAMES[pageId]=new Frame;
		FRAMES[pageId]->pageId = pageId;		
		FRAMES[pageId]->dirty = 0;		
		FRAMES[pageId]->pinCount = 1;		
		FRAMES[pageId]->refBit = 0;		
		FRAMES[pageId]->pageDisk = std::vector<char>(DBParams::pageSize);
		DM->ReadPage(pageId, FRAMES[pageId]->pageDisk.data());
	}
	else
	{
		Frame *replacementFrame = NULL;
		do{
			for(decltype(FRAMES)::iterator it = FRAMES.begin(); it != FRAMES.end(); it++){
				if(it->second->pinCount==0 && it->second->refBit==1) it->second->refBit=0;
				else if(it->second->pinCount==0 && it->second->refBit==0) {
					replacementFrame=it->second;
				}
			}
		}while(replacementFrame==NULL);

		replacementFrame->pinCount = 1;
		replacementFrame->dirty = 0;
		replacementFrame->refBit = 1;
		replacementFrame->pageId = pageId;
		replacementFrame->pageDisk.insert(replacementFrame->pageDisk.begin(), DBParams::pageSize, 0);
		replacementFrame->pageDisk.clear();
		DM->ReadPage(pageId, replacementFrame->pageDisk.data());
	}
}

void BufferManager::FreePage(PageId& pageId, bool valdirty)
{
	FRAMES[pageId]->dirty = valdirty;
	FRAMES[pageId]->pinCount = 0;
	if (!valdirty) return;
	DM->WritePage(pageId, FRAMES[pageId]->pageDisk.data());
}

void BufferManager::FlushAll()
{
	for (decltype(FRAMES)::iterator it = FRAMES.begin(); it != FRAMES.end(); it++)
	{
		FreePage(it->first, it->second->dirty);
	}
}
