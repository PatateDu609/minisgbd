#include <limits.h>
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "parser.hpp"

class ParserTests : public ::testing::Test
{
protected:
	virtual void setUp()
	{
	}

	virtual void tearDown()
	{
	}
};

TEST(ParserTests, testParserBATCHINSERT)
{
	std::vector<std::string> operators{"INTO", "FROM FILE"};
	std::vector<std::string> values{"nomRelation", "nomFichier.csv"};
	std::string command = "INTO nomRelation FROM FILE nomFichier.csv";

	std::map<std::string, std::string> parsed = parse(command, operators);

	for (size_t i = 0; i < operators.size(); i++)
	{
		ASSERT_NE(parsed.find(operators[i]), parsed.end());
		ASSERT_EQ(parsed[operators[i]], values[i]);
	}
}

TEST(ParserTests, testParserINSERT)
{
	std::vector<std::string> operators{"INTO", "RECORD"};
	std::vector<std::string> values{"nomRelation", "(val1,val2,val3,val4,val5)"};
	std::string command = "INTO nomRelation RECORD (val1,val2,val3,val4,val5)";

	std::map<std::string, std::string> parsed = parse(command, operators);

	for (size_t i = 0; i < operators.size(); i++)
	{
		ASSERT_NE(parsed.find(operators[i]), parsed.end());
		ASSERT_EQ(parsed[operators[i]], values[i]);
	}
}

TEST(ParserTests, testParserSELECTALL)
{
	std::vector<std::string> operators{"FROM"};
	std::vector<std::string> values{"nomRelation"};
	std::string command = "FROM nomRelation";

	std::map<std::string, std::string> parsed = parse(command, operators);

	for (size_t i = 0; i < operators.size(); i++)
	{
		ASSERT_NE(parsed.find(operators[i]), parsed.end());
		ASSERT_EQ(parsed[operators[i]], values[i]);
	}
}

TEST(ParserTests, testParserSELECTS)
{
	std::vector<std::string> operators{"FROM", "WHERE"};
	std::vector<std::string> values{"nomRelation", "C2=19"};
	std::string command = "FROM nomRelation WHERE C2=19";

	std::map<std::string, std::string> parsed = parse(command, operators);

	for (size_t i = 0; i < operators.size(); i++)
	{
		ASSERT_NE(parsed.find(operators[i]), parsed.end());
		ASSERT_EQ(parsed[operators[i]], values[i]);
	}
}

TEST(ParserTests, testParserSELECTC)
{
	std::vector<std::string> operators{"FROM", "WHERE"};
	std::vector<std::string> values{"nomRelation", "AA=1 AND CC=2 AND CC1=2 AND CC25=3 AND CC2=4"};
	std::string command = "FROM nomRelation WHERE AA=1 AND CC=2 AND CC1=2 AND CC25=3 AND CC2=4";

	std::map<std::string, std::string> parsed = parse(command, operators);

	for (size_t i = 0; i < operators.size(); i++)
	{
		ASSERT_NE(parsed.find(operators[i]), parsed.end());
		ASSERT_EQ(parsed[operators[i]], values[i]);
	}
}

TEST(ParserTests, testParserUPDATE)
{
	std::vector<std::string> operators{"UPDATE", "SET", "WHERE"};
	std::vector<std::string> values{"R", "BB=rrr", "CC=2"};
	std::string command = "UPDATE R SET BB=rrr WHERE CC=2";

	std::map<std::string, std::string> parsed = parse(command, operators);

	for (size_t i = 0; i < operators.size(); i++)
	{
		ASSERT_NE(parsed.find(operators[i]), parsed.end());
		ASSERT_EQ(parsed[operators[i]], values[i]);
	}
}

TEST(ParserTests, testParseValuesRecord)
{
	std::ostringstream oss("(val0");
	std::vector<std::string> values;

	oss << "(val0";
	values.push_back("val0");
	for (int i = 1; i < 50; i++)
	{
		std::string value = "val";
		values.push_back(value + std::to_string(i));
		oss << "," << value << i;
	}
	oss << ")";

	std::vector<std::string> parsed = parseValues(oss.str());
	for (size_t i = 0; i < parsed.size(); i++)
		ASSERT_EQ(parsed[i], values[i]);
}

TEST(ParserTests, testParseValuesBatch)
{
	std::ostringstream oss;
	std::vector<std::string> values;

	values.push_back("val0");
	oss << "val0";
	for (int i = 1; i < 50; i++)
	{
		values.push_back("val" + std::to_string(i));
		oss << "," << "val" << i;
	}

	std::vector<std::string> parsed = parseValues(oss.str(), true);
	for (size_t i = 0; i < parsed.size(); i++)
		ASSERT_EQ(parsed[i], values[i]);
}

TEST(ParserTests, testParseOneCondition)
{
	std::vector<std::string> values{"CC", "2"};
	std::string condition = values[0] + "=" + values[1];

	std::vector<std::vector<std::string>> parsed = parseCondition(condition);
	std::vector<std::vector<std::string>> comp{values};
	ASSERT_EQ(parsed, comp);
}

TEST(ParserTests, testParseConditions)
{
	std::ostringstream oss;
	std::vector<std::vector<std::string>> values;

	for (int i = 0; i < 10; i++)
	{
		std::string index = std::to_string(i);
		std::vector<std::string> value{"CC" + index, std::to_string(i + 556)};
		values.push_back(value);
		if (i != 0)
			oss << " AND ";
		oss << value[0] << "=" << value[1];
	}
	std::vector<std::vector<std::string>> parsed = parseCondition(oss.str());
	ASSERT_EQ(parsed, values);
}

TEST(ParserTests, testParseConditionsNotSimple)
{
	std::ostringstream oss;
	std::vector<std::vector<std::string>> values;
	std::vector<std::string> operators;

	operators.push_back("<=");
	operators.push_back(">=");
	operators.push_back("<");
	operators.push_back(">");
	operators.push_back("=");

	for (int i = 0; i < 10; i++)
	{
		std::string index = std::to_string(i);
		int si = rand() % operators.size();
		std::vector<std::string> value{"CC" + index, operators[si], std::to_string(i + 556)};
		values.push_back(value);
		if (i != 0)
			oss << " AND ";
		oss << value[0] << operators[si] << value[2];
	}
	std::vector<std::vector<std::string>> parsed = parseCondition(oss.str(), false);
	ASSERT_EQ(parsed, values);
}
