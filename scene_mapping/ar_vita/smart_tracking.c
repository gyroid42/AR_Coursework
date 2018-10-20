/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <libsysmodule.h>

//#define ENABLE_DBGPRINTF
#include "common/common.h"
#include "common/sample_main.h"

#include <libsmart.h>
#include <smart/scene_mapping.h>
#include <smart/target_tracking.h>
#pragma comment(lib, "libSceSmart_stub.a")


#define ENABLE_WAAR

#define ENABLE_FILTER

//#define ENABLE_USERMALLOC


#undef DEBUG_PRINTF
#define DEBUG_PRINTF	sampleSetStatusLine

#if defined(ENABLE_USERMALLOC)
#	include <mspace.h>
#endif

#if defined(ENABLE_FILTER)
#	include "common/filter.h"
#endif

#define SMART_HEAP_SIZE						(64 * 1024 * 1024)

#define SMART_CORE_THREAD_PRIORITY			(THREAD_PRIORITY_NORMAL)
//#define SMART_CORE_THREAD_STACK_SIZE		(THREAD_DEFAULT_STACK_SIZE)
#define SMART_CORE_THREAD_STACK_SIZE		(SCE_KERNEL_STACK_SIZE_MAX)

#define SMART_DISPATCH_THREAD_PRIORITY		(THREAD_PRIORITY_NORMAL)
//#define SMART_DISPATCH_THREAD_STACK_SIZE	(THREAD_DEFAULT_STACK_SIZE)
#define SMART_DISPATCH_THREAD_STACK_SIZE	(SCE_KERNEL_STACK_SIZE_MAX)

//#define SMART_WORKER_THREAD_MAX				(0)
#define SMART_WORKER_THREAD_MAX				(2)
//#define SMART_WORKER_THREAD_PRIORITY		(THREAD_PRIORITY_NORMAL)
#define SMART_WORKER_THREAD_PRIORITY		(THREAD_PRIORITY_BELOW_NORMAL)
//#define SMART_WORKER_THREAD_STACK_SIZE		(THREAD_DEFAULT_STACK_SIZE)
#define SMART_WORKER_THREAD_STACK_SIZE		(SCE_KERNEL_STACK_SIZE_MAX)

#define SMART_TRACKING_THREAD_PRIORITY		(THREAD_PRIORITY_NORMAL)
//#define SMART_TRACKING_THREAD_STACK_SIZE	(THREAD_DEFAULT_STACK_SIZE)
#define SMART_TRACKING_THREAD_STACK_SIZE	(SCE_KERNEL_STACK_SIZE_MAX)

#define SMART_TARGET_MAX		(2 + 6)
#define SMART_TARGET_DICT_01	"app0:samples/data/engines/api_libsmart/smartar01.v9.dic"
#define SMART_TARGET_DICT_02	"app0:samples/data/engines/api_libsmart/smartar02.v9.dic"

#define SMART_MAP				"savedata0:map001.dat"

#define SMART_RESULT_MAX		(3 + 1)
#define SMART_RESULT_LOW		(1)
#define SMART_LANDMARK_MAX		(SCE_SMART_SCENE_MAPPING_MAX_NUM_LANDMARKS)
#define SMART_IP_MAX			(SCE_SMART_SCENE_MAPPING_MAX_NUM_INITIALIZATION_POINTS)

#if defined(ENABLE_WAAR)
#	define SMART_DICT_TRACKING_MAX	(1)
#	define SMART_WAAR_TRACKING_MAX	(5)
#	define SMART_TRACKING_MAX		(SMART_DICT_TRACKING_MAX + SMART_WAAR_TRACKING_MAX)
#else
#	define SMART_DICT_TRACKING_MAX	(1)
#	define SMART_TRACKING_MAX		SMART_DICT_TRACKING_MAX
#endif

#if defined(ENABLE_FILTER)
// If FILTER_COEFF is larger (ex.3or4), jitter by libsmart is decreased and delay-frame is increased.
#	define FILTER_COEFF			(2)						// FILTER_COEFF >= 1
#	define FILTER_RESULT_MAX	((FILTER_COEFF)*2+1)	// FILTER_BUFFER_COUNT >=FILTER_RESULT_MAX.
#	define DELAY_FRAME			((FILTER_COEFF)+2)
#else
//#	define DELAY_FRAME			(0)
#	define DELAY_FRAME			(2)
//#	define DELAY_FRAME			(3)
#endif


char const* smartSampleName(void){
	return "smart_tracking";
}

enum{
	SCENE_MAPPING_OP_NOP,
	SCENE_MAPPING_OP_START,
	SCENE_MAPPING_OP_STOP,
	SCENE_MAPPING_OP_RESET,
	SCENE_MAPPING_OP_SAVE_MAP,
};

enum{
	TARGET_TRACKING_OP_NOP,
	TARGET_TRACKING_OP_START,
	TARGET_TRACKING_OP_STOP,
	TARGET_TRACKING_OP_RESET,
};

typedef struct{
	SInt lost;
	SInt detect;

	MyVector pos;
	MyQuaternion rot;

	struct{
		SceSmartSceneMappingResult res;
#if defined(ENABLE_FILTER)
		struct{
			Bool enable;
			FilterState state;
		} filter;
#endif
#if DELAY_FRAME != 0
		CameraBuffer* image_queue[DELAY_FRAME];
#endif
	}update;

	struct{
		UInt lost;
		SInt target;
		MyVector pos;
		MyQuaternion rot;
		SceUInt32 timestamp;
		SceSmartTargetTrackingResult res;
#if defined(ENABLE_FILTER)
		struct{
			Bool enable;
			SceInt32 targetId;
			FilterState state;
		} filter;
#endif
	}tracking[SMART_TRACKING_MAX];

	struct{
		UInt num;
		SceSmartSceneMappingLandmarkInfo info[SMART_LANDMARK_MAX];
	}landmarks;

	struct{
		UInt num;
		SceSmartSceneMappingInitializationPointInfo info[SMART_IP_MAX];
	}initial;

	struct{
		SceInt32 id;
		SceSmartTargetInfo info;
	}target[SMART_TARGET_MAX];

	struct{
		volatile Bool exit;
		MyThread thread;
		MyEvent evt;
		MySemaphore smp;
	}coreThread;

	struct{
		volatile Bool exit;
		MyThread thread;
		MyEvent evt;

		SceSmartVector maskWork[3 * SMART_TRACKING_MAX * 2];

		struct{
			SceSmartSceneMappingResult sm;
		}volatile res[SMART_RESULT_MAX];
		volatile UInt rpos;
		volatile UInt wpos;

		MyAtomic32 op;
		volatile SceSmartSceneMappingInitMode mode;
	}dispatchThread;

#if SMART_WORKER_THREAD_MAX != 0
	struct{
		volatile Bool exit;
		MyThread thread[SMART_WORKER_THREAD_MAX];
		MyEventFlag flg;
		MySemaphore smp;
	}workerThread;
#endif

	struct{
		volatile Bool exit;
		volatile Bool run;
		MyThread thread;
		MyEvent evt;
		MyMutex mtx;
		SceSmartTargetTrackingResult res[SMART_TRACKING_MAX];
		SceUInt32 timestamp;

		MyAtomic32 op;
	}trackingThread;

#if defined(ENABLE_USERMALLOC)
	struct{
		mspace msp;
		SceUID uid;
		void* ptr;
	}heap;
#endif

	struct{
		struct{
			volatile float fps;
			volatile SceUInt32 pt;
			SceUInt32 t1;
		}mapping;

#if SMART_WORKER_THREAD_MAX != 0
		struct{
			SceUInt32 volatile pt;
		}worker[SMART_WORKER_THREAD_MAX];
#endif
		struct{
			volatile float fps;
			volatile SceUInt32 pt;
			SceUInt32 t1;
		}tracking;
	}debug;

	int initStep;
}SmartTrackingState;

static SmartTrackingState s_smartTrackingState;


static bool g_MappingReset = false;
static bool g_MappingChangeMode = false;
static bool g_MappingSaveMap = false;



static void _smartUpdateCtrl(bool reset, bool change_mode, bool save_map);
static void _sampleUpdateView(void);
static void _sampleDrawLandmarks(void);

static size_t _smartStreamRead(void* dsc, void* buf, size_t size){
	return fread(buf, 1, size, (FILE*)dsc);
}
#if 0
static size_t _smartStreamWrite(void* dsc, const void* buf, size_t size){
	return fwrite(buf, 1, size, (FILE*)dsc);
}
#endif
static size_t _smartSaveDataStreamRead(void* dsc, void* buf, size_t size){
	return sampleSaveDataStreamRead((SaveDataStream*)dsc, buf, size);
}
static size_t _smartSaveDataStreamWrite(void* dsc, const void* buf, size_t size){
	return sampleSaveDataStreamWrite((SaveDataStream*)dsc, buf, size);
}


