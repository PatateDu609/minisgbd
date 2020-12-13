#include "RelationInfo.hpp"

int sizeofType(std::string type)
{
	switch (type[0])
	{
	case 's':
		return std::stoi(type.substr(6));
	case 'i':
		return sizeof(int);
	case 'f':
		return sizeof(float);
	default:
		return 0;
	}
}

int getRelInfoSize(const RelationInfo &relInfo)
{
	int sizeBuffered = 0;

	for (std::string type : relInfo.TYPES)
		sizeBuffered += sizeofType(type);
	return sizeBuffered;
}
