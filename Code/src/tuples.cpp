#include "tuples.hpp"

template<class T>
static bool applyOperator(T val, std::string op, T val2)
{
	if (op == "=")
		return val == val2;
	else if (op == "<=")
		return val <= val2;
	else if (op == ">=")
		return val >= val2;
	else if (op == ">")
		return val > val2;
	else if (op == "<")
		return val < val2;
	return false;
}

static bool isValid(const RelationInfo& relInfo, const Record& rc, const std::vector<std::vector<std::string>>& conditions)
{
	std::vector<std::string> values = rc.getValues(), types = relInfo.TYPES, noms = relInfo.NOMS;

	for (size_t i = 0; i < conditions.size(); i++)
	{
		std::string op = (conditions[i].size() == 2ul ? "=" : conditions[i][1]);
		auto it = std::find(noms.begin(), noms.end(), conditions[i][0]);
		size_t fromStart = it - noms.begin();

		switch (types[fromStart][0])
		{
			case 'i':
				if (!applyOperator(std::stoi(values[fromStart]), op, std::stoi(conditions[i][conditions[i].size() - 1])))
					return false;
				break;
			case 'f':
				if (!applyOperator(std::stof(values[fromStart]), op, std::stof(conditions[i][conditions[i].size() - 1])))
					return false;
				break;
			case 's':
				if (!applyOperator(values[fromStart], op, conditions[i][conditions[i].size() - 1]))
					return false;
				break;
		}
	}
	return true;
}

std::vector<Record> restrictTuples(const RelationInfo& relInfo, std::vector<Record> tuples, const std::vector<std::vector<std::string>>& conditions)
{
	std::vector<Record> restricted;

	for (size_t i = 0; i < tuples.size(); i++)
		if (isValid(relInfo, tuples[i], conditions))
			restricted.push_back(tuples[i]);
	return restricted;
}