static DECLARE_THREAD_PROC(_smartCoreThread){
	SmartTrackingState* const s = &s_smartTrackingState;
	UNUSED_THREAD_ARG();

	DEBUG_PRINTF("%s: start\n", __FUNCTION__);

	for(;;){
		myEventWait(&s->coreThread.evt, INFINITE);
		if(s->coreThread.exit != FALSE){
			break;
		}

		for(;;){
			int res;
			SceUInt32 t0;
			SceUInt32 t1;
			SceUInt32 t2;
			if(mySemaphoreWait(&s->coreThread.smp, 0) == FALSE){
				break;
			}
			{
				t2 = sceKernelGetProcessTimeLow();
			}
			res = sceSmartSceneMappingRunCore();
			if(res != SCE_SMART_ERROR_NOT_REQUIRED){
				t0 = sceKernelGetProcessTimeLow();
				t1 = s->debug.mapping.t1;
				s->debug.mapping.t1 = t0;
				s->debug.mapping.fps = 1000000.0f / (t0 - t1);
				s->debug.mapping.pt = t0 - t2;
			}
			mySemaphoreSignal(&s->coreThread.smp, 1);
			ASSERT((res == SCE_OK) || (res == SCE_SMART_ERROR_NOT_REQUIRED) || (res == 1));
			if(res == 1){
				continue;
			}
			break;
		}
	}

	DEBUG_PRINTF("%s: exit\n", __FUNCTION__);

	THREAD_RETURN(0);
}

static void _smartSetupMask(SceSmartSceneMappingMask* mask, SceUInt32 timestamp){
	SmartTrackingState* const s = &s_smartTrackingState;
	MyMatrix pm;
	UInt i;
	UInt n = 0;

	static struct{
		SceUInt32 timestamp;
		SceSmartTargetTrackingResult res;
	}tracking[SMART_TRACKING_MAX];

	UNUSED(timestamp);

	ASSERT(mask != NULL);

	myMatrixPerspective(&pm, PROJECTION_FOV, (float)PROJECTION_WIDTH / (float)PROJECTION_HEIGHT, PROJECTION_ZNEAR, PROJECTION_ZFAR);

	// Get target informations from trackingThread.
	{
		myMutexLock(&s->trackingThread.mtx);
		ASSERT(bulkof(s->trackingThread.res) >= bulkof(tracking));
		for(i = 0; i < bulkof(tracking); i++){
			tracking[i].res = s->trackingThread.res[i];
			tracking[i].timestamp = s->trackingThread.timestamp;
		}
		myMutexUnlock(&s->trackingThread.mtx);
	}

	// Make mask.
	for(i = 0; i < bulkof(tracking); i++){
		UInt target;
		ASSERT(n * 3 * 2 < bulkof(s->dispatchThread.maskWork));
		if(tracking[i].res.state != SCE_SMART_TARGET_TRACKING_STATE_TARGET_TRACKING){
			continue;
		}
		{
			SceInt32 detected = tracking[i].res.detectedTargetID;
			UInt j;
			for(j = 0; j < bulkof(s->target); j++){
				if(s->target[j].id == detected){
					break;
				}
			}
			if(j == bulkof(s->target)){
				continue;
			}
			target = j;
		}
		{
			MyMatrix mt;
			{
				MyVector v0;
				MyQuaternion q0;
				MyVector v1;
				MyMatrix m1;
				MyMatrix m2;
				MyMatrix m3;
				MyMatrix m4;
				myQuaternionSet(&q0, tracking[i].res.rot.x, tracking[i].res.rot.y, tracking[i].res.rot.z, tracking[i].res.rot.w);
				myVectorSet(&v0, tracking[i].res.pos.x, tracking[i].res.pos.y, tracking[i].res.pos.z);
				myQuaternionConjugate(&q0, &q0);
				myVectorMulScalar(&v0, &v0, -1.0f);
				myMatrixRotationQuaternion(&m1, &q0);
				myMatrixTranslationVector(&m2, &v0);
				myMatrixScaling(&m3, 0.5f, -0.5f, 1.0f);
				myVectorSet(&v1, 0.5f, 0.5f, 0.0f);
				myMatrixTranslationVector(&m4, &v1);
				myMatrixMul(&mt, &m1, &m2);
				myMatrixMul(&mt, &pm, &mt);
				myMatrixMul(&mt, &m3, &mt);
				myMatrixMul(&mt, &m4, &mt);
			}
			// Build triangle list.
			{
				UInt j;
				float hw = s->target[target].info.physicalWidth * 0.5f;
				float hh = s->target[target].info.physicalHeight * 0.5f;
				MyVectorEx v[4];
				SceSmartVector* tri = &s->dispatchThread.maskWork[n * 3 * 2];

				myVectorSet(&v[0].v, -hw, -hh, 0.0f);
				myVectorSet(&v[1].v, +hw, -hh, 0.0f);
				myVectorSet(&v[2].v, +hw, +hh, 0.0f);
				myVectorSet(&v[3].v, -hw, +hh, 0.0f);
				for(j = 0; j < 4; j++){
					MyVectorEx vt;
					myMatrixMulVector(&vt.v, &mt, &v[j].v);
					myVectorMulScalar(&v[j].v, &vt.v, (vt.s.w < FLT_EPSILON) ? 1.0f : (1.0f / vt.s.w));
				}
				tri[0].x = v[0].s.x;
				tri[0].y = v[0].s.y;
				tri[0].z = 0.0f;
				tri[1].x = v[1].s.x;
				tri[1].y = v[1].s.y;
				tri[1].z = 0.0f;
				tri[2].x = v[2].s.x;
				tri[2].y = v[2].s.y;
				tri[2].z = 0.0f;

				tri[3].x = v[2].s.x;
				tri[3].y = v[2].s.y;
				tri[3].z = 0.0f;
				tri[4].x = v[3].s.x;
				tri[4].y = v[3].s.y;
				tri[4].z = 0.0f;
				tri[5].x = v[0].s.x;
				tri[5].y = v[0].s.y;
				tri[5].z = 0.0f;

				n++;
			}
		}
	}

	mask->num = n * 2;
	mask->triangleList = s->dispatchThread.maskWork;
}

static DECLARE_THREAD_PROC(_smartDispatchThread){
	SmartTrackingState* const s = &s_smartTrackingState;
	SceUInt64 timestampi = 0;
	Bool run = FALSE;
	UNUSED_THREAD_ARG();

	DEBUG_PRINTF("%s: start\n", __FUNCTION__);

	for(;;){
		Bool raise = FALSE;
		SceSmartSceneMappingResult res;

		myEventWait(&s->dispatchThread.evt, INFINITE);
		if(s->dispatchThread.exit != FALSE){
			break;
		}

		if(run != FALSE){
			CameraBuffer* image;
			SceCameraRead* read;
			SceMotionSensorState* motion;

			image = sampleGetCameraImage();
			read = sampleGetCameraBufferRead(image);
			motion = sampleGetCameraMotion(image);

			if(timestampi != read->qwTimestamp){
				SceSmartSceneMappingInput args;
				SceSmartSceneMappingMask mask;
				args.motion.count = SCE_MOTION_MAX_NUM_STATES;
				args.motion.states = motion;
				args.image.data = read->pvIBase;
				args.image.timestamp = read->qwTimestamp;

				_smartSetupMask(&mask, (SceUInt32)read->qwTimestamp);

				SCECHK(sceSmartSceneMappingDispatchAndQueryWithMask(&res, &args, &mask));
				timestampi = read->qwTimestamp;
				raise = TRUE;
			}
			sampleUnlockCameraBuffer(image);
		}else{
			memset(&res, 0, sizeof(res));
			res.state = SCE_SMART_SCENE_MAPPING_STATE_IDLE;
		}

		if(s->dispatchThread.op != SCENE_MAPPING_OP_NOP){
			MyAtomic32 op = myAtomic32Swap(&s->dispatchThread.op, SCENE_MAPPING_OP_NOP);
			Bool loadMap = FALSE;

			mySemaphoreWait(&s->coreThread.smp, INFINITE);

			if((op == SCENE_MAPPING_OP_RESET) && (run == FALSE)){
				op = SCENE_MAPPING_OP_START;
			}

			switch(op){
			case SCENE_MAPPING_OP_NOP:
				break;
			case SCENE_MAPPING_OP_START:
				if(run != FALSE){
					SCECHK(sceSmartSceneMappingStop());
				}
				{
					SCECHK(sceSmartSceneMappingStart(s->dispatchThread.mode));
					res.state = SCE_SMART_SCENE_MAPPING_STATE_IDLE;
					if(s->dispatchThread.mode == SCE_SMART_SCENE_MAPPING_INIT_NULL){
						loadMap = TRUE;
					}
					run = TRUE;
					raise = FALSE;
				}
				break;
			case SCENE_MAPPING_OP_STOP:
				if(run != FALSE){
					SCECHK(sceSmartSceneMappingStop());
					res.state = SCE_SMART_SCENE_MAPPING_STATE_IDLE;
					run = FALSE;
					raise = FALSE;
				}
				break;
			case SCENE_MAPPING_OP_SAVE_MAP:
				if(run != FALSE){
					SaveDataStream* fp = sampleSaveDataStreamOpen(SMART_MAP, FALSE);
					if(fp == NULL){
						DEBUG_PRINTF("sampleSaveDataStreamOpen(\"%s\"): failed\n", SMART_MAP);
					} else {
						SceSmartStreamOut stream;
						stream.dsc = fp;
						stream.write = _smartSaveDataStreamWrite;
						SCECHK(sceSmartSceneMappingSaveMap(&stream));
						sampleSaveDataStreamClose(fp);
					}
#if 1
					SCECHK(sceSmartSceneMappingFixMap(TRUE));
#endif
				}
				break;
			case SCENE_MAPPING_OP_RESET:
				if(run != FALSE){
					SCECHK(sceSmartSceneMappingReset());
					res.state = SCE_SMART_SCENE_MAPPING_STATE_IDLE;
					raise = FALSE;
				}
				break;
			default:
				break;
			}

			if(loadMap != 0){
				SceSmartStreamIn mapStream;
				SaveDataStream* fp;
				ASSERT(s->dispatchThread.mode == SCE_SMART_SCENE_MAPPING_INIT_NULL);
				fp = sampleSaveDataStreamOpen(SMART_MAP, TRUE);
				if(fp == NULL){
					DEBUG_PRINTF("sampleSaveDataStreamOpen(\"%s\"): failed\n", SMART_MAP);
				}else{
					mapStream.dsc = fp;
					mapStream.read = _smartSaveDataStreamRead;
					SCECHK(sceSmartSceneMappingLoadMap(&mapStream));
					SCECHK(sceSmartSceneMappingFixMap(TRUE));
					SCECHK(sceSmartSceneMappingForceLocalize());
					sampleSaveDataStreamClose(fp);
				}
			}

			mySemaphoreSignal(&s->coreThread.smp, 1);
		}

		if(raise != FALSE){
			myEventRaise(&s->coreThread.evt);

			{
				SceUInt32 wpos = s->dispatchThread.wpos;
				SceUInt32 npos = (wpos + 1) % SMART_RESULT_MAX;
				ASSERT(npos != s->dispatchThread.rpos);
				if(npos != s->dispatchThread.rpos){
					s->dispatchThread.res[wpos].sm = res;
					__builtin_dmb();
					s->dispatchThread.wpos = npos;
				}
			}
		}
	}

	if(run != FALSE){
		SCECHK(sceSmartSceneMappingStop());
	}

	DEBUG_PRINTF("%s: exit\n", __FUNCTION__);

	THREAD_RETURN(0);
}

