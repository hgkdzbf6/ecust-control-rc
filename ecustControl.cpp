#include "ViconUtils.h"
#include "LogUtils.h"
#include "MyProtocol.h"
#include "mysql/mysql.h"
#include "SQLUtils.h"
#include "MyWayPoint.h"
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h>  // File control definitions
#include <termios.h> // POSIX terminal control definitionss
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <curses.h>
using namespace zbf;

#define BALL_MODE
#define LOG_DEBUG_ARRAY
//#define LOG_NORMAL

ViconUtils *vicon;
LogUtils *logUtils;
LogUtils *logUtils2;
SQLUtils *sql;
MyWayPoint  *mwp;
MyViconData viconData;
SensorData sensorData;
SystemState systemState;
FusionData fusionData;

CmdData sendCmdData={0};
CmdData receiveCmdData={0};
DebugData receiveDebugData={0};
DebugData sendDebugData={0};

SqlData sqlData={0};
ParamDebug sendParamDebug={0};
ParamDebug receiveParamDebug={0};
PositionWayPointData receivePositionWayPointData={0};
PositionWayPointData sendPositionWayPointData={0};

LandSignal sendLandSignal={LAND_MODE_NONE};
LandSignal receiveLandSignal={LAND_MODE_NONE};

NormalData sendNormalData={0};
NormalData receiveNormalData={0};

DebugArray receiveDebugArray={{0},{0}};
PackageDefine pack_id;
unsigned char allDataBuffer[400]={0};
//MyViconData receivedViconData;
//SensorData receivedSensorData;
//SystemState receivedSystemState;
//FusionData receivedFusionData;


 long signed int (*read_callback)(int,void*,unsigned long);
 long signed int (*write_callback)(int,const void*,unsigned long int);

int fd;
clock_t clock_start,clock_end;
clock_t max_send_time=0,min_send_time=30000;
#ifdef PARAM_DEBUG_MODE
PackageDefine pd=PACKAGE_DEFINE_PARAM;
unsigned char pl=PARAM_DEBUG_LENGTH;
#endif

#ifdef DEBUG_DATA_MODE
PackageDefine pd=PACKAGE_DEFINE_VICON;
unsigned char pl=VICON_DATA_LENGTH;
#endif


unsigned char buffer[400];
unsigned char record_flag=0;
unsigned char cmd_flag=PACKAGE_DEFINE_DEBUG;
unsigned char show_flag=1;
unsigned char way_point_flag=0;
unsigned char received_k_info_flag=0;

void send_thread(int a);
void* receive_thread(void*);
void menu();
void packSqlData();
void packDebugArray();
void packSimpleData();
void nextMenu(char cmd);
void createHeader();
void showReceiveParamDebugOnce();
void showSendParamDebug();
void setSendParamDebug();
void setSendDebugData();
void initMyWayPoint();
void showSendDebugData();
void showDebugData(int pre_timestamp);
void setSendPositionWayPointData();
void setBallData();
void logBallData();
void setSendNormalData();
void showReceiveNormalData();
void showReceiveDebugArray();
void showReceivePositionWayPointData();
void showSendPositionWayPointDataOnce();
//manual:
//comming params:
//it consist of 3 params
//first is what USB port your device used.
//second is what vicon segment and subject are your vicon object used.
//third is the send cmd
//fourth is the switch of mysql data

