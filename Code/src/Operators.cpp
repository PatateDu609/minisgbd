#include "PageId.hpp"

bool  operator<(const PageId& a, const PageId& b) {
	if (a.FileIdx < b.FileIdx)
	{
		return true;
	} 
	else if(a.PageIdx < b.PageIdx) {
		return true;
	} 
	return false; 
} 

bool  operator==(const PageId& a, const PageId& b) {
	if (a.FileIdx == b.FileIdx && a.PageIdx == b.PageIdx)
	{
		return true;
	} 
	else return false;
} 
bool  operator!=(const PageId& a, const PageId& b) {
	return !(a == b);
} 
bool  operator>(const PageId& a, const PageId& b) {
	return (PageId&)b < a;
} 