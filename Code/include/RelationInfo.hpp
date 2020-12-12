#ifndef RELATIONINFO_HPP
#define RELATIONINFO_HPP
#include <vector>
#include <string>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

struct RelationInfo
{
	int NBRE_COLONNES;
	std::string NOM_RELATION;
	std::vector<std::string> TYPES;
	std::vector<std::string> NOMS;

	int fileIdx;
	int recordSize;
	int slotCount;

	template <class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		(void)version;
		ar & NOM_RELATION;
		ar & TYPES;
		ar & NOMS;
		NBRE_COLONNES = NOMS.size();
	}
};


bool operator==(const RelationInfo& a, const RelationInfo& b);

int sizeofType(std::string type);
int getRelInfoSize(const RelationInfo& relInfo);

#endif