int main(int argc, char* argv[]){
	//pthread_t p_send;
	pthread_t p_receieve;
	char port[30]="/dev/ttyUSB";
	char vicon_name[20]="H";
	if(argc>1){
		if(strcmp(argv[1],"1")==0){
			strcat(port,"1");
		}else{
			strcat(port,"0");
		}
	}else{
		strcat(port,"0");
	}
	if(argc>2){
		if(strcmp(argv[2],"1")==0){
			strcat(vicon_name,"1");
		}else{
			strcat(vicon_name,"2");
		}
	}else{
		strcat(vicon_name,"2");
	}
	if(argc>3){
		if(strcmp(argv[3],"1")==0){
			record_flag=1;
		}else if(strcmp(argv[3],"2")==0){
			record_flag=2;
		}else if(strcmp(argv[3],"0")==0){
			record_flag=0;
		}
	}else{
		record_flag=0;
	}
	if(argc>4){
		if(strcmp(argv[4],"5")==0){
			cmd_flag=PACKAGE_DEFINE_DEBUG;
		}else{
			cmd_flag=PACKAGE_DEFINE_NOMAL_DATA;
		}
	}else{
		cmd_flag=PACKAGE_DEFINE_NOMAL_DATA;
	}

	sendParamDebug.kp_v=0.5f;
	sendParamDebug.ki_p=0.0f;
	sendParamDebug.kp_p=0.8f;
	sendParamDebug.ki_v=0.05f;

	printf("this is custom protocol of autopilot sdk 2.0\n");
	printf("the purpose of this program is to increase the sending frequency\n");
	printf("port is %s\n",port);
	printf("vicon name is %s\n",vicon_name);
	printf("if you wait for too long time, please check your network config.\n");
	vicon=new ViconUtils(vicon_name,vicon_name);
	vicon->vicon_init();

	initMyWayPoint();

	printf("vicon_init_ok!\n");
	printf("log utils init ok\n");
	if(record_flag==1){
		sql=new SQLUtils();
		printf("sql utils init ok.\n");
	}else if(record_flag==2){
		logUtils = new LogUtils();
		logUtils2 = new LogUtils(1);
#ifdef LOG_NORMAL
		createHeader();
#endif
		printf("log utils init ok.\n");
	}else{
		printf("you have switch off the mysql switch.\n");
	}

	//sendNormalData.sp_flag=0;

	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	struct termios port_settings; // structure to store the port settings in
	cfsetispeed(&port_settings, B57600); // set baud rates
	port_settings.c_cflag = B57600 | CS8 | CREAD | CLOCAL;
	port_settings.c_iflag = IGNPAR;
	port_settings.c_oflag = 0;
	port_settings.c_lflag = 0;
	tcsetattr(fd, TCSANOW, &port_settings); // apply the settings to the port

	read_callback=read;
	write_callback=write;
	printf("thread start!\n");
	//pthread_create(&p_send, NULL, send_thread, NULL);
	pthread_create(&p_receieve, NULL, receive_thread, NULL);
	int res=0;
	signal(SIGALRM,send_thread);
	struct itimerval tick;
	memset(&tick,0,sizeof(tick));
	tick.it_value.tv_sec=0;
	tick.it_value.tv_usec=20000;
	  tick.it_interval.tv_sec = 0;
	  tick.it_interval.tv_usec = 20000;
	  res = setitimer(ITIMER_REAL, &tick, NULL);
	  if (res) {
	    printf("Set timer failed!!/n");
	  }
	  sendNormalData.sp_flag=PACKAGE_DEFINE_NOMAL_DATA;
	  while(1){
		menu();
	}
	return 0;
}
void initMyWayPoint(){
	mwp=new MyWayPoint(WAY_POINT_TYPE_POSITION);
	mwp->setTolerance(30);
//	mwp->addNewPositionWayPoint(0,0,1000);
//	mwp->addNewPositionWayPoint(500,0,1000);
//	mwp->addNewPositionWayPoint(-500,0,1000);
//	mwp->addNewPositionWayPoint(-500,500,1000);
//	mwp->addNewPositionWayPoint(-500,-500,1000);
//	mwp->addNewPositionWayPoint(500,-500,1000);
//	mwp->addNewPositionWayPoint(0,0,1000);
	mwp->generateCircle(20,0,0,700,750,0);
	mwp->generateCircle(20,0,0,700,750,0);
	mwp->generateCircle(20,0,0,700,750,0);
	mwp->addNewPositionWayPoint(-740.766296,117.325836,100);
	mwp->showPositionWayPoint();
}
void createHeader(){
	logUtils->log_in("timestamp");
	logUtils->log_pause();
	logUtils->log_in("x");
	logUtils->log_pause();
	logUtils->log_in("y");
	logUtils->log_pause();
	logUtils->log_in("z");
	logUtils->log_pause();
	logUtils->log_in("vx");
	logUtils->log_pause();
	logUtils->log_in("vy");
	logUtils->log_pause();
	logUtils->log_in("vz");
	logUtils->log_pause();
	logUtils->log_in("yaw");
	logUtils->log_pause();
	logUtils->log_in("sp_x");
	logUtils->log_pause();
	logUtils->log_in("sp_y");
	logUtils->log_pause();
	logUtils->log_in("sp_z");
	logUtils->log_pause();
	logUtils->log_in("sp_flag");
	logUtils->log_pause();
	logUtils->log_in("debug_1");
	logUtils->log_pause();
	logUtils->log_in("debug_2");
	logUtils->log_pause();
	logUtils->log_in("debug_3");
	logUtils->log_pause();
	logUtils->log_in("debug_4");
	logUtils->log_end();
}

