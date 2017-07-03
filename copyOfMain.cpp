#include "ViconUtils.h"
#include "LogUtils.h"
#include "MyProtocol.h"

#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h>  // File control definitions
#include <termios.h> // POSIX terminal control definitionss
#include <pthread.h>
#include <time.h>

ViconUtils *vicon;
LogUtils *logUtils;

MyViconData viconData;
SensorData sensorData;
SystemState systemState;
FusionData fusionData;

DebugData debugData;
//MyViconData receivedViconData;
//SensorData receivedSensorData;
//SystemState receivedSystemState;
//FusionData receivedFusionData;


 long signed int (*read_callback)(int,void*,unsigned long);
 long signed int (*write_callback)(int,const void*,unsigned long int);

int fd;
PackageDefine pd=PACKAGE_DEFINE_VICON;
unsigned char pl=VICON_DATA_LENGTH;

unsigned char buffer[255];

void* send_thread(void*);
void* receive_thread(void*);
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
	printf("this is custom protocol of autopilot sdk 2.0\n");
	printf("the purpose of this program is to increase the sending frequency\n");
	printf("port is %s\n",port);
	printf("vicon name is %s\n",vicon_name);
	printf("if you wait for too long time, please check your network config.\n");
	vicon=new ViconUtils(vicon_name,vicon_name);
	logUtils=new LogUtils();
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

	}
	return 0;
}

void* send_thread(void* ha=NULL){
	while(1){
		//get vicon data from workstation
		vicon->get_translation_data();
		vicon->get_rotation_data();
		vicon->check();
		vicon->get_speed();
		//set the struct data
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
		//send it
		my_send(fd,pd,pl,&viconData,1);
		//sleep
		usleep(20000);
	}
}

void* receive_thread(void* ha=NULL){
//	int fd,
//			void* buffer,
//			void* data,
//			unsigned char check
	static int pre_timestamp=0;
	tcflush(fd,TCIOFLUSH);
	while(1){
		if(my_receive(fd,buffer,&debugData,1)){
//			logUtils->log_in(receivedViconData.timestamp-pre_timestamp);
//			logUtils->log_end();
//			printf("received ok!\n");
//			printf("vicon data:\n");
//			printf("timestamp:%d\td_timestamp:%d\n",
//					receivedViconData.timestamp
//					,receivedViconData.timestamp-pre_timestamp);
//			printf("x:%f\ty:%f\tz:%f\n"
//					,receivedViconData.x,
//					receivedViconData.y,
//					receivedViconData.z);
//			printf("roll:%f\tpitch:%f\tyaw:%f\n"
//					,receivedViconData.vx,
//					receivedViconData.vy,
//					receivedViconData.vz);
//			printf("vx:%f\tvy:%f\tvz:%f\n"
//					,receivedViconData.vx,
//					receivedViconData.vy,
//					receivedViconData.vz);
//			pre_timestamp=receivedViconData.timestamp;
		}
	}
}