#if SMART_WORKER_THREAD_MAX != 0
static DECLARE_THREAD_PROC(_smartWorkerThread){
	SmartTrackingState* const s = &s_smartTrackingState;

	int threadIdx = (int)THREAD_GET_ARG();

	DEBUG_PRINTF("%s: start: %d\n", __FUNCTION__, threadIdx);

	for(;;){
		SceUInt32 t0;
		SceUInt32 t1;
		SceInt32 res;
		myEventFlagWait(&s->workerThread.flg, 1 << threadIdx, MY_EVENT_FLAG_WAITMODE_AND, INFINITE);
		if(s->workerThread.exit != FALSE){
			break;
		}

		if(mySemaphoreWait(&s->workerThread.smp, 0) == FALSE){
			continue;
		}

		{
			t1 = sceKernelGetProcessTimeLow();
		}

		do{
			res = sceSmartTargetTrackingRunWorker();
			ASSERT((res >= SCE_OK) || (res == SCE_SMART_ERROR_NOT_REQUIRED) || (res == SCE_SMART_ERROR_NOT_STARTED));
		}while(res == 1);

		{
			t0 = sceKernelGetProcessTimeLow();
			s->debug.worker[threadIdx].pt += t0 - t1;
		}

		mySemaphoreSignal(&s->workerThread.smp, 1);
	}

	DEBUG_PRINTF("%s: exit\n", __FUNCTION__);


	THREAD_RETURN(0);
}
#endif // SMART_WORKER_THREAD_MAX != 0

static DECLARE_THREAD_PROC(_smartTrackingThread){
	SmartTrackingState* const s = &s_smartTrackingState;
	SceUInt64 timestamp = 0;
	Bool run = FALSE;

	UNUSED_THREAD_ARG();

	DEBUG_PRINTF("%s: start\n", __FUNCTION__);

	for(;;){
		myEventWait(&s->trackingThread.evt, INFINITE);
		if(s->trackingThread.exit != FALSE){
			break;
		}

		{
			MyAtomic32 op = myAtomic32Swap(&s->trackingThread.op, TARGET_TRACKING_OP_NOP);

			if(op != TARGET_TRACKING_OP_NOP){
#if SMART_WORKER_THREAD_MAX != 0
				int i;
				for(i = 0; i < SMART_WORKER_THREAD_MAX; i++){
					mySemaphoreWait(&s->workerThread.smp, INFINITE);
				}
#endif
				if((run == FALSE) && (op == TARGET_TRACKING_OP_RESET)){
					op = TARGET_TRACKING_OP_START;
				}

				switch(op){
				case TARGET_TRACKING_OP_NOP:
					break;
				case TARGET_TRACKING_OP_START:
					if(run == FALSE){
						SCECHK(sceSmartTargetTrackingStart(SMART_DICT_TRACKING_MAX));
						run = TRUE;
					}
					break;
				case TARGET_TRACKING_OP_STOP:
					if(run != FALSE){
						SCECHK(sceSmartTargetTrackingStop());
						run = FALSE;
					}
					break;
				case TARGET_TRACKING_OP_RESET:
					if(run != FALSE){
						SCECHK(sceSmartTargetTrackingReset());
					}
					break;
				default:
					break;
				}
				s->trackingThread.run = run;
#if SMART_WORKER_THREAD_MAX != 0
				mySemaphoreSignal(&s->workerThread.smp, SMART_WORKER_THREAD_MAX);
#endif
			}
		}

		if(run != FALSE){
			SceSmartTargetTrackingResult res[SMART_TRACKING_MAX];
			SceInt32 num = 0;
			CameraBuffer* image;
			SceCameraRead* read;

			image = sampleGetCameraImage();
			read = sampleGetCameraBufferRead(image);

			if(read->qwTimestamp != timestamp){
				SceUInt32 t0;
				SceUInt32 t1;
				SceUInt32 t2;
				SceInt32 i;

				timestamp = read->qwTimestamp;
				t2 = sceKernelGetProcessTimeLow();

#if SMART_WORKER_THREAD_MAX == 0
				num = sceSmartTargetTrackingRun(read->pvIBase);
				ASSERT(0 <= num);
#else
				{
					SceSmartTargetTrackingInput args;
#if defined(ENABLE_WAAR)
					args.motion.states = sampleGetCameraMotion(image);
					args.motion.count = SCE_MOTION_MAX_NUM_STATES;
#else
					// there is no need motion data except WAAR
					args.motion.states = NULL;
					args.motion.count = 0;
#endif
					args.image.data = read->pvIBase;
					args.image.timestamp = read->qwTimestamp;
					num = sceSmartTargetTrackingDispatchAndQuery(&args);
					ASSERT(num >= 0);

					myEventFlagSet(&s->workerThread.flg, 0xffffffffU);
				}
#endif // SMART_WORKER_THREAD_MAX == 0

				num = sceSmartTargetTrackingGetResults(res, bulkof(res));
				ASSERT(0 <= num);
				for(i = num; i < (SceInt32)bulkof(res); i++){
					res[i].detectedTargetID = -1;
					res[i].state = SCE_SMART_TARGET_TRACKING_STATE_TARGET_SEARCH;
				}

				t0 = sceKernelGetProcessTimeLow();
				t1 = s->debug.tracking.t1;
				s->debug.tracking.t1 = t0;
				s->debug.tracking.fps = 1000000.0f / (t0 - t1);
				s->debug.tracking.pt = t0 - t2;
			}

			sampleUnlockCameraBuffer(image);

			{
				myMutexLock(&s->trackingThread.mtx);
				if(s->trackingThread.op == TARGET_TRACKING_OP_NOP){
					memcpy((void*)s->trackingThread.res, res, sizeof(s->trackingThread.res));
					s->trackingThread.timestamp = timestamp;
				}
				myMutexUnlock(&s->trackingThread.mtx);
			}
		}
	}

	if(run != FALSE){
#if SMART_WORKER_THREAD_MAX == 0
		SCECHK(sceSmartTargetTrackingStop());
#else
		SceInt32 res;
		int i;
		for(i = 0; i < SMART_WORKER_THREAD_MAX; i++){
			mySemaphoreWait(&s->workerThread.smp, INFINITE);
		}
		for(;;){
			res = sceSmartTargetTrackingStop();
			ASSERT((res >= SCE_OK) || (res == SCE_SMART_ERROR_BUSY));
			if(res != SCE_SMART_ERROR_BUSY){
				break;
			}
			do{
				res = sceSmartTargetTrackingRunWorker();
				ASSERT((res >= SCE_OK) || (res == SCE_SMART_ERROR_NOT_REQUIRED));
			}while(res == 1);
		}
#endif
	}

	DEBUG_PRINTF("%s: exit\n", __FUNCTION__);

	THREAD_RETURN(0);
}

