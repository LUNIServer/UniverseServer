#include "Database.h"
#include "AccountsDB.h"
#include "CharactersDB.h"
#include "InventoryDB.h"
#include "ServerDB.h"
#include "Logger.h"
#include <iostream>
using namespace std;

MYSQL * Database::_con = NULL; // Set MYSQL connection variable to NULL
std::vector<MySQLTable *> Database::tables;

ColData::ColData(std::string t, bool n, std::string k, std::string d, std::string e){
	this->type = t;
	this->null = n;
	this->key = k;
	this->default = d;
	this->extra = e;
};

ColData::ColData(){
	this->type = "";
	this->null = false;
	this->key = "";
	this->default = "NULL";
	this->extra = "";
}

// Connect to the MySQL database. Requires the host, database, DB user, and DB password to connect
unsigned int Database::Connect(const string& host, const string& database, const string& username, const string& password) {
	_con = mysql_init(NULL); //init sql connection
	if (_con == NULL) return 1;
	//if (!_con) throw MySqlException("Can't start mysql service!\n");

	MYSQL * _cont = mysql_real_connect(_con, host.c_str(), username.c_str(), password.c_str(), database.c_str(), 0, NULL, 0); //connection to luni database
	if (_cont == NULL){
		Logger::logError("MAIN", "MYSQL", "connecting the database", mysql_error(_con));
		unsigned int state =  mysql_errno(_con);
		Logger::log("MAIN", "CONFIG", "MYSQL Error #" + std::to_string(state));
		switch (state){
		case 1044:
			std::cout << std::endl;
			std::cout << "  == [ERROR HELP] ==" << std::endl;
			std::cout << std::endl;
			std::cout << "  This User can not be used to access the database" << std::endl;
			std::cout << "  The current user is '" + username + "'." << std::endl;
			std::cout << "  Possible reasons for this are:" << std::endl;
			std::cout << "  - There is no user with this name" << std::endl;
			std::cout << "  - There is a typo in the username" << std::endl;
			std::cout << "  - The wrong config file has been loaded" << std::endl;
			std::cout << std::endl;
			if (username != "root"){
				std::cout << "  The default user of MYSQL is 'root'." << std::endl
					<< "  If you haven't created a new user, try using this name" << std::endl << std::endl;
			}
			if (username == ""){
				// This never happens because the parser sets 'root' as default when no value is found.
				std::cout << "  MYSQL needs a user name to work. You cannot leave this field empty." << std::endl << std::endl;
			}
			break;
		case 1045:
			//Could not log in / wrong password
			std::cout << std::endl;
			std::cout << "  == [ERROR HELP] ==" << std::endl;
			std::cout << std::endl;
			if (password != ""){
				std::cout << "  The password used to connect to the database was wrong" << std::endl;
				std::cout << "  The current password is '" + password + "'." << std::endl;
				std::cout << "  The current user is '" + username + "'." << std::endl;
				std::cout << "  Possible reasons for this are:" << std::endl;
				std::cout << "  - The password specified in the config file is wrong" << std::endl;
				std::cout << "  - The wrong config file has been loaded" << std::endl;
				std::cout << std::endl;
			}
			else{
				std::cout << "  The database requires a password to connect with this user" << std::endl;
				std::cout << "  The current user is '" + username + "'." << std::endl;
				std::cout << "  Possible reasons for this are:" << std::endl;
				std::cout << "  - The password for this user has not been specified in the config file" << std::endl;
				std::cout << "  - The wrong config file has been loaded" << std::endl;
				std::cout << std::endl;
			}
			break;
		case 1049:
			std::cout << std::endl;
			std::cout << "  == [ERROR HELP] ==" << std::endl;
			std::cout << std::endl;
			std::cout << "  This database is not available on the MYSQL server" << std::endl;
			std::cout << "  The current database is '" + database + "'" << std::endl;
			std::cout << "  Possible reasons for this are:" << std::endl;
			std::cout << "  - There is no database with this name" << std::endl;
			std::cout << "  - There is a typo in this name" << std::endl;
			std::cout << "  - The wrong config file has been loaded" << std::endl;
			std::cout << "  - The database might not have been imported yet" << std::endl;
			std::cout << std::endl;

			std::cout << "  The default database name is 'luni'" << std::endl;
			if (database == "luni"){
				std::cout << "  Make sure, the database has been imported" << std::endl;
			}
			else{
				std::cout << "  If you haven't set a different name, try using this name" << std::endl;
			}
			std::cout << std::endl;
			break;
		case 2003:
			std::cout << std::endl;
			std::cout << "  == [ERROR HELP] ==" << std::endl;
			std::cout << std::endl;
			std::cout << "  No MYSQL Database found at '" << host << "'" << std::endl;
			std::cout << "  Possible reasons for this are:" << std::endl;
			std::cout << "  - The host name points to the wrong server" << std::endl;
			std::cout << "  - The wrong config file has been loaded" << std::endl;
			std::cout << "  - The MYSQL server is not running" << std::endl;
			std::cout << std::endl;
			if (!(host == "localhost" || host == "127.0.0.1")){
				std::cout << "  The default host is 'localhost' or '127.0.0.1'" << std::endl;
				std::cout << "  If you have not expicitly chosen something different," << std::endl;
				std::cout << "  try this as host" << std::endl;
				std::cout << std::endl;
			}
			
			break;
		case 2005:
			std::cout << std::endl;
			std::cout << "  == [ERROR HELP] ==" << std::endl;
			std::cout << std::endl;
			std::cout << "  The server host '" << host << "' could not be found" << std::endl;
			std::cout << "  Possible reasons for this are:" << std::endl;
			std::cout << "  - There is a typo in the host name" << std::endl;
			std::cout << "  - The wrong config file has been loaded" << std::endl;
			std::cout << std::endl;
			if (!(host == "localhost" || host == "127.0.0.1")){
				std::cout << "  The default host is 'localhost' or '127.0.0.1'" << std::endl;
				std::cout << "  If you have not expicitly chosen something different," << std::endl;
				std::cout << "  try this as host" << std::endl;
				std::cout << std::endl;
			}
			
			break;
		}
		return state;
	}
	_con = _cont;

	mysql_query(_con, "SET NAMES 'utf8';");

	return 0;
}