void menu(){
	static char showFlag=1;
	static char cmd=0;
	char meiyong;
	if(showFlag!=0){
		showFlag=0;
		printf("welcome to the menu\n");
		printf("press 0 toggle show received message\n");

		printf("increase kp_p by 0.01 please press 1\n");
		printf("decrease kp_p by 0.01 please press 2\n");

		printf("increase ki_p by 0.01 please press 3\n");
		printf("decrease ki_p by 0.01 please press 4\n");

		printf("increase kp_v by 0.01 please press 5\n");
		printf("decrease kp_v by 0.01 please press 6\n");

		printf("increase ki_v by 0.01 please press 7\n");
		printf("decrease ki_v by 0.01 please press 8\n");

		printf("increase thrust by 50 please press 9\n");
		printf("decrease thrust by 50 please press a\n");
	}else{
		printf("please input another cmd.\n");
	}
	fflush(stdin);
	scanf("%c%c",&cmd,&meiyong);
	fflush(stdin);
	nextMenu(cmd);
}

void nextMenu(char cmd){
	if(cmd=='0'){
		//0 is the mode, press i can toggle show the vicon data or not
		printf("you have switch debug param mode.\n");
		show_flag=(show_flag+1)%2;
	}
	else if((cmd<='9'&&cmd>'0')||(cmd=='a')){
		printf("param valid.\n");
		switch(cmd){
		case '0':
			break;
		case '1':
			//this is test mode, i can change the param of pid wirelessly.
			cmd_flag=PACKAGE_DEFINE_PARAM;
			break;
		case '2':
			//the original position way point mode
			//it will move to next point until it approach the current point
			way_point_flag=1;
			cmd_flag=PACKAGE_DEFINE_POSITION_WAY_POINT;
			sendLandSignal.mode=LAND_MODE_NONE;
			break;
		case '3':
			//it is also a position way point mode
			//in a fixed time, it will send a message to the aircraft
			sendNormalData.sp_flag=1;
			//way_point_flag=2;
			//cmd_flag=PACKAGE_DEFINE_POSITION_WAY_POINT;
			///sendLandSignal.mode=LAND_MODE_NONE;
			break;
		case '4':
			//here i set way_point_flag=0, for i don't want the waypoint data influence
			//the data of land data
			way_point_flag=0;
			sendNormalData.sp_flag=2;
			cmd_flag=PACKAGE_DEFINE_LAND;
			sendLandSignal.mode=LAND_MODE_SLOW;
			break;
		case '5':
			sendNormalData.sp_flag=2;
			break;
		case '6':
			sendNormalData.sp_flag=PACKAGE_DEFINE_DEBUG_ARRAY;
			break;
		case '7':
			break;
		case '8':
			break;
		case '9':
			break;
		case 'a':
			break;
		default:
			break;
		}
		//showReceiveParamDebugOnce();
	}else{
		printf("param invalid, please retry.\n");
	}
}

