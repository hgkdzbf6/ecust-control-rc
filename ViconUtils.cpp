#include "ViconUtils.h"
//构造函数吧
ViconUtils::ViconUtils(std::string  _subject, std::string _segment)
:flag(false),subject(_subject),segment(_segment),HOST("192.168.1.212:801")
{
	vicon_init();
}

ViconUtils::ViconUtils()
:flag(false),subject("H2"),segment("H2"),HOST("192.168.1.212:801")
{
	vicon_init();
}

void ViconUtils::get_unlabeled_marker(void){
	MyClient.GetFrame();
	unsigned int UnlabeledMarkerCount = MyClient.GetUnlabeledMarkerCount().MarkerCount;
	int i;
	for(i=0;i<UnlabeledMarkerCount;i++){
		UnlabeledMarkerOutput =  MyClient.GetUnlabeledMarkerGlobalTranslation(i);
		unlabeledMarker.Translations[i][0]=UnlabeledMarkerOutput.Translation[0];
		unlabeledMarker.Translations[i][1]=UnlabeledMarkerOutput.Translation[1];
		unlabeledMarker.Translations[i][2]=UnlabeledMarkerOutput.Translation[2];
	}
	unlabeledMarker.count=UnlabeledMarkerCount;
}

int ViconUtils::unlabeled_count(){
	return unlabeledMarker.count;
}

void ViconUtils::get_ball(){
	get_unlabeled_marker();
	int i;
//	判断高度是否大于BALL_MIN_HEIGHT
	for(i=0;i<unlabeledMarker.count;i++){
		if(unlabeledMarker.Translations[i][2]>BALL_MIN_HEIGHT){
			ballData.position[0]=unlabeledMarker.Translations[i][0];
			ballData.position[1]=unlabeledMarker.Translations[i][1];
			ballData.position[2]=unlabeledMarker.Translations[i][2];
			break;
		}
	}
//	算速度
	ball_frame_num= MyClient.GetFrameNumber();
	if(ball_frame_num.Result==2){
		ballData.timestamp=ball_frame_num.FrameNumber;
	}
	if(ballData.pre_timestamp==0){
		ballData.speed[0]=0;
		ballData.speed[1]=0;
		ballData.speed[2]=0;
	}else{
		double dt=(ballData.timestamp-ballData.pre_timestamp)/VICON_FREQ;
		double dpx=ballData.position[0]-ballData.pre_position[0];
		double dpy=ballData.position[1]-ballData.pre_position[1];
		double dpz=ballData.position[2]-ballData.pre_position[2];
		ballData.speed[0]=dpx/dt;
		ballData.speed[1]=dpy/dt;
		ballData.speed[2]=dpz/dt;
	}
	ballData.pre_timestamp=ballData.timestamp;
	ballData.pre_position[0]=ballData.position[0];
	ballData.pre_position[1]=ballData.position[1];
	ballData.pre_position[2]=ballData.position[2];
}

double ViconUtils::ball_speed(int num){
	return ballData.speed[num];
}
double ViconUtils::ball_position(int num){
	return ballData.position[num];
}
double ViconUtils::ball_timestamp(){
	return ballData.timestamp;
}

double ViconUtils::unlabeled_marker(int num,int num2){
	if(num<unlabeledMarker.count){
		return unlabeledMarker.Translations[num][num2];
	}
	return 0;
}

//初始化
void ViconUtils:: vicon_init(){
	memset(p_speed,0,sizeof(double)*3);
    while( !MyClient.IsConnected().Connected ){
    	MyClient.Connect(HOST);
    }
	//if wait too long time,it may be caused by you choose wrong network
//    使能获取数据
	MyClient.EnableSegmentData();
	MyClient.EnableUnlabeledMarkerData();
	MyClient.GetFrame();
//	先预先取得一次数据
	get_translation_data();
	get_rotation_data();
//	check的作用是检查有效的两组数据，然后为算速度做准备
	check();
}
ViconUtils::~ViconUtils()
{
	MyClient.Disconnect();
}

bool ViconUtils::get_translation_data()
{
	int i=0;
	//MyClient.Connect(HOST);
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
			printf("PositionOutput.Result=%d\n",PositionOutput.Result);
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
	//MyClient.Connect(HOST);
	//try 10 times
	for(i=0;i<MAX_TRY_TIMES;i++){
		MyClient.EnableSegmentData();
		rotation_frame_num= MyClient.GetFrameNumber();
		frame=MyClient.GetFrame();
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
	//MyClient.Connect(HOST);
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
