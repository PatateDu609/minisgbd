#include <limits.h>
#include <gtest/gtest.h>
#include "DiskManager.hpp"
#include "DBParams.hpp"
#include <cstdio>
#include <fstream>
#include <string>

class DiskManagerTest : public ::testing::Test
{
protected:
	virtual void setUp()
	{

	}

	virtual void tearDown()
	{

	}
};

// TESTS SUR LES FONCTIONS UTILITAIRES...

TEST(DiskManagerTest, testGetFilename)
{
	EXPECT_EQ(getFilename(0), "DB/Tests/Data_0.rf");
	EXPECT_EQ(getFilename(242), "DB/Tests/Data_242.rf");
	EXPECT_EQ(getFilename(3788), "DB/Tests/Data_3788.rf");
	EXPECT_EQ(getFilename(100), "DB/Tests/Data_100.rf");
	EXPECT_EQ(getFilename(46), "DB/Tests/Data_46.rf");
}

TEST(DiskManagerTest, testFileExists)
{
	EXPECT_TRUE(fileExists("Makefile"));
	remove("bonjour");
	EXPECT_FALSE(fileExists("bonjour"));
	EXPECT_TRUE(fileExists("Code/src/main.cpp"));
	EXPECT_TRUE(fileExists("Code/src/DBManager.cpp"));
}

// TESTS SUR LA FONCTION CREATEFILE

TEST(DiskManagerTest, testCreateFileStandard)
{
	DiskManager *DM = DiskManager::getInstance();
	int idx = 0;

	remove(getFilename(idx).c_str());

	DM->CreateFile(idx);
	ASSERT_TRUE(fileExists(getFilename(idx)));

	std::fstream file(getFilename(idx), std::ios::in | std::ios::ate);

	EXPECT_EQ(file.tellg(), 0);

	DiskManager::resetInstance();
	file.close();
	remove(getFilename(idx).c_str());
}

TEST(DiskManagerTest, testCreateFileWithExistingFile)
{
	DiskManager *DM = DiskManager::getInstance();
	int idx = 0;

	std::fstream file(getFilename(idx), std::ios::out);
	file.write("Hello World !", 13);
	file.close();

	std::stringstream buf;
	std::streambuf *old = std::cerr.rdbuf(buf.rdbuf());
	DM->CreateFile(idx);
	std::string printed = buf.str();
	std::cerr.rdbuf(old);

	EXPECT_EQ(printed, "ERREUR : Le fichier Data_" + std::to_string(idx) + ".rf existe déjà!\n");

	remove(getFilename(idx).c_str());
	DiskManager::resetInstance();
}

// TESTS SUR LA FONCTION ADDPAGE

TEST(DiskManagerTest, testAddPageStandard)
{
	DiskManager *DM = DiskManager::getInstance();
	PageId pageId;
	int idx = 0;
	bool valid = true;
	char content[DBParams::pageSize];
	size_t size;

	std::fstream file(getFilename(idx), std::ios::out);
	file.close();

	pageId = DM->AddPage(idx);
	size = sizeFile(getFilename(idx));
	EXPECT_EQ(DBParams::pageSize, size);
	EXPECT_EQ(pageId.FileIdx, 0);
	EXPECT_EQ(pageId.PageIdx, 0);

	file.open(getFilename(idx), std::ios::in);
	file.read(content, DBParams::pageSize);
	for (char c : content)
	{
		if (c != 0)
		{
			valid = false;
			break;
		}
	}
	file.close();

	EXPECT_TRUE(valid);
	pageId = DM->AddPage(idx);
	size = sizeFile(getFilename(idx));
	EXPECT_EQ(DBParams::pageSize * 2, size);
	EXPECT_EQ(pageId.FileIdx, 0);
	EXPECT_EQ(pageId.PageIdx, 1);

	file.open(getFilename(idx), std::ios::in);
	file.seekg(DBParams::pageSize);
	file.read(content, DBParams::pageSize);
	for (char c : content)
	{
		if (c != 0)
		{
			valid = false;
			break;
		}
	}
	EXPECT_TRUE(valid);
	file.close();

	remove(getFilename(idx).c_str());
	DiskManager::resetInstance();
}

TEST(DiskManagerTest, testAddPageWrongFile)
{
	DiskManager *DM = DiskManager::getInstance();

	PageId pageId = DM->AddPage(1);
	EXPECT_EQ(-1, pageId.FileIdx);
	EXPECT_EQ(-1, pageId.PageIdx);

	DiskManager::resetInstance();
}
