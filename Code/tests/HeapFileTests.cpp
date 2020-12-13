#include <limits.h>
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "DiskManager.hpp"
#include "HeapFile.hpp"
#include "testsUtils.hpp"
#include "Rid.hpp"

class HeapFileTests : public ::testing ::Test
{
protected:
	virtual void setUp()
	{
	}

	virtual void tearDown()
	{
	}
};

//TEST DE LA FONCTION CREATENEWONDISK
TEST(HeapFileTests, testCreateNewOnDisk)
{
	RelationInfo rel = createRelation(1);
	HeapFile HF(rel);
	HF.createNewOnDisk();
	std::ifstream ifs(getFilename(rel.fileIdx));
	EXPECT_TRUE(ifs.is_open());
	remove(getFilename(rel.fileIdx).c_str());
	BufferManager::resetInstance();
}

//TEST DE LA FONCTION ADDDATAPAGE
TEST(HeapFileTests, testAddDataPage)
{
	DiskManager *DM = DiskManager::getInstance();
	BufferManager *BM = BufferManager::getInstance();
	RelationInfo rel = createRelation(5);
	HeapFile HF(rel);
	DM->CreateFile(rel.fileIdx);
	DM->AddPage(rel.fileIdx);

	std::vector<char> oldHeader = *HF.loadHeader(BM);
	HF.freeHeader(BM, false);
	HF.addDataPage();
	std::vector<char> newHeader = *HF.loadHeader(BM);
	HF.freeHeader(BM, false);
	EXPECT_NE(oldHeader, newHeader);

	std::vector<char> id(oldHeader.begin(), oldHeader.begin() + 4);
	int oldId = *(reinterpret_cast<const int *>(id.data()));

	id = std::vector<char>(newHeader.begin(), newHeader.begin() + 4);
	int newId = *(reinterpret_cast<const int *>(id.data()));
	EXPECT_EQ(newId, oldId + 1);

	int DPi = rel.slotCount;
	auto lastPos = newHeader.begin() + newId * 4;
	id = std::vector<char>(lastPos, lastPos + 4);
	int newDPi = *(reinterpret_cast<const int *>(id.data()));

	EXPECT_EQ(newDPi, DPi);

	BufferManager::resetInstance();
	remove(getFilename(rel.fileIdx).c_str());
}

//TEST DE LA FONCTION GETFREEPAGEID

TEST(HeapFileTests, testGetFreeDataPageIdFirst)
{
	DiskManager *DM = DiskManager::getInstance();
	BufferManager *BM = BufferManager::getInstance();
	RelationInfo rel = createRelation(5);
	HeapFile HF(rel);

	DM->CreateFile(rel.fileIdx);
	DM->AddPage(rel.fileIdx);

	std::vector<char> *header = HF.loadHeader(BM);

	int id = 5;
	const char *idc = reinterpret_cast<const char *>(&id);
	for (int i = 0; i < 4; i++)
		header->at(i) = idc[i];

	int sc = 115;
	idc = reinterpret_cast<const char *>(&sc);
	for (int i = 1; i <= id; i++)
	{
		for (int j = 0; j < 4; j++)
			header->at(i * 4 + j) = idc[j];
		sc = 0;
		idc = reinterpret_cast<const char *>(&sc);
	}
	HF.freeHeader(BM, true);

	header = HF.loadHeader(BM);

	PageId pid = HF.getFreeDataPageId();

	EXPECT_EQ(pid.PageIdx, 1);

	DM->resetInstance();
	BM->resetInstance();
	remove(getFilename(rel.fileIdx).c_str());
}

TEST(HeapFileTests, testGetFreeDataPageIdLast)
{
	DiskManager *DM = DiskManager::getInstance();
	BufferManager *BM = BufferManager::getInstance();
	RelationInfo rel = createRelation(5);
	HeapFile HF(rel);

	DM->CreateFile(rel.fileIdx);
	DM->AddPage(rel.fileIdx);

	std::vector<char> *header = HF.loadHeader(BM);

	int id = 5;
	const char *idc = reinterpret_cast<const char *>(&id);
	for (int i = 0; i < 4; i++)
		header->at(i) = idc[i];

	int sc = 0;
	idc = reinterpret_cast<const char *>(&sc);
	for (int i = 1; i <= id; i++)
	{
		for (int j = 0; j < 4; j++)
			header->at(i * 4 + j) = idc[j];
		sc = (i + 1 == id ? 155 : 0);
		idc = reinterpret_cast<const char *>(&sc);
	}
	HF.freeHeader(BM, true);

	header = HF.loadHeader(BM);

	PageId pid = HF.getFreeDataPageId();

	EXPECT_EQ(pid.PageIdx, id);

	DM->resetInstance();
	BM->resetInstance();
	remove(getFilename(rel.fileIdx).c_str());
}

