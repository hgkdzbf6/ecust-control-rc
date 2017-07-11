/*
 * SQLUtils.cpp
 *
 *  Created on: 2017年6月30日
 *      Author: zbf
 */

#include "SQLUtils.h"

SQLUtils::SQLUtils():
host("localhost"),user("root"),pwd("1"),databaseName("ecust415")
{
	// TODO Auto-generated constructor stub
	init();
}
int SQLUtils::init(void){
	string query;
	int res;
	mysql_init(&mysql);
	mysql_close(&mysql);
	mysql_real_connect(&mysql,
			host.c_str(),
			user.c_str(),
			pwd.c_str(),
			databaseName.c_str(),
			3306,NULL,0);
	res=mysql_query(&mysql,
			"create database if not exists ecust415;");
	if(res!=0)printf("1 %d error occurs.\n",res);
	res=mysql_query(&mysql,
			"use ecust415;");
	if(res!=0)printf("2 %d error occurs.\n",res);
	query="create table if not exists "+createTableName()+"("+
		"	vicon_timestamp int unsigned,"+
		"	vicon_x float,"+
		"	vicon_y float,"+
		"	vicon_z float,"+
		"	vicon_vx float,"+
		"	vicon_vy float,"+
		"	vicon_vz float,"+
		"	vicon_yaw float,"+
		"	sp_x float,"+
		"	sp_y float,"+
		"	sp_z float,"+
		"	sp_flag int,"+
		"	debug_1 float,"+
		"	debug_2 float,"+
		"	debug_3 float,"+
		"	debug_4 float,"+
		"	primary key(vicon_timestamp)"+
	")engine=innodb default charset=utf8;";
	res=mysql_query(&mysql,
			query.c_str());
	if(res!=0)printf("3 %d error occurs:%s\n",mysql_errno(&mysql),mysql_error(&mysql));
	return 1;
}

void SQLUtils::dataIn(SqlData *sqlData){
	string query;
	int res;
	char temp[10];
	query="insert into "+tableName+"("+
			"	vicon_timestamp,"+
			"	vicon_x,"+
			"	vicon_y,"+
			"	vicon_z,"+
			"	vicon_vx,"+
			"	vicon_vy,"+
			"	vicon_vz,"+
			"	vicon_yaw,"+
			"	sp_x,"+
			"	sp_y,"+
			"	sp_z,"+
			"	sp_flag,"+
			"	debug_1,"+
			"	debug_2,"+
			"	debug_3,"+
			"	debug_4)"+
			"values(";
	sprintf(temp,"%d",sqlData->timestamp);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->x);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->y);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->z);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->vx);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->vy);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->vz);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->yaw);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->sp_x);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->sp_y);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->sp_z);
	query+=temp;
	query+=",";
	sprintf(temp,"%d",sqlData->sp_flag);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->debug_1);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->debug_2);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->debug_3);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->debug_4);
	query+=temp;
	query+=");";
	printf("%s",query.c_str());
	res=mysql_query(&mysql,
			query.c_str());
	if(res!=0)printf("4 %d error occurs:%s\n",mysql_errno(&mysql),mysql_error(&mysql));
}

string SQLUtils::createTableName(){
	time_t tt=time(NULL);
	tm* t=localtime(&tt);
	timeStream<< 1900+t->tm_year
		<<setfill('0') << setw(2)<<1+t->tm_mon
		<<setfill('0') << setw(2)<<t->tm_mday
		<<setfill('0') << setw(2)<<t->tm_hour
		<<setfill('0') << setw(2)<<t->tm_min
		<<setfill('0') << setw(2)<<t->tm_sec;
	timeStr=timeStream.str();
	tableName="data_"+timeStr;
	return tableName;
}


SQLUtils::~SQLUtils() {
	// TODO Auto-generated destructor stub
	mysql_close(&mysql);
}

