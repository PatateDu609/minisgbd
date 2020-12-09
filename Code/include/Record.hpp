#ifndef RECORD_HPP
#define RECORD_HPP

#include "RelationInfo.hpp"
#include <vector>
#include <string>

class Record
{
private:
	RelationInfo relInfo;
	std::vector<std::string> values;
	int sizeBuffered;

	void setSizeBuffered();
	int sizeofType(std::string type) const;

public:
	Record(const RelationInfo& rel);
	~Record();

	void writeToBuffer(std::vector<char>& buff, size_t position);
	void readFromBuffer(std::vector<char> buff, size_t position);
};

#endif