// Query the MySQL database for a specific string
MYSQL_RES * Database::Query(const string & query) {
	if (_con == NULL) throw MySqlException("Database connection error!\n");
	mysql_query(_con, (char*)query.c_str());
	return mysql_store_result(_con);
}

MYSQL * Database::getConnection(){
	return _con;
}

void Database::checkEnv(){
	Logger::log("DBMS", "ENVCHECK", "Starting Database checks", LOG_NORMAL);
	std::unordered_map<std::string, std::unordered_map<std::string, compare<ColData *> *> *> tableMap;

	for (unsigned int j = 0; j < Database::tables.size(); j++){
		MySQLTable * tbl = Database::tables.at(j);
		std::string tblName = tbl->getName();
		std::unordered_map<std::string, compare<ColData *> *> * map = new std::unordered_map<std::string, compare<ColData *> *>();
		tableMap.insert(std::make_pair(tblName, map));
		tbl->mapTable(map);
	}

	std::string sql = "SHOW TABLES";
	MYSQL_RES * res = Database::Query(sql);
	if (res != NULL){
		tableMap.reserve((unsigned int)mysql_num_rows(res));
		for (int i = 0; i < mysql_num_rows(res); i++){
			MYSQL_ROW row = mysql_fetch_row(res);
			std::string tblName = row[0];
			std::unordered_map<std::string, compare<ColData *> *> * map;
			std::unordered_map<std::string, std::unordered_map<std::string, compare<ColData *> * > * >::iterator it = tableMap.find(tblName);
			//Logger::log("DBMS", "ENVCHECK", "Found Table: `" + tblName + "`", LOG_NORMAL);
			if (it == tableMap.end()){
				map = new std::unordered_map<std::string, compare<ColData *> *>();
				tableMap.insert(std::make_pair(tblName, map));
			}
			else{
				map = it->second;
			}
			
			MYSQL_RES * res2;
			std::string sql2 = "SHOW COLUMNS FROM `" + tblName + "`";
			res2 = Database::Query(sql2);
			if (res2 != NULL){
				for (int i = 0; i < mysql_num_rows(res2); i++){
					MYSQL_ROW row2 = mysql_fetch_row(res2);
					std::string colName = row2[0];
					
					std::unordered_map<std::string, compare<ColData *> *>::iterator it2 = map->find(colName);
					compare<ColData *> * cmp;
					if (it2 == map->end()){
						cmp = new compare<ColData *>();
						map->insert(std::make_pair(colName, cmp));
					}
					else{
						cmp = it2->second;
					}

					cmp->left = new ColData();
					cmp->left->type = row2[1];
					if (std::string(row2[2]) == "YES") cmp->left->null = true; else cmp->left->null = false;
					if (row2[3]) cmp->left->key = row2[3];
					if (row2[4]) cmp->left->default = row2[4];
					if (row2[5]) cmp->left->extra = row2[5];

					//Logger::log("DBMS", "ENVCHECK", "Found Column: `" + std::string(row2[0]) + "`", LOG_NORMAL);
				}
			}
			mysql_free_result(res2);
		}
	}
	mysql_free_result(res);

	for (const auto & table_pair : tableMap) {
		
		bool tblRequired = false;
		bool tblError = false;
		std::unordered_map<std::string, compare<ColData *> *> map = *table_pair.second;
		for (const auto & table_col_pair : map ) {
			if (table_col_pair.second->right){
				tblRequired = true;
				if (table_col_pair.second->left){
					//Logger::log("DBMS", "ENVCHECK", "Column: `" + table_col_pair.first + "`");
					//Logger::log("DBMS", "ENVCHECK", "Checking match");
					if (table_col_pair.second->right->default != table_col_pair.second->left->default){
						Logger::log("DBMS", "ENVCHECK", "Column: `" + table_col_pair.first + "` mismatching default");
						tblError = true;
					}
					if (table_col_pair.second->right->extra != table_col_pair.second->left->extra){
						Logger::log("DBMS", "ENVCHECK", "Column: `" + table_col_pair.first + "` mismatching extra");
						tblError = true;
					}
					if (table_col_pair.second->right->key != table_col_pair.second->left->key){
						Logger::log("DBMS", "ENVCHECK", "Column: `" + table_col_pair.first + "` mismatching key");
						tblError = true;
					}
					if (table_col_pair.second->right->null != table_col_pair.second->left->null){
						Logger::log("DBMS", "ENVCHECK", "Column: `" + table_col_pair.first + "` mismatching null");
						tblError = true;
					}
					if (table_col_pair.second->right->type != table_col_pair.second->left->type){
						Logger::log("DBMS", "ENVCHECK", "Column: `" + table_col_pair.first + "` mismatching type");
						tblError = true;
					}
					//Compare:
					//table_col_pair.second->right
					//table_col_pair.second->left
				}
				else{
					Logger::log("DBMS", "ENVCHECK", "Column: `" + table_col_pair.first + "` not provided");
					tblError = true;
				}
			}
			else{
				//Logger::log("DBMS", "ENVCHECK", "Column not required");
			}
		}
		if (!tblRequired){
			Logger::log("DBMS", "ENVCHECK", "Table: `" + table_pair.first + "` not required");
		}
		else if (tblError){
			Logger::log("DBMS", "ENVCHECK", "Table: `" + table_pair.first + "` not matching");
		}
		else{
			Logger::log("DBMS", "ENVCHECK", "Table: `" + table_pair.first + "` matched");
		}
	}
}

void Database::registerTables(){
	Database::tables.reserve(1);
	//AccountsDB
	Database::registerTable(new AccountsTable());
	Database::registerTable(new SessionsTable());
	//CharactersDB
	Database::registerTable(new CharactersTable());
	Database::registerTable(new FriendsTable());
	Database::registerTable(new MissionsTable());
	Database::registerTable(new MailsTable());
	//InventoryDB
	Database::registerTable(new InventoryTable());
	Database::registerTable(new EquipmentTable());
	Database::registerTable(new ObjectsTable());
	//ServerDB
	Database::registerTable(new InstancesTable());
}

void Database::registerTable(MySQLTable * table){
	Database::tables.push_back(table);
}

void Database::addColToMap(std::unordered_map<std::string, compare<ColData *> *> * data, std::string colName, ColData * colData){
	std::unordered_map<std::string, compare<ColData *> *>::iterator it;

	compare<ColData *> * col;
	it = data->find(colName);
	if (it == data->end()){
		col = new compare<ColData *>();
		data->insert(std::make_pair(colName, col));
	}
	else{
		col = it->second;
	}
	col->right = colData;
}