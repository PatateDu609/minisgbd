#include "BufferManager.hpp"
#include <iostream>

BufferManager* BufferManager :: INSTANCE = NULL;

BufferManager::BufferManager() : DM(NULL)
{
	DM = DiskManager::getInstance();
}

BufferManager::~BufferManager(){
	for(decltype(FRAMES)::iterator it = FRAMES.begin(); it != FRAMES.end(); it++){
		delete it->second->pageDisk;
		it->second->pageDisk = NULL;
		delete it->second;
	}
	DiskManager::resetInstance();
}

void BufferManager::resetInstance(){
	if (INSTANCE) delete INSTANCE;
	INSTANCE = NULL;
}

BufferManager* BufferManager::getInstance()
{
	if (!INSTANCE) INSTANCE = new BufferManager();
	return INSTANCE;
}

std::vector<char>* BufferManager::GetPage(const PageId& pageId)
{
	if(FRAMES.find(pageId) != FRAMES.end()){
		FRAMES[pageId]->dirty = 0;
		FRAMES[pageId]->pinCount = 1;
		return FRAMES[pageId]->pageDisk;
	} else if((int)FRAMES.size() < DBParams::frameCount) {
		FRAMES[pageId] = new Frame;
		FRAMES[pageId]->pageId = pageId;
		FRAMES[pageId]->dirty = 0;
		FRAMES[pageId]->pinCount = 1;
		FRAMES[pageId]->refBit = 0;
		FRAMES[pageId]->pageDisk = new std::vector<char>(DBParams::pageSize);
		DM->ReadPage(pageId, FRAMES[pageId]->pageDisk->data());
		return FRAMES[pageId]->pageDisk;
	}
	else
	{
		Frame *replacementFrame = nullptr;
		decltype(FRAMES)::iterator it;
		for(it = FRAMES.begin(); it != FRAMES.end(); it++){
			if(it->second->pinCount==0 && it->second->refBit==1) it->second->refBit=0;
			else if(it->second->pinCount==0 && it->second->refBit==0) {
				replacementFrame=it->second;
				break ;
			}
		}
		if (!replacementFrame)
			return NULL;
		if (replacementFrame->dirty)
			DM->WritePage(replacementFrame->pageId, replacementFrame->pageDisk->data());
		replacementFrame->pinCount = 1;
		replacementFrame->dirty = false;
		replacementFrame->refBit = 1;
		replacementFrame->pageId = pageId;
		replacementFrame->pageDisk->clear();
		replacementFrame->pageDisk->resize(DBParams::pageSize);
		DM->ReadPage(pageId, replacementFrame->pageDisk->data());
		FRAMES.erase(it);
		FRAMES[pageId] = replacementFrame;
		return replacementFrame->pageDisk;
	}
}

void BufferManager::FreePage(const PageId& pageId, bool valdirty)
{
	FRAMES[pageId]->dirty = valdirty;
	FRAMES[pageId]->pinCount = 0;
	FRAMES[pageId]->refBit = 1;
	auto tmp = FRAMES[pageId]->pageDisk;
	FRAMES[pageId]->pageDisk = new std::vector<char>(*tmp);
	delete tmp;
}

void BufferManager::FlushAll()
{
	for (decltype(FRAMES)::iterator it = FRAMES.begin(); it != FRAMES.end(); it++)
	{
		if (it->second->dirty)
			DM->WritePage(it->first, it->second->pageDisk->data());
		delete it->second->pageDisk;
		delete it->second;
	}
	FRAMES.clear();
}

std::map<PageId, Frame*> BufferManager::getFrames() const{
	return FRAMES;
}

void BufferManager::setFrames(const std::map<PageId, Frame*>& frames) {
	FRAMES = frames;
}
