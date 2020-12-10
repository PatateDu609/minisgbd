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

TEST(DiskManagerTest, testPageExists)
{
	DiskManager *DM = DiskManager::getInstance();
	char BUFFER[DBParams::pageSize];
	int NEEDED = 100;
	int idx = 0;

	std::fstream file(getFilename(idx), std::ios::out | std::ios::trunc | std::ios::binary);
	for (int page = 0; page < NEEDED; page++)
	{
		for (int i = 0; i < DBParams::pageSize; i++)
			BUFFER[i] = 6;
		file.write(BUFFER, DBParams::pageSize);
	}
	file.close();
	PageId pageId;
	pageId.FileIdx = idx;
	pageId.PageIdx = 5;
	EXPECT_TRUE(DM->PageExists(pageId));
	pageId.PageIdx = 60;
	EXPECT_TRUE(DM->PageExists(pageId));
	pageId.PageIdx = 0;
	EXPECT_TRUE(DM->PageExists(pageId));
	pageId.PageIdx = 99;
	EXPECT_TRUE(DM->PageExists(pageId));
	pageId.PageIdx = 100;
	EXPECT_FALSE(DM->PageExists(pageId));
	remove(getFilename(idx).c_str());

	idx = 10;
	NEEDED = 1000;
	file.open(getFilename(idx), std::ios::out | std::ios::trunc | std::ios::binary);
	for (int page = 0; page < NEEDED; page++)
	{
		for (int i = 0; i < DBParams::pageSize; i++)
			BUFFER[i] = 6;
		file.write(BUFFER, DBParams::pageSize);
	}
	file.close();
	pageId.FileIdx = idx;
	pageId.PageIdx = 65;
	EXPECT_TRUE(DM->PageExists(pageId));
	pageId.PageIdx = 122;
	EXPECT_TRUE(DM->PageExists(pageId));
	pageId.PageIdx = 999;
	EXPECT_TRUE(DM->PageExists(pageId));
	pageId.PageIdx = 0;
	EXPECT_TRUE(DM->PageExists(pageId));
	pageId.PageIdx = 4656;
	EXPECT_FALSE(DM->PageExists(pageId));
	remove(getFilename(idx).c_str());
	DM->resetInstance();
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
	int size;

	std::fstream file(getFilename(idx), std::ios::out);
	file.close();

	pageId = DM->AddPage(idx);
	size = (int)sizeFile(getFilename(idx));
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

// TESTS SUR LA FONCTION READPAGE

TEST(DiskManagerTest, testReadPageStandard)
{
	std::srand(std::time(NULL));
	DiskManager *DM = DiskManager::getInstance();
	std::vector<char *> BUFFERS;
	char *BUFFER;
	int NEEDED = 100;
	int idx = 0;

	std::fstream file(getFilename(idx), std::ios::out | std::ios::trunc | std::ios::binary);
	for (int page = 0; page < NEEDED; page++)
	{
		BUFFER = new char[DBParams::pageSize];
		for (int i = 0; i < DBParams::pageSize; i++)
			BUFFER[i] = std::rand() % 128;
		BUFFERS.push_back(BUFFER);
		file.write(BUFFER, DBParams::pageSize);
	}
	file.close();

	char TEST[DBParams::pageSize];
	PageId pageId = {.FileIdx = idx, 0};
	int i = 0;
	for (int page = 0; page < NEEDED; page++)
	{
		pageId.PageIdx = page;
		DM->ReadPage(pageId, TEST);
		for (i = 0; i < DBParams::pageSize && BUFFERS[page][i] == TEST[i]; i++)
			;
		EXPECT_EQ(i, DBParams::pageSize);
	}

	DM->resetInstance();
	remove(getFilename(idx).c_str());
	for (char *v : BUFFERS)
		delete[] v;
}

TEST(DiskManagerTest, testReadPageWrongFile)
{
	DiskManager *DM = DiskManager::getInstance();
	int fdx = 0;
	PageId pageId = {.FileIdx = fdx, .PageIdx = 0};

	std::stringstream buffer;
	std::streambuf *old = std::cerr.rdbuf(buffer.rdbuf());
	char buf[DBParams::pageSize];

	DM->ReadPage(pageId, buf);
	EXPECT_EQ(buffer.str(), "ERREUR : Le fichier Data_" + std::to_string(fdx) + ".rf n'a pas pu être ouvert !\n");

	DM->resetInstance();
	std::cerr.rdbuf(old);
}

TEST(DiskManagerTest, testReadPageWrongPage)
{
	std::srand(std::time(NULL));
	DiskManager *DM = DiskManager::getInstance();
	std::vector<char *> BUFFERS;
	char *BUFFER;
	int NEEDED = 100;
	int idx = 0;
	PageId pageId;
	std::fstream file(getFilename(idx), std::ios::out | std::ios::trunc | std::ios::binary);

	pageId.FileIdx = idx;
	for (int page = 0; page < NEEDED; page++)
	{
		BUFFER = new char[DBParams::pageSize];
		for (int i = 0; i < DBParams::pageSize; i++)
			BUFFER[i] = std::rand() % 128;
		BUFFERS.push_back(BUFFER);
		file.write(BUFFER, DBParams::pageSize);
	}
	file.close();

	std::stringstream buffer;
	std::streambuf *old = std::cerr.rdbuf(buffer.rdbuf());
	char buf[DBParams::pageSize];
	pageId.PageIdx = NEEDED;
	DM->ReadPage(pageId, buf);
	EXPECT_EQ(buffer.str(), "ERREUR : La page demandée n'existe pas dans le fichier Data_" + std::to_string(idx) + ".rf !\n");

	buffer.str("");
	pageId.PageIdx = NEEDED + 1;
	DM->ReadPage(pageId, buf);
	EXPECT_EQ(buffer.str(), "ERREUR : La page demandée n'existe pas dans le fichier Data_" + std::to_string(idx) + ".rf !\n");

	DM->resetInstance();
	std::cerr.rdbuf(old);
	remove(getFilename(idx).c_str());
	for (char *v : BUFFERS)
		delete[] v;
}

// TESTS SUR LA FONCTION WRITEPAGE

TEST(DiskManagerTest, testWritePageStandard)
{
	std::srand(std::time(NULL));
	char *BUFFER;
	DiskManager *DM = DiskManager::getInstance();
	std::vector<char *> BUFFERS;
	int NEEDED = 100;
	PageId pageId;
	int idx = 0;
	char BUF[DBParams::pageSize];

	std::fstream file(getFilename(idx), std::ios::trunc | std::ios::out);
	bzero(BUF, DBParams::pageSize);
	for (int page = 0; page < NEEDED; page++)
		file.write(BUF, DBParams::pageSize);
	file.close();

	pageId.FileIdx = idx;
	for (int page = 0; page < NEEDED; page++)
	{
		pageId.PageIdx = page;
		BUFFER = new char[DBParams::pageSize];
		for (int i = 0; i < DBParams::pageSize; i++)
			BUFFER[i] = std::rand() % 128;
		BUFFERS.push_back(BUFFER);
		DM->WritePage(pageId, BUFFER);
	}

	ASSERT_EQ((int)sizeFile(getFilename(idx)), DBParams::pageSize * NEEDED);
	file.open(getFilename(idx), std::ios::in);
	int i;
	for (int page = 0; page < NEEDED; page++)
	{
		file.read(BUF, DBParams::pageSize);
		for (i = 0; i < DBParams::pageSize && BUF[i] == BUFFERS[page][i]; i++)
			;
		EXPECT_EQ(i, DBParams::pageSize);
	}
	file.close();
	DM->resetInstance();
	remove(getFilename(idx).c_str());
	for (char *v : BUFFERS)
		delete[] v;
}

TEST(DiskManagerTest, testWritePageWrongFile)
{
	DiskManager *DM = DiskManager::getInstance();
	int fdx = 0;
	PageId pageId = {.FileIdx = fdx, .PageIdx = 0};

	std::stringstream buffer;
	std::streambuf *old = std::cerr.rdbuf(buffer.rdbuf());
	char buf[DBParams::pageSize];

	DM->WritePage(pageId, buf);
	EXPECT_EQ(buffer.str(), "ERREUR : Le fichier Data_" + std::to_string(fdx) + ".rf n'a pas pu être ouvert !\n");

	DM->resetInstance();
	std::cerr.rdbuf(old);
}

TEST(DiskManagerTest, testWritePageWrongPage)
{
	std::srand(std::time(NULL));
	DiskManager *DM = DiskManager::getInstance();
	std::vector<char *> BUFFERS;
	char *BUFFER;
	int NEEDED = 100;
	int idx = 0;
	PageId pageId;
	std::fstream file(getFilename(idx), std::ios::out | std::ios::trunc | std::ios::binary);

	pageId.FileIdx = idx;
	for (int page = 0; page < NEEDED; page++)
	{
		BUFFER = new char[DBParams::pageSize];
		for (int i = 0; i < DBParams::pageSize; i++)
			BUFFER[i] = std::rand() % 128;
		BUFFERS.push_back(BUFFER);
		file.write(BUFFER, DBParams::pageSize);
	}
	file.close();

	std::stringstream buffer;
	std::streambuf *old = std::cerr.rdbuf(buffer.rdbuf());
	char buf[DBParams::pageSize];
	pageId.PageIdx = NEEDED;
	DM->WritePage(pageId, buf);
	EXPECT_EQ(buffer.str(), "ERREUR : La page demandée n'existe pas dans le fichier Data_" + std::to_string(idx) + ".rf !\n");

	buffer.str("");
	pageId.PageIdx = NEEDED + 1;
	DM->WritePage(pageId, buf);
	EXPECT_EQ(buffer.str(), "ERREUR : La page demandée n'existe pas dans le fichier Data_" + std::to_string(idx) + ".rf !\n");

	DM->resetInstance();
	std::cerr.rdbuf(old);
	remove(getFilename(idx).c_str());
	for (char *v : BUFFERS)
		delete[] v;
}
