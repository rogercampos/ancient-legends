#ifndef DBMANAGER_H_
#define DBMANAGER_H_

#define MYSQLPP_MYSQL_HEADERS_BURIED

#include <mysql++/mysql++.h>

namespace db {

class DBManager
{
protected:
	DBManager();
	~DBManager();
	DBManager(const DBManager&);
	DBManager& operator= (const DBManager&);

private:
	static DBManager *instance;

	mysqlpp::Connection *conn;

	static char *db;
	static char *server;
	static char *user;
	static char *psswd;

public:
	static DBManager* get_instance();
	void init_connection();

	void get_zones(mysqlpp::StoreQueryResult &res);
	void get_rooms(mysqlpp::StoreQueryResult &res);

	int login_procedure(std::string name, std::string psswd);
	void get_player(int id, mysqlpp::StoreQueryResult &res);
};



}

#endif /*DBMANAGER_H_*/
