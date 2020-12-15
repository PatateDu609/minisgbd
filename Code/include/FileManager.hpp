#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include "HeapFile.hpp"

class FileManager
{
private:
	std::vector<HeapFile> heapFiles;

	static FileManager *INSTANCE;

	FileManager();

public:
	static FileManager *getInstance();
	static void resetInstance();

	void Init();
	void CreateRelationFile(RelationInfo &relInfo);
	Rid InsertRecordInRelation(Record &record, RelationInfo &relName);
	std::vector<Record> SelectAllFromRelation(RelationInfo &relName);

	void updateRecords(RelationInfo& relName, const std::vector<Record>& records);

	void reset();
	void clean();
};

#endif