static void _smartSendTargetTrackingOp(int32_t op){
	SmartTrackingState* const s = &s_smartTrackingState;
	{
		UInt i;
		myMutexLock(&s->trackingThread.mtx);
		myAtomic32Swap(&s->trackingThread.op, (MyAtomic32)op);
		for(i = 0; i < bulkof(s->trackingThread.res); i++){
			s->trackingThread.res[i].state = SCE_SMART_TARGET_TRACKING_STATE_IDLE;
		}
		myMutexUnlock(&s->trackingThread.mtx);
	}

	{
		UInt i;
		for(i = 0; i < bulkof(s->tracking); i++){
			s->tracking[i].target = -1;
			s->tracking[i].lost = 1;
			s->tracking[i].res.state = SCE_SMART_TARGET_TRACKING_STATE_IDLE;
		}
	}
}

static void _smartSendSceneMappingOp(int32_t op){
	SmartTrackingState* const s = &s_smartTrackingState;
	myAtomic32Swap(&s->dispatchThread.op, op);
	{
		s->detect = -1;
		s->lost = -1;
		sampleSetCameraAutoControlHold(FALSE);
	}
	_smartSendTargetTrackingOp(TARGET_TRACKING_OP_STOP);
}

static void _smartUpdateResult(CameraBuffer* image){
	SmartTrackingState* const s = &s_smartTrackingState;

#if DELAY_FRAME != 0
	{
		CameraBuffer* dimage = s->update.image_queue[0];
		UInt i = 0;
#if DELAY_FRAME > 1
		for(i = 0; i < DELAY_FRAME - 1; i++){
			s->update.image_queue[i] = s->update.image_queue[i + 1];
		}
#endif
		sampleLockCameraBuffer(image);
		s->update.image_queue[i] = image;
		image = dimage;
		sampleOverrideCameraImage(image);
		sampleUnlockCameraBuffer(image);
	}
#endif

	{
		SceSmartSceneMappingResult res = s->update.res;

		if((s->lost < 0) || (image == NULL)){
			if(s->dispatchThread.op == SCENE_MAPPING_OP_NOP){
				s->lost = 1;
			}
			s->dispatchThread.rpos = s->dispatchThread.wpos;
			res.state = SCE_SMART_SCENE_MAPPING_STATE_IDLE;

		}else{
			SceCameraRead* read = sampleGetCameraBufferRead(image);
			SceUInt64 timestamp = read->qwTimestamp;
			UInt rpos = s->dispatchThread.rpos;
			while(rpos != s->dispatchThread.wpos){
#if defined(ENABLE_FILTER)
				if (s->update.filter.enable != FALSE) {
					//
				}else
#endif
				if((SceInt32)(timestamp - s->dispatchThread.res[rpos].sm.timestamp) < 0){
#if 1
					UInt n = (SMART_RESULT_MAX - 1 + rpos - s->dispatchThread.wpos) % SMART_RESULT_MAX;
					if(n <= SMART_RESULT_LOW){
						rpos = (rpos + 1) % SMART_RESULT_MAX;
						__builtin_dmb();
						s->dispatchThread.rpos = rpos;
						//printf("drop result\n");
						continue;
					}
#endif
					break;
				}
				res = s->dispatchThread.res[rpos].sm;
				rpos = (rpos + 1) % SMART_RESULT_MAX;
				__builtin_dmb();
				s->dispatchThread.rpos = rpos;
#if defined(ENABLE_FILTER)
				if (s->update.filter.enable == TRUE) {
					if ((s->update.res.state != SCE_SMART_SCENE_MAPPING_STATE_SLAM) && (s->update.res.state != SCE_SMART_SCENE_MAPPING_STATE_LOCALIZE)) {
						// Resets filter state.
						filterReset(&s->update.filter.state);
					} else {
						// res : filtered result.
						filterSceneMappingRun(&s->update.filter.state, &res, &res);
					}
				}
#endif
			}
		}

		// Null mode
		if((res.state == SCE_SMART_SCENE_MAPPING_STATE_LOCALIZE) && (s->detect < 0)){
			res.state = SCE_SMART_SCENE_MAPPING_STATE_SEARCH;
		}

		switch(res.state){
		case SCE_SMART_SCENE_MAPPING_STATE_SLAM:
			{
				SceInt32 pres;
				SceCameraRead* read = sampleGetCameraBufferRead(image);
#if defined(ENABLE_FILTER)
				if ((SceInt32)(read->qwTimestamp - res.timestamp) < 0) {
					// sceSmartSceneMappingPropagateResult() rejects the result whose propagation-time is minus.
					s->update.res = res;
					break;
				}
#endif
				pres = sceSmartSceneMappingPropagateResult(&res, read->qwTimestamp, &s->update.res);
				if(pres == SCE_SMART_ERROR_INVALID_VALUE){
					DEBUG_PRINTF("sceSmartSceneMappingPropagateResult: failed: force restart!!\n");
					_smartSendSceneMappingOp(SCENE_MAPPING_OP_RESET);
					pres = SCE_OK;
				}
				ASSERT(pres == SCE_OK);
			}
			break;
		case SCE_SMART_SCENE_MAPPING_STATE_LOCALIZE:
			{
				SceInt32 pres;
				SceCameraRead* read = sampleGetCameraBufferRead(image);
#if defined(ENABLE_FILTER)
				if ((SceInt32)(read->qwTimestamp - res.timestamp) < 0) {
					// sceSmartSceneMappingPropagateResult() rejects the result whose propagation-time is minus.
					s->update.res.state = res.state;
					break;
				}
#endif
				{
					// reset acceleration values; clear accelerometer noise
					s->update.res.vel.x = 0.0f;
					s->update.res.vel.y = 0.0f;
					s->update.res.vel.z = 0.0f;
				}
				pres = sceSmartSceneMappingPropagateResult(&s->update.res, read->qwTimestamp, &s->update.res);
				if(pres == SCE_SMART_ERROR_INVALID_VALUE){
					DEBUG_PRINTF("sceSmartSceneMappingPropagateResult: failed: force restart!!\n");
					_smartSendSceneMappingOp(SCENE_MAPPING_OP_RESET);
					pres = SCE_OK;
				}
				ASSERT(pres == SCE_OK);
				s->update.res.state = res.state;
			}
			break;

		case SCE_SMART_SCENE_MAPPING_STATE_LOCALIZE_IMPOSSIBLE:
		case SCE_SMART_SCENE_MAPPING_STATE_IDLE:
		case SCE_SMART_SCENE_MAPPING_STATE_SEARCH:
		default:
			{
				memset(&s->update.res, 0, sizeof(s->update.res));
				s->update.res.state = res.state;
				s->update.res.rot.w = 1.0f;
			}
			break;
		}
	}

	// Update landmarks.
	if(s->update.res.state == SCE_SMART_SCENE_MAPPING_STATE_SLAM){
		SceInt32 res = sceSmartSceneMappingGetLandmarkInfo(s->landmarks.info, bulkof(s->landmarks.info));
		SCECHK(res);
		s->landmarks.num = res;
	}else{
		s->landmarks.num = 0;
	}

	// Update initialization points.
	if(s->update.res.state == SCE_SMART_SCENE_MAPPING_STATE_SEARCH){
		SceInt32 res = sceSmartSceneMappingGetInitializationPointInfo(s->initial.info, bulkof(s->initial.info));
		SCECHK(res);
		s->initial.num = res;
	}else{
		s->initial.num = 0;
	}

	// Start next dispatch.
	myEventRaise(&s->dispatchThread.evt);


	// Update tracking results.
	{
		UInt i;
		myMutexLock(&s->trackingThread.mtx);
		ASSERT(bulkof(s->trackingThread.res) >= bulkof(s->tracking));
		for(i = 0; i < bulkof(s->tracking); i++){
			s->tracking[i].res = s->trackingThread.res[i];
			s->tracking[i].timestamp = s->trackingThread.timestamp;
		}
		myMutexUnlock(&s->trackingThread.mtx);
	}

	// Start next tracking.
	myEventRaise(&s->trackingThread.evt);

	{
		MyVector v0;
		MyQuaternion q0;
		myVectorSet(&v0, s->update.res.pos.x, s->update.res.pos.y, s->update.res.pos.z);
		myQuaternionSet(&q0, s->update.res.rot.x, s->update.res.rot.y, s->update.res.rot.z, s->update.res.rot.w);
		myVectorMulScalar(&v0, &v0, -1.0f);
		myQuaternionConjugate(&q0, &q0);
		s->pos = v0;
		s->rot = q0;
	}

	{
		UInt i;
		for(i = 0; i < bulkof(s->tracking); i++){
			if(s->tracking[i].res.state == SCE_SMART_TARGET_TRACKING_STATE_TARGET_TRACKING){
				{
					SceSmartTargetTrackingResult* res = &s->tracking[i].res;
#if defined(ENABLE_FILTER)
					if (s->tracking[i].filter.targetId != res->detectedTargetID) {
						s->tracking[i].filter.targetId = res->detectedTargetID;
						filterReset(&s->tracking[i].filter.state);
					}
					filterTargetTrackingRun(&(s->tracking[i].filter.state), res, res);
#endif
					myVectorSet(&s->tracking[i].pos, res->pos.x, res->pos.y, res->pos.z);
					myQuaternionSet(&s->tracking[i].rot, res->rot.x, res->rot.y, res->rot.z, res->rot.w);
				}
				{
					SceInt32 detected = s->tracking[i].res.detectedTargetID;
					if((s->tracking[i].target < 0) || (s->target[s->tracking[i].target].id != detected)){
						UInt j;
						for(j = 0; j < bulkof(s->target); j++){
							if(s->target[j].id == detected){
								break;
							}
						}
						s->tracking[i].target = (SInt)j;
					}
				}
				s->tracking[i].lost = 0;
			}else{
#if defined(ENABLE_FILTER)
				s->tracking[i].filter.targetId = -1;
				filterReset(&(s->tracking[i].filter.state));
#endif
				s->tracking[i].lost = 1;
			}
		}
	}

	if(s->lost < 0){
		// not detected

	}else if(s->update.res.state == SCE_SMART_SCENE_MAPPING_STATE_SLAM){
		if(s->detect < 0){
			// detected
			SInt i = bulkof(s->target);
#if 1
			SceInt32 detected = s->update.res.detectedTargetID;
			for(i = 0; i < (SInt)bulkof(s->target); i++){
				if(s->target[i].id == detected){
					break;
				}
			}
#endif
			s->detect = i;

			sampleSetCameraAutoControlHold(TRUE);

			// Start target tracking.
			_smartSendTargetTrackingOp(TARGET_TRACKING_OP_START);
		}
		s->lost = 0;
	}else{
		if((s->detect >= 0) && (s->update.res.state == SCE_SMART_SCENE_MAPPING_STATE_SEARCH)){
			// lost
			s->detect = -1;

			sampleSetCameraAutoControlHold(FALSE);

			// Stop target tracking.
			_smartSendTargetTrackingOp(TARGET_TRACKING_OP_STOP);
		}
		s->lost = 1;
	}

#if defined(ENABLE_FILTER)
	if (s->update.filter.enable == FALSE) {
		filterReset(&s->update.filter.state);
	}
#endif
}

