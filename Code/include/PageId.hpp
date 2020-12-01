#ifndef PAGEID_HPP
#define PAGEID_HPP

#include <iostream>

struct PageId
{
	int FileIdx;
	int PageIdx;
};

bool  operator<(const PageId& a, const PageId& b);
bool  operator==(const PageId& a, const PageId& b); 
bool  operator!=(const PageId& a, const PageId& b);
bool  operator>(const PageId& a, const PageId& b);

std::ostream& operator<<(std::ostream& oss, const PageId& pid);

#endif
