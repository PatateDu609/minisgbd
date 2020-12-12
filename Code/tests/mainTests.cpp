#include <limits.h>
#include <cmath>
#include <gtest/gtest.h>
#include "DBParams.hpp"


int DBParams::pageSize = 4096;
int DBParams::frameCount = 2;
std::string DBParams::DBPath = "DB/Tests/";

int main(int ac, char **av)
{
	::testing::InitGoogleTest(&ac, av);
	return RUN_ALL_TESTS();
}