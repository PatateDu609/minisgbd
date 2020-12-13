#include "FileManager.hpp"
#include "DBInfo.hpp"

struct HeapFileRelInfoComparator : public std::unary_function<std::string, bool>
{
	const RelationInfo &relInfo;

	HeapFileRelInfoComparator(const RelationInfo &rel) : relInfo(rel)
	{
	}

	bool operator()(const HeapFile &HF)
	{
		return HF.relInfo == relInfo;
	}
};

FileManager *FileManager::INSTANCE = NULL;

FileManager::FileManager()
{
}

FileManager *FileManager::getInstance()
{
	if (!INSTANCE)
		INSTANCE = new FileManager();
	return INSTANCE;
}

void FileManager::resetInstance()
{
	if (INSTANCE)
		delete INSTANCE;
	INSTANCE = NULL;
}

void FileManager::Init()
{
	std::vector<RelationInfo> infos = DBInfo::getInstance()->getInfo();

	heapFiles.reserve(infos.size());
	for (RelationInfo info : infos)
		heapFiles.emplace_back(info);
}

void FileManager::CreateRelationFile(RelationInfo &relInfo)
{
	heapFiles.emplace_back(relInfo);
	heapFiles.back().createNewOnDisk();
}

Rid FileManager::InsertRecordInRelation(Record &record, RelationInfo &relName)
{
	return std::find_if(heapFiles.begin(), heapFiles.end(), HeapFileRelInfoComparator(relName))->InsertRecord(record);
}

std::vector<Record> FileManager::SelectAllFromRelation(RelationInfo &relName)
{
	return std::find_if(heapFiles.begin(), heapFiles.end(), HeapFileRelInfoComparator(relName))->GetAllRecords();
}

void FileManager::reset()
{
	for (HeapFile HF : heapFiles)
		remove(getFilename(HF.relInfo.fileIdx).c_str());
	heapFiles.clear();
}

void FileManager::clean()
{
	for (HeapFile HF : heapFiles)
	{
		remove(getFilename(HF.relInfo.fileIdx).c_str());
		HF.createNewOnDisk();
	}
}
