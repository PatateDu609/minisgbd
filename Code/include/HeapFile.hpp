#ifndef HEAPFILE_HPP
#define HEAPFILE_HPP

#include <gtest/gtest.h>
#include "RelationInfo.hpp"
#include "BufferManager.hpp"
#include "Rid.hpp"
#include "Record.hpp"
#include <vector>

class HeapFileTests;

class HeapFile
{
private:
	const RelationInfo& relInfo;

	std::vector<char> *loadHeader(BufferManager *BM);
	void freeHeader(BufferManager *BM, bool dirty);

	void createNewOnDisk();
	PageId addDataPage();
	PageId getFreeDataPageId();
	Rid writeRecordToDataPage(const Record& rc, const PageId& pageId);
	std::vector<Record> getRecordsInDataPage(const PageId& pageId);

	FRIEND_TEST(HeapFileTests, testCreateNewOnDisk);
	FRIEND_TEST(HeapFileTests, testAddDataPage);
	FRIEND_TEST(HeapFileTests, testGetFreeDataPageIdFirst);
	FRIEND_TEST(HeapFileTests, testGetFreeDataPageIdLast);
	FRIEND_TEST(HeapFileTests, testGetFreeDataPageIdRandom);
	FRIEND_TEST(HeapFileTests, testWriteRecordToDataPage);

public:
	HeapFile(const RelationInfo& rel);
	Rid InsertRecord(Record rc);
	std::vector<Record> GetAllRecords();
};

#endif