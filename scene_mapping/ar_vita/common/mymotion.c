/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2014 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#define ENABLE_DBGPRINTF

#include "mymotion.h"
#include <string.h>

static float const s_iirFilterCoef[] = {
	4.99554318039100331e-01,
	-9.98217272156401436e-01,
	0.00000000000000000e+00,
	1.78431832057465798e-03,
	1.78431832057465798e-03,
	0.00000000000000000e+00,

	2.49776960387297853e-01,
	-1.99821409262267169e+00,
	9.98217273574094066e-01,
	3.18379186913836787e-06,
	6.36758373827673574e-06,
	3.18379186913836787e-06,
};


MyMotionState* myMotionInit(UInt num){
	MyMotionState* state;
	size_t size = sizeof(MyMotionState) + sizeof(MyMotionMotion) * num + (bulkof(s_iirFilterCoef) / 6) * 2 * sizeof(MyVector);
	state = malloc(size);
	if(state == NULL){
		return NULL;
	}
	memset(state, 0, size);
	state->num = num;
	state->pos = 0;
	state->cnt = 0;
	state->log = (MyMotionMotion*)((UIntPtr)state + sizeof(MyMotionState));
	state->znx = (MyVector*)((UIntPtr)state + sizeof(MyMotionState) + sizeof(MyMotionMotion) * num);
	return state;
}


void myMotionRelease(MyMotionState* state){
	size_t size = sizeof(MyMotionState) + sizeof(MyMotionMotion) * state->num + (bulkof(s_iirFilterCoef) / 6) * 2 * sizeof(MyVector);
	memset(state, 0, size);
	free(state);
}

void myMotionUpdate(MyMotionState* state, SceMotionSensorState const* data, UInt num){
	SceUInt32 timestamp;
	SceUInt64 hostTimestamp;
	UInt pos;
	UInt cnt;
	SInt i;
	SInt j;
	ASSERT(num != 0);
	pos = state->pos;
	cnt = state->cnt;
	if(cnt == 0){
		i = (SInt)num - 1;
		myQuaternionSet(&state->rot, 0.0f, 0.0f, 0.0f, 1.0f);
		{ // initialize filter
			UInt k;
			MyVector vt;
			myVectorSet4(&vt, data[i].accelerometer.x, data[i].accelerometer.y, data[i].accelerometer.z, 0.0f);
			myVectorMulScalar(&vt, &vt, 9.8f);
			for(k = 0; k < bulkof(s_iirFilterCoef) / 6; k++){
				myVectorMulScalar(&vt, &vt, s_iirFilterCoef[k * 6 + 0]);
				myVectorMulScalar(&vt, &vt, 1.0f / (1.0f + s_iirFilterCoef[k * 6 + 1] + s_iirFilterCoef[k * 6 + 2]));
				state->znx[k * 2 + 0] = vt;
				state->znx[k * 2 + 1] = vt;
				myVectorMulScalar(&vt, &vt, s_iirFilterCoef[k * 6 + 3] + s_iirFilterCoef[k * 6 + 4] + s_iirFilterCoef[k * 6 + 5]);
			}
		}
		timestamp = data[i].timestamp;
	}else{
		timestamp = state->log[pos].sensor.timestamp;
		for(i = (SInt)num - 1; i >= 0; i--){
			if((SInt32)(data[i].timestamp - timestamp) > 0){
				break;
			}
		}
	}
	for(j = i; j >= 0; j--){
		pos = (pos + 1) % state->num;
		cnt = (cnt < state->num) ? cnt + 1 : cnt;
		state->log[pos].sensor = data[j];
		{
			float dt = (data[j].timestamp - timestamp) * 0.000001f;
			state->log[pos].dt = dt;
			{
				MyVector vt;
				MyQuaternion qt;
				myVectorSet(&vt, data[j].gyro.x, data[j].gyro.y, data[j].gyro.z);
				myVectorMulScalar(&vt, &vt, dt);
				myQuaternionRotationGyro(&qt, &vt);
				state->log[pos].rot = qt;
				myQuaternionMul(&qt, &state->rot, &qt);
				myQuaternionNormalize(&qt, &qt);
				state->rot = qt;
			}
			{
				MyVector vt;
				MyQuaternion qt = state->rot;
				myVectorSet(&vt, data[j].accelerometer.x, data[j].accelerometer.y, data[j].accelerometer.z);
				myVectorMulScalar(&vt, &vt, 9.8f);		// G -> m/s^2
				state->log[pos].acc = vt;
				myQuaternionRotateVectorR(&vt, &qt, &vt);
#if 1
				{ // filterning
					UInt k;
					for(k = 0; k < bulkof(s_iirFilterCoef) / 6; k++){
						MyVector z_1 = state->znx[k * 2 + 0];
						MyVector z_2 = state->znx[k * 2 + 1];
						MyVector zb1;
						MyVector zb2;
						MyVector za1;
						MyVector za2;
						myVectorMulScalar(&vt, &vt, s_iirFilterCoef[k * 6 + 0]);
						myVectorMulScalar(&zb1, &z_1, s_iirFilterCoef[k * 6 + 1]);
						myVectorMulScalar(&zb2, &z_2, s_iirFilterCoef[k * 6 + 2]);
						myVectorSub(&vt, &vt, &zb1);
						myVectorSub(&vt, &vt, &zb2);
						state->znx[k * 2 + 0] = vt;
						state->znx[k * 2 + 1] = z_1;
						myVectorMulScalar(&vt, &vt, s_iirFilterCoef[k * 6 + 3]);
						myVectorMulScalar(&za1, &z_1, s_iirFilterCoef[k * 6 + 4]);
						myVectorMulScalar(&za2, &z_2, s_iirFilterCoef[k * 6 + 5]);
						myVectorAdd(&vt, &vt, &za1);
						myVectorAdd(&vt, &vt, &za2);
					}
				}
#endif
				//printf("%f %f %f %f\n", myVectorGetX(&vt), myVectorGetY(&vt), myVectorGetZ(&vt), myVectorLength(&vt));
				myQuaternionRotateVector(&vt, &qt, &vt);
				state->log[pos].grv = vt;
			}
			timestamp = data[j].timestamp;
		}
	}
	state->pos = pos;
	state->cnt = cnt;
	hostTimestamp = state->log[pos].sensor.hostTimestamp;
	timestamp = state->log[pos].sensor.timestamp;
	for(; j < i; j++){
		if(state->log[pos].sensor.hostTimestamp != hostTimestamp){
			hostTimestamp = state->log[pos].sensor.hostTimestamp;
			timestamp = state->log[pos].sensor.timestamp;
		}else{
			UInt d = timestamp - state->log[pos].sensor.timestamp;
			state->log[pos].sensor.hostTimestamp = hostTimestamp - d;
		}
		pos = (pos + state->num - 1) % state->num;
	}
#if 0
	hostTimestamp = state->log[pos].sensor.hostTimestamp;
	timestamp = state->log[pos].sensor.timestamp;
	for(; j >= 0; j--){
		pos = (pos + 1) % state->num;
		//DEBUG_PRINTF("%d %08d<%08d> %08d<%08d>: %d:%08d\n", pos, state->log[pos].sensor.timestamp, state->log[pos].sensor.timestamp - timestamp, (UInt32)(state->log[pos].sensor.hostTimestamp), (UInt32)(state->log[pos].sensor.hostTimestamp - hostTimestamp), j, (UInt32)data[j].hostTimestamp);
		DEBUG_PRINTF("%d %f : %f %f %f (%f) : %f %f %f (%f)\n", pos, state->log[pos].dt, myVectorGetX(&state->log[pos].grv), myVectorGetY(&state->log[pos].grv), myVectorGetZ(&state->log[pos].grv), myVectorLength(&state->log[pos].grv), myVectorGetX(&state->log[pos].acc), myVectorGetY(&state->log[pos].acc), myVectorGetZ(&state->log[pos].acc), myVectorLength(&state->log[pos].acc));
		hostTimestamp = state->log[pos].sensor.hostTimestamp;
		timestamp = state->log[pos].sensor.timestamp;
	}
	DEBUG_PRINTF("\n");
#endif
}

