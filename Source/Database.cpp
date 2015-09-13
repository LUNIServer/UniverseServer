#include "Database.h"
#include "Logger.h"
#include <iostream>
using namespace std;

MYSQL * Database::_con = NULL; // Set MYSQL connection variable to NULL

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
		case 2005:
			std::cout << std::endl;
			std::cout << "  == [ERROR HELP] ==" << std::endl;
			std::cout << std::endl;
			std::cout << "  No MYSQL server found at host '" << host << "'" << std::endl;
			std::cout << "  Possible reasons for this are:" << std::endl;
			std::cout << "  - There is a typo in the host name" << std::endl;
			std::cout << "  - The wrong config file has been loaded" << std::endl;
			std::cout << "  - The MYSQL Server is not running" << std::endl;
			std::cout << std::endl;
			if (!(host == "localhost" || host == "127.0.0.1")){
				std::cout << "  The default host is 'localhost' or '127.0.0.1'" << std::endl;
				std::cout << "  If you have not expicitly chosen something different," << std::endl;
				std::cout << "  try this as host" << std::endl;
			}
			std::cout << std::endl;
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