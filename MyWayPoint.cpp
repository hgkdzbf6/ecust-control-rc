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
}
void MyWayPoint::addNewPositionWayPoint(PositionWayPoint pwp){
	PositionWayPointNode* p=new PositionWayPointNode();
	memcpy(&(p->pwp),&pwp,sizeof(PositionWayPoint));
}

} /* namespace zbf */
