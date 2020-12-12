#include "Record.hpp"
#include <iostream>
#include <iomanip>

Record::Record(const RelationInfo &rel) : relInfo(rel), sizeBuffered(getRelInfoSize(rel))
{

}

Record::~Record()
{
}

void Record::writeToBuffer(std::vector<char> &buff, size_t position)
{
	if (buff.size() - position < sizeBuffered)
	{
		std::cerr << "ERREUR : la taille des données à écrire est trop grande pour l'espace disponible" << std::endl;
		return;
	}
	const char *casted;
	int size;
	for (size_t i = 0; i < values.size(); i++)
	{
		switch (relInfo.TYPES[i][0])
		{
		case 's':
			casted = reinterpret_cast<const char *>(values[i].c_str());
			break;
		case 'i':
		{
			int valint = std::stoi(values[i]);
			casted = reinterpret_cast<const char *>(&valint);
		}
			break;
		case 'f':
		{
			float valfloat = std::stof(values[i]);
			casted = reinterpret_cast<const char *>(&valfloat);
		}
			break;
		}
		size = sizeofType(relInfo.TYPES[i]);
		for (int j = 0; j < size; j++)
		{
			buff[position] = casted[j];
			position++;
		}
	}
}

void Record::readFromBuffer(std::vector<char> buff, size_t position)
{
	if (buff.size() - position < sizeBuffered)
	{
		std::cerr << "ERREUR : Il n'y a pas assez de données pour constituer une entrée valide" << std::endl;
		return;
	}

	values.clear();
	std::vector<char> raw;
	decltype(buff.begin()) it;
	int size;
	std::string value;
	for (size_t i = 0; i < relInfo.NOMS.size(); i++)
	{
		size = sizeofType(relInfo.TYPES[i]);
		it = buff.begin() + position;
		for (int j = 0; j < size; j++, it++)
			;
		raw = std::vector<char>(buff.begin() + position, it);
		raw.push_back('\0');
		position += size;

		switch (relInfo.TYPES[i][0])
		{
		case 's':
			values.push_back(std::string(raw.data()));
			break;
		case 'i':
		{
			const int *valint = reinterpret_cast<const int *>(raw.data());
			values.push_back(std::to_string(*valint));
		}
		break;
		case 'f':
		{
			const float *valfloat = reinterpret_cast<const float *>(raw.data());
			values.push_back(std::to_string(*valfloat));
		}
		break;
		}
	}
}