/*
 * MyWayPoint.cpp
 *
 *  Created on: 2017年7月8日
 *      Author: zbf
 */

#include "MyWayPoint.h"

namespace zbf {

MyWayPoint::MyWayPoint() :
	wpt(WAY_POINT_TYPE_NORMAL),
	firstPositionWayPoint(nullptr),
	currentPositionWayPoint(nullptr),
	firstWayPoint(nullptr),
	currentWayPoint(nullptr),
	isRunning(false){
	// TODO Auto-generated constructor stub

}
MyWayPoint::MyWayPoint(WAY_POINT_TYPE the_wpt):
	wpt(the_wpt),
	firstPositionWayPoint(nullptr),
	currentPositionWayPoint(nullptr),
	firstWayPoint(nullptr),
	currentWayPoint(nullptr),
	isRunning(false){
	// TODO Auto-generated constructor stub
}

MyWayPoint::~MyWayPoint() {
	// TODO Auto-generated destructor stub
	WayPointNode* wpn;
	PositionWayPointNode *pwpn;
	if(wpt==WAY_POINT_TYPE_NORMAL){
		while(firstWayPoint!=nullptr){
			wpn=firstWayPoint->next;
			firstWayPoint->next=wpn->next;
			delete wpn;
		}
	}
	if(wpt==WAY_POINT_TYPE_POSITION ){
		while(firstPositionWayPoint!=nullptr){
			pwpn=firstPositionWayPoint->next;
			firstPositionWayPoint->next=pwpn->next;
			delete pwpn;
		}
	}
}
void MyWayPoint::addNewPositionWayPoint(PositionWayPoint pwp){
	PositionWayPointNode* p=new PositionWayPointNode();
	memcpy(&(p->pwp),&pwp,sizeof(PositionWayPoint));
	if(firstPositionWayPoint==nullptr){
		currentPositionWayPoint=p;
		firstPositionWayPoint=p;
	}else{
		currentPositionWayPoint->next=p;
		currentPositionWayPoint=p;
	}
}

void MyWayPoint::addNewPositionWayPoint(float x,float y,float z){
	PositionWayPoint* pwp=new PositionWayPoint();
	pwp->x=x;
	pwp->y=y;
	pwp->z=z;
	addNewPositionWayPoint(*pwp);
	delete pwp;
}
void MyWayPoint::addNewWayPoint(float x,float y,float z,float vx,float vy,float vz,float yaw){
	WayPoint* wp=new WayPoint();
	wp->pwp.x=x;
	wp->pwp.y=y;
	wp->pwp.z=z;
	wp->vwp.vx=vx;
	wp->vwp.vy=vy;
	wp->vwp.vz=vz;
	wp->yaw=yaw;
	addNewWayPoint(*wp);
	delete wp;
}
void MyWayPoint::addNewWayPoint(WayPoint wp){
	WayPointNode* p=new WayPointNode();
	memcpy(&(p->wp),&wp,sizeof(WayPoint));
	if(firstWayPoint==nullptr){
		currentWayPoint=p;
		firstWayPoint=p;
	}else{
		currentWayPoint->next=p;
		currentWayPoint=p;
	}
}
void MyWayPoint::showWayPoint(){
	WayPointNode* wpn;
	wpn=firstWayPoint;
	printf("WayPoint list:\n[");
	while(wpn!=nullptr){
		printf("\n [\n");
		printf("  %f,%f,%f,\n",wpn->wp.pwp.x,wpn->wp.pwp.y,wpn->wp.pwp.z);
		printf("  %f,%f,%f,\n",wpn->wp.vwp.vx,wpn->wp.vwp.vy,wpn->wp.vwp.vz);
		printf("  %f\n",wpn->wp.yaw);
		printf(" ]\n");
		if(wpn->next!=nullptr)
			printf(",");
		wpn=wpn->next;
	}
	printf("]\n");
}
void MyWayPoint::showPositionWayPoint(){
	PositionWayPointNode* pwpn;
	pwpn=firstPositionWayPoint;
	printf("WayPoint list:\n[");
	while(pwpn!=nullptr){
		printf("\n ");
		printf(" [%f,%f,%f]",pwpn->pwp.x,pwpn->pwp.y,pwpn->pwp.z);
		pwpn=pwpn->next;
	}
	printf("\n]\n");
}

WayPoint* MyWayPoint::sendCurrentWayPoint(){
	return &(firstWayPoint->wp);
}

PositionWayPoint* MyWayPoint::sendCurrentPositionWayPoint(){
	return &(firstPositionWayPoint->pwp);
}

bool MyWayPoint::gotoNextPositionWayPoint(TOLERANCE_MODE mode,float x,float y,float z){
	static int try_times=0;
	PositionWayPointNode* p;
	if(mode==TOLERANCE_MODE_DISTANCE){
		if(distance(x,y,z,
				firstPositionWayPoint->pwp.x,
				firstPositionWayPoint->pwp.y,
				firstPositionWayPoint->pwp.z)<this->tolerance.distance)
		{
			if(try_times++>10){
				p=firstPositionWayPoint;
				firstPositionWayPoint=firstPositionWayPoint->next;
				delete p;
				printf("it flies to next position way point.\n");
				return true;
			}else{
				return false;
			}
		}
		try_times=0;
		return false;
	}
	try_times=0;
	return false;
}

float  MyWayPoint::distance(float x1,float y1,float z1,float x2,float y2,float z2){
	float ex=x2-x1;
	float ey=y2-y1;
	float ez=z2-z1;
	return sqrtf(ex*ex+ey*ey+ez*ez);
}

void MyWayPoint::setTolerance(float distance){
	this->tolerance.mode=TOLERANCE_MODE_DISTANCE;
	this->tolerance.distance=distance;
}
//parameter introduction:
// i: index of generate
//freq: send freq
//count: point count
//speed:
PositionWayPoint* MyWayPoint::guideCircle(int i,int count,float speed, float centerX,float centerY,float centerZ,
	float radius, float firstAngle){
	float dAngle=2*speed*MY_PI/count;
	PositionWayPoint* pwp=new PositionWayPoint();
	pwp->x=centerX+radius*cosf(dAngle*i+firstAngle);
	pwp->y=centerY+radius*sinf(dAngle*i+firstAngle);
	pwp->z=centerZ;
	return pwp;
}

void MyWayPoint::generateCircle(int count, float centerX,float centerY,float centerZ,
		float radius, float firstAngle){
	float dAngle=2*MY_PI/count;
	int i;
	for(i=0;i<count;i++){
		this->addNewPositionWayPoint(centerX+radius*cosf(dAngle*i+firstAngle)
		,centerY+radius*sinf(dAngle*i+firstAngle),centerZ);
	}
}

void MyWayPoint::generateSin(int count,int waveCount,float amplitude,float firstAngle,
		float len,float x,float y,float z){
	float dAngle=2*MY_PI/count*waveCount;
	float dLen=len/count;
	int i;
	for(i=0;i<count;i++){
		this->addNewPositionWayPoint(
				x+amplitude*sinf(i*dAngle+firstAngle),
				y+dLen*i,
				z);
	}
}

} /* namespace zbf */
