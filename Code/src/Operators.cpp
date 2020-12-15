#include "PageId.hpp"
#include "RelationInfo.hpp"
#include "Record.hpp"
#include <iomanip>

bool operator<(const PageId &a, const PageId &b)
{
	if (a.FileIdx < b.FileIdx)
	{
		return true;
	}
	else if (a.PageIdx < b.PageIdx)
	{
		return true;
	}
	return false;
}

bool operator==(const PageId &a, const PageId &b)
{
	if (a.FileIdx == b.FileIdx && a.PageIdx == b.PageIdx)
	{
		return true;
	}
	else
		return false;
}
bool operator!=(const PageId &a, const PageId &b)
{
	return !(a == b);
}
bool operator>(const PageId &a, const PageId &b)
{
	return (PageId &)b < a;
}

std::ostream &operator<<(std::ostream &oss, const PageId &pid)
{
	oss << "File id : " << pid.FileIdx;
	oss << ", Page id : " << pid.PageIdx << std::endl;
	return oss;
}

std::ostream &operator<<(std::ostream &oss, const Rid &rid)
{
	oss << "SlotId = " << rid.slotIdx;
	oss << ", Page id = " << rid.pageId;
	return oss;
}

bool operator==(const RelationInfo &a, const RelationInfo &b)
{
	return a.NBRE_COLONNES == b.NBRE_COLONNES &&
		   a.NOM_RELATION == a.NOM_RELATION &&
		   a.TYPES == b.TYPES &&
		   a.NOMS == b.NOMS;
}

bool operator==(const RelationInfo &rel, const std::string &name)
{
	return rel.NOM_RELATION == name;
}

std::ostream& operator<<(std::ostream& os, const Record& record)
{
	std::vector<std::string> values = record.getValues();

	if (values.empty())
		return os;
	os << values[0];
	for (size_t i = 1; i < values.size(); i++)
	{
		os << " ; " << values[i];
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Record>& records)
{
	for (size_t i = 0; i < records.size(); i++)
		os << records[i] << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, const RelationInfo& rel)
{
	os << "Nom relation : " << rel.NOM_RELATION << std::endl;
	os << "Nombre colonnes : " << rel.NBRE_COLONNES << std::endl;
	os << "FileIdx : " << rel.fileIdx << std::endl;
	os << "RelSize : " << rel.recordSize << std::endl;
	os << "SlotCount : " << rel.slotCount << "(" << std::hex << rel.slotCount << std::dec << ")" << std::endl;

	os << "Colonnes :" << std::endl;
	for (size_t i = 0; i < rel.NOMS.size(); i++)
		os << " - " << rel.NOMS[i] << "(" << rel.TYPES[i] << ")" << std::endl;
	return os;
}

bool operator==(const Record& a, const Record& b)
{
	return a.getValues() == b.getValues();
}