static void _smartLoadTargets(void){
	SmartTrackingState* const s = &s_smartTrackingState;
	static char const* const dict_list[] = {
		SMART_TARGET_DICT_01,
		SMART_TARGET_DICT_02,
	};
	static SceInt32 const arpc_list[] = {
		SCE_SMART_MARKER_01,
		SCE_SMART_MARKER_02,
		SCE_SMART_MARKER_03,
		SCE_SMART_MARKER_04,
		SCE_SMART_MARKER_05,
		SCE_SMART_MARKER_06,
	};
	UInt i;
	UInt j;
	ASSERT(bulkof(s->target) >= (bulkof(dict_list) + bulkof(arpc_list)));
	i = 0;
	for(j = 0; j < bulkof(dict_list); j++, i++){
		char const* dict_path = dict_list[j];
		FILE* fp;
		s->target[i].id = SCE_SMART_INVALID_TARGET_ID;
		fp = fopen(dict_path, "rb");
		if(fp == NULL){
			DEBUG_PRINTF("fopen(\"%s\"): failed\n", dict_path);
			continue;
		}
		{
			SceInt32 returnCode;
			SceInt32 targetId = SCE_SMART_INVALID_TARGET_ID;
			SceSmartStreamIn stream;
			stream.read = _smartStreamRead;
			stream.dsc = fp;
			ASSERT(stream.dsc != NULL);
			returnCode = sceSmartCreateLearnedImageTarget(&targetId, &stream);
			if(returnCode < SCE_OK){
				DEBUG_PRINTF("sceSmartCreateLearnedImageTarget(%08x): failed: \"%s\" \n", returnCode, dict_path);
				continue;
			}
			ASSERT(i < bulkof(s->target));
			s->target[i].id = targetId;
			SCECHK(sceSmartGetTargetInfo(targetId, &s->target[i].info));
		}
		fclose(fp);
	}
	for(j = 0; j < bulkof(arpc_list); j++, i++){
		SceInt32 targetId = arpc_list[j];
		ASSERT(i < bulkof(s->target));
		s->target[i].id = targetId;
		SCECHK(sceSmartGetTargetInfo(targetId, &s->target[i].info));
	}
	for(; i < bulkof(s->target); i++){
		s->target[i].id = SCE_SMART_INVALID_TARGET_ID;
	}
}

static void _smartDestroyTargets(void){
	SmartTrackingState* const s = &s_smartTrackingState;
	UInt i;
	for(i = 0; i < bulkof(s->target); i++){
		SceInt32 targetId = s->target[i].id;
		if(targetId == SCE_SMART_INVALID_TARGET_ID){
			continue;
		}
		if((targetId >= SCE_SMART_MARKER_01) && (targetId <= SCE_SMART_MARKER_06)){
			continue;
		}
		SCECHK(sceSmartDestroyTarget(targetId));
	}
}

#if defined(ENABLE_USERMALLOC)
#if 0
static void* _myalloc(void* dsc, size_t size){
	UNUSED(dsc);
	return malloc(size);
}
static void _myfree(void* dsc, void* ptr){
	UNUSED(dsc);
	free(ptr);
}
#else
static void* _myalloc(void* dsc, size_t size){
	return mspace_malloc((mspace)dsc, size);
}
static void _myfree(void* dsc, void* ptr){
	mspace_free((mspace)dsc, ptr);
}
#endif
#endif // defined(ENABLE_USERMALLOC)

int smartInitialize(void){
	SmartTrackingState* const s = &s_smartTrackingState;
	int returnCode;
	memset(&s_smartTrackingState, 0, sizeof(s_smartTrackingState));

#if defined(SCE_SYSMODULE_SMART)
	returnCode = sceSysmoduleLoadModule(SCE_SYSMODULE_SMART);
	SCE_DBG_ASSERT(returnCode >= SCE_OK);
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->initStep = 1;
#endif

#if defined(ENABLE_USERMALLOC)
	{
		SceUID uid;
		void* ptr;
		uid = sceKernelAllocMemBlock("smartHeap", SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA, SMART_HEAP_SIZE, SCE_NULL);
		ASSERT(uid >= SCE_OK);
		if(uid < SCE_OK){
			return uid;
		}
		SCECHK(sceKernelGetMemBlockBase(uid, &ptr));

		s->heap.ptr = ptr;
		s->heap.uid = uid;
		s->heap.msp = mspace_create(ptr, SMART_HEAP_SIZE);
		ASSERT(s->heap.msp != NULL);
		if(s->heap.msp == NULL){
			return -1;
		}
	}
	s->initStep = 2;
#endif

	{
#if defined(ENABLE_USERMALLOC)
		SceSmartMemoryAllocator	mem;
		mem.allocate = _myalloc;//mspace_malloc;
		mem.deallocate = _myfree;//mspace_free;
		mem.dsc = s->heap.msp;

		returnCode = sceSmartInit(&mem);
#else
		returnCode = sceSmartInit(NULL);
#endif
		ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}
	}
	s->initStep = 3;

	_smartLoadTargets();
	s->initStep = 4;

	// Initialize scene mapping.
	{
		UInt i;
		s->lost = -1;
		s->detect = -1;
		s->dispatchThread.mode = SCE_SMART_SCENE_MAPPING_INIT_HFG;
		//s->dispatchThread.mode = SCE_SMART_SCENE_MAPPING_INIT_SFM;
		s->dispatchThread.op = SCENE_MAPPING_OP_NOP;

#if 1 // Natural Marker SMART_TARGET_DICT_01
		for(i = 0; i < 1; i++){
			ASSERT(i < bulkof(s->target));
			if(s->target[i].id != SCE_SMART_INVALID_TARGET_ID){
				SCECHK(sceSmartSceneMappingRegisterTarget(s->target[i].id));
			}
		}
#endif
#if defined(ENABLE_WAAR) // ARPLayCards 06
		for(i = 2 + 5; i < 2 + 6; i++){
			ASSERT(i < bulkof(s->target));
			if(s->target[i].id != SCE_SMART_INVALID_TARGET_ID){
				SCECHK(sceSmartSceneMappingRegisterTarget(s->target[i].id));
			}
		}
#endif

		SCECHK(sceSmartSceneMappingSetDispatchMode(SCE_SMART_SCENE_MAPPING_DISPATCH_MODE_ASYNC));
		SCECHK(sceSmartSceneMappingEnableMask(SMART_TRACKING_MAX * 2));
	}
	s->initStep = 5;

	// Initialize target tracking.
	{
		UInt i;

		s->trackingThread.op = TARGET_TRACKING_OP_NOP;
		s->trackingThread.run = FALSE;

		for(i = 0; i < bulkof(s->tracking); i++){
			s->tracking[i].lost = 1;
			s->tracking[i].target = -1;
			s->tracking[i].res.state = SCE_SMART_TARGET_TRACKING_STATE_IDLE;
		}
		for(i = 0; i < bulkof(s->trackingThread.res); i++){
			s->trackingThread.res[i].state = SCE_SMART_TARGET_TRACKING_STATE_IDLE;
		}

#if 1 // Natural Marker SMART_TARGET_DICT_02
		for(i = 1; i < 2; i++){
			ASSERT(i < bulkof(s->target));
			if(s->target[i].id != SCE_SMART_INVALID_TARGET_ID){
				SCECHK(sceSmartTargetTrackingRegisterTarget(s->target[i].id));
			}
		}
#endif
#if defined(ENABLE_WAAR) // ARPLayCards 01-05
		for(i = 2; i < 2 + 5; i++){
			ASSERT(i < bulkof(s->target));
			if(s->target[i].id != SCE_SMART_INVALID_TARGET_ID){
				SCECHK(sceSmartTargetTrackingRegisterTarget(s->target[i].id));
			}
		}
#endif
	}
	s->initStep = 6;

	{
		returnCode = myEventCreate(&s->coreThread.evt);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		returnCode = mySemaphoreCreate(&s->coreThread.smp, 1, 1);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		s->coreThread.exit = FALSE;
		returnCode = myThreadStartEx(&s->coreThread.thread, "smartCore", _smartCoreThread, 0, SMART_CORE_THREAD_PRIORITY, SMART_CORE_THREAD_STACK_SIZE);
		if(returnCode < SCE_OK){
			return returnCode;
		}
	}
	s->initStep = 7;

	{
		returnCode = myEventCreate(&s->dispatchThread.evt);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		s->dispatchThread.exit = FALSE;
		returnCode = myThreadStartEx(&s->dispatchThread.thread, "smartDispatch", _smartDispatchThread, 0, SMART_DISPATCH_THREAD_PRIORITY, SMART_DISPATCH_THREAD_STACK_SIZE);
		if(returnCode < SCE_OK){
			return returnCode;
		}
	}
	s->initStep = 8;

