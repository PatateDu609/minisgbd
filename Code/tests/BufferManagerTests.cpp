#include <limits.h>
#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <string>
#include "DiskManager.hpp"
#include "DBParams.hpp"
#include "BufferManager.hpp"

class BufferManagerTest : public :: testing :: Test
{
protected:
	virtual void setUp()
	{

	}

	virtual void tearDown()
	{

	}
};


void createRandFile(int idx, int nbrePages = 2)
{
	DiskManager *DM = DiskManager::getInstance();
	idx = 0;
	std::fstream file(getFilename(idx), std::ios::out);
	for(int i = 0; i < nbrePages; i++){
		for(int j = 0; j < DBParams::pageSize; j++){
		 file.write("abcdefghijklmnopqrstuvwxyz" + (i % 26), 1);
		}   
	}
	file.close();
} 

bool framesEquality(const std::map<PageId, Frame*>& a, const std::map<PageId, Frame*>& b)
{
	for (auto it = a.cbegin(); it != a.cend(); it++)
		if (b.find(it->first) == b.cend())
			return false;
	return true;
}

// TESTS DES FONCTIONS UTILITAIRES

// TESTS DE LA FONCTION GETPAGE

TEST(BufferManagerTest, testFrameVide){
	int idx = 0;
	createRandFile(idx);
	PageId id = (PageId){ .FileIdx = idx, .PageIdx = 0};
	BufferManager *BM = BufferManager::getInstance();
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'a'));
	id.PageIdx = 1; 
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'b'));    
	remove(getFilename(idx).c_str());
	BufferManager::resetInstance();
}

TEST(BufferManagerTest, testFrameEnDouble){
	int idx = 0;
	createRandFile(idx);
	PageId id = (PageId){ .FileIdx = idx, .PageIdx = 0};
	BufferManager *BM = BufferManager::getInstance();
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'a'));
	id.PageIdx = 1; 
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'b'));
	auto frames = BM->getFrames();
	frames[id]->pinCount = 0;
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'b'));
	id.PageIdx = 0;
	frames[id]->pinCount = 0;
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'a'));
	remove(getFilename(idx).c_str());
	BufferManager::resetInstance();
}

TEST(BufferManagerTest, testPolRempl){
	int idx = 0;
	createRandFile(idx, 4);
	PageId id = (PageId){ .FileIdx = idx, .PageIdx = 0};
	BufferManager *BM = BufferManager::getInstance();
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'a'));
	id.PageIdx = 1; 
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'b'));
	auto frames = BM->getFrames();
	frames[id]->pinCount = 0;
	frames[id]->refBit = 1;
	id.PageIdx = 3;
	EXPECT_EQ(BM->GetPage(id), nullptr);
	id.PageIdx = 1;
	EXPECT_EQ(frames[id]->refBit, 0);
	id.PageIdx = 3;
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'd'));
	id.PageIdx = 0;
	frames[id]->pinCount = 0;
	frames[id]->refBit = 1;
	id.PageIdx = 2;
	EXPECT_EQ(BM->GetPage(id), nullptr);
	id.PageIdx = 0;
	EXPECT_EQ(frames[id]->refBit, 0);
	id.PageIdx = 2;
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'c'));
	BufferManager::resetInstance();
	remove(getFilename(idx).c_str());
}

TEST(BufferManagerTest, testEcriture){
	int idx = 0;
	createRandFile(idx, 4);
	PageId id = (PageId){ .FileIdx = idx, .PageIdx = 0};
	BufferManager *BM = BufferManager::getInstance();
	DiskManager *DM = DiskManager::getInstance();


	std::vector<char> *t1 = BM->GetPage(id);
	auto frames = BM->getFrames();
	EXPECT_EQ(*t1, std::vector<char>(DBParams::pageSize, 'a'));

	for (int i = 0; i < t1->size(); i++)
		t1->at(i) = '0';
	std::vector<char> old = std::vector<char>(*t1);
	EXPECT_EQ(old, *frames[id]->pageDisk);

	id.PageIdx = 1;
	t1 = BM->GetPage(id);
	EXPECT_EQ(*t1, std::vector<char>(DBParams::pageSize, 'b')); 
	for (int i = 0; i < t1->size(); i++)
		t1->at(i) = '1';
	std::vector<char> old1 = std::vector<char>(*t1);

	id.PageIdx = 0;
	frames[id]->pinCount = 0;
	frames[id]->refBit = 1;
	frames[id]->dirty = 1;
	id.PageIdx = 3;
	EXPECT_EQ(BM->GetPage(id), nullptr);
	id.PageIdx = 0;
	EXPECT_EQ(frames[id]->refBit, 0);
	id.PageIdx = 3;
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'd'));
	id.PageIdx = 0;
	std::vector<char> testing(DBParams::pageSize);
	DM->ReadPage(id, testing.data());
	EXPECT_EQ(testing, old);


	// id.PageIdx = 1;
	// frames[id]->pinCount = 0;
	// frames[id]->refBit = 1;
	// frames[id]->dirty = 1;
	// id.PageIdx = 2;
	// EXPECT_EQ(BM->GetPage(id), nullptr);
	// id.PageIdx = 1;
	// EXPECT_EQ(frames[id]->refBit, 0);
	// id.PageIdx = 2;
	// EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'c'));
	// id.PageIdx = 1;
	// testing.clear();
	// testing.resize(DBParams::pageSize);
	// DM->ReadPage(id, testing.data());
	// EXPECT_EQ(testing, old1);

	remove(getFilename(idx).c_str());
	BufferManager::resetInstance();
}

TEST(BufferManagerTest, testFreePage){
	int idx = 0;
	createRandFile(0);
	PageId id = (PageId){ .FileIdx = idx, .PageIdx = 0};
	BufferManager *BM = BufferManager::getInstance();
	auto frame = BM->getFrames();
	frame[id] = new Frame;

	auto pd = frame[id]->pageDisk;

	frame[id]->pinCount = 1;
	frame[id]->refBit = 1;
	frame[id]->pageDisk = new std::vector<char> ();
	frame[id]->dirty = 0;
	BM->setFrames(frame);
	BM->FreePage(id, false);
	EXPECT_EQ(frame[id]->pinCount, 0);
	EXPECT_EQ(frame[id]->refBit, 1);
	EXPECT_NE(frame[id]->pageDisk, pd);
	BufferManager::resetInstance();
	remove(getFilename(idx).c_str());
}


TEST(BufferManagerTest, testFlushAll){
	int idx = 0;
	createRandFile(idx);
	PageId id = (PageId){ .FileIdx = idx, .PageIdx = 0};
	BufferManager *BM = BufferManager::getInstance();
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'a'));
	id.PageIdx = 1; 
	EXPECT_EQ(*BM->GetPage(id), std::vector<char>(DBParams::pageSize, 'b'));    
	BM->FlushAll();
	EXPECT_TRUE(BM->getFrames().empty());
	remove(getFilename(idx).c_str());
	BufferManager::resetInstance();

}










