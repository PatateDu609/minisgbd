#include "Record.hpp"
#include <iostream>

Record::Record(const RelationInfo& rel) : relInfo(rel), sizeBuffered(0)
{
	setSizeBuffered();
}

Record::~Record()
{

}

int Record::sizeofType(std::string type) const
{
	switch (type[0])
		{
		case 's':
			return std::stoi(type.substr(6));
		case 'i':
			return sizeof(int);
		case 'f':
			return sizeof(float);
		}
}

void Record::setSizeBuffered()
{
	for (std::string type : relInfo.TYPES)
		sizeBuffered += sizeofType(type);
}

void Record::writeToBuffer(std::vector<char>& buff, size_t position)
{
	if (buff.size() - position > sizeBuffered)
	{
		std::cerr << "ERREUR : la taille des données à écrire est trop grande pour l'espace disponible" << std::endl;
		return ;
	}

	const unsigned char *casted;
	int size;
	for (int i = 0; i < values.size(); i++)
	{
		switch (relInfo.TYPES[i][0])
		{
		case 's':
			casted = reinterpret_cast<const unsigned char *>(values[i].c_str());
			break;
		case 'i':
			casted = reinterpret_cast<unsigned char *>(&values[i]);
			break;
		case 'f':
			casted = reinterpret_cast<unsigned char *>(&values[i]);
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
	if (buff.size() - position > sizeBuffered)
	{
		std::cerr << "ERREUR : Il n'y a pas assez de données pour constituer une entrée valide" << std::endl;
		return ;
	}

	values.clear();
	std::vector<char> raw;
	decltype(buff.begin()) it;
	int size;
	std::string value;
	for (int i = 0; i < relInfo.NOMS.size(); i++)
	{
		size = sizeofType(relInfo.TYPES[i]);
		it = buff.begin() + position;
		for (int j = 0; j < size; j++, it++);
		raw = std::vector<char>(buff.begin() + position, it);
		raw.push_back('\0');
		position += size;
		value = std::string(raw.data());

		switch (relInfo.TYPES[i][0])
		{
		case 's':
			values.push_back(value);
			break;
		case 'i':
			{
				const int *valint = reinterpret_cast<const int *>(value.c_str());
				values.push_back(std::to_string(*valint));
			}
			break;
		case 'f':
			{
				const float *valfloat = reinterpret_cast<const float *>(value.c_str());
				values.push_back(std::to_string(*valfloat));
			}
			break;
		}
	}
}