#if SMART_WORKER_THREAD_MAX != 0
	{
		UInt i;
		s->workerThread.exit = FALSE;
		returnCode = mySemaphoreCreate(&s->workerThread.smp, SMART_WORKER_THREAD_MAX, SMART_WORKER_THREAD_MAX);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		returnCode = myEventFlagCreate(&s->workerThread.flg);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		for(i = 0; i < SMART_WORKER_THREAD_MAX; i++){
			returnCode = myThreadStartEx(&s->workerThread.thread[i], "smartWorker", _smartWorkerThread, i, SMART_WORKER_THREAD_PRIORITY, SMART_WORKER_THREAD_STACK_SIZE);
			if(returnCode < SCE_OK){
				return returnCode;
			}
		}
	}
	s->initStep = 9;
#endif
	{
		returnCode = myEventCreate(&s->trackingThread.evt);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		returnCode = myMutexCreate(&s->trackingThread.mtx);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		s->trackingThread.exit = FALSE;
		returnCode = myThreadStartEx(&s->trackingThread.thread, "smartTracking", _smartTrackingThread, 0, SMART_TRACKING_THREAD_PRIORITY, SMART_TRACKING_THREAD_STACK_SIZE);
		if(returnCode < SCE_OK){
			return returnCode;
		}
	}
	s->initStep = 10;

#if defined(ENABLE_FILTER)
	{
		s->update.filter.enable = TRUE;
		filterInitialize(&s->update.filter.state, FILTER_RESULT_MAX);
	}
	{
		UInt i;
		for (i =0; i < bulkof(s->tracking); ++i) {
			s->tracking[i].filter.targetId = -1;
			filterInitialize(&(s->tracking[i].filter.state), FILTER_RESULT_MAX);
		}
	}
#endif
	s->initStep = 11;

	sleep(100);	// ;-)
	DEBUG_PRINTF("sample start!\n");
	return SCE_OK;
}

void smartRelease(void){
	SmartTrackingState* const s = &s_smartTrackingState;

#if SMART_WORKER_THREAD_MAX != 0
	{
		UInt i;
		s->workerThread.exit = TRUE;
		if(myEventFlagIsValid(&s->workerThread.flg) != FALSE){
			myEventFlagSet(&s->workerThread.flg, 0xffffffffU);
		}
		for(i = 0; i < SMART_WORKER_THREAD_MAX; i++){
			if(myThreadIsValid(&s->workerThread.thread[i]) != FALSE){
				myThreadJoin(&s->workerThread.thread[i]);
			}
		}
	}
#endif

	{
		s->trackingThread.exit = TRUE;
		if(myEventIsValid(&s->trackingThread.evt) != FALSE){
			myEventRaise(&s->trackingThread.evt);
		}
		if(myThreadIsValid(&s->trackingThread.thread) != FALSE){
			myThreadJoin(&s->trackingThread.thread);
		}
	}

	if(myMutexIsValid(&s->trackingThread.mtx) != FALSE){
		myMutexRelease(&s->trackingThread.mtx);
	}
	if(myEventIsValid(&s->trackingThread.evt) != FALSE){
		myEventRelease(&s->trackingThread.evt);
	}

#if SMART_WORKER_THREAD_MAX != 0
	if(mySemaphoreIsValid(&s->workerThread.smp) != FALSE){
		mySemaphoreRelease(&s->workerThread.smp);
	}
	if(myEventIsValid(&s->workerThread.flg) != FALSE){
		myEventFlagRelease(&s->workerThread.flg);
	}
#endif

	if(s->initStep >= 6){
		UInt i;
		for(i = 1; i < 2 + 5; i++){
			if(s->target[i].id != SCE_SMART_INVALID_TARGET_ID){
				SCECHK(sceSmartTargetTrackingUnregisterTarget(s->target[i].id));
			}
		}
	}

	{
		s->coreThread.exit = TRUE;
		if(myEventIsValid(&s->coreThread.evt) != FALSE){
			myEventRaise(&s->coreThread.evt);
		}
		if(myThreadIsValid(&s->coreThread.thread) != FALSE){
			myThreadJoin(&s->coreThread.thread);
		}
	}

	{
		s->dispatchThread.exit = TRUE;
		if(myEventIsValid(&s->dispatchThread.evt) != FALSE){
			myEventRaise(&s->dispatchThread.evt);
		}
		if(myThreadIsValid(&s->dispatchThread.thread) != FALSE){
			myThreadJoin(&s->dispatchThread.thread);
		}
	}

	if(myEventIsValid(&s->dispatchThread.evt) != FALSE){
		myEventRelease(&s->dispatchThread.evt);
	}
	if(myEventIsValid(&s->coreThread.evt) != FALSE){
		myEventRelease(&s->coreThread.evt);
	}
	if(mySemaphoreIsValid(&s->coreThread.smp) != FALSE){
		mySemaphoreRelease(&s->coreThread.smp);
	}

	if(s->initStep >= 5){
		if(s->target[0].id != SCE_SMART_INVALID_TARGET_ID){
			SCECHK(sceSmartSceneMappingUnregisterTarget(s->target[0].id));
		}
		if(s->target[2 + 6 - 1].id != SCE_SMART_INVALID_TARGET_ID){
			SCECHK(sceSmartSceneMappingUnregisterTarget(s->target[2 + 6 - 1].id));
		}
	}

	if(s->initStep >= 4){
		_smartDestroyTargets();
	}

	if(s->initStep >= 3){
		SCECHK(sceSmartRelease());
	}

#if defined(ENABLE_USERMALLOC)
	if(s->initStep >= 2){
		if(s->heap.msp != NULL){
			int res = mspace_destroy(s->heap.msp);
			TOUCH(res);
			ASSERT(res == 0);
		}

		if(s->heap.uid >= SCE_OK){
			SCECHK(sceKernelFreeMemBlock(s->heap.uid));
			s->heap.uid = 0;
			s->heap.ptr = NULL;
		}
	}
#endif

#if defined(SCE_SYSMODULE_SMART)
	if(s->initStep >= 1){
		SCECHK(sceSysmoduleUnloadModule(SCE_SYSMODULE_SMART));
	}
#endif

	memset(&s_smartTrackingState, 0, sizeof(s_smartTrackingState));
}

