#ifndef RECORD_HPP
#define RECORD_HPP

#include "RelationInfo.hpp"
#include "Rid.hpp"
#include <vector>
#include <string>
#include <gtest/gtest.h>

class RecordTests;

class Record
{
private:
	RelationInfo& relInfo;
	Rid rid;
	std::vector<std::string> values;

	FRIEND_TEST(RecordTests, testSizeofType);
	FRIEND_TEST(RecordTests, testWriteToBufferErreur);
	FRIEND_TEST(RecordTests, testReadFromBufferErreur);
	FRIEND_TEST(RecordTests, testRWBuffer);

public:
	Record(RelationInfo &rel);
	Record(const Record& other);
	~Record();

	Record& operator=(const Record& other);

	void writeToBuffer(std::vector<char> &buff, size_t position);
	void readFromBuffer(const std::vector<char>& buff, size_t position);

	std::vector<std::string> getValues() const;
	void setValues(const std::vector<std::string>& values);
	void setRid(const Rid& rid);
	Rid getRid() const;
};

std::ostream& operator<<(std::ostream& os, const Record& record);
std::ostream& operator<<(std::ostream& os, const std::vector<Record>& records);
bool operator==(const Record& a, const Record& b);

#endif
