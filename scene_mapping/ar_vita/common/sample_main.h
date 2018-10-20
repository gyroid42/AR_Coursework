/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2011 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef SAMPLE_MAIN_H
#define SAMPLE_MAIN_H

#include "common.h"
#include "myvector.h"

#include <ctrl.h>
#include <camera.h>
#include <motion.h>

#include <libsmart.h>

#if defined(__cplusplus)
extern "C"{
#endif

#define PROJECTION_FOV				SCE_SMART_IMAGE_FOV
#define PROJECTION_ZNEAR			(0.01f)
#define PROJECTION_ZFAR				(100.0f)
#define PROJECTION_WIDTH			SCE_SMART_IMAGE_WIDTH
#define PROJECTION_HEIGHT			SCE_SMART_IMAGE_HEIGHT

#define DISPLAY_WIDTH				960
#define DISPLAY_HEIGHT				544

#define VIEW_SCALE_H				(1.0f)
#define VIEW_SCALE_V				((float)CAMERA_HEIGHT / CAMERA_WIDTH * DISPLAY_WIDTH / DISPLAY_HEIGHT)

#define LINES_VERTEX_SIZE			(32768)
#define TRIANGLES_TEX_VERTEX_SIZE	(32768)
#define TRIANGLES_COL_VERTEX_SIZE	(32768)

#define CAMERA_RESOLUTION			(SCE_CAMERA_RESOLUTION_VGA)
#define CAMERA_FRAMERATE			(SCE_CAMERA_FRAMERATE_60)
#define CAMERA_WIDTH				(640)
#define CAMERA_HEIGHT				(480)

//#define CAMERA_DEVICE				(SCE_CAMERA_DEVICE_FRONT)
#define CAMERA_DEVICE				(SCE_CAMERA_DEVICE_BACK)
//#define CAMERA_BUFFER_COUNT		(6)
#define CAMERA_BUFFER_COUNT			(8)

#define CAMERA_FORMAT				(SCE_CAMERA_FORMAT_YUV420_PLANE)

#define CAMERA_ATTRIBUTE_ISO		(SCE_CAMERA_ATTRIBUTE_ISO_AUTO)
#define CAMERA_ATTRIBUTE_WHITEBALANCE (SCE_CAMERA_ATTRIBUTE_WHITEBALANCE_AUTO)

#define APP_DATA_COUNT				(3)

#define SAVEDATA_SIZE				(8 * 1024 * 1024)

#define REPLAY_DAT_SIZE				(128 * 1024 * 1024)

#define VSYNC_THREAD_PRIORITY		(THREAD_PRIORITY_ABOVE_NORMAL)
#define VSYNC_THREAD_STACK_SIZE		(THREAD_DEFAULT_STACK_SIZE)

#define CTRL_TEST(raw, trg, btn)	((~trg & btn) | (raw & ~btn))

SceUInt32 sampleGetButtonsRaw(void);
SceUInt32 sampleGetButtonsTrg(void);
void sampleGetAnalogLeft(float* ox, float* oy);
void sampleGetAnalogRight(float* ox, float* oy);

typedef struct CameraBuffer CameraBuffer;

typedef struct SaveDataStream SaveDataStream;

CameraBuffer* sampleGetCameraImage(void);
SceMotionSensorState* sampleGetCameraMotion(CameraBuffer* buf);
SceCameraRead* sampleGetCameraBufferRead(CameraBuffer* buf);
void sampleLockCameraBuffer(CameraBuffer* buf);
void sampleUnlockCameraBuffer(CameraBuffer* bud);

void sampleSetCameraAutoControlHold(Bool hold);

void sampleOverrideCameraImage(CameraBuffer* image);

void sampleAddLine(float x0, float y0, float z0, UInt32 c0, float x1, float y1, float z1, UInt32 c1);
void sampleAddLineP(MyMatrix const* m0, float x0, float y0, float z0, UInt32 c0, float x1, float y1, float z1, UInt32 c1);
void sampleAddLineV(MyMatrix const* m0, MyVector const* v0, UInt32 c0, MyVector const* v1, UInt32 c1);
void sampleAddTriangleC(MyMatrix const* m0, MyVector const* v0, UInt32 c0, MyVector const* v1, UInt32 c1, MyVector const* v2, UInt32 c2);
void sampleAddTriangleT(MyMatrix const* m0, MyVector const* v0, MyVector const* t0, MyVector const* v1, MyVector const* t1, MyVector const* v2, MyVector const* t2);

void sampleClearLines(void);

void sampleSetViewMatrix(MyMatrix const* mat);
void sampleGetViewMatrix(MyMatrix* mat);
void sampleGetProjMatrix(MyMatrix* mat);

void sampleDebugout(char const* str);
void sampleSetStatusLine(char const* text, ...);


SaveDataStream* sampleSaveDataStreamOpen(char const* path, Bool read);
void sampleSaveDataStreamClose(SaveDataStream* stream);
size_t sampleSaveDataStreamWrite(SaveDataStream* stream, void const* buf, size_t size);
size_t sampleSaveDataStreamRead(SaveDataStream* stream, void* buf, size_t size);


// user defined functions in each sample codes
extern int smartInitialize(void);
extern void smartRelease(void);
extern void smartUpdate(SceUInt32 span, CameraBuffer* image);

extern char const* smartSampleName(void);

#if defined(__cplusplus)
} //extern "C"{
#endif

#endif /* SAMPLE_MAIN_H */