void myMotionReset(MyMotionState* state){
	state->cnt = 0;
}


// t0 -> t1
SceUInt64 myMotionGetMotion(MyMotionState* state, MyMotionMotion* mmo, SceUInt64 t0, SceUInt64 t1){
	UInt pos = state->pos;
	UInt i;
	UInt n;
	MyVector acc;
	MyQuaternion rot;
	float dt;
	for(i = 0; i < state->cnt; i++){
		if((SInt64)(state->log[pos].sensor.hostTimestamp - t0) <= 0){
			break;
		}
		pos = (pos + state->num - 1) % state->num;
	}
	pos = (pos + 1) % state->num;
	n = 0;
	myVectorSet4(&acc, 0.0f, 0.0f, 0.0f, 0.0f);
	myQuaternionSet(&rot, 0.0f, 0.0f, 0.0f, 1.0f);
	dt = 0.0f;
	while((i > 0) && (SInt64)(state->log[pos].sensor.hostTimestamp - t1) <= 0){
		n++;
		i--;
		dt += state->log[pos].dt;
		myVectorAdd(&acc, &acc, &state->log[pos].acc);
		myQuaternionMul(&rot, &rot, &state->log[pos].rot);
		myQuaternionNormalize(&rot, &rot);
		pos = (pos + 1) % state->num;
	}
	if(n != 0){
		myVectorMulScalar(&acc, &acc, 1.0f / n);
	}
	pos = (pos + state->num - 1) % state->num;
	*mmo = state->log[pos];
	mmo->acc = acc;
	mmo->rot = rot;
	mmo->dt = dt;

#if 0
	DEBUG_PRINTF("%d-%d <%d %d %d>: %f : %f %f %f (%f) : %f %f %f (%f)\n", (pos + state->num - n + 1) % state->num, pos, (UInt32)t0, (UInt32)t1, (UInt32)state->log[pos].sensor.hostTimestamp, dt, myVectorGetX(&mmo->grv), myVectorGetY(&mmo->grv), myVectorGetZ(&mmo->grv), myVectorLength(&mmo->grv), myVectorGetX(&mmo->acc), myVectorGetY(&mmo->acc), myVectorGetZ(&mmo->acc), myVectorLength(&mmo->acc));
#endif
	return state->log[pos].sensor.hostTimestamp;
}

