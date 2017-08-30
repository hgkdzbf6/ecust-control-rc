#pragma once
#include "DataStreamClient.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef MY_PI
#define MY_PI 3.14159265358979
#endif

#define VICON_FREQ 200.0f
using namespace std;

#define MAX_TRY_TIMES 3
#define BALL_MIN_HEIGHT 300
//typedef unsigned long u_int32_t;
typedef struct __POINT_DATA{
	u_int32_t pre_timestamp;
	u_int32_t timestamp;
	double speed[3];
	double pre_position[3];
	double position[3];
}PointData;

typedef PointData BallData;

typedef struct __VICON_DATA{
	u_int32_t tp_position;
	u_int32_t tp_angle;
	u_int32_t tp_quaternion;
	double Translations[3];
	double Rotations[3];
	double Quaternion[4];
}ViconData;


//最大marker点数量
#define UNLABELED_MARKER_MAX_NUM 100
//未标记的marker点
typedef struct __UNLABELED_MARKER{
	int count;
	double Translations[UNLABELED_MARKER_MAX_NUM][3];
}UnlabeledMarker;


class ViconUtils
{
public:
	ViconUtils();
	ViconUtils(std::string subjects, std::string segments);
	~ViconUtils();
	bool get_translation_data();
	bool get_rotation_data();
	bool get_quaternion();
	void get_unlabeled_marker();
	void get_speed();
	long tp(int num);
	void update_data();
	double speed(int num);
	double translation(int num);
	double quaternion(int num);
	int unlabeled_count();
	double rotation(int num);
	void get_ball();
	void get_ball2();
	double ball_speed(int num);
	double ball_position(int num);
	double ball_timestamp();
	double unlabeled_marker(int num,int num2);
	void check();
	void vicon_init();
private:
	ViconDataStreamSDK::CPP::Output_GetFrameNumber position_frame_num;
	ViconDataStreamSDK::CPP::Output_GetFrameNumber rotation_frame_num;
	ViconDataStreamSDK::CPP::Output_GetFrameNumber quaternion_frame_num;
	ViconDataStreamSDK::CPP::Output_GetFrameNumber ball_frame_num;

	ViconDataStreamSDK::CPP::Output_GetFrame frame;
	std::string HOST;
	std::string subject;
	std::string segment;
	ViconDataStreamSDK::CPP::Client MyClient;
	ViconData pre_valid;
	ViconData last_valid;
	ViconData raw;

	UnlabeledMarker unlabeledMarker;
	double p_speed[3];
	BallData ballData;

	ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationEulerXYZ AngleOutput;
	ViconDataStreamSDK::CPP::Output_GetSegmentGlobalTranslation PositionOutput;
	ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationQuaternion QuaternionOutput;
	ViconDataStreamSDK::CPP::Output_GetUnlabeledMarkerGlobalTranslation UnlabeledMarkerOutput;
	bool flag;
};

