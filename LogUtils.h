/*
 * LogUtils.h
 *
 *  Created on: 2017年6月19日
 *      Author: zbf
 */

#ifndef LOGUTILS_H_
#define LOGUTILS_H_
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <string>
#include <string.h>
#include <unistd.h>
//#include <cstdint>

typedef float single;
#define DATA_NUM 6
class LogUtils {
public:
	LogUtils(int i = 0);
	void log_in(int num);
	void log_in(long num);
	void log_in(std::string num);
	void log_in(single num);
	void log_pause();
	void log_end();
	void log(short int* nums);
	void log(long* nums);
	void log(single* nums);
	bool readIn();
	~LogUtils();
private:
	std::string fileName;
	std::ostringstream timeStream;
	std::string timeStr;
	std::string createName();
	std::string createName(int i);
	std::string createHeader();
	int data[DATA_NUM];
	std::ofstream* _file;
	std::ifstream* _file_read;
};

#endif /* LOGUTILS_H_ */