void send_thread(int a){
	static clock_t temp=0;
		clock_start=clock();
		//printf("%ld\n",clock_start-temp);
		fflush(stdout);
		temp=clock_start;
		//get vicon data from workstation
		vicon->get_translation_data();
		vicon->get_rotation_data();
		vicon->check();
		vicon->get_speed();
		vicon->update_data();
		vicon->get_ball();
		//setSendPositionWayPointData();
		setBallData();
		sendCmdData.cmd=cmd_flag;
		switch(sendCmdData.cmd){
		case PACKAGE_DEFINE_CMD:
			my_send(fd,PACKAGE_DEFINE_CMD,
					getPackageLength(PACKAGE_DEFINE_CMD),
					&sendCmdData,1);
			break;
		case PACKAGE_DEFINE_NOMAL_DATA:
			setSendNormalData();
			my_send(fd,PACKAGE_DEFINE_NOMAL_DATA,
					getPackageLength(PACKAGE_DEFINE_NOMAL_DATA)
					,&sendNormalData,1);
			sendNormalData.sp_flag=PACKAGE_DEFINE_NOMAL_DATA;
			break;
		case PACKAGE_DEFINE_DEBUG:
			setSendDebugData();
			//showSendDebugData();
			my_send(fd,PACKAGE_DEFINE_DEBUG,
				getPackageLength(PACKAGE_DEFINE_DEBUG),
				&sendDebugData,1);
			break;
		case PACKAGE_DEFINE_PARAM:
			setSendParamDebug();
			showSendParamDebug();
			my_send(fd,PACKAGE_DEFINE_PARAM,
					getPackageLength(PACKAGE_DEFINE_PARAM),
					&sendParamDebug,1);
			break;
		case PACKAGE_DEFINE_POSITION_WAY_POINT:
			//setSendPositionWayPointData();
			showSendPositionWayPointDataOnce();
			my_send(fd,PACKAGE_DEFINE_POSITION_WAY_POINT,
					getPackageLength(PACKAGE_DEFINE_POSITION_WAY_POINT),
					&sendPositionWayPointData,1);
			break;
		case PACKAGE_DEFINE_LAND:
			printf("land signal sent!\n");
			my_send(fd,PACKAGE_DEFINE_LAND,
					getPackageLength(PACKAGE_DEFINE_LAND),
					&sendLandSignal,1);
			break;
		}
}
void setSendPositionWayPointData(){
	static int i=0;
	static int freq=0;
	PositionWayPoint* pwp;
	if(sendNormalData.sp_flag==1){
		//pwp=mwp->guideCircle(i++,1000,1,0,0,750,750,0);
		pwp=mwp->generateList(i++,POSITION_DATA_LENGTH-1);
		sendNormalData.sp_x=pwp->x;
		sendNormalData.sp_y=pwp->y;
		sendNormalData.sp_z=pwp->z;
		//cmd_flag=PACKAGE_DEFINE_POSITION_WAY_POINT;
	}else if(sendNormalData.sp_flag==0){
		sendNormalData.sp_x=458;
		sendNormalData.sp_y=-1242;
		sendNormalData.sp_z=200;
	}
//	if(way_point_flag==1){
//		sendPositionWayPointData.x=mwp->sendCurrentPositionWayPoint()->x;
//		sendPositionWayPointData.y=mwp->sendCurrentPositionWayPoint()->y;
//		sendPositionWayPointData.z=mwp->sendCurrentPositionWayPoint()->z;
//		if(mwp->gotoNextPositionWayPoint(TOLERANCE_MODE_DISTANCE,
//				receiveDebugData.x,receiveDebugData.y,receiveDebugData.z)){
//			printf("it flies to next position way point.\n");
//			mwp->showPositionWayPoint();
//			cmd_flag=PACKAGE_DEFINE_POSITION_WAY_POINT;
//		}
//	}else if(way_point_flag==2){
//		if(freq++>10){
//			pwp=mwp->guideCircle(i++,1000,2,0,0,750,750,0);
//			memcpy(&sendPositionWayPointData,pwp,sizeof(PositionWayPoint));
//			cmd_flag=PACKAGE_DEFINE_POSITION_WAY_POINT;
//			freq=0;
//			delete pwp;
//		}
//	}
}

