/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include "filter.h"

static void
filterEnqueueResult(const MyVector* const pos,
					const MyQuaternion* const rot,
					const MyVector* const vel,
					const MyVector* const angVel,
					const SceUInt64* const timestamp,
					UInt maxQueue,
					UInt nData_in,
					FilterDataElement* queue,
					UInt* nData_out)
{
	ASSERT(maxQueue <= FILTER_BUFFER_COUNT);
	ASSERT(nData_in <= maxQueue);
	ASSERT(pos != NULL);
	ASSERT(rot != NULL);

	UInt sidx;
	(*nData_out) = nData_in;
	if (*nData_out >= maxQueue) {
		// shift data.
		UInt i;
		for (i = 0; i < maxQueue-1; ++i) {
			queue[i] = queue[i+1];
		}
		// define set index.
		sidx = maxQueue-1;
	} else {
		// increment nData_out.
		++(*nData_out);
		// define set index.
		sidx = (*nData_out)-1;
	}

	// Sets data.
	{
		queue[sidx].pos = *pos;
		queue[sidx].rot = *rot;
		if (vel)		queue[sidx].vel = *vel;
		if (angVel)		queue[sidx].angVel = *angVel;
		if (timestamp)	queue[sidx].timestamp = *timestamp;
	}
}

static void 
filterSimpleMovingAverage(UInt n, const FilterDataElement* queue, 
						  MyVector* spos,
						  MyQuaternion* srot,
						  MyVector* svel,
						  MyVector* sangVel,
						  SceUInt64* stimestamp)
{
	ASSERT(spos != NULL);
	ASSERT(srot != NULL);

	// Initialize 
	myVectorSet( spos,    0.f, 0.f, 0.f);
	myVectorSet4(&srot->v,  0.f, 0.f, 0.f, 0.f);
	if (svel)    myVectorSet(svel,    0.f, 0.f, 0.f);
	if (sangVel) myVectorSet(sangVel, 0.f, 0.f, 0.f);
	if (stimestamp) *stimestamp = 0U;

	{
		UInt i;
		for (i = 0; i < n; ++i) {
			myVectorAdd(spos, spos, &(queue[i].pos));
			{
				// sign alignment of quaternion.
				float dot4;
				{
					MyVectorEx vx0;
					MyVectorEx vx1;
					vx0.v = srot->v;
					vx1.v = queue[i].rot.v;
					dot4 = vx0.s.x * vx1.s.x + vx0.s.y * vx1.s.y + vx0.s.z * vx1.s.z + vx0.s.w * vx1.s.w;
				}
				if (dot4 >= 0.f) {
					myVectorAdd4(&srot->v, &srot->v, &(queue[i].rot.v));
				} else {
					myVectorSub4(&srot->v, &srot->v, &(queue[i].rot.v));
				}
			}

			if (svel)		myVectorAdd(svel, svel, &(queue[i].vel));
			if (sangVel)	myVectorAdd(sangVel, sangVel, &(queue[i].angVel));
			if (stimestamp)	*stimestamp += queue[i].timestamp;
		}
	}

	myVectorMulScalar(spos, spos, 1.f/(float)n);
	myQuaternionNormalize(srot, srot);

	if (svel)		myVectorMulScalar(svel, svel, 1.f/(float)n);
	if (sangVel)	myVectorMulScalar(sangVel, sangVel, 1.f/(float)n);
	if (stimestamp) *stimestamp /= n;
}

void filterSceneMappingRun(FilterState* state, SceSmartSceneMappingResult* sres, const SceSmartSceneMappingResult* const lastResult)
{
	// Enqueue result.
	{
		MyVector		mpos, mvel, mangvel;
		MyQuaternion	mrot;
		SceUInt64		mtimestamp;

		myVectorSet(&mpos, lastResult->pos.x, lastResult->pos.y, lastResult->pos.z);
		myQuaternionSet(&mrot, lastResult->rot.x, lastResult->rot.y, lastResult->rot.z, lastResult->rot.w);
		myVectorSet(&mvel, lastResult->vel.x, lastResult->vel.y, lastResult->vel.z);
		myVectorSet(&mangvel, lastResult->angVel.x, lastResult->angVel.y, lastResult->angVel.z);
		mtimestamp = lastResult->timestamp;

		filterEnqueueResult(&mpos, &mrot, &mvel, &mangvel, &mtimestamp, state->nmax, state->npos, state->queue, &(state->npos));
	}


	// Filter result.
	{
		MyVector		spos, svel, sangVel;
		MyQuaternion	srot;
		SceUInt64		stimestamp;

		filterSimpleMovingAverage(state->npos, state->queue, &spos, &srot, &svel, &sangVel, &stimestamp);

		sres->pos.x 	= myVectorGetX(&spos); 
		sres->pos.y 	= myVectorGetY(&spos); 
		sres->pos.z 	= myVectorGetZ(&spos);

		sres->rot.x 	= myVectorGetX(&srot.v);
		sres->rot.y 	= myVectorGetY(&srot.v);
		sres->rot.z 	= myVectorGetZ(&srot.v);
		sres->rot.w 	= myVectorGetW(&srot.v);

		sres->vel.x 	= myVectorGetX(&svel); 
		sres->vel.y 	= myVectorGetY(&svel); 
		sres->vel.z 	= myVectorGetZ(&svel);

		sres->angVel.x 	= myVectorGetX(&sangVel); 
		sres->angVel.y 	= myVectorGetY(&sangVel); 
		sres->angVel.z 	= myVectorGetZ(&sangVel);

		sres->timestamp = stimestamp;
	}
}

void filterTargetTrackingRun(FilterState* state, SceSmartTargetTrackingResult* sres, const SceSmartTargetTrackingResult* const lastResult)
{
	// Enqueue result.
	{
		MyVector		mpos;
		MyQuaternion	mrot;
		myVectorSet(&mpos, lastResult->pos.x, lastResult->pos.y, lastResult->pos.z);
		myQuaternionSet(&mrot, lastResult->rot.x, lastResult->rot.y, lastResult->rot.z, lastResult->rot.w);

		filterEnqueueResult(&mpos, &mrot, NULL, NULL, NULL, state->nmax, state->npos, state->queue, &(state->npos));
	}

	// Filter result.
	{
		MyVector		spos;
		MyQuaternion	srot;
		filterSimpleMovingAverage(state->npos, state->queue, &spos, &srot, NULL, NULL, NULL);

		sres->pos.x 	= myVectorGetX(&spos); 
		sres->pos.y 	= myVectorGetY(&spos); 
		sres->pos.z 	= myVectorGetZ(&spos);

		sres->rot.x 	= myVectorGetX(&srot.v);
		sres->rot.y 	= myVectorGetY(&srot.v);
		sres->rot.z 	= myVectorGetZ(&srot.v);
		sres->rot.w 	= myVectorGetW(&srot.v);
	}
}

void filterInitialize(FilterState* state, UInt nmax)
{
	state->nmax = nmax;
	filterReset(state);
}

void filterReset(FilterState* state)
{
	state->npos = 0U;
}
