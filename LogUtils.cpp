/*
 * LogUtils.cpp
 *
 *  Created on: 2017年6月19日
 *      Author: zbf
 */

#include "LogUtils.h"

LogUtils::LogUtils() {
	// TODO Auto-generated constructor stub
	_file=new std::ofstream("./data.csv");
	_file_read=new std::ifstream("./setting.ini");
	_file->is_open();
}

LogUtils::~LogUtils() {
	// TODO Auto-generated destructor stub
	_file->close();
	_file_read->close();
	delete _file;
	delete _file_read;
	_file=NULL;
	_file_read=NULL;
}

void LogUtils::log_in(int num){
	*_file<<num;
}
void LogUtils::log_in(single num){
	*_file<<num;
}
void LogUtils::log_in(long num){
	*_file<<num;
}
void LogUtils::log_pause(){
	*_file<<",";
}
void LogUtils::log_end(){
	*_file<<std::endl;
	_file->flush();
}
void LogUtils::log(short int* nums){
	int i;
	log_in(nums[0]);
	for(i=1;i<DATA_NUM;i++){
		log_pause();
		log_in(nums[i]);
	}
	log_end();
}
void LogUtils::log(long* nums){
	int i;
	log_in(nums[0]);
	for(i=1;i<DATA_NUM;i++){
		log_pause();
		log_in(nums[i]);
	}
	log_end();
}
void LogUtils::log(single* nums){
	int i;
	log_in(nums[0]);
	for(i=1;i<DATA_NUM;i++){
		log_pause();
		log_in(nums[i]);
	}
	log_end();
}