TEST(HeapFileTests, testGetFreeDataPageIdRandom)
{
	std::srand(std::time(nullptr));
	DiskManager *DM = DiskManager::getInstance();
	BufferManager *BM = BufferManager::getInstance();
	RelationInfo rel = createRelation(5);
	HeapFile HF(rel);

	DM->CreateFile(rel.fileIdx);
	DM->AddPage(rel.fileIdx);

	std::vector<char> *header = HF.loadHeader(BM);

	int id = 5;
	const char *idc = reinterpret_cast<const char *>(&id);
	for (int i = 0; i < 4; i++)
		header->at(i) = idc[i];

	int sc = 0;
	int first = 2;
	idc = reinterpret_cast<const char *>(&sc);
	for (int i = 1; i <= id; i++)
	{
		for (int j = 0; j < 4; j++)
			header->at(i * 4 + j) = idc[j];
		if (i + 1 == first)
		{
			sc = 552;
			idc = reinterpret_cast<const char *>(&sc);
		}
	}
	HF.freeHeader(BM, true);

	header = HF.loadHeader(BM);

	PageId pid = HF.getFreeDataPageId();

	EXPECT_EQ(pid.PageIdx, first);

	DM->resetInstance();
	BM->resetInstance();
	remove(getFilename(rel.fileIdx).c_str());
}

//TEST DE LA FONCTION TEST DE LA MÉTHODE writeRecordToDataPage

TEST(HeapFileTests, testWriteRecordToDataPage)
{
	DiskManager *DM = DiskManager::getInstance();
	BufferManager *BM = BufferManager::getInstance();
	RelationInfo rel = createRelation(5);
	HeapFile HF(rel);
	Record rc(rel);

	rc.setValues(createValues(rel));

	DM->CreateFile(rel.fileIdx);
	DM->AddPage(rel.fileIdx);

	std::vector<char> *header = HF.loadHeader(BM);
	int id = 1;
	int DPi = rel.slotCount;

	const char *idc = reinterpret_cast<const char *>(&id);
	const char *DPic = reinterpret_cast<const char *>(&DPi);
	for (int i = 0; i < 4; i++)
	{
		header->at(i) = idc[i];
		header->at(i + 4) = DPic[i];
	}
	HF.freeHeader(BM, true);

	PageId pid = DM->AddPage(rel.fileIdx);

	Rid rid = HF.writeRecordToDataPage(rc, pid);
	header = HF.loadHeader(BM);
	std::vector<char> idv(header->begin() + 4, header->begin() + 8);
	int newDPi = *(reinterpret_cast<const int *>(idv.data()));
	EXPECT_EQ(rid.slotIdx, 0);
	EXPECT_EQ(newDPi, DPi - 1);
	DPi = newDPi;
	HF.freeHeader(BM, false);

	rid = HF.writeRecordToDataPage(rc, pid);
	header = HF.loadHeader(BM);
	idv = std::vector<char>(header->begin() + 4, header->begin() + 8);
	newDPi = *(reinterpret_cast<const int *>(idv.data()));
	EXPECT_EQ(rid.slotIdx, 1);
	EXPECT_EQ(newDPi, DPi - 1);

	BM->resetInstance();
	DM->resetInstance();
	remove(getFilename(rel.fileIdx).c_str());
}

//TEST DE LA FONCTION TEST DE LA MÉTHODE getRecordsInDataPage

TEST(HeapFileTests, testGetRecordsInDataPageEmptyPage)
{
	DiskManager *DM = DiskManager::getInstance();
	BufferManager *BM = BufferManager::getInstance();
	RelationInfo rel = createRelation(5);
	HeapFile HF(rel);
	Record rc(rel);

	rc.setValues(createValues(rel));

	DM->CreateFile(rel.fileIdx);
	DM->AddPage(rel.fileIdx);
	DM->AddPage(rel.fileIdx);

	std::vector<char> *header = HF.loadHeader(BM);
	int id = 1;
	int DPi = rel.slotCount;

	const char *idc = reinterpret_cast<const char *>(&id);
	const char *DPic = reinterpret_cast<const char *>(&DPi);
	for (int i = 0; i < 4; i++)
	{
		header->at(i) = idc[i];
		header->at(i + 4) = DPic[i];
	}
	HF.freeHeader(BM, true);

	std::vector<Record> records = HF.getRecordsInDataPage((PageId){.FileIdx = rel.fileIdx, .PageIdx = 1});

	EXPECT_TRUE(records.empty());

	BM->resetInstance();
	DM->resetInstance();
	remove(getFilename(rel.fileIdx).c_str());
}

