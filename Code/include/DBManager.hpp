#ifndef DBMANAGER_HPP
#define DBMANAGER_HPP
#include "DBInfo.hpp"
#include <string>
#include <vector>

class DBManager
{
private:
    static DBManager* INSTANCE;
    DBInfo* DB_INFO;
    
public:
    DBManager(/* args */);
    ~DBManager();
    static DBManager* getInstance();
    void init();
    void finish();
    void processCommand(std::string COMMANDE);
    void createRelation(std::string NOM_REL, int NBRE, std::vector<std::string> TYPES, std::vector<std::string> NOM_COL);
};

#endif