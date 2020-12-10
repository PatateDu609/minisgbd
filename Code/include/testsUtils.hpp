#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include "RelationInfo.hpp"
#include <vector>

RelationInfo createRelation(int n);
std::vector<std::string> createValues(RelationInfo rel);

#endif