void smartUpdate(SceUInt32 span, CameraBuffer* image){
	SmartTrackingState* const s = &s_smartTrackingState;

	UNUSED(span);

	_smartUpdateCtrl(g_MappingReset, g_MappingChangeMode, g_MappingSaveMap);

	_smartUpdateResult(image);

	_sampleUpdateView();

//	_sampleDrawLandmarks();

#if 0
	// Camera Image
	{
		float xl = -VIEW_SCALE_H;
		float xr = +VIEW_SCALE_H;
		float yt = -VIEW_SCALE_V;
		float yb = +VIEW_SCALE_V;
		MyVector v0 = MY_VECTOR_MAKE4(xl, yt, 1.0f, 1.0f);
		MyVector v1 = MY_VECTOR_MAKE4(xr, yt, 1.0f, 1.0f);
		MyVector v2 = MY_VECTOR_MAKE4(xr, yb, 1.0f, 1.0f);
		MyVector v3 = MY_VECTOR_MAKE4(xl, yb, 1.0f, 1.0f);
		MyVector t0 = MY_VECTOR_MAKE4(0.0f, 1.0f, 0.0f, 0.0f);
		MyVector t1 = MY_VECTOR_MAKE4(1.0f, 1.0f, 0.0f, 0.0f);
		MyVector t2 = MY_VECTOR_MAKE4(1.0f, 0.0f, 0.0f, 0.0f);
		MyVector t3 = MY_VECTOR_MAKE4(0.0f, 0.0f, 0.0f, 0.0f);
		sampleAddTriangleT(NULL, &v0, &t0, &v1, &t1, &v2, &t2);
		sampleAddTriangleT(NULL, &v2, &t2, &v3, &t3, &v0, &t0);
	}
#endif

	{
		char work[1024] = "\0";
		UInt i;
		size_t p;

		static const char* const initMode[] = {
			"LEARNED_IMAGE",
			"WAAR",
			"HFG",
			"VFG",
			"SFM",
			"DRYRUN",
			"NULL",
		};
		static const char* const state[] = {
			"IDLE",
			"SEARCH",
			"SLAM",
			"LOCALIZE",
			"IMPOSSIBLE",
		};

		p = strlen(work);
		snprintf(&work[p], sizeof(work) - p, "FPS(Mapping) : %5.2f[%3u]\nFPS(Tracking): %5.2f[%3u]\n", s->debug.mapping.fps, s->debug.mapping.pt / 1000, s->debug.tracking.fps, s->debug.tracking.pt / 1000);

#if SMART_WORKER_THREAD_MAX != 0
		for(i = 0; i < SMART_WORKER_THREAD_MAX; i++){
			SceUInt32 pt = s->debug.worker[i].pt;
			s->debug.worker[i].pt = 0;
			p = strlen(work);
			snprintf(&work[p], sizeof(work) - p, "Worker%u : [%3u]\n", i, pt / 1000);
		}
#endif
		p = strlen(work);
		snprintf(&work[p], sizeof(work) - p, "Mode: %s\nState: %s\nLandmarks:%u\n\nCircle: reset\nSquare: change mode\nTriangle: fix & save map\n", initMode[s->dispatchThread.mode], state[s->update.res.state], s->landmarks.num + s->initial.num);

		sampleDebugout(work);
	}
}

static void _smartUpdateCtrl(bool reset, bool change_mode, bool save_map){
	SmartTrackingState* const s = &s_smartTrackingState;


	if(reset){
		DEBUG_PRINTF("reset\n");
		_smartSendSceneMappingOp(SCENE_MAPPING_OP_RESET);
	}
	if(change_mode){
		DEBUG_PRINTF("change mode\n");
		_smartSendSceneMappingOp(SCENE_MAPPING_OP_STOP);
		switch(s->dispatchThread.mode){
		case SCE_SMART_SCENE_MAPPING_INIT_LEARNED_IMAGE:
			s->dispatchThread.mode = SCE_SMART_SCENE_MAPPING_INIT_WAAR;
			break;
		case SCE_SMART_SCENE_MAPPING_INIT_WAAR:
			s->dispatchThread.mode = SCE_SMART_SCENE_MAPPING_INIT_HFG;
			break;
		case SCE_SMART_SCENE_MAPPING_INIT_HFG:
			s->dispatchThread.mode = SCE_SMART_SCENE_MAPPING_INIT_VFG;
			break;
		case SCE_SMART_SCENE_MAPPING_INIT_VFG:
			s->dispatchThread.mode = SCE_SMART_SCENE_MAPPING_INIT_SFM;
			break;
		case SCE_SMART_SCENE_MAPPING_INIT_SFM:
			s->dispatchThread.mode = SCE_SMART_SCENE_MAPPING_INIT_NULL;
			break;
		case SCE_SMART_SCENE_MAPPING_INIT_NULL:
			s->dispatchThread.mode = SCE_SMART_SCENE_MAPPING_INIT_DRYRUN;
			break;
		case SCE_SMART_SCENE_MAPPING_INIT_DRYRUN:
			s->dispatchThread.mode = SCE_SMART_SCENE_MAPPING_INIT_LEARNED_IMAGE;
			break;
		default:
			DEBUG_PRINTF("unsupported mode\n");
			ASSERT(FALSE);
			break;
		}
	}
	if(save_map){
		DEBUG_PRINTF("save map\n");
		_smartSendSceneMappingOp(SCENE_MAPPING_OP_SAVE_MAP);
	}

#if defined(ENABLE_QUIT)
	if(CTRL_TEST(raw, trg, SCE_CTRL_START) == 0){
		quit();
	}
#endif

	g_MappingReset = false;
	g_MappingChangeMode = false;
	g_MappingSaveMap = false;
}

static void _sampleUpdateView(void){
	SmartTrackingState* const s = &s_smartTrackingState;
	MyMatrix vm;
	MyMatrix m0;
	MyMatrix m1;
	myMatrixRotationQuaternion(&m0, &s->rot);
	myMatrixTranslationVector(&m1, &s->pos);
	myMatrixMul(&vm, &m0, &m1);
	sampleSetViewMatrix(&vm);
}

