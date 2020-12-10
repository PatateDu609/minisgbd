#include <limits.h>
#include <gtest/gtest.h>
#include "DBInfo.hpp"
#include "DBParams.hpp"
#include "testsUtils.hpp"
#include <cstdio>
#include <fstream>

class DBInfoTests : public ::testing::Test
{
protected:
	virtual void setUp()
	{
	}

	virtual void tearDown()
	{
	}
};

TEST(DBInfoTests, testNoFile)
{
	remove((DBParams::DBPath + "Catalog.def").c_str());
	DBInfo *DI = DBInfo::getInstance();

	DI->init();
	EXPECT_TRUE(DI->INFO.empty());
	DBInfo::resetInstance();
}

TEST(DBInfoTests, testInitFinish)
{
	remove((DBParams::DBPath + "Catalog.def").c_str());
	DBInfo *DI = DBInfo::getInstance();
	std::vector<RelationInfo> infos;


	int n = 10;
	for (int i = 0; i < n; i++)
		DI->addRelation(createRelation(10));
	infos = DI->INFO;
	DI->finish();
	DBInfo::resetInstance();

	DI = DBInfo::getInstance();
	DI->init();
	EXPECT_EQ(infos, DI->INFO);

	remove((DBParams::DBPath + "Catalog.def").c_str());
}