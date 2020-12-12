#ifndef RECORD_HPP
#define RECORD_HPP

#include "RelationInfo.hpp"
#include <vector>
#include <string>
#include <gtest/gtest.h>

class RecordTests;

class Record
{
private:
	RelationInfo relInfo;
	std::vector<std::string> values;
	size_t sizeBuffered;


	FRIEND_TEST(RecordTests, testSizeofType);
	FRIEND_TEST(RecordTests, testWriteToBufferErreur);
	FRIEND_TEST(RecordTests, testReadFromBufferErreur);
	FRIEND_TEST(RecordTests, testRWBuffer);

public:
	Record(const RelationInfo &rel);
	~Record();

	void writeToBuffer(std::vector<char> &buff, size_t position);
	void readFromBuffer(std::vector<char> buff, size_t position);
};

#endif
