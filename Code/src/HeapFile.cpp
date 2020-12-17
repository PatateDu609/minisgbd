#include "HeapFile.hpp"
#include "DiskManager.hpp"
#include "BufferManager.hpp"
#include <fstream>

HeapFile::HeapFile(RelationInfo &rel) : relInfo(rel)
{
}

HeapFile::~HeapFile()
{

}

void HeapFile::createNewOnDisk()
{
	DiskManager *DM = DiskManager::getInstance();

	DM->CreateFile(relInfo.fileIdx);
	DM->AddPage(relInfo.fileIdx);
}

std::vector<char> *HeapFile::loadHeader(BufferManager *BM)
{
	PageId headerId = {.FileIdx = relInfo.fileIdx, .PageIdx = 0};
	BM = BufferManager::getInstance();
	std::vector<char> *header = BM->GetPage(headerId);
	if (header == NULL)
		header = BM->GetPage(headerId);
	return header;
}

void HeapFile::freeHeader(BufferManager *BM, bool dirty)
{
	BM = BufferManager::getInstance();
	PageId headerId = {.FileIdx = relInfo.fileIdx, .PageIdx = 0};
	BM->FreePage(headerId, dirty);
}

PageId HeapFile::addDataPage()
{
	BufferManager *BM = BufferManager::getInstance();
	DiskManager *DM = DiskManager::getInstance();
	PageId result = DM->AddPage(relInfo.fileIdx);
	std::vector<char> *header = loadHeader(BM);

	std::vector<char> idv(header->begin(), header->begin() + 4);
	int id = *(reinterpret_cast<const int *>(idv.data()));

	id++;

	if (id > DBParams::pageSize / 4)
	{
		freeHeader(BM, false);
		return (PageId){.FileIdx = -1, .PageIdx = -1};
	}

	int DPi = relInfo.slotCount;

	const char *idc = reinterpret_cast<const char *>(&id);
	const char *DPic = reinterpret_cast<const char *>(&DPi);
	for (int i = 0; i < 4; i++)
	{
		header->at(i) = idc[i];
		header->at(i + id * 4) = DPic[i];
	}
	freeHeader(BM, true);

	return result;
}

PageId HeapFile::getFreeDataPageId()
{
	BufferManager *BM = BufferManager::getInstance();
	std::vector<char> *header = loadHeader(BM);

	std::vector<char> idv(header->begin(), header->begin() + 4);
	int id = *(reinterpret_cast<const int *>(idv.data()));

	for (int i = 1; i <= id; i++)
	{
		idv = std::vector<char>(header->begin() + (i * 4), header->begin() + (i * 4 + 4));
		int DPi = *(reinterpret_cast<const int *>(idv.data()));
		if (DPi)
		{
			freeHeader(BM, false);
			return (PageId){.FileIdx = relInfo.fileIdx, .PageIdx = i};
		}
	}
	freeHeader(BM, false);
	return (PageId){.FileIdx = -1, .PageIdx = -1};
}

Rid HeapFile::writeRecordToDataPage(Record &rc, const PageId &pageId)
{
	BufferManager *BM = BufferManager::getInstance();
	std::vector<char> *header = loadHeader(BM);

	std::vector<char> idv(header->begin() + (pageId.PageIdx * 4), header->begin() + (pageId.PageIdx * 4) + 4);
	int DPi = *(reinterpret_cast<const int *>(idv.data()));

	std::vector<char> *raw = BM->GetPage(pageId);
	if (raw == NULL)
		raw = BM->GetPage(pageId);
	rc.writeToBuffer(*raw, (relInfo.slotCount - DPi) * relInfo.recordSize);

	BM->FreePage(pageId, true);

	DPi--;
	const char *DPic = reinterpret_cast<const char *>(&DPi);
	for (int i = 0; i < 4; i++)
		header->at(pageId.PageIdx * 4 + i) = DPic[i];
	freeHeader(BM, true);

	return (Rid){.pageId = pageId, .slotIdx = relInfo.slotCount - DPi - 1};
}

std::vector<Record> HeapFile::getRecordsInDataPage(const PageId &pageId)
{
	BufferManager *BM = BufferManager::getInstance();

	std::vector<char> *header = loadHeader(BM);
	std::vector<char> idv(header->begin() + (pageId.PageIdx * 4), header->begin() + (pageId.PageIdx * 4) + 4);
	int DPi = *(reinterpret_cast<const int *>(idv.data()));
	freeHeader(BM, false);

	std::vector<char> *raw = BM->GetPage(pageId);
	if (raw == NULL)
		raw = BM->GetPage(pageId);

	std::vector<Record> records;
	for (int i = 0; i < relInfo.slotCount - DPi; i++)
	{
		int position = i * relInfo.recordSize;
		Record rc(relInfo);
		rc.setRid((Rid){ .pageId = pageId, .slotIdx = i });
		rc.readFromBuffer(*raw, position);
		records.push_back(rc);
	}
	BM->FreePage(pageId, false);
	return records;
}

Rid HeapFile::InsertRecord(Record &rc)
{
	PageId pid = getFreeDataPageId();
	if (pid.FileIdx == -1 || pid.PageIdx == -1)
		pid = addDataPage();
	return writeRecordToDataPage(rc, pid);
}

