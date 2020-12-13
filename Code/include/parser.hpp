#ifndef PARSER_HPP
#define PARSER_HPP

#include <map>
#include <vector>
#include <string>
#include <array>
#include <algorithm>

std::map<std::string, std::string> parse(std::string command, std::vector<std::string> operators);
std::vector<std::string> parseValues(std::string tuple, bool batch = false);
std::vector<std::array<std::string, 2>> parseCondition(std::string conditions);
void printParsed(const std::map<std::string, std::string>& parsed);

#endif
