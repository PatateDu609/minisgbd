#ifndef RID_HPP
#define RID_HPP

#include "PageId.hpp"

struct Rid
{
	PageId pageId;
	int slotIdx;
};

std::ostream& operator<<(std::ostream& os, const Rid& rid);

#endif
