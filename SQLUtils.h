/*
 * SQLUtils.h
 *
 *  Created on: 2017年6月30日
 *      Author: zbf
 */

#ifndef SQLUTILS_H_
#define SQLUTILS_H_
#include "mysql/mysql.h"
#include <iostream>
#include <time.h>
#include <iomanip>
#include <sstream>
#include "MyProtocol.h"
#include <unistd.h>
using namespace std;
class SQLUtils {
public:
	SQLUtils();
	int init();
	void dataIn(SqlData *sqlData);
	string createTableName();
	virtual ~SQLUtils();
private:
	MYSQL mysql;
	string host;
	string tableName;
	std::string user;
	std::string pwd;
	string databaseName;
	ostringstream timeStream;
	string timeStr;
};

#endif /* SQLUTILS_H_ */
