/*
 * MyWayPoint.h
 *
 *  Created on: 2017年7月8日
 *      Author: zbf
 */

#ifndef MYWAYPOINT_H_
#define MYWAYPOINT_H_

#include <string.h>
namespace zbf {

#ifndef NULL
#define NULL 0
#endif

typedef enum __way_point_type{
	WAY_POINT_TYPE_NORMAL,
	WAY_POINT_TYPE_POSITION,
}WAY_POINT_TYPE;

typedef struct __my_position_way_point{
	float x;
	float y;
	float z;
}PositionWayPoint;

typedef struct __my_velocity_way_point{
	float vx;
	float vy;
	float vz;
}VelocityWayPoint;

typedef struct __my_way_point{
	PositionWayPoint pwp;
	VelocityWayPoint vwp;
	float yaw;
}WayPoint;

typedef struct __my_position_way_point_node{
	PositionWayPoint pwp;
	__my_position_way_point_node* next;
}PositionWayPointNode;

typedef struct __my_way_point_node{
	WayPoint wp;
	__my_way_point_node* next;
}WayPointNode;

typedef struct __error_tolerance{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
	float yaw;
}Tolerance;

typedef PositionWayPoint PositionPoint;
typedef VelocityWayPoint VelocityPoint;
typedef WayPoint RealPoint;

class MyWayPoint {
public:
	MyWayPoint();
	MyWayPoint(WAY_POINT_TYPE wpt);
	void addNewWayPoint(WayPoint wp);
	void addNewPositionWayPoint(PositionWayPoint pwp);
	//void run();
	WayPointNode* sendCurrentWayPoint();
	bool gotoNextPositionWayPoint();
	bool gotoNextWayPoint();
	virtual ~MyWayPoint();
private:
	WAY_POINT_TYPE wpt;
	Tolerance tolerance;
	bool isRunning=false;

	WayPointNode* firstWayPoint;
	WayPointNode* currentWayPoint;
	PositionWayPointNode* firstPositionWayPoint;
	PositionWayPointNode* currentPositionWayPoint;
};

} /* namespace zbf */

#endif /* MYWAYPOINT_H_ */