TEST(HeapFileTests, testGetRecordsInDataPage)
{
	int nb = 10;
	DiskManager *DM = DiskManager::getInstance();
	BufferManager *BM = BufferManager::getInstance();
	RelationInfo rel = createRelation(5);
	HeapFile HF(rel);
	Record rc(rel);

	rc.setValues(createValues(rel));

	DM->CreateFile(rel.fileIdx);
	DM->AddPage(rel.fileIdx);
	DM->AddPage(rel.fileIdx);


	std::vector<char> *header = HF.loadHeader(BM);
	int id = 1;
	int DPi = rel.slotCount - nb;

	const char *idc = reinterpret_cast<const char *>(&id);
	const char *DPic = reinterpret_cast<const char *>(&DPi);
	for (int i = 0; i < 4; i++)
	{
		header->at(i) = idc[i];
		header->at(i + 4) = DPic[i];
	}
	HF.freeHeader(BM, true);

	PageId pid = (PageId){.FileIdx = rel.fileIdx, .PageIdx = 1};

	std::vector<char> *raw = BM->GetPage(pid);
	for (int i = 0; i < nb; i++)
	{
		Record rc(rel);
		rc.setValues(createValues(rel));
		rc.writeToBuffer(*raw, i * rel.recordSize);
	}
	BM->FreePage(pid, true);

	std::vector<Record> records = HF.getRecordsInDataPage(pid);

	EXPECT_EQ((int)records.size(), nb);

	BM->resetInstance();
	DM->resetInstance();
	remove(getFilename(rel.fileIdx).c_str());
}

//TEST DE LA FONCTION TEST DE LA MÉTHODE GetAllRecords

TEST(HeapFileTests, testGetAllRecordsEmptyPage)
{
	DiskManager *DM = DiskManager::getInstance();
	BufferManager *BM = BufferManager::getInstance();
	RelationInfo rel = createRelation(5);
	HeapFile HF(rel);
	Record rc(rel);

	rc.setValues(createValues(rel));

	DM->CreateFile(rel.fileIdx);
	DM->AddPage(rel.fileIdx);
	DM->AddPage(rel.fileIdx);
	DM->AddPage(rel.fileIdx);

	std::vector<char> *header = HF.loadHeader(BM);
	int id = 2;
	int DPi = rel.slotCount;

	const char *idc = reinterpret_cast<const char *>(&id);
	const char *DPic = reinterpret_cast<const char *>(&DPi);
	for (int i = 0; i < 4; i++)
	{
		header->at(i) = idc[i];
		header->at(i + 4) = DPic[i];
		header->at(i + 8) = DPic[i];
	}
	HF.freeHeader(BM, true);

	std::vector<Record> records = HF.GetAllRecords();

	EXPECT_TRUE(records.empty());

	BM->resetInstance();
	DM->resetInstance();
	remove(getFilename(rel.fileIdx).c_str());
}

TEST(HeapFileTests, testGetAllRecords)
{
	int nb = 10;
	DiskManager *DM = DiskManager::getInstance();
	BufferManager *BM = BufferManager::getInstance();
	RelationInfo rel = createRelation(5);
	HeapFile HF(rel);
	Record rc(rel);

	rc.setValues(createValues(rel));

	DM->CreateFile(rel.fileIdx);
	DM->AddPage(rel.fileIdx);
	DM->AddPage(rel.fileIdx);
	DM->AddPage(rel.fileIdx);

	std::vector<char> *header = HF.loadHeader(BM);
	int id = 2;
	int DPi = rel.slotCount - nb;

	const char *idc = reinterpret_cast<const char *>(&id);
	const char *DPic = reinterpret_cast<const char *>(&DPi);
	for (int i = 0; i < 4; i++)
	{
		header->at(i) = idc[i];
		header->at(i + 4) = DPic[i];
		header->at(i + 8) = DPic[i];
	}
	HF.freeHeader(BM, true);

	PageId pid = (PageId){.FileIdx = rel.fileIdx, .PageIdx = 1};

	std::vector<char> *raw = BM->GetPage(pid);
	for (int i = 0; i < nb; i++)
	{
		Record rc(rel);
		rc.setValues(createValues(rel));
		rc.writeToBuffer(*raw, i * rel.recordSize);
	}
	BM->FreePage(pid, true);

	pid = (PageId){.FileIdx = rel.fileIdx, .PageIdx = 2};

	raw = BM->GetPage(pid);
	if (!raw)
		raw = BM->GetPage(pid);
	for (int i = 0; i < nb; i++)
	{
		Record rc(rel);
		rc.setValues(createValues(rel));
		rc.writeToBuffer(*raw, i * rel.recordSize);
	}
	BM->FreePage(pid, true);

	std::vector<Record> records = HF.GetAllRecords();

	EXPECT_EQ((int)records.size(), nb * 2);

	BM->resetInstance();
	DM->resetInstance();
	remove(getFilename(rel.fileIdx).c_str());
}
