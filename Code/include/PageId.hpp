#ifndef PAGEID_HPP
#define PAGEID_HPP

struct PageId
{
	int FileIdx;
	int PageIdx;
};

bool  operator<(const PageId& a, const PageId& b);
bool  operator==(const PageId& a, const PageId& b); 
bool  operator!=(const PageId& a, const PageId& b);
bool  operator>(const PageId& a, const PageId& b);

#endif
