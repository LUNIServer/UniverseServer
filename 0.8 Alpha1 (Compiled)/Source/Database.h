#pragma once

#include <string>
#include <unordered_map>

#include <my_global.h> // If you have included MySQL in Project Settings
#include <mysql.h> // If you have included MySQL in Project Settings

// This is a MySQL Exception subclasses from std::exception
class MySqlException : public std::exception {
	public:
	MySqlException() : std::exception("mysql error") {}
	MySqlException(const std::string& msg) : std::exception(msg.c_str()) {}
};

enum MYSQL_STATUS : unsigned int{
	SUCESS = 0,
	NOT_ENOUGH_MEMORY = 1,
};

template <typename T>
struct compare{
	T left; //Current state
	T right; //System state
};

class ColData{
public:
	std::string type;
	bool null;
	std::string key;
	std::string default;
	std::string extra;
	ColData(std::string t, bool n, std::string k, std::string d, std::string e);
	ColData();
};

class MySQLTable{
public:
	virtual std::string getName() = 0;
	virtual void mapTable(std::unordered_map<std::string, compare<ColData *> *> * data) = 0;
};

// This is the declaration for methods that will be used to connect to
// MySQL database
class Database {
	private:
		static MYSQL * _con; // This is the MySQL Connection
		static std::vector<MySQLTable *> tables;
		Database() {} // Private Database initializer
	public:
		// Initialize connection.
		// This takes the host, the database, the DB username, and the DB password as strings and connects the server to
		// the MySQL database
		static unsigned int Connect(const std::string& host, const std::string& database, const std::string& username, const std::string& password);

		// Query the MySQL database and return results
		static MYSQL_RES * Query(const std::string & query); 
		static MYSQL * getConnection();
		static void checkEnv();
		static void registerTables();
		static void registerTable(MySQLTable * tbl);
		static void addColToMap(std::unordered_map<std::string, compare<ColData *> *> * data, std::string colName, ColData * colData);
};