void setBallData(){
	sendNormalData.sp_x=vicon->ball_position(0);
	sendNormalData.sp_y=vicon->ball_position(1);
	sendNormalData.sp_z=vicon->ball_position(2);
	sendNormalData.debug_1=vicon->ball_timestamp();
	sendNormalData.debug_2=vicon->ball_speed(0);
	sendNormalData.debug_3=vicon->ball_speed(1);
	sendNormalData.debug_4=vicon->ball_speed(2);
}
void logBallData() {
	logUtils2->log_in((float) vicon->ball_position(0));
	logUtils2->log_pause();
	logUtils2->log_in((float) vicon->ball_position(1));
	logUtils2->log_pause();
	logUtils2->log_in((float) vicon->ball_position(2));
	logUtils2->log_pause();
	logUtils2->log_in((float) vicon->ball_timestamp());
	logUtils2->log_pause();
	logUtils2->log_in((float) vicon->ball_speed(0));
	logUtils2->log_pause();
	logUtils2->log_in((float) vicon->ball_speed(1));
	logUtils2->log_pause();
	logUtils2->log_in((float) vicon->ball_speed(2));
	logUtils2->log_pause();
	logUtils2->log_end();
}

void* receive_thread(void* ha=NULL){
//	int fd,
//			void* buffer,
//			void* data,
//			unsigned char check
	static ReceiveState receive_state=RECEIVE_STATE_NOT_COMPLETED;
	static int pre_timestamp=0;
	tcflush(fd,TCIOFLUSH);
	while(1){
		receive_state=my_receive(fd,(void*)buffer,
				(void*)(&allDataBuffer),(int*)(&pack_id),1);
		if(receive_state==RECEIVE_STATE_SUCCESS){
			if(record_flag==1){
				packSqlData();
				sql->dataIn(&sqlData);
			}else if(record_flag==2){
#ifdef LOG_NORMAL
				packSimpleData();
#endif
			}
			switch(pack_id){
			case PACKAGE_DEFINE_STATUS:
				break;
			case PACKAGE_DEFINE_VICON:
				break;
			case PACKAGE_DEFINE_SENSOR:
				break;
			case PACKAGE_DEFINE_FUSION:
				break;
			case PACKAGE_DEFINE_DEBUG:
				memcpy(&receiveDebugData
						,allDataBuffer,getPackageLength(pack_id));
				showDebugData(pre_timestamp);
				pre_timestamp=receiveDebugData.timestamp;
				break;
			case PACKAGE_DEFINE_PARAM:
				memcpy(&receiveParamDebug
						,allDataBuffer,getPackageLength(pack_id));
				showReceiveParamDebugOnce();
				cmd_flag=PACKAGE_DEFINE_NOMAL_DATA;
				break;
			case PACKAGE_DEFINE_CMD:
				break;
			case PACKAGE_DEFINE_POSITION_WAY_POINT:
				memcpy(&receivePositionWayPointData
						,allDataBuffer,getPackageLength(pack_id));
				showReceivePositionWayPointData();
				cmd_flag=PACKAGE_DEFINE_NOMAL_DATA;
				break;
			case PACKAGE_DEFINE_LAND:
				memcpy(&receiveLandSignal,allDataBuffer,
						getPackageLength(pack_id));
				printf("aircraft will land soon!\n");
				cmd_flag=PACKAGE_DEFINE_NOMAL_DATA;
				break;
			case PACKAGE_DEFINE_NOMAL_DATA:
				memcpy(&receiveNormalData,allDataBuffer
						,getPackageLength(pack_id));
				showReceiveNormalData();
				cmd_flag=PACKAGE_DEFINE_NOMAL_DATA;
				break;
			case PACKAGE_DEFINE_DEBUG_ARRAY:
					memcpy(&receiveDebugArray,allDataBuffer,
							getPackageLength(pack_id));
					showReceiveDebugArray();
#ifdef LOG_DEBUG_ARRAY
					packDebugArray();
#endif
					cmd_flag=PACKAGE_DEFINE_NOMAL_DATA;
					sendNormalData.sp_flag=1;
					break;
			default:
				break;
			}
		}
	}
}

