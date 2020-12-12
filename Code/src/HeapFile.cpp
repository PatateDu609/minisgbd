#include "HeapFile.hpp"
#include "DiskManager.hpp"
#include "BufferManager.hpp"

HeapFile::HeapFile(const RelationInfo &rel) : relInfo(rel)
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

void HeapFile::freeHeader(BufferManager *BM, bool dirty){
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
		header->at(i + id + 1) = DPic[i];
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

Rid HeapFile::writeRecordToDataPage(const Record& rc, const PageId& pageId)
{
	BufferManager *BM = BufferManager::getInstance();
	std::vector<char> *header = loadHeader(BM);

	std::vector<char> idv(header->begin() + (pageId->PageIdx * 4), header->begin() + (pageId->PageIdx * 4) + 4);
	int DPi = *(reinterpret_cast<const int *>(idv.data()));
	
	std::vector<char> *raw = BM->GetPage(pageId);
	if (raw == NULL)
		raw = BM->GetPage(pageId);
	rc.writeToBuffer(*raw, (relInfo.slotCount - DPi) * relInfo.recordSize);
	BM->FreePage(pageId, true);

	DPi--;
	const char *DPic = reinterpret_cast<const char *>(&DPi);
	for (int i = 0; i < 4; i++)
		header[pageId->pageIdx * 4 + i] = DPic[i];

	return (Rid){ .pageId = pageId, .slotIdx = relInfo.slotCount - DPi };
}
