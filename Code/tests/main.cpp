#include <limits.h>
#include <cmath>
#include <gtest/gtest.h>

double cubic(double d)
{
	return pow(d, 3);
}

class testMath : public ::testing::Test
{
protected:
	virtual void setUp()
	{

	}

	virtual void tearDown()
	{

	}
};

TEST(testMath, CubeTesting)
{
	EXPECT_EQ(1000, cubic(10));
}

#ifdef TESTS
int main(int ac, char **av)
{
	::testing::InitGoogleTest(&ac, av);
	return RUN_ALL_TESTS();
}
#endif
