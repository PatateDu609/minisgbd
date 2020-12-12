#include "DBInfo.hpp"
#include "DBParams.hpp"
#include <fstream>

DBInfo *DBInfo::INSTANCE = NULL;

DBInfo::DBInfo()
{
}

DBInfo::~DBInfo()
{
}

void DBInfo::resetInstance(){
	if (INSTANCE) delete INSTANCE;
	INSTANCE = NULL;
}

DBInfo* DBInfo::getInstance()
{
	if (!INSTANCE) INSTANCE = new DBInfo();
	return INSTANCE;
}

void DBInfo::init()
{
	std::ifstream ifs(DBParams::DBPath + "Catalog.def");
	if (!ifs.is_open())
		return;
	boost::archive::text_iarchive ia(ifs);
	ia >> *this;
}

void DBInfo::finish()
{
	std::ofstream ofs(DBParams::DBPath + "Catalog.def");
	boost::archive::text_oarchive oa(ofs);
	oa << *this;
}

void DBInfo::addRelation(const RelationInfo &RELATION)
{
	INFO.push_back(RELATION);
}

int DBInfo::getCompteur() const
{
	return INFO.size();
}