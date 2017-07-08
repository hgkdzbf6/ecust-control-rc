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
	currentWayPoint(nullptr){
	// TODO Auto-generated constructor stub

}
MyWayPoint::MyWayPoint(WAY_POINT_TYPE the_wpt):
	wpt(the_wpt),
	firstPositionWayPoint(nullptr),
	currentPositionWayPoint(nullptr),
	firstWayPoint(nullptr),
	currentWayPoint(nullptr){
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
		firstPositionWayPoint=p;
	}
	currentPositionWayPoint->next=p;
	currentPositionWayPoint=p;
}
void MyWayPoint::addNewWayPoint(WayPoint wp){
	WayPointNode* p=new WayPointNode();
	memcpy(&(p->wp),&wp,sizeof(WayPoint));
	if(firstWayPoint==nullptr){
		firstWayPoint=p;
	}
	currentWayPoint->next=p;
	currentWayPoint=p;
}
void MyWayPoint::showWayPoint(){
	WayPointNode* wpn;
	wpn=firstWayPoint;
	printf("WayPoint list:\n[");
	while(wpn!=nullptr){
		printf("\n\t[\n");
		printf("\t\t%f,%f,%f,\n",wpn->wp.pwp.x,wpn->wp.pwp.y,wpn->wp.pwp.z);
		printf("\t\t%f,%f,%f,\n",wpn->wp.vwp.vx,wpn->wp.vwp.vy,wpn->wp.vwp.vz);
		printf("\t\t%f\n",wpn->wp.yaw);
		printf("\t]\n");
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
		printf("\n\t[\n");
		printf("\t\t%f,%f,%f,\n",pwpn->pwp.x,pwpn->pwp.y,pwpn->pwp.z);
		printf("\t]\n");
	}
	printf("]\n");
}

WayPoint* sendCurrentWayPoint(){
	return curren
}
} /* namespace zbf */
