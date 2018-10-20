/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <math.h>
#include <float.h>

#include "common.h"

#define USE_SIMD


#if defined(__cplusplus)
extern "C"{
#endif


#if defined(USE_SIMD)
typedef float MyFloat;
struct MyVector{
	float32x4_t q;
} __attribute__((vecreturn));
#else
typedef float MyFloat;
struct DECLARE_ALIGNED(MyVector{
	float f[4];
}, 16);
#endif
typedef struct MyVector MyVector;

typedef struct MyQuaternion{
	MyVector v;
}MyQuaternion;

typedef struct MyMatrix{
	MyVector v[4];
}MyMatrix;

typedef union{
	struct{
		float x;
		float y;
		float z;
		float w;
	}s;
	float f[4];
	MyVector v;
}MyVectorEx;

typedef union{
	struct{
		float x;
		float y;
		float z;
		float w;
	}s;
	float f[4];
	MyVector v;
	MyQuaternion q;
}MyQuaternionEx;

typedef union{
	struct{
		float x;
		float y;
		float z;
		float w;
	}s[4];
	float f[4][4];
	MyVector v[4];
	MyMatrix m;
}MyMatrixEx;

INLINE MyFloat myFloatReciprocal(MyFloat f0) ALWAYS_INLINE;
INLINE MyFloat myFloatSqrtf(MyFloat f0) ALWAYS_INLINE;
INLINE MyFloat myFloatAbs(MyFloat f0) ALWAYS_INLINE;

INLINE void myVectorReciprocal4(MyVector* v0, MyVector const* v1);
INLINE void myVectorSqrt4(MyVector* v0, MyVector const* v1);

INLINE MyFloat myVectorGetX(MyVector const* v0) ALWAYS_INLINE;
INLINE MyFloat myVectorGetY(MyVector const* v0) ALWAYS_INLINE;
INLINE MyFloat myVectorGetZ(MyVector const* v0) ALWAYS_INLINE;
INLINE MyFloat myVectorGetW(MyVector const* v0) ALWAYS_INLINE;
INLINE void myVectorSetX(MyVector* v0, MyFloat s1) ALWAYS_INLINE;
INLINE void myVectorSetY(MyVector* v0, MyFloat s1) ALWAYS_INLINE;
INLINE void myVectorSetZ(MyVector* v0, MyFloat s1) ALWAYS_INLINE;
INLINE void myVectorSetW(MyVector* v0, MyFloat s1) ALWAYS_INLINE;

INLINE void myVectorSet(MyVector* v0, float x, float y, float z) ALWAYS_INLINE;
INLINE void myVectorMulScalar(MyVector* v0, MyVector const* v1, MyFloat s2) ALWAYS_INLINE;
INLINE void myVectorAdd(MyVector* v0, MyVector const* v1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myVectorSub(MyVector* v0, MyVector const* v1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myVectorMul(MyVector* v0, MyVector const* v1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myVectorCross(MyVector* v0, MyVector const* v1, MyVector const* v2) ALWAYS_INLINE;
INLINE MyFloat myVectorDot(MyVector const* v0, MyVector const* v1) ALWAYS_INLINE;
INLINE MyFloat myVectorLength(MyVector const* v0) ALWAYS_INLINE;
INLINE Bool myVectorNormalizeRHW(MyVector* v0, MyVector const* v1) ALWAYS_INLINE;
INLINE void myVectorNormalize(MyVector* v0, MyVector const* v1) ALWAYS_INLINE;
INLINE void myVectorLerp(MyVector* v0, MyVector const* v1, MyVector const* v2, float t) ALWAYS_INLINE;

INLINE void myVectorSet4(MyVector* v0, float x, float y, float z, float w) ALWAYS_INLINE;
INLINE void myVectorMin4(MyVector* v0, MyVector const* v1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myVectorMax4(MyVector* v0, MyVector const* v1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myVectorMulScalar4(MyVector* v0, MyVector const* v1, MyFloat s2) ALWAYS_INLINE;
INLINE void myVectorAdd4(MyVector* v0, MyVector const* v1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myVectorSub4(MyVector* v0, MyVector const* v1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myVectorMul4(MyVector* v0, MyVector const* v1, MyVector const* v2) ALWAYS_INLINE;
INLINE MyFloat myVectorDot4(MyVector const* v0, MyVector const* v1) ALWAYS_INLINE;
INLINE MyFloat myVectorLength4(MyVector const* v0) ALWAYS_INLINE;
INLINE void myVectorNormalize4(MyVector* v0, MyVector const* v1) ALWAYS_INLINE;
INLINE void myVectorLerp4(MyVector* v0, MyVector const* v1, MyVector const* v2, float t) ALWAYS_INLINE;

INLINE void myQuaternionSet(MyQuaternion* q0, float x, float y, float z, float w) ALWAYS_INLINE;
INLINE void myQuaternionConjugate(MyQuaternion* q0, MyQuaternion const* q1) ALWAYS_INLINE;
INLINE void myQuaternionNormalize(MyQuaternion* q0, MyQuaternion const* q1) ALWAYS_INLINE;
INLINE void myQuaternionMul(MyQuaternion* q0, MyQuaternion const* q1, MyQuaternion const* q2) ALWAYS_INLINE;
INLINE void myQuaternionRotationAxis(MyQuaternion* q0, MyVector const* v1, MyFloat th) ALWAYS_INLINE;
INLINE void myQuaternionRotationGyro(MyQuaternion* q0, MyVector const* v1) ALWAYS_INLINE;
INLINE void myQuaternionRotationMatrix(MyQuaternion* q0, MyMatrix const* m1) ALWAYS_INLINE;
INLINE void myQuaternionRotateVector(MyVector* v0, MyQuaternion const* q1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myQuaternionRotateVectorR(MyVector* v0, MyQuaternion const* q1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myQuaternionSlerp(MyQuaternion* q0, MyQuaternion const* q1, MyQuaternion const* q2, float t) ALWAYS_INLINE;

INLINE void myMatrixIdentity(MyMatrix* m0) ALWAYS_INLINE;
INLINE void myMatrixScaling(MyMatrix* m0, float sx, float sy, float sz) ALWAYS_INLINE;
INLINE void myMatrixTranslation(MyMatrix* m0, float tx, float ty, float tz) ALWAYS_INLINE;
INLINE void myMatrixTranslationVector(MyMatrix* m0, MyVector const* v1) ALWAYS_INLINE;
INLINE void myMatrixRotationX(MyMatrix* m0, float rx) ALWAYS_INLINE;
INLINE void myMatrixRotationY(MyMatrix* m0, float ry) ALWAYS_INLINE;
INLINE void myMatrixRotationZ(MyMatrix* m0, float rz) ALWAYS_INLINE;
INLINE void myMatrixRotationQuaternion(MyMatrix* m0, MyQuaternion const* q1) ALWAYS_INLINE;
INLINE void myMatrixPerspective(MyMatrix* m0, float fovy, float aspect, float near, float far) ALWAYS_INLINE;
INLINE void myMatrixMulVector(MyVector* v0, MyMatrix const* m1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myMatrixMulVector4(MyVector* v0, MyMatrix const* m1, MyVector const* v2) ALWAYS_INLINE;
INLINE void myMatrixMul(MyMatrix* m0, MyMatrix const* m1, MyMatrix const* m2) ALWAYS_INLINE;
INLINE void myMatrixMulScalar(MyMatrix* m0, MyMatrix const* m1, MyFloat s2) ALWAYS_INLINE;
INLINE void myMatrixAdd(MyMatrix* m0, MyMatrix const* m1, MyMatrix const* m2) ALWAYS_INLINE;
INLINE void myMatrixTranspose(MyMatrix* m0, MyMatrix const* m1) ALWAYS_INLINE;
INLINE void myMatrixInverse(MyMatrix* m0, MyMatrix const* m1) ALWAYS_INLINE;

INLINE SInt myTriangleIntersection(MyVector* vr, MyVector const* org, MyVector const* dir, MyVector const* t0, MyVector const* t1, MyVector const* t2) ALWAYS_INLINE;

#define MY_VECTOR_MAKE(xx, yy, zz)		(MyVector){{ xx, yy, zz, 1.0f }}
#define MY_VECTOR_MAKE4(xx, yy, zz, ww)	(MyVector){{ xx, yy, zz, ww }}


INLINE MyFloat myFloatReciprocal(MyFloat f0){
	float f0_ = (float)f0;
	ASSERT((f0_ > +FLT_EPSILON) || (f0_ < -FLT_EPSILON));
	return (MyFloat)(((f0_ > +FLT_EPSILON) || (f0_ < -FLT_EPSILON)) ? (1.0f / f0_) : INFINITY);
}
INLINE MyFloat myFloatSqrtf(MyFloat f0){
	return (MyFloat)sqrtf((float)f0);
}
INLINE MyFloat myFloatAbs(MyFloat f0){
	return (MyFloat)fabs((float)f0);
}

INLINE void myVectorReciprocal4(MyVector* v0, MyVector const* v1){
#if 0//defined(USE_SIMD)
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	vx1.v = *v1;
	vx0.s.x = 1.0f / vx1.s.x;
	vx0.s.y = 1.0f / vx1.s.y;
	vx0.s.z = 1.0f / vx1.s.z;
	vx0.s.w = 1.0f / vx1.s.w;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorSqrt4(MyVector* v0, MyVector const* v1){
#if 0//defined(USE_SIMD)
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	vx1.v = *v1;
	vx0.s.x = sqrtf(vx1.s.x);
	vx0.s.y = sqrtf(vx1.s.y);
	vx0.s.z = sqrtf(vx1.s.z);
	vx0.s.w = sqrtf(vx1.s.w);
	*v0 = vx0.v;
#endif
}

INLINE MyFloat myVectorGetX(MyVector const* v0){
#if defined(USE_SIMD)
	return v0->q[0];
#else
	MyVectorEx vx0;
	vx0.v = *v0;
	return vx0.s.x;
#endif
}
INLINE MyFloat myVectorGetY(MyVector const* v0){
#if defined(USE_SIMD)
	return v0->q[1];
#else
	MyVectorEx vx0;
	vx0.v = *v0;
	return vx0.s.y;
#endif
}
INLINE MyFloat myVectorGetZ(MyVector const* v0){
#if defined(USE_SIMD)
	return v0->q[2];
#else
	MyVectorEx vx0;
	vx0.v = *v0;
	return vx0.s.z;
#endif
}
INLINE MyFloat myVectorGetW(MyVector const* v0){
#if defined(USE_SIMD)
	return v0->q[3];
#else
	MyVectorEx vx0;
	vx0.v = *v0;
	return vx0.s.w;
#endif
}
INLINE void myVectorSetX(MyVector* v0, MyFloat s1){
#if defined(USE_SIMD)
	v0->q[0] = s1;
#else
	MyVectorEx vx0;
	vx0.v = *v0;
	vx0.s.x = s1;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorSetY(MyVector* v0, MyFloat s1){
#if defined(USE_SIMD)
	v0->q[1] = s1;
#else
	MyVectorEx vx0;
	vx0.v = *v0;
	vx0.s.y = s1;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorSetZ(MyVector* v0, MyFloat s1){
#if defined(USE_SIMD)
	v0->q[2] = s1;
#else
	MyVectorEx vx0;
	vx0.v = *v0;
	vx0.s.z = s1;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorSetW(MyVector* v0, MyFloat s1){
#if defined(USE_SIMD)
	v0->q[3] = s1;
#else
	MyVectorEx vx0;
	vx0.v = *v0;
	vx0.s.w = s1;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorSet(MyVector* v0, float x, float y, float z){
#if defined(USE_SIMD)
	v0->q = (float32x4_t){ x, y, z, 1.0f };
#else
	MyVectorEx vx0;
	vx0.s.x = x;
	vx0.s.y = y;
	vx0.s.z = z;
	//vx0.s.w = 1.0f;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorSet4(MyVector* v0, float x, float y, float z, float w){
#if defined(USE_SIMD)
	v0->q = (float32x4_t){ x, y, z, w };
#else
	MyVectorEx vx0;
	vx0.s.x = x;
	vx0.s.y = y;
	vx0.s.z = z;
	vx0.s.w = w;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorMin4(MyVector* v0, MyVector const* v1, MyVector const* v2){
#if 0//defined(USE_SIMD)
#else
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	myVectorSet4(v0, (vx1.s.x <= vx2.s.x) ? vx1.s.x : vx2.s.x, (vx1.s.y <= vx2.s.y) ? vx1.s.y : vx2.s.y, (vx1.s.z <= vx2.s.z) ? vx1.s.z : vx2.s.z, (vx1.s.w <= vx2.s.w) ? vx1.s.w : vx2.s.w);
#endif
}
INLINE void myVectorMax4(MyVector* v0, MyVector const* v1, MyVector const* v2){
#if 0//defined(USE_SIMD)
#else
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	myVectorSet4(v0, (vx1.s.x >= vx2.s.x) ? vx1.s.x : vx2.s.x, (vx1.s.y >= vx2.s.y) ? vx1.s.y : vx2.s.y, (vx1.s.z >= vx2.s.z) ? vx1.s.z : vx2.s.z, (vx1.s.w >= vx2.s.w) ? vx1.s.w : vx2.s.w);
#endif
}
INLINE void myVectorMulScalar(MyVector* v0, MyVector const* v1, MyFloat s2){
#if defined(USE_SIMD)
	v0->q = vmulq_n_f32(v1->q, s2);
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	vx1.v = *v1;
	vx0.s.x = vx1.s.x * s2;
	vx0.s.y = vx1.s.y * s2;
	vx0.s.z = vx1.s.z * s2;
	//vx0.s.w = 1.0f;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorMulScalar4(MyVector* v0, MyVector const* v1, MyFloat s2){
#if defined(USE_SIMD)
	v0->q = vmulq_n_f32(v1->q, s2);
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	vx1.v = *v1;
	vx0.s.x = vx1.s.x * s2;
	vx0.s.y = vx1.s.y * s2;
	vx0.s.z = vx1.s.z * s2;
	vx0.s.w = vx1.s.w * s2;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorAdd(MyVector* v0, MyVector const* v1, MyVector const* v2){
#if defined(USE_SIMD)
	v0->q = vaddq_f32(v1->q, v2->q);
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	vx0.s.x = vx1.s.x + vx2.s.x;
	vx0.s.y = vx1.s.y + vx2.s.y;
	vx0.s.z = vx1.s.z + vx2.s.z;
	//vx0.s.w = 1.0f;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorAdd4(MyVector* v0, MyVector const* v1, MyVector const* v2){
#if defined(USE_SIMD)
	v0->q = vaddq_f32(v1->q, v2->q);
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	vx0.s.x = vx1.s.x + vx2.s.x;
	vx0.s.y = vx1.s.y + vx2.s.y;
	vx0.s.z = vx1.s.z + vx2.s.z;
	vx0.s.w = vx1.s.w + vx2.s.w;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorSub(MyVector* v0, MyVector const* v1, MyVector const* v2){
#if defined(USE_SIMD)
	v0->q = vsubq_f32(v1->q, v2->q);
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	vx0.s.x = vx1.s.x - vx2.s.x;
	vx0.s.y = vx1.s.y - vx2.s.y;
	vx0.s.z = vx1.s.z - vx2.s.z;
	//vx0.s.w = 1.0f;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorSub4(MyVector* v0, MyVector const* v1, MyVector const* v2){
#if defined(USE_SIMD)
	v0->q = vsubq_f32(v1->q, v2->q);
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	vx0.s.x = vx1.s.x - vx2.s.x;
	vx0.s.y = vx1.s.y - vx2.s.y;
	vx0.s.z = vx1.s.z - vx2.s.z;
	vx0.s.w = vx1.s.w - vx2.s.w;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorCross(MyVector* v0, MyVector const* v1, MyVector const* v2){
#if 0//defined(USE_SIMD)
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	vx0.s.x = vx1.s.y * vx2.s.z - vx1.s.z * vx2.s.y;
	vx0.s.y = vx1.s.z * vx2.s.x - vx1.s.x * vx2.s.z;
	vx0.s.z = vx1.s.x * vx2.s.y - vx1.s.y * vx2.s.x;
	*v0 = vx0.v;
#endif
}
INLINE MyFloat myVectorDot(MyVector const* v0, MyVector const* v1){
#if defined(USE_SIMD)
	MyVector vt;
	vt.q = vmulq_f32(v0->q, v1->q);
	return vt.q[0] + vt.q[1] + vt.q[2];
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	vx0.v = *v0;
	vx1.v = *v1;
	return vx0.s.x * vx1.s.x + vx0.s.y * vx1.s.y + vx0.s.z * vx1.s.z;
#endif
}
INLINE MyFloat myVectorDot4(MyVector const* v0, MyVector const* v1){
#if 0//defined(USE_SIMD)
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	vx0.v = *v0;
	vx1.v = *v1;
	return vx0.s.x * vx1.s.x + vx0.s.y * vx1.s.y + vx0.s.z * vx1.s.z + vx0.s.w * vx1.s.w;
#endif
}
INLINE void myVectorMul(MyVector* v0, MyVector const* v1, MyVector const* v2){
#if defined(USE_SIMD)
	v0->q = vmulq_f32(v1->q, v2->q);
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	vx0.s.x = vx1.s.x * vx2.s.x;
	vx0.s.y = vx1.s.y * vx2.s.y;
	vx0.s.z = vx1.s.z * vx2.s.z;
	//vx0.s.w = 1.0f;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorMul4(MyVector* v0, MyVector const* v1, MyVector const* v2){
#if defined(USE_SIMD)
	v0->q = vmulq_f32(v1->q, v2->q);
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	vx0.s.x = vx1.s.x * vx2.s.x;
	vx0.s.y = vx1.s.y * vx2.s.y;
	vx0.s.z = vx1.s.z * vx2.s.z;
	vx0.s.w = vx1.s.w * vx2.s.w;
	*v0 = vx0.v;
#endif
}
INLINE MyFloat myVectorLength(MyVector const* v0){
#if 0//defined(USE_SIMD)
#else
	return myFloatSqrtf(myVectorDot(v0, v0));
#endif
}
INLINE MyFloat myVectorLength4(MyVector const* v0){
#if defined(USE_SIMD)
	float32x4_t qt0 = vmulq_f32(v0->q, v0->q);
	float32x2_t dt1 = vget_low_f32(qt0);
	float32x2_t dt2 = vget_high_f32(qt0);
	float32x2_t dt3 = vadd_f32(dt1, dt2);
	float32x2_t dt4 = vpadd_f32(dt3, dt3);
	return myFloatSqrtf(dt4[0]);
#else
	MyVectorEx vx0;
	vx0.v = *v0;
	return sqrtf(vx0.s.x * vx0.s.x + vx0.s.y * vx0.s.y + vx0.s.z * vx0.s.z + vx0.s.w * vx0.s.w);
#endif
}
INLINE Bool myVectorNormalizeRHW(MyVector* v0, MyVector const* v1){
	float w = (float)myVectorGetW(v1);
	float rhw;
	if((w > -FLT_EPSILON) && (w < +FLT_EPSILON)){
		return FALSE;
	}
	rhw = 1.0f / w;
#if defined(USE_SIMD)
	v0->q = vmulq_n_f32(v1->q, rhw);
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	vx1.v = *v1;
	vx0.s.x = vx1.s.x * rhw;
	vx0.s.y = vx1.s.y * rhw;
	vx0.s.z = vx1.s.z * rhw;
	//vx0.s.w = 1.0f;
	*v0 = vx0.v;
#endif
	return TRUE;
}
INLINE void myVectorNormalize(MyVector* v0, MyVector const* v1){
#if defined(USE_SIMD)
	MyFloat l = myVectorLength(v1);
	MyFloat rl = myFloatReciprocal(l);
	v0->q = vmulq_n_f32(v1->q, rl);
#else
	MyFloat l = myVectorLength(v1);
	MyFloat rl = myFloatReciprocal(l);
	MyVectorEx vx0;
	MyVectorEx vx1;
	vx1.v = *v1;
	vx0.s.x = vx1.s.x * rl;
	vx0.s.y = vx1.s.y * rl;
	vx0.s.z = vx1.s.z * rl;
	//vx0.s.w = 1.0f;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorNormalize4(MyVector* v0, MyVector const* v1){
#if defined(USE_SIMD)
	MyFloat l = myVectorLength4(v1);
	MyFloat rl = myFloatReciprocal(l);
	v0->q = vmulq_n_f32(v1->q, rl);
#else
	MyFloat l = myVectorLength4(v1);
	MyFloat rl = myFloatReciprocal(l);
	MyVectorEx vx0;
	MyVectorEx vx1;
	ASSERT(l > FLT_EPSILON);
	vx1.v = *v1;
	vx0.s.x = vx1.s.x * rl;
	vx0.s.y = vx1.s.y * rl;
	vx0.s.z = vx1.s.z * rl;
	vx0.s.w = vx1.s.w * rl;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorLerp(MyVector* v0, MyVector const* v1, MyVector const* v2, float t){
#if defined(USE_SIMD)
	float32x4_t qt;
	qt = vmulq_n_f32(v1->q, 1.0f - t);
	qt = vmlaq_n_f32(qt, v2->q, t);
	v0->q = qt;
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	vx0.s.x = vx1.s.x + (vx2.s.x - vx1.s.x) * t;
	vx0.s.y = vx1.s.y + (vx2.s.y - vx1.s.y) * t;
	vx0.s.z = vx1.s.z + (vx2.s.z - vx1.s.z) * t;
	//vx0.s.w = 1.0f;
	*v0 = vx0.v;
#endif
}
INLINE void myVectorLerp4(MyVector* v0, MyVector const* v1, MyVector const* v2, float t){
#if defined(USE_SIMD)
	float32x4_t qt;
	qt = vmulq_n_f32(v1->q, 1.0f - t);
	qt = vmlaq_n_f32(qt, v2->q, t);
	v0->q = qt;
#else
	MyVectorEx vx0;
	MyVectorEx vx1;
	MyVectorEx vx2;
	vx1.v = *v1;
	vx2.v = *v2;
	vx0.s.x = vx1.s.x + (vx2.s.x - vx1.s.x) * t;
	vx0.s.y = vx1.s.y + (vx2.s.y - vx1.s.y) * t;
	vx0.s.z = vx1.s.z + (vx2.s.z - vx1.s.z) * t;
	vx0.s.w = vx1.s.w + (vx2.s.w - vx1.s.w) * t;
	*v0 = vx0.v;
#endif
}
INLINE void myQuaternionSet(MyQuaternion* q0, float x, float y, float z, float w){
	myVectorSet4(&q0->v, x, y, z, w);
}
INLINE void myQuaternionConjugate(MyQuaternion* q0, MyQuaternion const* q1){
#if 0//defined(USE_SIMD)
#else
	MyQuaternionEx qx1;
	qx1.q = *q1;
	myQuaternionSet(q0, -qx1.s.x, -qx1.s.y, -qx1.s.z, +qx1.s.w);
#endif
}
INLINE void myQuaternionNormalize(MyQuaternion* q0, MyQuaternion const* q1){
	myVectorNormalize4(&q0->v, &q1->v);
}
INLINE void myQuaternionMul(MyQuaternion* q0, MyQuaternion const* q1, MyQuaternion const* q2){
#if defined(USE_SIMD)
	float32x4_t qv1 = q1->v.q;
	float32x4_t qv2 = q2->v.q;
	MyQuaternionEx qtx;
	MyQuaternionEx qty;
	MyQuaternionEx qtz;
	MyQuaternionEx qtw;
	float x;
	float y;
	float z;
	float w;
	qtx.v.q = vmulq_lane_f32(qv2, vget_low_f32(qv1), 0);
	qty.v.q = vmulq_lane_f32(qv2, vget_low_f32(qv1), 1);
	qtz.v.q = vmulq_lane_f32(qv2, vget_high_f32(qv1), 0);
	qtw.v.q = vmulq_lane_f32(qv2, vget_high_f32(qv1), 1);
	x = qtw.s.x + qtx.s.w + qty.s.z - qtz.s.y;
	y = qtw.s.y + qty.s.w + qtz.s.x - qtx.s.z;
	z = qtw.s.z + qtz.s.w + qtx.s.y - qty.s.x;
	w = qtw.s.w - qtx.s.x - qty.s.y - qtz.s.z;
	myQuaternionSet(q0, x, y, z, w);
#else
	MyQuaternionEx qx0;
	MyQuaternionEx qx1;
	MyQuaternionEx qx2;
	qx1.q = *q1;
	qx2.q = *q2;
	qx0.s.x = qx1.s.w * qx2.s.x + qx1.s.x * qx2.s.w + qx1.s.y * qx2.s.z - qx1.s.z * qx2.s.y;
	qx0.s.y = qx1.s.w * qx2.s.y + qx1.s.y * qx2.s.w + qx1.s.z * qx2.s.x - qx1.s.x * qx2.s.z;
	qx0.s.z = qx1.s.w * qx2.s.z + qx1.s.z * qx2.s.w + qx1.s.x * qx2.s.y - qx1.s.y * qx2.s.x;
	qx0.s.w = qx1.s.w * qx2.s.w - qx1.s.x * qx2.s.x - qx1.s.y * qx2.s.y - qx1.s.z * qx2.s.z;
	*q0 = qx0.q;
#endif
}
INLINE void myQuaternionRotationAxis(MyQuaternion* q0, MyVector const* v1, MyFloat th){
	MyFloat ht = th * 0.5f;
	MyFloat s = sinf(ht);
	MyFloat c = cosf(ht);
	MyVectorEx vx1;
	vx1.v = *v1;
	myQuaternionSet(q0, vx1.s.x * s, vx1.s.y * s, vx1.s.z * s, c);
}
INLINE void myQuaternionRotationGyro(MyQuaternion* q0, MyVector const* v1){
	MyQuaternion qt;
	MyQuaternion qx;
	MyQuaternion qy;
	MyQuaternion qz;
	MyVectorEx ht;
	myVectorMulScalar(&ht.v, v1, 0.5f);
	myQuaternionSet(&qt, 0.0f, 0.0f, 0.0f, 1.0f);
	myQuaternionSet(&qx, sinf(ht.s.x), 0.0f, 0.0f, cosf(ht.s.x));
	myQuaternionSet(&qy, 0.0f, sinf(ht.s.y), 0.0f, cosf(ht.s.y));
	myQuaternionSet(&qz, 0.0f, 0.0f, sinf(ht.s.z), cosf(ht.s.z));
	myQuaternionMul(&qt, &qt, &qx);
	myQuaternionMul(&qt, &qt, &qy);
	myQuaternionMul(&qt, &qt, &qz);
	*q0 = qt;
}
INLINE void myQuaternionRotationMatrix(MyQuaternion* q0, MyMatrix const* m1){
	UInt i;
	MyVectorEx vt;
	MyMatrixEx mx1;
	mx1.m = *m1;
	vt.s.x = +mx1.f[0][0] - mx1.f[1][1] - mx1.f[2][2] + 1.0f;
	vt.s.y = -mx1.f[0][0] + mx1.f[1][1] - mx1.f[2][2] + 1.0f;
	vt.s.z = -mx1.f[0][0] - mx1.f[1][1] + mx1.f[2][2] + 1.0f;
	vt.s.w = +mx1.f[0][0] + mx1.f[1][1] + mx1.f[2][2] + 1.0f;
	{
		i = 0;
		if(vt.f[i] < vt.f[1]){
			i = 1;
		}
		if(vt.f[i] < vt.f[2]){
			i = 2;
		}
		if(vt.f[i] < vt.f[3]){
			i = 3;
		}
		if(vt.f[i] < FLT_EPSILON){ // error
			myQuaternionSet(q0, 0.0f, 0.0f, 0.0f, 1.0f);
			return;
		}
	}
	{
		MyFloat rvq = 0.5f / sqrtf(vt.f[i]);
		switch(i){
		case 0:
			myQuaternionSet(q0,
				vt.f[i],
				(mx1.f[1][0] + mx1.f[0][1]),
				(mx1.f[0][2] + mx1.f[2][0]),
				(mx1.f[2][1] - mx1.f[1][2]));
			break;
		case 1:
			myQuaternionSet(q0,
				(mx1.f[1][0] + mx1.f[0][1]),
				vt.f[i],
				(mx1.f[2][1] + mx1.f[1][2]),
				(mx1.f[0][2] - mx1.f[2][0]));
			break;
		case 2:
			myQuaternionSet(q0,
				(mx1.f[0][2] + mx1.f[2][0]),
				(mx1.f[2][1] + mx1.f[1][2]),
				vt.f[i],
				(mx1.f[1][0] - mx1.f[0][1]));
			break;
		case 3:
			myQuaternionSet(q0,
				(mx1.f[2][1] - mx1.f[1][2]),
				(mx1.f[0][2] - mx1.f[2][0]),
				(mx1.f[1][0] - mx1.f[0][1]),
				vt.f[i]);
			break;
		}
		myVectorMulScalar(&q0->v, &q0->v, rvq);
	}
}

INLINE void myQuaternionRotateVector(MyVector* v0, MyQuaternion const* q1, MyVector const* v2){
	MyQuaternion qq = *q1;
	MyQuaternion qr;
	MyQuaternion qp;
	qp.v = *v2;
	myVectorSetW(&qp.v, (MyFloat)(0.0f));
	myQuaternionConjugate(&qr, &qq);
	myQuaternionMul(&qp, &qr, &qp);
	myQuaternionMul(&qp, &qp, &qq);
	*v0 = qp.v;
}
INLINE void myQuaternionRotateVectorR(MyVector* v0, MyQuaternion const* q1, MyVector const* v2){
	MyQuaternion qq = *q1;
	MyQuaternion qr;
	MyQuaternion qp;
	qp.v = *v2;
	myVectorSetW(&qp.v, (MyFloat)(0.0f));
	myQuaternionConjugate(&qr, &qq);
	myQuaternionMul(&qp, &qq, &qp);
	myQuaternionMul(&qp, &qp, &qr);
	*v0 = qp.v;
}
INLINE void myQuaternionSlerp(MyQuaternion* q0, MyQuaternion const* q1, MyQuaternion const* q2, float t){
	MyFloat d = myVectorDot4(&q1->v, &q2->v);
	if(fabs((float)d) > (1.0f - FLT_EPSILON)){
		*q0 = *q2;
	}else{
		float th = acosf((float)d);
		float rt = 1.0f / sinf(th);
		MyFloat t1 = sinf((1.0f - t) * th) * rt;
		MyFloat t2 = sinf(t * th) * rt;
		MyVector vt0;
		MyVector vt1;
		myVectorMulScalar4(&vt0, &q1->v, t1);
		myVectorMulScalar4(&vt1, &q2->v, t2);
		myVectorAdd4(&q0->v, &vt0, &vt1);
	}
}
INLINE void myMatrixIdentity(MyMatrix* m0){
	myVectorSet4(&m0->v[0], 1.0f, 0.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[1], 0.0f, 1.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[2], 0.0f, 0.0f, 1.0f, 0.0f);
	myVectorSet4(&m0->v[3], 0.0f, 0.0f, 0.0f, 1.0f);
}
INLINE void myMatrixScaling(MyMatrix* m0, float sx, float sy, float sz){
	myVectorSet4(&m0->v[0], sx, 0.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[1], 0.0f, sy, 0.0f, 0.0f);
	myVectorSet4(&m0->v[2], 0.0f, 0.0f, sz, 0.0f);
	myVectorSet4(&m0->v[3], 0.0f, 0.0f, 0.0f, 1.0f);
}
INLINE void myMatrixTranslation(MyMatrix* m0, float tx, float ty, float tz){
	myVectorSet4(&m0->v[0], 1.0f, 0.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[1], 0.0f, 1.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[2], 0.0f, 0.0f, 1.0f, 0.0f);
	myVectorSet4(&m0->v[3], tx, ty, tz, 1.0f);
}
INLINE void myMatrixTranslationVector(MyMatrix* m0, MyVector const* v1){
	MyVector t0 = *v1;
	myVectorSetW(&t0, 1.0f);
	myVectorSet4(&m0->v[0], 1.0f, 0.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[1], 0.0f, 1.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[2], 0.0f, 0.0f, 1.0f, 0.0f);
	m0->v[3] = t0;
}
INLINE void myMatrixRotationX(MyMatrix* m0, float rx){
	float c = cosf(rx);
	float s = sinf(rx);
	myVectorSet4(&m0->v[0], 1.0f, 0.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[1], 0.0f,   +c,   +s, 0.0f);
	myVectorSet4(&m0->v[2], 0.0f,   -s,   +c, 0.0f);
	myVectorSet4(&m0->v[3], 0.0f, 0.0f, 0.0f, 1.0f);
}
INLINE void myMatrixRotationY(MyMatrix* m0, float ry){
	float c = cosf(ry);
	float s = sinf(ry);
	myVectorSet4(&m0->v[0],   +c, 0.0f,   -s, 0.0f);
	myVectorSet4(&m0->v[1], 0.0f, 1.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[2],   +s, 0.0f,   +c, 0.0f);
	myVectorSet4(&m0->v[3], 0.0f, 0.0f, 0.0f, 1.0f);
}
INLINE void myMatrixRotationZ(MyMatrix* m0, float rz){
	float c = cosf(rz);
	float s = sinf(rz);
	myVectorSet4(&m0->v[0],   +c,   +s, 0.0f, 0.0f);
	myVectorSet4(&m0->v[1],   -s,   +c, 0.0f, 0.0f);
	myVectorSet4(&m0->v[2], 0.0f, 0.0f, 1.0f, 0.0f);
	myVectorSet4(&m0->v[3], 0.0f, 0.0f, 0.0f, 1.0f);
}
INLINE void myMatrixRotationQuaternion(MyMatrix* m0, MyQuaternion const* q1){
	MyQuaternionEx qx1;
	qx1.q = *q1;
	{
		MyFloat xx2 = qx1.s.x * qx1.s.x * 2.0f;
		MyFloat yy2 = qx1.s.y * qx1.s.y * 2.0f;
		MyFloat zz2 = qx1.s.z * qx1.s.z * 2.0f;
		MyFloat xy2 = qx1.s.x * qx1.s.y * 2.0f;
		MyFloat xz2 = qx1.s.x * qx1.s.z * 2.0f;
		MyFloat xw2 = qx1.s.x * qx1.s.w * 2.0f;
		MyFloat yz2 = qx1.s.y * qx1.s.z * 2.0f;
		MyFloat yw2 = qx1.s.y * qx1.s.w * 2.0f;
		MyFloat zw2 = qx1.s.z * qx1.s.w * 2.0f;
		myVectorSet4(&m0->v[0], 1.0f - yy2 - zz2, xy2 + zw2, xz2 - yw2, 0.0f);
		myVectorSet4(&m0->v[1], xy2 - zw2, 1.0f - xx2 - zz2, yz2 + xw2, 0.0f);
		myVectorSet4(&m0->v[2], xz2 + yw2, yz2 - xw2, 1.0f - xx2 - yy2, 0.0f);
		myVectorSet4(&m0->v[3], 0.0f, 0.0f, 0.0f, 1.0f);
	}
}
INLINE void myMatrixPerspective(MyMatrix* m0, float fovy, float aspect, float near, float far){
	float f = 1.0f / tanf(fovy * 0.5f);
	float r = 1.0f / (near - far);
	myVectorSet4(&m0->v[0], f / aspect, 0.0f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[1], 0.0f, f, 0.0f, 0.0f);
	myVectorSet4(&m0->v[2], 0.0f, 0.0f, (near + far) * r, -1.0f);
	myVectorSet4(&m0->v[3], 0.0f, 0.0f, (near * far) * r * 2.0f, 0.0f);
}
INLINE void myMatrixMulVector(MyVector* v0, MyMatrix const* m1, MyVector const* v2){
#if defined(USE_SIMD)
	float32x4_t qt;
	float32x4_t qv2 = v2->q;
	qt = vmulq_lane_f32(m1->v[0].q, vget_low_f32(qv2), 0);
	qt = vmlaq_lane_f32(qt, m1->v[1].q, vget_low_f32(qv2), 1);
	qt = vmlaq_lane_f32(qt, m1->v[2].q, vget_high_f32(qv2), 0);
	//qt = vmlaq_lane_f32(qt, m1->v[3].q, vget_high_f32(qv2), 1);
	qt = vaddq_f32(qt, m1->v[3].q);
	v0->q = qt;
#else
	MyVectorEx vx0;
	MyMatrixEx mx1;
	MyVectorEx vx2;
	MyFloat x;
	MyFloat y;
	MyFloat z;
	//MyFloat w;
	mx1.m = *m1;
	vx2.v = *v2;
	x = vx2.s.x;
	y = vx2.s.y;
	z = vx2.s.z;
	//w = vx2.s.w;
	vx0.s.x = mx1.f[0][0] * x + mx1.f[1][0] * y + mx1.f[2][0] * z + mx1.f[3][0];// * w;
	vx0.s.y = mx1.f[0][1] * x + mx1.f[1][1] * y + mx1.f[2][1] * z + mx1.f[3][1];// * w;
	vx0.s.z = mx1.f[0][2] * x + mx1.f[1][2] * y + mx1.f[2][2] * z + mx1.f[3][2];// * w;
	vx0.s.w = mx1.f[0][3] * x + mx1.f[1][3] * y + mx1.f[2][3] * z + mx1.f[3][3];// * w;
	*v0 = vx0.v;
#endif
}
INLINE void myMatrixMulVector4(MyVector* v0, MyMatrix const* m1, MyVector const* v2){
#if defined(USE_SIMD)
	float32x4_t qt;
	float32x4_t qv2 = v2->q;
	qt = vmulq_lane_f32(m1->v[0].q, vget_low_f32(qv2), 0);
	qt = vmlaq_lane_f32(qt, m1->v[1].q, vget_low_f32(qv2), 1);
	qt = vmlaq_lane_f32(qt, m1->v[2].q, vget_high_f32(qv2), 0);
	qt = vmlaq_lane_f32(qt, m1->v[3].q, vget_high_f32(qv2), 1);
	v0->q = qt;
#else
	MyVectorEx vx0;
	MyMatrixEx mx1;
	MyVectorEx vx2;
	MyFloat x;
	MyFloat y;
	MyFloat z;
	MyFloat w;
	mx1.m = *m1;
	vx2.v = *v2;
	x = vx2.s.x;
	y = vx2.s.y;
	z = vx2.s.z;
	w = vx2.s.w;
	vx0.s.x = mx1.f[0][0] * x + mx1.f[1][0] * y + mx1.f[2][0] * z + mx1.f[3][0] * w;
	vx0.s.y = mx1.f[0][1] * x + mx1.f[1][1] * y + mx1.f[2][1] * z + mx1.f[3][1] * w;
	vx0.s.z = mx1.f[0][2] * x + mx1.f[1][2] * y + mx1.f[2][2] * z + mx1.f[3][2] * w;
	vx0.s.w = mx1.f[0][3] * x + mx1.f[1][3] * y + mx1.f[2][3] * z + mx1.f[3][3] * w;
	*v0 = vx0.v;
#endif
}
INLINE void myMatrixMul(MyMatrix* m0, MyMatrix const* m1, MyMatrix const* m2){
	MyMatrix mt;
	myMatrixMulVector4(&mt.v[0], m1, &m2->v[0]);
	myMatrixMulVector4(&mt.v[1], m1, &m2->v[1]);
	myMatrixMulVector4(&mt.v[2], m1, &m2->v[2]);
	myMatrixMulVector4(&mt.v[3], m1, &m2->v[3]);
	*m0 = mt;
}
INLINE void myMatrixMulScalar(MyMatrix* m0, MyMatrix const* m1, MyFloat s2){
	MyMatrix mt;
	myVectorMulScalar4(&mt.v[0], &m1->v[0], s2);
	myVectorMulScalar4(&mt.v[1], &m1->v[1], s2);
	myVectorMulScalar4(&mt.v[2], &m1->v[2], s2);
	myVectorMulScalar4(&mt.v[3], &m1->v[3], s2);
	*m0 = mt;
}
INLINE void myMatrixAdd(MyMatrix* m0, MyMatrix const* m1, MyMatrix const* m2){
	MyMatrix mt;
	myVectorAdd4(&mt.v[0], &m1->v[0], &m2->v[0]);
	myVectorAdd4(&mt.v[1], &m1->v[1], &m2->v[1]);
	myVectorAdd4(&mt.v[2], &m1->v[2], &m2->v[2]);
	myVectorAdd4(&mt.v[3], &m1->v[3], &m2->v[3]);
	*m0 = mt;
}
INLINE void myMatrixTranspose(MyMatrix* m0, MyMatrix const* m1){
	MyMatrixEx mx0;
	MyMatrixEx mx1;
	mx1.m = *m1;
	int i;
	int j;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			mx0.f[i][j] = mx1.f[j][i];
		}
	}
	*m0 = mx0.m;
}
INLINE void myMatrixInverse(MyMatrix* m0, MyMatrix const* m1){
	MyMatrixEx mx0;
	MyMatrixEx mx1;
	float det;
	mx1.m = *m1;
	mx0.f[0][0] =
		+ mx1.f[1][1] * mx1.f[2][2] * mx1.f[3][3]
		+ mx1.f[1][2] * mx1.f[2][3] * mx1.f[3][1]
		+ mx1.f[1][3] * mx1.f[2][1] * mx1.f[3][2]
		- mx1.f[1][1] * mx1.f[2][3] * mx1.f[3][2]
		- mx1.f[1][2] * mx1.f[2][1] * mx1.f[3][3]
		- mx1.f[1][3] * mx1.f[2][2] * mx1.f[3][1];
	mx0.f[0][1] =
		- mx1.f[2][1] * mx1.f[3][2] * mx1.f[0][3]
		- mx1.f[2][2] * mx1.f[3][3] * mx1.f[0][1]
		- mx1.f[2][3] * mx1.f[3][1] * mx1.f[0][2]
		+ mx1.f[2][1] * mx1.f[3][3] * mx1.f[0][2]
		+ mx1.f[2][2] * mx1.f[3][1] * mx1.f[0][3]
		+ mx1.f[2][3] * mx1.f[3][2] * mx1.f[0][1];
	mx0.f[0][2] =
		+ mx1.f[3][1] * mx1.f[0][2] * mx1.f[1][3]
		+ mx1.f[3][2] * mx1.f[0][3] * mx1.f[1][1]
		+ mx1.f[3][3] * mx1.f[0][1] * mx1.f[1][2]
		- mx1.f[3][1] * mx1.f[0][3] * mx1.f[1][2]
		- mx1.f[3][2] * mx1.f[0][1] * mx1.f[1][3]
		- mx1.f[3][3] * mx1.f[0][2] * mx1.f[1][1];
	mx0.f[0][3] =
		- mx1.f[0][1] * mx1.f[1][2] * mx1.f[2][3]
		- mx1.f[0][2] * mx1.f[1][3] * mx1.f[2][1]
		- mx1.f[0][3] * mx1.f[1][1] * mx1.f[2][2]
		+ mx1.f[0][1] * mx1.f[1][3] * mx1.f[2][2]
		+ mx1.f[0][2] * mx1.f[1][1] * mx1.f[2][3]
		+ mx1.f[0][3] * mx1.f[1][2] * mx1.f[2][1];
	det =
		+ mx0.f[0][0] * mx1.f[0][0]
		+ mx0.f[0][1] * mx1.f[1][0]
		+ mx0.f[0][2] * mx1.f[2][0]
		+ mx0.f[0][3] * mx1.f[3][0];
	if(fabs(det) < FLT_EPSILON){
		myMatrixIdentity(m0);
		return;
	}
	mx0.f[1][0] =
		- mx1.f[1][2] * mx1.f[2][3] * mx1.f[3][0]
		- mx1.f[1][3] * mx1.f[2][0] * mx1.f[3][2]
		- mx1.f[1][0] * mx1.f[2][2] * mx1.f[3][3]
		+ mx1.f[1][2] * mx1.f[2][0] * mx1.f[3][3]
		+ mx1.f[1][3] * mx1.f[2][2] * mx1.f[3][0]
		+ mx1.f[1][0] * mx1.f[2][3] * mx1.f[3][2];
	mx0.f[1][1] =
		+ mx1.f[2][2] * mx1.f[3][3] * mx1.f[0][0]
		+ mx1.f[2][3] * mx1.f[3][0] * mx1.f[0][2]
		+ mx1.f[2][0] * mx1.f[3][2] * mx1.f[0][3]
		- mx1.f[2][2] * mx1.f[3][0] * mx1.f[0][3]
		- mx1.f[2][3] * mx1.f[3][2] * mx1.f[0][0]
		- mx1.f[2][0] * mx1.f[3][3] * mx1.f[0][2];
	mx0.f[1][2] =
		- mx1.f[3][2] * mx1.f[0][3] * mx1.f[1][0]
		- mx1.f[3][3] * mx1.f[0][0] * mx1.f[1][2]
		- mx1.f[3][0] * mx1.f[0][2] * mx1.f[1][3]
		+ mx1.f[3][2] * mx1.f[0][0] * mx1.f[1][3]
		+ mx1.f[3][3] * mx1.f[0][2] * mx1.f[1][0]
		+ mx1.f[3][0] * mx1.f[0][3] * mx1.f[1][2];
	mx0.f[1][3] =
		+ mx1.f[0][2] * mx1.f[1][3] * mx1.f[2][0]
		+ mx1.f[0][3] * mx1.f[1][0] * mx1.f[2][2]
		+ mx1.f[0][0] * mx1.f[1][2] * mx1.f[2][3]
		- mx1.f[0][2] * mx1.f[1][0] * mx1.f[2][3]
		- mx1.f[0][3] * mx1.f[1][2] * mx1.f[2][0]
		- mx1.f[0][0] * mx1.f[1][3] * mx1.f[2][2];
	mx0.f[2][0] =
		+ mx1.f[1][3] * mx1.f[2][0] * mx1.f[3][1]
		+ mx1.f[1][0] * mx1.f[2][1] * mx1.f[3][3]
		+ mx1.f[1][1] * mx1.f[2][3] * mx1.f[3][0]
		- mx1.f[1][3] * mx1.f[2][1] * mx1.f[3][0]
		- mx1.f[1][0] * mx1.f[2][3] * mx1.f[3][1]
		- mx1.f[1][1] * mx1.f[2][0] * mx1.f[3][3];
	mx0.f[2][1] =
		- mx1.f[2][3] * mx1.f[3][0] * mx1.f[0][1]
		- mx1.f[2][0] * mx1.f[3][1] * mx1.f[0][3]
		- mx1.f[2][1] * mx1.f[3][3] * mx1.f[0][0]
		+ mx1.f[2][3] * mx1.f[3][1] * mx1.f[0][0]
		+ mx1.f[2][0] * mx1.f[3][3] * mx1.f[0][1]
		+ mx1.f[2][1] * mx1.f[3][0] * mx1.f[0][3];
	mx0.f[2][2] =
		+ mx1.f[3][3] * mx1.f[0][0] * mx1.f[1][1]
		+ mx1.f[3][0] * mx1.f[0][1] * mx1.f[1][3]
		+ mx1.f[3][1] * mx1.f[0][3] * mx1.f[1][0]
		- mx1.f[3][3] * mx1.f[0][1] * mx1.f[1][0]
		- mx1.f[3][0] * mx1.f[0][3] * mx1.f[1][1]
		- mx1.f[3][1] * mx1.f[0][0] * mx1.f[1][3];
	mx0.f[2][3] =
		- mx1.f[0][3] * mx1.f[1][0] * mx1.f[2][1]
		- mx1.f[0][0] * mx1.f[1][1] * mx1.f[2][3]
		- mx1.f[0][1] * mx1.f[1][3] * mx1.f[2][0]
		+ mx1.f[0][3] * mx1.f[1][1] * mx1.f[2][0]
		+ mx1.f[0][0] * mx1.f[1][3] * mx1.f[2][1]
		+ mx1.f[0][1] * mx1.f[1][0] * mx1.f[2][3];
	mx0.f[3][0] =
		- mx1.f[1][0] * mx1.f[2][1] * mx1.f[3][2]
		- mx1.f[1][1] * mx1.f[2][2] * mx1.f[3][0]
		- mx1.f[1][2] * mx1.f[2][0] * mx1.f[3][1]
		+ mx1.f[1][0] * mx1.f[2][2] * mx1.f[3][1]
		+ mx1.f[1][1] * mx1.f[2][0] * mx1.f[3][2]
		+ mx1.f[1][2] * mx1.f[2][1] * mx1.f[3][0];
	mx0.f[3][1] =
		+ mx1.f[2][0] * mx1.f[3][1] * mx1.f[0][2]
		+ mx1.f[2][1] * mx1.f[3][2] * mx1.f[0][0]
		+ mx1.f[2][2] * mx1.f[3][0] * mx1.f[0][1]
		- mx1.f[2][0] * mx1.f[3][2] * mx1.f[0][1]
		- mx1.f[2][1] * mx1.f[3][0] * mx1.f[0][2]
		- mx1.f[2][2] * mx1.f[3][1] * mx1.f[0][0];
	mx0.f[3][2] =
		- mx1.f[3][0] * mx1.f[0][1] * mx1.f[1][2]
		- mx1.f[3][1] * mx1.f[0][2] * mx1.f[1][0]
		- mx1.f[3][2] * mx1.f[0][0] * mx1.f[1][1]
		+ mx1.f[3][0] * mx1.f[0][2] * mx1.f[1][1]
		+ mx1.f[3][1] * mx1.f[0][0] * mx1.f[1][2]
		+ mx1.f[3][2] * mx1.f[0][1] * mx1.f[1][0];
	mx0.f[3][3] =
		+ mx1.f[0][0] * mx1.f[1][1] * mx1.f[2][2]
		+ mx1.f[0][1] * mx1.f[1][2] * mx1.f[2][0]
		+ mx1.f[0][2] * mx1.f[1][0] * mx1.f[2][1]
		- mx1.f[0][0] * mx1.f[1][2] * mx1.f[2][1]
		- mx1.f[0][1] * mx1.f[1][0] * mx1.f[2][2]
		- mx1.f[0][2] * mx1.f[1][1] * mx1.f[2][0];
	myMatrixMulScalar(m0, &mx0.m, 1.0f / det);
}

// vr:t,u,v
INLINE SInt myTriangleIntersection(MyVector* vr, MyVector const* org, MyVector const* dir, MyVector const* t0, MyVector const* t1, MyVector const* t2){
	MyVector e1;
	MyVector e2;
	MyVector pvec;
	MyVector tvec;
	MyVector qvec;
	MyFloat idet;
	MyFloat det;
	MyFloat t;
	MyFloat u;
	MyFloat v;

	myVectorSub(&e1, t1, t0);
	myVectorSub(&e2, t2, t0);
	myVectorCross(&pvec, dir, &e2);
	det = myVectorDot(&pvec, &e1);
	if((det < +1.0E-06) && (det > -1.0E-06)){
		return 0;
	}
	idet = 1.0f / det;
	myVectorSub(&tvec, org, t0);
	myVectorCross(&qvec, &tvec, &e1);
	u = myVectorDot(&tvec, &pvec) * idet;
	v = myVectorDot(&qvec, dir) * idet;
	t = myVectorDot(&qvec, &e2) * idet;
	if(u < 0.0f){
		return 0;
	}
	if(v < 0.0){
		return 0;
	}
	if((u + v) > 1.0f){
		return 0;
	}
	if(t < 0.0){
		return 0;
	}
	myVectorSet(vr, t, u, v);
	return (det < 0.0f) ? -1 : +1;
}

#if defined(__cplusplus)
} //extern "C"{
#endif


#endif /* MYVECTOR_H */
