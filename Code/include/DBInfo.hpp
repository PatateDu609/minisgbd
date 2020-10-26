#ifndef DBINFO_HPP
#define DBINFO_HPP
#include <vector>
#include "RelationInfo.hpp"


class DBInfo
{
private:
    std::vector<RelationInfo> INFO;
    int COMPTEUR;
    static DBInfo* INSTANCE;

public:
    DBInfo(/* args */);
    ~DBInfo();

    void init();
    void finish();
    void addRelation(RelationInfo RELATION);
};

#endif