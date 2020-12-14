#ifndef RELATIONINFO_HPP
#define RELATIONINFO_HPP

#include <vector>
#include <string>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "DBParams.hpp"

struct RelationInfo
{
	int NBRE_COLONNES;
	std::string NOM_RELATION;
	std::vector<std::string> TYPES;
	std::vector<std::string> NOMS;

	~RelationInfo()
	{
	}

	int fileIdx;
	int recordSize;
	int slotCount;

	template <class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		(void)version;
		ar &NOM_RELATION;
		ar &TYPES;
		ar &NOMS;
		ar & fileIdx;
		ar & recordSize;
		ar & slotCount;
		NBRE_COLONNES = NOMS.size();
	}
};

int sizeofType(std::string type);
int getRelInfoSize(const RelationInfo &relInfo);

bool operator==(const RelationInfo &a, const RelationInfo &b);
bool operator==(const RelationInfo &rel, const std::string &name);
std::ostream& operator<<(std::ostream& os, const RelationInfo& rel);

#endif