std::vector<Record> HeapFile::GetAllRecords()
{
	BufferManager *BM = BufferManager::getInstance();
	std::vector<char> *header = loadHeader(BM);

	std::vector<char> idv(header->begin(), header->begin() + 4);
	int id = *(reinterpret_cast<const int *>(idv.data()));

	freeHeader(BM, false);

	std::vector<Record> records, pageRecords;
	PageId pid{.FileIdx = relInfo.fileIdx, .PageIdx = 0};
	for (int i = 1; i <= id; i++)
	{
		pid.PageIdx = i;
		pageRecords = getRecordsInDataPage(pid);
		records.insert(records.end(), pageRecords.begin(), pageRecords.end());
	}
	return records;
}

void HeapFile::updateRecords(std::vector<Record> records)
{
	BufferManager *BM = BufferManager::getInstance();
	std::vector<char> *raw;
	size_t pos;
	Rid rid;

	for (size_t i = 0; i < records.size(); i++)
	{
		rid = records[i].getRid();
		pos = rid.slotIdx * relInfo.recordSize;
		raw = BM->GetPage(rid.pageId);

		if (!raw)
			raw = BM->GetPage(rid.pageId);

		records[i].writeToBuffer(*raw, pos);
		BM->FreePage(rid.pageId, true);
	}
}

int HeapFile::extractKey(const std::string& key, const Rid& rid)
{
	BufferManager *BM = BufferManager::getInstance();

	std::vector<char> *raw = BM->GetPage(rid.pageId);
	if (!raw)
		raw = BM->GetPage(rid.pageId);
	int diff = std::distance(relInfo.NOMS.begin(), std::find(relInfo.NOMS.begin(), relInfo.NOMS.end(), key));
	int realPos = rid.slotIdx * relInfo.recordSize;

	for (int i = 0; i < diff; i++)
		realPos += sizeofType(relInfo.TYPES[i]);

	std::vector<char> keyv(raw->begin() + realPos, raw->begin() + realPos + 4);
	return *reinterpret_cast<const int *>(keyv.data());
}

std::vector<std::pair<int, std::vector<Rid>>> HeapFile::loadBulk(const PageId& pid, const std::string& key)
{
	BufferManager *BM = BufferManager::getInstance();

	std::vector<char> *header = loadHeader(BM);
	std::vector<char> idv(header->begin() + (pid.PageIdx * 4), header->begin() + (pid.PageIdx * 4) + 4);
	int DPi = *(reinterpret_cast<const int *>(idv.data()));
	freeHeader(BM, false);

	std::vector<char> *raw = BM->GetPage(pid);
	if (!raw)
		raw = BM->GetPage(pid);
	std::vector<std::pair<int, std::vector<Rid>>> result;
	std::vector<int> keys;
	for (int i = 0; i < relInfo.slotCount - DPi; i++)
	{
		Rid rid{ .pageId = pid, .slotIdx = i };
		int valKey = extractKey(key, rid);
		auto it = std::find_if(result.begin(), result.end(), BulkKeyFinder(valKey));

		if (it != result.end())
			it->second.push_back(rid);
		else
			result.push_back(std::make_pair(valKey, std::vector<Rid>{rid}));
	}
	BM->FreePage(pid, false);
	return result;
}

void HeapFile::createIndex(const std::string& key, int order)
{
	BufferManager *BM = BufferManager::getInstance();
	BpTree *BT = new BpTree(order);
	std::vector<char> *header = loadHeader(BM);

	std::vector<char> idv(header->begin(), header->begin() + 4);
	int id = *(reinterpret_cast<const int *>(idv.data()));

	freeHeader(BM, false);

	PageId pid;
	std::vector<std::pair<int, std::vector<Rid>>> bulk;
	pid.FileIdx = relInfo.fileIdx;
	for (int i = 1; i <= id; i++)
	{
		pid.PageIdx = i;
		std::vector<std::pair<int, std::vector<Rid>>> loaded = loadBulk(pid, key);
		bulk.insert(bulk.end(), loaded.begin(), loaded.end());
	}
	std::sort(bulk.begin(), bulk.end(), [](const std::pair<int, std::vector<Rid>>& a, const std::pair<int, std::vector<Rid>>& b) { return a.first < b.first; });
	std::cout << "BT = " << BT << std::endl;
	BT = BT->insertBulk(bulk);
	std::cout << "BT = " << BT << std::endl;

	indexes[key] = BT;
}

Record HeapFile::getRecord(const Rid& rid)
{
	BufferManager *BM = BufferManager::getInstance();
	Record rc(relInfo);

	std::vector<char> *raw = BM->GetPage(rid.pageId);
	if (!raw)
		raw = BM->GetPage(rid.pageId);
	rc.readFromBuffer(*raw, rid.slotIdx * relInfo.recordSize);
	return rc;
}

std::vector<Record> HeapFile::selectIndex(const std::string& key, int value)
{
	std::vector<Record> records;

	std::vector<Rid> rids = indexes[key]->search(value);

	for (size_t i = 0; i < rids.size(); i++)
		records.push_back(getRecord(rids[i]));
	return records;
}
