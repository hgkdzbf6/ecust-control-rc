#pragma once
#include "DataStreamClient.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef MY_PI
#define MY_PI 3.14159265358979
#endif

#define VICON_FREQ 200
using namespace std;

#define MAX_TRY_TIMES 10

//typedef unsigned long u_int32_t;

typedef struct __VICON_DATA{
	u_int32_t tp_position;
	u_int32_t tp_angle;
	u_int32_t tp_quaternion;
	double Translations[3];
	double Rotations[3];
	double Quaternion[4];
}ViconData;


class ViconUtils
{
public:
	ViconUtils();
	ViconUtils(std::string subjects, std::string segments);
	~ViconUtils();
	bool get_translation_data();
	bool get_rotation_data();
	bool get_quaternion();
	void get_speed();
	long tp(int num);
	void update_data();
	double speed(int num);
	double translation(int num);
	double quaternion(int num);
	double rotation(int num);
	void check();
	void vicon_init();
private:
	ViconDataStreamSDK::CPP::Output_GetFrameNumber position_frame_num;
	ViconDataStreamSDK::CPP::Output_GetFrameNumber rotation_frame_num;
	ViconDataStreamSDK::CPP::Output_GetFrameNumber quaternion_frame_num;

	std::string HOST;
	std::string subject;
	std::string segment;
	ViconDataStreamSDK::CPP::Client MyClient;
	ViconData pre_valid;
	ViconData last_valid;
	ViconData raw;
	double p_speed[3];
	ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationEulerXYZ AngleOutput;
	ViconDataStreamSDK::CPP::Output_GetSegmentGlobalTranslation PositionOutput;
	ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationQuaternion QuaternionOutput;
	bool flag;
};

