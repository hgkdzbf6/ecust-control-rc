#include "ViconUtils.h"
#include "LogUtils.h"
#include "MyProtocol.h"
#include "mysql/mysql.h"
#include "SQLUtils.h"
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h>  // File control definitions
#include <termios.h> // POSIX terminal control definitionss
#include <pthread.h>
#include <time.h>
#include <curses.h>
ViconUtils *vicon;
LogUtils *logUtils;
SQLUtils *sql;

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
PackageDefine pack_id;
unsigned char allDataBuffer[256]={0};
//MyViconData receivedViconData;
//SensorData receivedSensorData;
//SystemState receivedSystemState;
//FusionData receivedFusionData;


 long signed int (*read_callback)(int,void*,unsigned long);
 long signed int (*write_callback)(int,const void*,unsigned long int);

int fd;
clock_t clock_start,clock_end;
clock_t max_send_time,min_send_time;
#ifdef PARAM_DEBUG_MODE
PackageDefine pd=PACKAGE_DEFINE_PARAM;
unsigned char pl=PARAM_DEBUG_LENGTH;
#endif

#ifdef DEBUG_DATA_MODE
PackageDefine pd=PACKAGE_DEFINE_VICON;
unsigned char pl=VICON_DATA_LENGTH;
#endif


unsigned char buffer[255];
unsigned char record_flag=0;
unsigned char cmd_flag=PACKAGE_DEFINE_DEBUG;
unsigned char show_flag=1;
unsigned char received_k_info_flag=0;

void* send_thread(void*);
void* receive_thread(void*);
void menu();
void packSqlData();
void packSimpleData();
void nextMenu(char cmd);
void createHeader();
void showReceiveParamDebugOnce();
void showSendParamDebug();
void setViconData();
void setSendParamDebug();
void setSendDebugData();
void showSendDebugData();
void showDebugData(int pre_timestamp);

//manual:
//comming params:
//it consist of 3 params
//first is what USB port your device used.
//second is what vicon segment and subject are your vicon object used.
//third is the send cmd
//fourth is the switch of mysql data

int main(int argc, char* argv[]){
	pthread_t p_send;
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
			cmd_flag=PACKAGE_DEFINE_PARAM;
		}
	}else{
		cmd_flag=PACKAGE_DEFINE_DEBUG;
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
	printf("vicon_init_ok!\n");
	printf("log utils init ok\n");
	if(record_flag==1){
		sql=new SQLUtils();
		printf("sql utils init ok.\n");
	}else if(record_flag==2){
		logUtils=new LogUtils();
		createHeader();
		printf("log utils init ok.\n");
	}else{
		printf("you have switch off the mysql switch.\n");
	}
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
	pthread_create(&p_send, NULL, send_thread, NULL);
	pthread_create(&p_receieve, NULL, receive_thread, NULL);
	while(1){
		menu();
	}
	return 0;
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
	logUtils->log_in("pitch");
	logUtils->log_pause();
	logUtils->log_in("roll");
	logUtils->log_pause();
	logUtils->log_in("yaw");
	logUtils->log_pause();
	logUtils->log_in("vx");
	logUtils->log_pause();
	logUtils->log_in("vy");
	logUtils->log_pause();
	logUtils->log_in("vz");
	logUtils->log_pause();
	logUtils->log_in("battery");
	logUtils->log_pause();
	logUtils->log_in("cpu_load");
	logUtils->log_pause();
	logUtils->log_in("vicon_count");
	logUtils->log_pause();
	logUtils->log_in("set_position");
	logUtils->log_pause();
	logUtils->log_in("set_velocity");
	logUtils->log_pause();
	logUtils->log_in("calc_thrust");
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
		printf("you have switch debug param mode.\n");
		show_flag=(show_flag+1)%2;
	}
	else if((cmd<='9'&&cmd>'0')||(cmd=='a')){
		printf("param valid.\n");
		switch(cmd){
		case '0':
			break;
		case '1':
			sendParamDebug.kp_p+=0.01;
			printf("paramDebug.kp_p values %f has been sent\n"
					,sendParamDebug.kp_p);
			break;
		case '2':
			sendParamDebug.kp_p-=0.01;
			printf("paramDebug.kp_p values %f has been sent\n"
					,sendParamDebug.kp_p);
			break;
		case '3':
			sendParamDebug.ki_p+=0.01;
			printf("paramDebug.ki_p values %f has been sent\n"
					,sendParamDebug.ki_p);
			break;
		case '4':
			sendParamDebug.ki_p-=0.01;
			printf("paramDebug.ki_p values %f has been sent\n"
					,sendParamDebug.ki_p);
			break;
		case '5':
			sendParamDebug.kp_v+=0.01;
			printf("paramDebug.kp_v values %f has been sent\n"
					,sendParamDebug.kp_v);
			break;
		case '6':
			sendParamDebug.kp_v-=0.01;
			printf("paramDebug.kp_v values %f has been sent\n"
					,sendParamDebug.kp_v);
			break;
		case '7':
			sendParamDebug.ki_v+=0.01;
			printf("paramDebug.ki_v values %f has been sent\n"
					,sendParamDebug.ki_v);
			break;
		case '8':
			sendParamDebug.ki_v-=0.01;
			printf("paramDebug.ki_v values %f has been sent\n"
					,sendParamDebug.ki_v);
			break;
		case '9':
			sendParamDebug.thrust+=50;
			printf("paramDebug.thrust values %d has been sent\n"
					,sendParamDebug.thrust);
			break;
		case 'a':
			sendParamDebug.thrust-=50;
			printf("paramDebug.thrust values %d has been sent\n"
					,sendParamDebug.thrust);
			break;
		default:
			break;
		}
		cmd_flag=PACKAGE_DEFINE_PARAM;
		//showReceiveParamDebugOnce();
	}else{
		printf("param invalid, please retry.\n");
	}
}

