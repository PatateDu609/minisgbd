#include "parser.hpp"
#include <iostream>
#include <sstream>

static void joinSpecialCases(std::vector<std::string>& result)
{
	std::vector<std::string>::iterator it;
	if ((it = std::find(result.begin(), result.end(), "FILE")) != result.end())
	{
		it--;
		*it = "FROM FILE";
		result.erase(it + 1);
	}
	if ((it = std::find(result.begin(), result.end(), "WHERE")) != result.end())
	{
		decltype(it) start = result.begin();
		it++;
		size_t toEnd = result.size() - (it - start);
		std::vector<std::string> afterWHERE(it, it + toEnd);
		result.erase(it, it + toEnd);

		std::ostringstream oss;

		oss << afterWHERE[0];
		for (size_t i = 1; i < afterWHERE.size(); i++)
			oss << " " << afterWHERE[i];
		result.push_back(oss.str());
	}
}

static std::vector<std::string> getWords(std::string str, std::string delim, bool first = false)
{
	std::size_t pos = 0;
	std::string token;
	std::vector<std::string> result;

	while ((pos = str.find(delim)) != std::string::npos)
	{
		token = str.substr(0, pos);
		result.push_back(token);
		str.erase(0, pos + delim.length());
		if (first)
			break ;
	}
	result.push_back(str);

	joinSpecialCases(result);
	return result;
}

std::map<std::string, std::string> parse(std::string command, std::vector<std::string> operators)
{
	std::map<std::string, std::string> parsed;
	std::vector<std::string> words = getWords(command, " ");

	size_t i = 0;
	for (std::string op : operators)
	{
		if (op.empty())
			parsed["EMPTY"] = words[i++];
		else
		{
			if (words[i] == op)
				i++;
			parsed[op] = words[i++];
		}
	}
	return parsed;
}

std::vector<std::string> parseValues(std::string tuple, bool batch)
{
	if (!batch)
		tuple = tuple.substr(1, tuple.length() - 2);
	return getWords(tuple, ",");
}

std::vector<std::vector<std::string>> parseCondition(std::string conditions, bool simple)
{
	std::vector<std::vector<std::string>> result;
	std::vector<std::string> words = getWords(conditions, " AND ");
	std::vector<std::string> operators;

	if (!simple)
	{
		operators.push_back("<=");
		operators.push_back(">=");
		operators.push_back("<");
		operators.push_back(">");
	}
	operators.push_back("=");

	for (std::string condition : words)
	{
		std::vector<std::string> parsed;

		for (size_t i = 0; i < operators.size(); i++)
		{
			parsed = getWords(condition, operators[i], true);
			if (parsed.size() >= 2)
			{
				if (!simple)
					parsed.insert(parsed.begin() + 1, operators[i]);
				break ;
			}
		}
		result.push_back(parsed);
	}

	return result;
}

void printParsed(const std::map<std::string, std::string>& parsed)
{
	for (auto it = parsed.begin(); it != parsed.end(); it++)
		std::cout << it->first << " -> " << it->second << std::endl;
}
