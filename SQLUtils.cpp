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
		"	vicon_pitch float,"+
		"	vicon_roll float,"+
		"	vicon_yaw float,"+
		"	vicon_vx float,"+
		"	vicon_vy float,"+
		"	vicon_vz float,"+
		"	vicon_battery int,"+
		"	vicon_cpu_load int,"+
		"	vicon_count int,"+
		"	vicon_set_position float,"+
		"	vicon_set_velocity float,"+
		"	vicon_calc_thrust float,"+
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
			"	vicon_pitch,"+
			"	vicon_roll,"+
			"	vicon_yaw,"+
			"	vicon_vx,"+
			"	vicon_vy,"+
			"	vicon_vz,"+
			"	vicon_battery,"+
			"	vicon_cpu_load,"+
			"	vicon_count,"+
			"	vicon_set_position,"+
			"	vicon_set_velocity,"+
			"	vicon_calc_thrust)"+
			"values(";
	sprintf(temp,"%d",sqlData->viconData.timestamp);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->viconData.x);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->viconData.y);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->viconData.z);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->viconData.pitch);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->viconData.roll);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->viconData.yaw);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->viconData.vx);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->viconData.vy);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->viconData.vz);
	query+=temp;
	query+=",";
	sprintf(temp,"%d",sqlData->state.battery);
	query+=temp;
	query+=",";
	sprintf(temp,"%d",sqlData->state.cpu_load);
	query+=temp;
	query+=",";
	sprintf(temp,"%d",sqlData->state.vicon_count);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->set_position);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->set_velocity);
	query+=temp;
	query+=",";
	sprintf(temp,"%f",sqlData->calc_thrust);
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

