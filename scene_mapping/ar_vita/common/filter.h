/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef FILTER_H
#define FILTER_H

#include "common.h"
#include "myvector.h"

#include <libsmart.h>
#include <smart/scene_mapping.h>
#include <smart/target_tracking.h>


#if defined(__cplusplus)
extern "C" {
#endif

#define FILTER_BUFFER_COUNT	(8)

typedef struct {
	MyVector        pos;
	MyQuaternion    rot;
	MyVector        vel;
	MyVector        angVel;
	SceUInt64       timestamp;
} FilterDataElement;

typedef struct {
	UInt nmax;
	UInt npos;
	FilterDataElement queue[FILTER_BUFFER_COUNT];
} FilterState;


void filterSceneMappingRun(FilterState* state, SceSmartSceneMappingResult* sres, const SceSmartSceneMappingResult* const latestResult);
void filterTargetTrackingRun(FilterState* state, SceSmartTargetTrackingResult* sres, const SceSmartTargetTrackingResult* const latestResult);
void filterInitialize(FilterState* state, UInt nmax);
void filterReset(FilterState* state);

#if defined(__cplusplus)
}
#endif

#endif /* FILTER_SCENE_MAPPING_H */
