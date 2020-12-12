#ifndef DBINFO_HPP
#define DBINFO_HPP

#include <limits.h>
#include <gtest/gtest.h>
#include "RelationInfo.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

class DBInfoTests;

class DBInfo
{
private:
	std::vector<RelationInfo> INFO;
	static DBInfo* INSTANCE;
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		(void)version;
		ar & INFO;
	}

	FRIEND_TEST(DBInfoTests, testNoFile);
	FRIEND_TEST(DBInfoTests, testInitFinish);

	DBInfo();
public:
	~DBInfo();

	static DBInfo *getInstance();
	static void resetInstance();
	
	void init();
	void finish();
	void addRelation(const RelationInfo& RELATION);
	int getCompteur() const;
};

#endif
