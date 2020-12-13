#include "PageId.hpp"
#include "RelationInfo.hpp"

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
