#include "ViconUtils.h"

ViconUtils::ViconUtils(std::string  _subject, std::string _segment)
:flag(false),subject(_subject),segment(_segment),HOST("192.168.1.106:801")
{
	vicon_init();
}

ViconUtils::ViconUtils()
:flag(false),subject("H1"),segment("H1"),HOST("192.168.1.106:801")
{
	vicon_init();
}

void ViconUtils:: vicon_init(){
	memset(p_speed,0,sizeof(double)*3);
	MyClient.Connect(HOST);
	//if wait too long time,it may be caused by you choose wrong network
	MyClient.EnableSegmentData();
	MyClient.GetFrame();
	get_translation_data();
	get_rotation_data();
	check();
}
ViconUtils::~ViconUtils()
{
	MyClient.Disconnect();
}

bool ViconUtils::get_translation_data()
{
	int i=0;
	MyClient.Connect(HOST);
	//try 10 times
	for(i=0;i<MAX_TRY_TIMES;i++){
		MyClient.EnableSegmentData();
		position_frame_num= MyClient.GetFrameNumber();
		MyClient.GetFrame();
		PositionOutput = MyClient.GetSegmentGlobalTranslation(subject, segment);
		if(PositionOutput.Result == 2){
			raw.Translations[0]= PositionOutput.Translation[0];
			raw.Translations[1] = PositionOutput.Translation[1];
			raw.Translations[2] = PositionOutput.Translation[2];
		}else{
			continue;
		}
		position_frame_num= MyClient.GetFrameNumber();
		if(position_frame_num.Result==2){
			raw.tp_position=position_frame_num.FrameNumber;
			return true;
		}
	}
	return false;
}

bool ViconUtils::get_rotation_data()
{
	int i=0;
	MyClient.Connect(HOST);
	//try 10 times
	for(i=0;i<MAX_TRY_TIMES;i++){
		MyClient.EnableSegmentData();
		rotation_frame_num= MyClient.GetFrameNumber();
		MyClient.GetFrame();
		AngleOutput = MyClient.GetSegmentGlobalRotationEulerXYZ(subject, segment);
		if(AngleOutput.Result == 2){
			raw.Rotations[0]= AngleOutput.Rotation[0];
			raw.Rotations[1] = AngleOutput.Rotation[1];
			raw.Rotations[2] = AngleOutput.Rotation[2];
		}else{
			continue;
		}
		rotation_frame_num= MyClient.GetFrameNumber();
		if(rotation_frame_num.Result==2){
			raw.tp_angle=rotation_frame_num.FrameNumber;
			return true;
		}
	}
	return false;
}

bool ViconUtils::get_quaternion(){
	int i=0;
	MyClient.Connect(HOST);
	//try 10 times
	for(i=0;i<MAX_TRY_TIMES;i++){
		MyClient.EnableSegmentData();
		quaternion_frame_num= MyClient.GetFrameNumber();
		MyClient.GetFrame();
		QuaternionOutput = MyClient.GetSegmentGlobalRotationQuaternion(subject, segment);
		if(PositionOutput.Result == 2){
			raw.Quaternion[0]= QuaternionOutput.Rotation[0];
			raw.Quaternion[1]= QuaternionOutput.Rotation[1];
			raw.Quaternion[2]= QuaternionOutput.Rotation[2];
			raw.Quaternion[3]= QuaternionOutput.Rotation[3];
		}else{
			continue;
		}
		quaternion_frame_num= MyClient.GetFrameNumber();
		if(quaternion_frame_num.Result==2){
			raw.tp_quaternion=quaternion_frame_num.FrameNumber;
			return true;
		}
	}
	return false;
}
void ViconUtils::update_data(){
	memcpy(&pre_valid,&last_valid,sizeof(ViconData));
}
void ViconUtils::get_speed(void){
	u_int32_t dt=last_valid.tp_position-pre_valid.tp_position;
	if(dt==0)return;
	double dp0=last_valid.Translations[0]-pre_valid.Translations[0];
	double dp1=last_valid.Translations[1]-pre_valid.Translations[1];
	double dp2=last_valid.Translations[2]-pre_valid.Translations[2];

	p_speed[0]=dp0/dt*VICON_FREQ;
	p_speed[1]=dp1/dt*VICON_FREQ;
	p_speed[2]=dp2/dt*VICON_FREQ;

	update_data();
}

void ViconUtils::check(void){
	if(position_frame_num.Result==2 && PositionOutput.Result==2){
		memcpy(&last_valid,&raw,sizeof(ViconData));
	}
}

double ViconUtils::translation( int num)
{
	return last_valid.Translations[num];
}
double ViconUtils::rotation( int num)
{
	return last_valid.Rotations[num];
}
double ViconUtils::speed(int num){
	return p_speed[num];
}

double ViconUtils::quaternion(int num){
	return last_valid.Quaternion[num];
}
long ViconUtils::tp(int num){
	if(num==0)
	return last_valid.tp_position;
	return last_valid.tp_angle;
}