void* send_thread(void* ha=NULL){
	while(1){
		clock_start=clock();
		//get vicon data from workstation
		vicon->get_translation_data();
		vicon->get_rotation_data();
		vicon->check();
		vicon->get_speed();
		vicon->update_data();
		sendCmdData.cmd=cmd_flag;
//		my_send(fd,PACKAGE_DEFINE_CMD,
//					getPackageLength(PACKAGE_DEFINE_CMD),
//					&sendCmdData,1);
		if(sendCmdData.cmd==PACKAGE_DEFINE_DEBUG){
			setSendDebugData();
			//showSendDebugData();
			my_send(fd,PACKAGE_DEFINE_DEBUG,
				getPackageLength(PACKAGE_DEFINE_DEBUG),
				&sendDebugData,1);
		}else if(sendCmdData.cmd==PACKAGE_DEFINE_PARAM){
			setSendParamDebug();
			showSendParamDebug();
			my_send(fd,PACKAGE_DEFINE_PARAM,
					getPackageLength(PACKAGE_DEFINE_PARAM),
					&sendParamDebug,1);
		}
		//sleep
		usleep(20000);
		clock_end=clock();
		//printf("send pass time:%ld\n",clock_end-clock_start);
	}
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
				packSimpleData();
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
				memcpy(&receiveDebugData,allDataBuffer,getPackageLength(pack_id));
				showDebugData(pre_timestamp);
				pre_timestamp=receiveDebugData.timestamp;
				break;
			case PACKAGE_DEFINE_PARAM:
				memcpy(&receiveParamDebug,allDataBuffer,getPackageLength(pack_id));
				showReceiveParamDebugOnce();
				cmd_flag=PACKAGE_DEFINE_DEBUG;
				break;
			case PACKAGE_DEFINE_CMD:
				break;
			default:
				break;
			}
		}
	}
}

void packSqlData(){
	sqlData.viconData.timestamp=receiveDebugData.timestamp;
	sqlData.viconData.x=receiveDebugData.x;
	sqlData.viconData.y=receiveDebugData.y;
	sqlData.viconData.z=receiveDebugData.z;
	sqlData.viconData.pitch=receiveDebugData.pitch;
	sqlData.viconData.roll=receiveDebugData.roll;
	sqlData.viconData.yaw=receiveDebugData.yaw;
	sqlData.viconData.vx=receiveDebugData.vx;
	sqlData.viconData.vy=receiveDebugData.vy;
	sqlData.viconData.vz=receiveDebugData.vz;

	sqlData.state.battery=receiveDebugData.battery;
	sqlData.state.cpu_load=receiveDebugData.cpu_load;
	sqlData.state.vicon_count=receiveDebugData.vicon_count;
	sqlData.set_position=receiveDebugData.set_position;
	sqlData.set_velocity=receiveDebugData.set_velocity;
	sqlData.calc_thrust=receiveDebugData.calc_thrust;
}
void packSimpleData(){
	logUtils->log_in(receiveDebugData.timestamp);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.x);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.y);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.z);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.pitch);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.roll);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.yaw);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.vx);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.vy);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.vz);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.battery);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.cpu_load);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.vicon_count);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.set_position);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.set_velocity);
	logUtils->log_pause();
	logUtils->log_in(receiveDebugData.calc_thrust);
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
void showSendDebugData(){
	if(show_flag!=0){
		printf("sendDebugData.z:%f\n", sendDebugData.z);
		printf("sendDebugData.vz%f\n", sendDebugData.vz);
		printf("sendDebugData.timestamp:%d\n", sendDebugData.timestamp);
	}
}
void setViconData(){
	viconData.x=vicon->translation(0);
	viconData.y=vicon->translation(1);
	viconData.z=vicon->translation(2);
	viconData.pitch=vicon->translation(0);
	viconData.roll=vicon->translation(1);
	viconData.yaw=vicon->translation(2);
	viconData.vx=vicon->speed(0);
	viconData.vy=vicon->speed(1);
	viconData.vz=vicon->speed(2);
	viconData.timestamp=vicon->tp(0);
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
		printf("\tcalc_thrust:%f\n",receiveParamDebug.calc_thrust);
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