static void _sampleDrawLandmarks(void){
	SmartTrackingState* const s = &s_smartTrackingState;
	MyMatrix vm;
	MyMatrix pm;
	MyMatrix m0;
	sampleGetViewMatrix(&vm);
	sampleGetProjMatrix(&pm);
	myMatrixMul(&m0, &pm, &vm);

#if 1
	// frame
	if(s->update.res.state == SCE_SMART_SCENE_MAPPING_STATE_IDLE){
		UInt32 c0 = 0x000000ffU;		// Red
		sampleAddLine(16.0f, 16.0f, 1.0f, c0, 960.0f - 16.0f, 16.0f, 1.0f, c0);
		sampleAddLine(16.0f, 16.0f, 1.0f, c0, 16.0f, 544.0f - 16.0f, 1.0f, c0);
		sampleAddLine(16.0f, 544.0f - 16.0f, 1.0f, c0, 960.0f - 16.0f, 544.0f - 16.0f, 1.0f, c0);
		sampleAddLine(960.0f - 16.0f, 16.0f, 1.0f, c0, 960.0f - 16.0f, 544.0f - 16.0f, 1.0f, c0);
	}
#endif
#if 1
	// target
	if(s->detect >= 0){
		float hw = 0.1f;
		float hh = 0.1f;
		if(s->detect < (SInt)bulkof(s->target)){
			hw = s->target[s->detect].info.physicalWidth * 0.5f;
			hh = s->target[s->detect].info.physicalHeight * 0.5f;
		}
		{
			UInt32 col = 0x000000ffU;
			sampleAddLineP(&m0, -hw, -hh, 0.0f, col, +hw, -hh, 0.0f, col);
			sampleAddLineP(&m0, +hw, -hh, 0.0f, col, +hw, +hh, 0.0f, col);
			sampleAddLineP(&m0, +hw, +hh, 0.0f, col, -hw, +hh, 0.0f, col);
			sampleAddLineP(&m0, -hw, +hh, 0.0f, col, -hw, -hh, 0.0f, col);
		}
	}
#endif
#if 1
	// axis
	if(s->lost == 0){
		{
			UInt32 c0 = 0x000000ffU;
			UInt32 c1 = 0x0000ff00U;
			UInt32 c2 = 0x00ff0000U;
			float l = 0.1f;
			sampleAddLineP(&m0, 0.0f, 0.0f, 0.0f, c0, l, 0.0f, 0.0f, c0);
			sampleAddLineP(&m0, 0.0f, 0.0f, 0.0f, c1, 0.0f, l, 0.0f, c1);
			sampleAddLineP(&m0, 0.0f, 0.0f, 0.0f, c2, 0.0f, 0.0f, l, c2);
		}
	}
#endif
#if 1
	// landmarks
	{
		uint32_t i;
		uint32_t n = s->landmarks.num;
		for(i = 0; i < n; i++){
			UInt32 c0;
			float x0;
			float y0;
			float z0;
			x0 = s->landmarks.info[i].pos.x;
			y0 = s->landmarks.info[i].pos.y;
			z0 = s->landmarks.info[i].pos.z;
			switch(s->landmarks.info[i].state){
			case SCE_SMART_SCENE_MAPPING_LANDMARK_TRACKED:
				c0 = 0x0000ff00U;		// Lime
				break;
			case SCE_SMART_SCENE_MAPPING_LANDMARK_LOST:
				c0 = 0x000000ffU;		// Red
				break;
			case SCE_SMART_SCENE_MAPPING_LANDMARK_SUSPENDED:
				c0 = 0x0000ffffU;		// Yellow
				break;
			case SCE_SMART_SCENE_MAPPING_LANDMARK_MASKED:
				c0 = 0x00ff0000U;		// Blue
				break;
				/*
				c0 = 0x00ffff00U;		// Aqua
				c0 = 0x00ff00ffU;		// Fuchsia
				c0 = 0x00ffffffU;		// White
				*/
			default:
				c0 = 0x00000000U;		// Black
				break;
			}
#if 1
			{
				MyVectorEx vx0;
				myVectorSet(&vx0.v, x0, y0, z0);
				myMatrixMulVector(&vx0.v, &m0, &vx0.v);
				if((vx0.s.w > -FLT_EPSILON) && (vx0.s.w < +FLT_EPSILON)){
					continue;
				}
				myVectorMulScalar(&vx0.v, &vx0.v, 1.0f / vx0.s.w);
				{
					float lx = 4.0f / DISPLAY_WIDTH;
					float ly = 4.0f / DISPLAY_HEIGHT;
					MyVector v0;
					MyVector v1;
					MyVector v2;
					MyVector v3;
					myVectorSet4(&v0, vx0.s.x - lx, vx0.s.y - ly, vx0.s.z, vx0.s.w);
					myVectorSet4(&v1, vx0.s.x + lx, vx0.s.y - ly, vx0.s.z, vx0.s.w);
					myVectorSet4(&v2, vx0.s.x + lx, vx0.s.y + ly, vx0.s.z, vx0.s.w);
					myVectorSet4(&v3, vx0.s.x - lx, vx0.s.y + ly, vx0.s.z, vx0.s.w);
					sampleAddTriangleC(NULL, &v0, c0, &v1, c0, &v2, c0);
					sampleAddTriangleC(NULL, &v2, c0, &v3, c0, &v0, c0);
				}
			}
#else
			{
				float z = 0.0f;
				float l = 0.005f;
				sampleAddLineP(&m0, x0 - l, y0 - z, z0 - z, c0, x0 + l, y0 + z, z0 + z, c0);
				sampleAddLineP(&m0, x0 - z, y0 - l, z0 - z, c0, x0 + z, y0 + l, z0 + z, c0);
				sampleAddLineP(&m0, x0 - z, y0 - z, z0 - l, c0, x0 + z, y0 + z, z0 + l, c0);
			}
#endif
		}
	}
#endif
#if 1
	// initialization points
	{
		uint32_t i;
		uint32_t n = s->initial.num;
		for(i = 0; i < n; i++){
			UInt32 c0;
			UInt32 c1;
			UInt32 c2;
			float x0;
			float y0;
			float z0;
			float x1;
			float y1;
			float z1;
			float sh = (float)DISPLAY_WIDTH / CAMERA_WIDTH * VIEW_SCALE_H;
			float sv = (float)DISPLAY_HEIGHT / CAMERA_HEIGHT * VIEW_SCALE_V;
			float ox = ((float)DISPLAY_WIDTH - (float)CAMERA_WIDTH * sh) * 0.5f;
			float oy = ((float)DISPLAY_HEIGHT - (float)CAMERA_HEIGHT * sv) * 0.5f;
			float l = 7.0f;
			x0 = s->initial.info[i].posCurrent.x * sh + ox;
			y0 = s->initial.info[i].posCurrent.y * sv + oy;
			z0 = 1.0f;
			c0 = 0x00ffff00U;		// Aqua
			x1 = s->initial.info[i].posInitial.x * sh + ox;
			y1 = s->initial.info[i].posInitial.y * sv + oy;
			z1 = 1.0f;
			c1 = 0x0000ff00U;		// Lime
			sampleAddLine(x0, y0, z0, c0, x1, y1, z1, c1);
			c2 = 0x0000ffffU;		// Yellow
			sampleAddLine(x0 - l, y0, z0, c2, x0 + l, y0, z0, c2);
			sampleAddLine(x0, y0 - l, z0, c2, x0, y0 + l, z0, c2);
		}
	}
#endif
#if 1
	// tracking targets
	{
		UInt i;
		for(i = 0; i < bulkof(s->tracking); i++){
			if((s->tracking[i].target >= 0) && (s->tracking[i].lost == 0)){
				float hw;
				float hh;
				MyMatrix mt;
				{
					MyVector v0;
					MyQuaternion q0;
					MyMatrix m1;
					MyMatrix m2;
					myQuaternionConjugate(&q0, &s->tracking[i].rot);
					myVectorMulScalar(&v0, &s->tracking[i].pos, -1.0f);
					myMatrixRotationQuaternion(&m1, &q0);
					myMatrixTranslationVector(&m2, &v0);
					myMatrixMul(&mt, &m1, &m2);
					myQuaternionConjugate(&q0, &s->rot);
					myVectorMulScalar(&v0, &s->pos, -1.0f);
					myMatrixRotationQuaternion(&m1, &q0);
					myMatrixTranslationVector(&m2, &v0);
					myMatrixMul(&mt, &m1, &mt);
					myMatrixMul(&mt, &m2, &mt);
					myMatrixMul(&mt, &vm, &mt);
					myMatrixMul(&mt, &pm, &mt);
				}
				if(s->tracking[i].target < (SInt)bulkof(s->target)){
					SceSmartTargetInfo info;
					SCECHK(sceSmartGetTargetInfo(s->target[s->tracking[i].target].id, &info));
					hw = info.physicalWidth * 0.5f;
					hh = info.physicalHeight * 0.5f;
				}else{
					hw = 0.1f;
					hh = 0.1f;
				}
				{
					UInt32 col = 0x0000ff00U;
					sampleAddLineP(&mt, -hw, -hh, 0.0f, col, +hw, -hh, 0.0f, col);
					sampleAddLineP(&mt, +hw, -hh, 0.0f, col, +hw, +hh, 0.0f, col);
					sampleAddLineP(&mt, +hw, +hh, 0.0f, col, -hw, +hh, 0.0f, col);
					sampleAddLineP(&mt, -hw, +hh, 0.0f, col, -hw, -hh, 0.0f, col);
				}
			}
		}
	}
#endif
}

bool sampleTargetDetected(int idx)
{
	SmartTrackingState* const s = &s_smartTrackingState;
	return (s->tracking[idx].target >= 0) && (s->tracking[idx].lost == 0);
}

void sampleGetTargetRotPos(int idx, MyQuaternion* rot, MyVector* pos)
{
	SmartTrackingState* const s = &s_smartTrackingState;

	if ((s->tracking[idx].target >= 0) && (s->tracking[idx].lost == 0))
	{
		*pos = s->tracking[idx].pos;
		*rot = s->tracking[idx].rot;
	}
}

void sampleGetTargetTransform(int idx, MyMatrix* matrix)
{
	SmartTrackingState* const s = &s_smartTrackingState;
	if ((s->tracking[idx].target >= 0) && (s->tracking[idx].lost == 0)) {
		MyVector v0;
		MyQuaternion q0;
		MyMatrix m1;
		MyMatrix m2;
		myQuaternionConjugate(&q0, &s->tracking[idx].rot);
		myVectorMulScalar(&v0, &s->tracking[idx].pos, -1.0f);
		myMatrixRotationQuaternion(&m1, &q0);
		myMatrixTranslationVector(&m2, &v0);
		myMatrixMul(matrix, &m1, &m2);
	}
}

SceSmartSceneMappingState sampleGetSceneMappingState()
{
	SmartTrackingState* const s = &s_smartTrackingState;
	return s->update.res.state;
}

SceSmartSceneMappingInitMode sampleGetSceneMappingInitMode()
{
	SmartTrackingState* const s = &s_smartTrackingState;
	return s->dispatchThread.mode;
}

void sampleSceneMappingReset()
{
	g_MappingReset = true;
}

void sampleSceneMappingChangeMode()
{
	g_MappingChangeMode = true;
}

void sampleSceneMappingSaveMap()
{
	g_MappingSaveMap = true;
}

void sampleGetViewRotPos(MyQuaternion* rot, MyVector* pos)
{
	SmartTrackingState* const s = &s_smartTrackingState;
	*rot = s->rot;
	*pos = s->pos;
}

int sampleGetNumLandmarks()
{
	SmartTrackingState* const s = &s_smartTrackingState;
	return s->landmarks.num;
}

SceSmartSceneMappingLandmarkInfo sampleGetLandmarkInfo(int landmark_num)
{
	SmartTrackingState* const s = &s_smartTrackingState;
	return s->landmarks.info[landmark_num];
}

