#include <dbManager.h>
#include <mud_error.h>
#include <iostream>
namespace db
{

DBManager* DBManager::instance = 0;
char* DBManager::db = (char*)"mud";
char* DBManager::server = (char*)"casalcampos.com";
char* DBManager::user = (char*)"mud";
char* DBManager::psswd = (char*)"cucamonga";

DBManager* DBManager::get_instance() {
	if (instance == 0) {
		instance = new DBManager;
	}
	return instance;
}

DBManager::DBManager()
{
}

DBManager::~DBManager()
{
	conn->disconnect();
}

void DBManager::init_connection() {
	conn = new mysqlpp::Connection(false);

	if (!conn->connect(db, server, user, psswd)) {
		mud_error(conn->error());
	}
}

void DBManager::get_zones(mysqlpp::StoreQueryResult &res) {
	mysqlpp::Query query = conn->query("select * from zones");
	res = query.store();
	if (!res) {
		mud_error(query.error());
	}
}

void DBManager::get_rooms(mysqlpp::StoreQueryResult &res) {
	mysqlpp::Query query = conn->query("select * from rooms");
	res = query.store();
	if (!res) {
		mud_error(query.error());
	}
}

/**
 * Funcio que utilitza el ConnectionHandler per comprovar si un
 * login es correcte o no. Comprova si el nom de player i psswd
 * pasats existeixen. Si no son valids torna -1.
 * Si son valids, torna el id del player
 */
int DBManager::login_procedure(std::string name, std::string psswd) {
	mysqlpp::Query query = conn->query();
	query << "select id from players where name=" << mysqlpp::quote_only << name;

	mysqlpp::StoreQueryResult res = query.store();
	if (!res) {
		mud_error(query.error());
	}
	if (res.num_rows() == 0) return -1;
	return (int)res[0][0];
}


void DBManager::get_player(int id, mysqlpp::StoreQueryResult &res) {
	mysqlpp::Query query = conn->query();
	query << "select * from players where id=" << mysqlpp::quote_only << id;
	res = query.store();
	if (!res) {
		mud_error(query.error());
	}
}

}
