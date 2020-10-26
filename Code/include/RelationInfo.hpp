#ifndef RELATIONINFO_HPP
#define RELATIONINFO_HPP
#include <vector>
#include <string>

struct RelationInfo
{
    std::string NOM_RELATION;
    int NBRE_COLONNES;
    std::vector<std::string> TYPES;
    std::vector<std::string> NOMS;
};

#endif