void packSqlData(){
	memcpy(&sqlData,&receiveNormalData,sizeof(NormalData));
}
void packDebugArray(){
	int i=0;
	for(i=0;i<20;i++){
		logUtils->log_in(receiveDebugArray.time[i]);
		logUtils->log_pause();
		logUtils->log_in(receiveDebugArray.data[i]);
		logUtils->log_end();
	}
}
void packSimpleData(){
	logUtils->log_in(receiveNormalData.timestamp);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.x);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.y);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.z);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.vx);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.vy);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.vz);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.yaw);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.sp_x);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.sp_y);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.sp_z);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.sp_flag);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.debug_1);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.debug_2);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.debug_3);
	logUtils->log_pause();
	logUtils->log_in(receiveNormalData.debug_4);
	logUtils->log_end();
}
void setParamDebug(){
	//receiveDebug
}
void setSendDebugData(){
	sendDebugData.x=vicon->translation(0);
	sendDebugData.y=vicon->translation(1);
	sendDebugData.z=vicon->translation(2);
	sendDebugData.vx=vicon->speed(0);
	sendDebugData.vy=vicon->speed(1);
	sendDebugData.vz=vicon->speed(2);
	sendDebugData.pitch=vicon->rotation(0);
	sendDebugData.roll=vicon->rotation(1);
	sendDebugData.yaw=vicon->rotation(2);
	sendDebugData.timestamp=vicon->tp(0);
}
void setSendNormalData(){
	sendNormalData.timestamp=vicon->tp(0);
	sendNormalData.x=vicon->translation(0);
	sendNormalData.y=vicon->translation(1);
	sendNormalData.z=vicon->translation(2);
	sendNormalData.vx=vicon->speed(0);
	sendNormalData.vy=vicon->speed(1);
	sendNormalData.vz=vicon->speed(2);
	sendNormalData.yaw=vicon->rotation(2);
}
void showSendDebugData(){
	if(show_flag!=0){
		printf("sendDebugData.z:%f\n", sendDebugData.z);
		printf("sendDebugData.vz%f\n", sendDebugData.vz);
		printf("sendDebugData.timestamp:%d\n", sendDebugData.timestamp);
	}
}
void setSendParamDebug(){
	sendParamDebug.z=vicon->translation(2);
	sendParamDebug.vz=vicon->speed(2);
	if(received_k_info_flag==0&&receiveParamDebug.kp_p!=0){
		received_k_info_flag=1;
		sendParamDebug.kp_p=receiveParamDebug.kp_p;
		sendParamDebug.ki_p=receiveParamDebug.ki_p;
		sendParamDebug.kp_v=receiveParamDebug.kp_v;
		sendParamDebug.ki_v=receiveParamDebug.ki_v;
		sendParamDebug.thrust=receiveParamDebug.thrust;
	}
	//sendParamDebug.thrust;
	sendParamDebug.calc_thrust=receiveParamDebug.calc_thrust;
}
void showDebugData(int pre_timestamp){
	if(show_flag!=0){
		printf("received ok!\n");
		printf("battery:%d\n",receiveDebugData.battery);
		printf("cpu_load:%d\n",receiveDebugData.cpu_load);
		printf("vicon_count:%d\n",receiveDebugData.vicon_count);
		printf("timestamp:%d\td_timestamp:%d\n",
					receiveDebugData.timestamp
					,receiveDebugData.timestamp-pre_timestamp);

		printf("pitch:%f\n",receiveDebugData.pitch);
		printf("roll:%f\n",receiveDebugData.roll);
		printf("yaw:%f\n",receiveDebugData.yaw);

		printf("x:%f\tvx:%f\n",receiveDebugData.x,receiveDebugData.vx);
		printf("y:%f\tvy:%f\n",receiveDebugData.y,receiveDebugData.vy);
		printf("z:%f\tvz:%f\n",receiveDebugData.z,receiveDebugData.vz);
		printf("set_vx:%f\tset_vy:%f\n",receiveDebugData.set_position
				,receiveDebugData.set_velocity);
		printf("thrust:%f\n\n",receiveDebugData.calc_thrust);
	}
}
void showReceiveParamDebugOnce(){
	do{
		//printf("timestamp:%d\n",debugData.timestamp);
		printf("\nreceive ok:\n");
		printf("receive data:\n");
		printf("\tvicon_z:%f\n",receiveParamDebug.z);
		printf("\tvicon_vz:%f\n",receiveParamDebug.vz);
		printf("\tkp_p:%f\n",receiveParamDebug.kp_p);
		printf("\tki_p:%f\n",receiveParamDebug.ki_p);
		printf("\tkp_v:%f\n",receiveParamDebug.kp_v);
		printf("\tki_v:%f\n",receiveParamDebug.ki_v);
		printf("\tthrust:%d\n",receiveParamDebug.thrust);
		printf("\tcalc_thrustshowReceiveNormalData:%f\n",receiveParamDebug.calc_thrust);
		show_flag=0;
	}while(show_flag==1);
}
void showReceivePositionWayPointData(){
	do{
		//printf("timestamp:%d\n",debugData.timestamp);
		printf("\nreceive ok:\n");
		printf("receive way point data:\n");
		printf("\tx:%f\n",receivePositionWayPointData.x);
		printf("\ty:%f\n",receivePositionWayPointData.y);
		printf("\tz:%f\n",receivePositionWayPointData.z);
		show_flag=0;
	}while(show_flag==1);
}

