#include <limits.h>
#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <string>
#include "DiskManager.hpp"
#include "DBParams.hpp"
#include "BufferManager.hpp"

class BufferManagerTest : public ::testing::Test
{
protected:
	virtual void setUp()
	{

	}

	virtual void tearDown()
	{

	}
};

void createRandFile(int idx){
    DiskManager *DM = DiskManager::getInstance();
    idx = 0;
	std::fstream file(getFilename(idx), std::ios::out);
    for(int i = 0; i < DBParams::pageSize; i++){
        file.write("a", 1);
    }
    for(int j = 0; j < DBParams::pageSize; j++){
        file.write("b", 1);
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
//  remove(getFilename(idx).c_str());
    BufferManager::resetInstance();
}





