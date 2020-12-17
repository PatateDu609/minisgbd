#ifndef HEAPFILE_HPP
#define HEAPFILE_HPP

#include <gtest/gtest.h>
#include "RelationInfo.hpp"
#include "BufferManager.hpp"
#include "Rid.hpp"
#include "Record.hpp"
#include "BpTree.hpp"
#include <vector>
#include <utility>

class HeapFile
{
private:
	friend class FileManager;
	friend struct HeapFileRelInfoComparator;

	std::map<std::string, BpTree*> indexes;

	RelationInfo relInfo;

	int extractKey(const std::string& key, const Rid& rid);
	Record getRecord(const Rid& rid);

	std::vector<char> *loadHeader(BufferManager *BM);
	void freeHeader(BufferManager *BM, bool dirty);

	void createNewOnDisk();
	PageId addDataPage();
	PageId getFreeDataPageId();
	Rid writeRecordToDataPage(Record &rc, const PageId &pageId);
	std::vector<Record> getRecordsInDataPage(const PageId &pageId);
	void updateRecords(std::vector<Record> records);

	void createIndex(const std::string& key, int order);
	std::vector<std::pair<int, std::vector<Rid>>> loadBulk(const PageId& pid, const std::string& key);
	std::vector<Record> selectIndex(const std::string& key, int value);

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
	~HeapFile();

	Rid InsertRecord(Record &rc);
	std::vector<Record> GetAllRecords();
};

#endif
