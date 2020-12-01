#ifndef FRAME_HPP
#define FRAME_HPP

#include "PageId.hpp"
#include <vector>

struct Frame
{
	PageId pageId;
	bool dirty;
	int pinCount;
	int refBit;
	std::vector<char>* pageDisk;
};

#endif