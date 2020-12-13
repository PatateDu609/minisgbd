#ifndef HEAPFILE_HPP
#define HEAPFILE_HPP

#include <gtest/gtest.h>
#include "RelationInfo.hpp"
#include "BufferManager.hpp"
#include "Rid.hpp"
#include "Record.hpp"
#include <vector>

class HeapFile
{
private:
	friend class FileManager;
	friend struct HeapFileRelInfoComparator;

	RelationInfo relInfo;

	std::vector<char> *loadHeader(BufferManager *BM);
	void freeHeader(BufferManager *BM, bool dirty);

	void createNewOnDisk();
	PageId addDataPage();
	PageId getFreeDataPageId();
	Rid writeRecordToDataPage(Record &rc, const PageId &pageId);
	std::vector<Record> getRecordsInDataPage(const PageId &pageId);

	FRIEND_TEST(HeapFileTests, testCreateNewOnDisk);
	FRIEND_TEST(HeapFileTests, testAddDataPage);
	FRIEND_TEST(HeapFileTests, testGetFreeDataPageIdFirst);
	FRIEND_TEST(HeapFileTests, testGetFreeDataPageIdLast);
	FRIEND_TEST(HeapFileTests, testGetFreeDataPageIdRandom);
	FRIEND_TEST(HeapFileTests, testWriteRecordToDataPage);
	FRIEND_TEST(HeapFileTests, testGetRecordsInDataPageEmptyPage);
	FRIEND_TEST(HeapFileTests, testGetRecordsInDataPage);

	FRIEND_TEST(HeapFileTests, testGetAllRecordsEmptyPage);
	FRIEND_TEST(HeapFileTests, testGetAllRecords);

public:
	HeapFile(RelationInfo &rel);

	Rid InsertRecord(Record &rc);
	std::vector<Record> GetAllRecords();
};

#endif