void showSendParamDebug(){
	//printf("timestamp:%d\n",debugData.timestamp);
	if(show_flag!=0){
		printf("\n\nsend data:\n");
		printf("\tvicon_z:%f\n",sendParamDebug.z);
		printf("\tvicon_vz:%f\n",sendParamDebug.vz);
		printf("\tkp_p:%f\n",sendParamDebug.kp_p);
		printf("\tki_p:%f\n",sendParamDebug.ki_p);
		printf("\tkp_v:%f\n",sendParamDebug.kp_v);
		printf("\tki_v:%f\n",sendParamDebug.ki_v);
		printf("\tthrust:%d\n",sendParamDebug.thrust);
		printf("\tset_velocity:%f\n",sendParamDebug.set_velocity);
		printf("\tcalc_thrust:%f\n",sendParamDebug.calc_thrust);
	}
}
void showSendPositionWayPointDataOnce(){
	do{
		printf("\nsend ok:\n");
		printf("send way point data:\n");
		printf("\tx:%f\n",sendPositionWayPointData.x);
		printf("\ty:%f\n",sendPositionWayPointData.y);
		printf("\tz:%f\n",sendPositionWayPointData.z);
		show_flag=0;
	}while(show_flag==1);
}


void showSendParamDebugOnce(){
	do{
		printf("\n\nsend data:\n");
		printf("\tvicon_z:%f\n",sendParamDebug.z);
		printf("\tvicon_vz:%f\n",sendParamDebug.vz);
		printf("\tkp_p:%f\n",sendParamDebug.kp_p);
		printf("\tki_p:%f\n",sendParamDebug.ki_p);
		printf("\tkp_v:%f\n",sendParamDebug.kp_v);
		printf("\tki_v:%f\n",sendParamDebug.ki_v);
		printf("\tthrust:%d\n",sendParamDebug.thrust);
		printf("\tset_velocity:%f\n",sendParamDebug.set_velocity);
		printf("\tcalc_thrust:%f\n",sendParamDebug.calc_thrust);
		show_flag=0;
	}while(show_flag==1);
}
void showReceiveNormalData(){
	if(show_flag!=0){
		printf("\n\nreceived ok!\n");
		printf("sp_flag:%d\n",receiveNormalData.sp_flag);
		printf("debug_1:%f\n",receiveNormalData.debug_1);
		printf("debug_2:%f\n",receiveNormalData.debug_2);
		printf("debug_3:%f\n",receiveNormalData.debug_3);
		printf("debug_4:%f\n",receiveNormalData.debug_4);
		printf("timestamp:%d\n",receiveNormalData.timestamp);

		printf("yaw:%f\n",receiveNormalData.yaw);

		printf("x:%f\tvx:%f\n",receiveNormalData.x,receiveNormalData.vx);
		printf("y:%f\tvy:%f\n",receiveNormalData.y,receiveNormalData.vy);
		printf("z:%f\tvz:%f\n",receiveNormalData.z,receiveNormalData.vz);
		printf("sp_x:%f\tsp_y:%f\tsp_z:%f\n",receiveNormalData.sp_x
				,receiveNormalData.sp_y,receiveNormalData.sp_z);
	}
}
void showReceiveDebugArray(){
	int i;
//	if(show_flag!=0){
		printf("DebugData:\n");
		for(i=0;i<20;i++){
			printf("t:%d  roll:%f\n",receiveDebugArray.time[i],receiveDebugArray.data[i]);
		}
//	}
}
