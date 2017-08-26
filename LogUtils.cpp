/*
 * LogUtils.cpp
 *
 *  Created on: 2017年6月19日
 *      Author: zbf
 */

#include "LogUtils.h"

LogUtils::LogUtils(int i) {
	// TODO Auto-generated constructor stub
	createName(i);
	_file=new std::ofstream(this->fileName.c_str());
	//_file_read=new std::ifstream("./setting.ini");
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
std::string LogUtils::createName(int i) {
	time_t tt = time(NULL);
	tm* t = localtime(&tt);
	timeStream << 1900 + t->tm_year << std::setfill('0') << std::setw(2)
			<< 1 + t->tm_mon << std::setfill('0') << std::setw(2) << t->tm_mday
			<< std::setfill('0') << std::setw(2) << t->tm_hour
			<< std::setfill('0') << std::setw(2) << t->tm_min
			<< std::setfill('0') << std::setw(2) << t->tm_sec;
	timeStr = timeStream.str();
	fileName = "data" + std::to_string(i) + "_" + timeStr + ".csv";
	return fileName;
}
std::string LogUtils::createName(){
	return createName(0);
}
void LogUtils::log_in( int num){
	*_file<<num;
}
void LogUtils::log_in( std::string num){
	*_file<<num;
}

void LogUtils::log_in( single num){
	*_file<<num;
}
void LogUtils::log_in( long num){
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
