#include <limits.h>
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "Record.hpp"
#include "DBParams.hpp"
#include <boost/random.hpp>

class RecordTests : public ::testing ::Test
{
protected:
	virtual void setUp()
	{
	}

	virtual void tearDown()
	{
	}
};

RelationInfo createRelation(int n)
{
	std::vector<std::string> types;
	RelationInfo relInfo;
	relInfo.NBRE_COLONNES = n;
	relInfo.NOM_RELATION = "tests";

	for (int i = 0; i < n; i++)
	{
		relInfo.NOMS.push_back("C" + i);
		int type = std::rand() % 3;
		switch (type)
		{
		case 0:
			types.push_back("int");
			break;
		case 1:
			types.push_back("float");
			break;
		case 2:
		{
			std::string string_type = "string";
			string_type += std::to_string(std::rand() % 10 + 1);
			types.push_back(string_type);
		}
		break;
		}
	}

	relInfo.TYPES = types;
	relInfo.recordSize = getRelInfoSize(relInfo);
	relInfo.fileIdx = 0;
	relInfo.slotCount = relInfo.recordSize / DBParams::pageSize;
	return relInfo;
}

std::vector<std::string> createValues(RelationInfo rel){
	std::vector<std::string> values;


	auto randchar = []() -> char {
		const char charset[] = "0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		const size_t max_index = sizeof(charset) - 1;
		return charset[std::rand() % max_index];
	};


	for(size_t i = 0; i < rel.TYPES.size(); i++){
		switch (rel.TYPES[i][0])
		{
		case 'i':
			values.push_back(std::to_string(std::rand() % 1000));
			break;
		case 'f':
			values.push_back(std::to_string(static_cast<float> (std::rand()) / static_cast<float> (RAND_MAX / 1000)));
			break;
		case 's':
		{
			int size = std::stoi(rel.TYPES[i].substr(6));
			std::string str(size, 0);
			std::generate_n(str.begin(), size, randchar);
			values.push_back(str);
		}
			break;
		}
	}
	return values;
}

TEST(RecordTests, testSizeofType)
{
	std::srand(std::time(nullptr));
	int n = 10;
	RelationInfo relInfo = createRelation(n);

	for (int i = 0; i < n; i++)
	{
		std::string str = relInfo.TYPES[i];

		switch (str[0])
		{
		case 's':
		{
			EXPECT_EQ(sizeofType(str), std::stoi(str.substr(6)));
			break;
		}

		case 'i':
		{
			EXPECT_EQ(sizeofType(str), (int)sizeof(int));
			break;
		}

		case 'f':
		{
			EXPECT_EQ(sizeofType(str), (int)sizeof(float));
			break;
		}
		}
	}
}

// TESTS DES ERREURS DE READ/WRITE

TEST(RecordTests, testWriteToBufferErreur)
{
	RelationInfo relInfo;
	relInfo.NBRE_COLONNES = 5;
	relInfo.NOM_RELATION = "testWriteErreur";
	relInfo.NOMS.push_back("NomColonne1");
	relInfo.TYPES.push_back("int");
	relInfo.NOMS.push_back("NomColonne2");
	relInfo.TYPES.push_back("int");
	relInfo.NOMS.push_back("NomColonne3");
	relInfo.TYPES.push_back("float");
	relInfo.NOMS.push_back("NomColonne4");
	relInfo.TYPES.push_back("string5");
	relInfo.NOMS.push_back("NomColonne5");
	relInfo.TYPES.push_back("string8");

	relInfo.recordSize = getRelInfoSize(relInfo);

	Record rec(relInfo);
	int size = relInfo.recordSize;
	std::vector<char> buffer(1000);

	std::stringstream buf;
	std::streambuf *old = std::cerr.rdbuf(buf.rdbuf());
	rec.writeToBuffer(buffer, 999);
	std::string printed = buf.str();
	EXPECT_EQ(printed, "ERREUR : la taille des données à écrire est trop grande pour l'espace disponible\n");
	buf.str("");
	printed.clear();

	rec.writeToBuffer(buffer, 1000 - size + 1);
	printed = buf.str();
	EXPECT_EQ(printed, "ERREUR : la taille des données à écrire est trop grande pour l'espace disponible\n");
	buf.str("");
	printed.clear();

	rec.writeToBuffer(buffer, 1000 - size);
	printed = buf.str();
	EXPECT_EQ(printed, "");
	buf.str("");
	printed.clear();

	std::cerr.rdbuf(old);
}

TEST(RecordTests, testReadFromBufferErreur)
{
	RelationInfo relInfo;
	relInfo.NBRE_COLONNES = 5;
	relInfo.NOM_RELATION = "testWriteErreur";
	relInfo.NOMS.push_back("NomColonne1");
	relInfo.TYPES.push_back("int");
	relInfo.NOMS.push_back("NomColonne2");
	relInfo.TYPES.push_back("int");
	relInfo.NOMS.push_back("NomColonne3");
	relInfo.TYPES.push_back("float");
	relInfo.NOMS.push_back("NomColonne4");
	relInfo.TYPES.push_back("string5");
	relInfo.NOMS.push_back("NomColonne5");
	relInfo.TYPES.push_back("string8");

	relInfo.recordSize = getRelInfoSize(relInfo);

	Record rec(relInfo);
	int size = relInfo.recordSize;
	std::vector<char> buffer(1000);

	std::stringstream buf;
	std::streambuf *old = std::cerr.rdbuf(buf.rdbuf());
	rec.readFromBuffer(buffer, 999);
	std::string printed = buf.str();
	EXPECT_EQ(printed, "ERREUR : Il n'y a pas assez de données pour constituer une entrée valide\n");
	buf.str("");
	printed.clear();

	rec.readFromBuffer(buffer, 1000 - size + 1);
	printed = buf.str();
	EXPECT_EQ(printed, "ERREUR : Il n'y a pas assez de données pour constituer une entrée valide\n");
	buf.str("");
	printed.clear();

	rec.readFromBuffer(buffer, 1000 - size);
	printed = buf.str();
	EXPECT_EQ(printed, "");
	buf.str("");
	printed.clear();

	std::cerr.rdbuf(old);
}

//TESTS DES FONCTIONS WRITETOBUFFER ET READFROMBUFFER
TEST(RecordTests, testRWBuffer)
{
	std::srand(std::time(nullptr));
	auto randint = [](int A, int B) -> int
	{
		int r = (rand()%(B-A)) + A;
		return r;
	};
	
	int iterator = 25;
	RelationInfo rel;
	std::vector<char> buffer(1010);

	for (int i = 0; i < iterator; i++)
	{
		rel = createRelation(std::rand() % 100 + 1);

		Record rec(rel);
		std::vector<std::string> values = createValues(rel);
		rec.values = values;

		int pos = randint(0, buffer.size() - rel.recordSize);
		rec.writeToBuffer(buffer, pos);
		rec.readFromBuffer(buffer, pos);
		EXPECT_EQ(values, rec.values);
	}
}