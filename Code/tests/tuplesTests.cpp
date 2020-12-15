#include <limits.h>
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "tuples.hpp"

class TuplesTests : public ::testing::Test
{
protected:
	virtual void setUp()
	{
	}

	virtual void tearDown()
	{
	}
};

std::vector<Record> createRecords(RelationInfo& relInfo)
{
	std::vector<Record> records;
	std::vector<std::vector<std::string>> values;

	relInfo.NOM_RELATION = "rel";
	relInfo.TYPES = {"string3", "int", "float", "string5"};
	relInfo.NOMS = {"C1", "C2", "C3", "C4"};
	relInfo.NBRE_COLONNES = 4;

	values.push_back({"EGB", "220", "5.95", "Uoo"});
	values.push_back({"joG", "24", "1.4", "rxsun"});
	values.push_back({"XoU", "35", "14.54", "SAJQL"});
	values.push_back({"eob", "587", "576.45", "HKI"});
	values.push_back({"tIc", "124", "16.687", "bonjo"});
	values.push_back({"Rlf", "254", "68.354", "tGfgw"});
	values.push_back({"Blr", "55", "140.434", "RRmmf"});
	values.push_back({"SbI", "-52758", "104.3402", "RRmmf"});
	values.push_back({"Lot", "5", "1435.354204", "tGfgw"});
	values.push_back({"KUf", "514", "114.034", "utquc"});
	values.push_back({"WHu", "57", "1475.0034", "QjgZO"});
	values.push_back({"AES", "-455", "0.0004", "rxsun"});
	values.push_back({"Aya", "95", "-14.4", "rxsun"});
	values.push_back({"AES", "15", "57.654", "bonjo"});
	values.push_back({"OlM", "0", "786.54", "rxsun"});
	values.push_back({"rmH", "-5", "14.3", "bonjo"});
	values.push_back({"OlM", "-9", "14.4", "SAJQL"});
	values.push_back({"Aya", "0", ".0954", "bonjo"});
	values.push_back({"Aya", "5327", "54", "RRmmf"});
	values.push_back({"OlM", "52", "14.34", "utquc"});
	values.push_back({"rmH", "52", "14.34", "CEq"});
	values.push_back({"AES", "52", "654.01", "bonjo"});
	values.push_back({"Aya", "68", "14.", "bonjo"});
	values.push_back({"CEq", "58", "765.3", "rxsun"});
	values.push_back({"Aya", "58995", "4.40", "bonjo"});
	values.push_back({"Aya", "21486", "1.34", "bonjo"});

	for (size_t i = 0; i < values.size(); i++)
	{
		Record rc(relInfo);
		rc.setValues(values[i]);
		records.push_back(rc);
	}
	return records;
}

std::vector<Record> generateResult(RelationInfo& relInfo, std::vector<std::vector<std::string>> values)
{
	std::vector<Record> result;

	for (size_t i = 0; i < values.size(); i++)
	{
		Record rc(relInfo);
		rc.setValues(values[i]);
		result.push_back(rc);
	}
	return result;
}

TEST(TuplesTests, testSimpleConditionInt)
{
	RelationInfo relInfo;
	auto records = createRecords(relInfo);
	std::vector<std::vector<std::string>> condition{{"C2", "52"}};

	std::vector<std::vector<std::string>> expected = {
		{"OlM", "52", "14.34", "utquc"},
		{"rmH", "52", "14.34", "CEq"},
		{"AES", "52", "654.01", "bonjo"}
	};

	EXPECT_EQ(restrictTuples(relInfo, records, condition), generateResult(relInfo, expected));

	condition = {{"C2", "=", "52"}};
	EXPECT_EQ(restrictTuples(relInfo, records, condition), generateResult(relInfo, expected));

	condition = {{"C2", "99954654"}};
	EXPECT_EQ(restrictTuples(relInfo, records, condition), generateResult(relInfo, {}));
}

TEST(TuplesTests, testSimpleConditionFloat)
{
	RelationInfo relInfo;
	auto records = createRecords(relInfo);
	std::vector<std::vector<std::string>> condition{{"C3", "14.34"}};

	std::vector<std::vector<std::string>> expected = {
		{"OlM", "52", "14.34", "utquc"},
		{"rmH", "52", "14.34", "CEq"}
	};

	EXPECT_EQ(restrictTuples(relInfo, records, condition), generateResult(relInfo, expected));

	condition = {{"C3", "=", "14.34"}};
	EXPECT_EQ(restrictTuples(relInfo, records, condition), generateResult(relInfo, expected));

	condition = {{"C3", "9995.7984"}};
	EXPECT_EQ(restrictTuples(relInfo, records, condition), generateResult(relInfo, {}));
}

TEST(TuplesTests, testSimpleConditionString)
{
	RelationInfo relInfo;
	auto records = createRecords(relInfo);
	std::vector<std::vector<std::string>> condition{{"C1", "AES"}};

	std::vector<std::vector<std::string>> expected = {
		{"AES", "-455", "0.0004", "rxsun"},
		{"AES", "15", "57.654", "bonjo"},
		{"AES", "52", "654.01", "bonjo"}
	};

	EXPECT_EQ(restrictTuples(relInfo, records, condition), generateResult(relInfo, expected));

	condition = {{"C4", "=", "bonjo"}};
	expected = {
		{"tIc", "124", "16.687", "bonjo"},
		{"AES", "15", "57.654", "bonjo"},
		{"rmH", "-5", "14.3", "bonjo"},
		{"Aya", "0", ".0954", "bonjo"},
		{"AES", "52", "654.01", "bonjo"},
		{"Aya", "68", "14.", "bonjo"},
		{"Aya", "58995", "4.40", "bonjo"},
		{"Aya", "21486", "1.34", "bonjo"}
	};
	EXPECT_EQ(restrictTuples(relInfo, records, condition), generateResult(relInfo, expected));

	condition = {{"C1", "=", "bit"}};
	EXPECT_EQ(restrictTuples(relInfo, records, condition), generateResult(relInfo, {}));
}
