#ifndef TUPLES_HPP
#define TUPLES_HPP

#include <vector>
#include <array>
#include <string>
#include "RelationInfo.hpp"
#include "Record.hpp"

std::vector<Record> restrictTuples(const RelationInfo& relInfo, std::vector<Record> tuples, const std::vector<std::vector<std::string>>& condition);

#endif
