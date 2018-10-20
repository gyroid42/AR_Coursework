/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2014 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef MYMOTION_H
#define MYMOTION_H

#include <motion.h>

#include "common.h"
#include "myvector.h"


#if defined(__cplusplus)
extern "C" {
#endif

typedef struct MyMotionMotion{
	MyQuaternion rot;
	MyVector acc;
	MyVector grv;
	SceMotionSensorState sensor;
	float dt;
}MyMotionMotion;

typedef struct MyMotionState{
	UInt num;
	UInt pos;
	UInt cnt;
	MyMotionMotion* log;
	MyVector* znx;

	MyQuaternion rot;
}MyMotionState;

MyMotionState* myMotionInit(UInt num);
void myMotionRelease(MyMotionState* state);

void myMotionUpdate(MyMotionState* state, SceMotionSensorState const* data, UInt num);

void myMotionReset(MyMotionState* state);

// t0 -> t1
SceUInt64 myMotionGetMotion(MyMotionState* state, MyMotionMotion* motion, SceUInt64 t0, SceUInt64 t1);


#if defined(__cplusplus)
}
#endif

#endif /* MYMOTION_H */
