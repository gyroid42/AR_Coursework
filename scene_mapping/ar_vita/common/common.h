/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2011 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef COMMON_H
#define COMMON_H

//#define ENABLE_DBGPRINTF

//#define ENABLE_QUIT

#define USE_TINY_MUTEX

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <scetypes.h>
#include <sceconst.h>
#include <kernel.h>
#include <libdbg.h>

#include <sce_atomic.h>


#define MY_LIBC_HEAP_SIZE				UINTPTR_C(128 * 1024 * 1024)


#if defined(__SNC__)
#	pragma diag_suppress=1788	// padding
#endif

#if defined(ENABLE_DBGPRINTF)
#	define ABORT()					(void)(myDebugPrintf(__FUNCTION__, __LINE__, "abort\n"), myDebugAbort())
#	define BREAK()					(void)(myDebugPrintf(__FUNCTION__, __LINE__, "break\n"), myDebugBreak())
#	define DEBUG_PRINTF(a, ...)		myDebugPrintf(__FUNCTION__, __LINE__, (a), ##__VA_ARGS__)
#	define ASSERT(a)				{ (a) ? (void)0 : (void)(myDebugPrintf(__FUNCTION__, __LINE__, "assertion failed: " #a "\n"), myDebugAbort()); }
#	define ASSERT_MSG(a, frm, ...)	{ (a) ? (void)0 : (void)(myDebugPrintf(__FUNCTION__, __LINE__, frm, ##__VA_ARGS__), myDebugAbort()); }
#elif defined(_DEBUG)
#	define ABORT()					SCE_STOP()
#	define BREAK()					SCE_BREAK()
#	define DEBUG_PRINTF				SCE_DBG_LOG_INFO
#	define ASSERT					SCE_DBG_ASSERT
#	define ASSERT_MSG				SCE_DBG_ASSERT_MSG
#else
#	define ABORT()					myDebugAbort()
#	define BREAK()
#	define DEBUG_PRINTF(a, ...)
#	define ASSERT(a)
#	define ASSERT_MSG(a, ...)
#endif

#define SCECHK(a)		{ int sceres = (a); (void)sceres; ASSERT_MSG(sceres >= SCE_OK, "SceError: 0x%08x: " #a "\n", sceres); }
#define SCEWRN(a)		{ int sceres = (a); if(sceres < SCE_OK){ DEBUG_PRINTF("SceError: 0x%08x: " #a "\n", sceres); } }


#if defined(__GNUC__) || defined(__SNC__)
#	define DECLARE_ALIGNED(a, s)	a __attribute__((aligned(s)))
#	define ALWAYS_INLINE			__attribute__((always_inline))
#	define INLINE					extern inline
#endif

#define UNUSED(a)		(void)(a)
#define TOUCH(a)		(void)(sizeof(a))
#define ALIGNED(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))

#define INFINITE		(-1)

#define bulkof(a)		(sizeof(a) / sizeof(a[0]))
#define alignof(t)		offsetof(struct{ char c; t x; }, x)


#if defined(__cplusplus)
#	define REINTERPRET_CAST(a, b)	reinterpret_cast<a>(b)
#else
#	define REINTERPRET_CAST(a, b)	((a)(void*)(b))
#endif

#if defined(__cplusplus)
extern "C"{
#endif

int myDebugPrintf(char const* function, int line, char const* format, ...);
void myDebugBreak(void);
void myDebugAbort(void);
void myDebugStamp(char const* str, ...);


typedef struct{
	uint32_t counts;
	uint32_t units;
	size_t used;
	size_t max;
}MyMemInfo;
int myMemGetInfo(MyMemInfo* info);

int myMemThreadLocalHeapInit(size_t size);
int myMemThreadLocalHeapRelease(void);

void myMemCountStart(void);
void myMemCountStop(void);

typedef enum{
	FALSE = 0,
	TRUE
}Bool;


typedef uint_fast32_t	UInt;
typedef int_fast32_t	SInt;
typedef uintptr_t		UIntPtr;
typedef intptr_t		SIntPtr;

typedef uint64_t		UInt64;
typedef uint32_t		UInt32;
typedef uint16_t		UInt16;
typedef uint8_t			UInt8;
typedef int64_t			SInt64;
typedef int32_t			SInt32;
typedef int16_t			SInt16;
typedef int8_t			SInt8;

#define INTPTR_C(x)		(x)
#define UINTPTR_C(x)	(x ## U)

typedef volatile int32_t	DECLARE_ALIGNED(MyAtomic32, 4);

typedef SceUID				MyEvent;

typedef SceUID				MyEventFlag;

typedef SceUID				MySemaphore;

typedef struct{
#if defined(USE_TINY_MUTEX)
	SceUID evt;
	SceUInt32 pad;
	MyAtomic32 lock;
	MyAtomic32 ref;
#else
	SceInt32 res;
	SceKernelLwMutexWork lwMutex;
#endif
}MyMutex;

typedef SceUID				MyThread;
typedef SceInt32			MyThreadPrio;
typedef UIntPtr				MyThreadArg;
typedef UIntPtr				MyThreadVal;
typedef SceInt32 (*MyThreadProc)(SceSize argSize, void* arg);

#define DECLARE_THREAD_PROC(func)		SceInt32 func(SceSize argSize, void* argPtr)
#define UNUSED_THREAD_ARG()				(UNUSED(argSize), UNUSED(argPtr))
#define THREAD_GET_ARG()				(UNUSED(argSize), *(MyThreadArg*)(argPtr))
#define THREAD_RETURN(a)				{ SCECHK(sceKernelExitThread((SceInt32)a)); return 0; }

#define THREAD_PRIORITY_TIME_CRITICAL	(SCE_KERNEL_HIGHEST_PRIORITY_USER)
#define THREAD_PRIORITY_HIGHEST			(SCE_KERNEL_DEFAULT_PRIORITY_USER - 20)
#define THREAD_PRIORITY_ABOVE_NORMAL	(SCE_KERNEL_DEFAULT_PRIORITY_USER - 10)
#define THREAD_PRIORITY_NORMAL			(SCE_KERNEL_DEFAULT_PRIORITY_USER)
#define THREAD_PRIORITY_BELOW_NORMAL	(SCE_KERNEL_DEFAULT_PRIORITY_USER + 10)
#define THREAD_PRIORITY_LOWEST			(SCE_KERNEL_DEFAULT_PRIORITY_USER + 20)
#define THREAD_PRIORITY_IDLE			(SCE_KERNEL_LOWEST_PRIORITY_USER)

#define THREAD_DEFAULT_STACK_SIZE		(SCE_KERNEL_STACK_SIZE_MIN)
#define THREAD_DEFAULT_PRIORITY			(THREAD_PRIORITY_NORMAL)

#define MY_EVENT_FLAG_WAITMODE_AND		(SCE_KERNEL_EVF_WAITMODE_AND)
#define MY_EVENT_FLAG_WAITMODE_OR		(SCE_KERNEL_EVF_WAITMODE_OR)

INLINE void sleep(SInt msec) ALWAYS_INLINE;
INLINE void yield(void) ALWAYS_INLINE;
INLINE UInt32 system_time(void) ALWAYS_INLINE;
INLINE UInt64 system_time_l(void) ALWAYS_INLINE;

INLINE void myAtomic32Inc(MyAtomic32* p) ALWAYS_INLINE;
INLINE void myAtomic32Dec(MyAtomic32* p) ALWAYS_INLINE;
INLINE MyAtomic32 myAtomic32Swap(MyAtomic32* p, MyAtomic32 v) ALWAYS_INLINE;
INLINE MyAtomic32 myAtomic32Get(MyAtomic32 const* p) ALWAYS_INLINE;
INLINE MyAtomic32 myAtomic32CompareAndSwap(MyAtomic32* p, MyAtomic32 c, MyAtomic32 v) ALWAYS_INLINE;

INLINE int myEventCreate(MyEvent* evt) ALWAYS_INLINE;
INLINE void myEventRelease(MyEvent* evt) ALWAYS_INLINE;
INLINE void myEventRaise(MyEvent* evt) ALWAYS_INLINE;
INLINE void myEventClear(MyEvent* evt) ALWAYS_INLINE;
INLINE Bool myEventWait(MyEvent* evt, SInt msec) ALWAYS_INLINE;
INLINE Bool myEventIsValid(MyEvent* evt) ALWAYS_INLINE;

INLINE int myEventFlagCreate(MyEventFlag* flg) ALWAYS_INLINE;
INLINE void myEventFlagRelease(MyEventFlag* flg) ALWAYS_INLINE;
INLINE void myEventFlagSet(MyEventFlag* flg, UInt32 pattern) ALWAYS_INLINE;
INLINE void myEventFlagClear(MyEventFlag* flg, UInt32 pattern) ALWAYS_INLINE;
INLINE Bool myEventFlagWait(MyEventFlag* flg, UInt32 pattern, UInt32 mode, SInt msec) ALWAYS_INLINE;
INLINE Bool myEventFlagIsValid(MyEventFlag* flg) ALWAYS_INLINE;

INLINE int myMutexCreate(MyMutex* mtx) ALWAYS_INLINE;
INLINE void myMutexRelease(MyMutex* mtx) ALWAYS_INLINE;
INLINE void myMutexLock(MyMutex* mtx) ALWAYS_INLINE;
INLINE void myMutexUnlock(MyMutex* mtx) ALWAYS_INLINE;
INLINE Bool myMutexTryLock(MyMutex* mtx, SInt msec) ALWAYS_INLINE;
INLINE Bool myMutexIsValid(MyMutex* mtx) ALWAYS_INLINE;

INLINE int mySemaphoreCreate(MySemaphore* smp, UInt init, UInt max) ALWAYS_INLINE;
INLINE void mySemaphoreRelease(MySemaphore* smp) ALWAYS_INLINE;
INLINE void mySemaphoreSignal(MySemaphore* smp, UInt count) ALWAYS_INLINE;
INLINE Bool mySemaphoreWait(MySemaphore* smp, SInt msec) ALWAYS_INLINE;
INLINE Bool mySemaphoreIsValid(MySemaphore* smp) ALWAYS_INLINE;

INLINE void myThreadSetPriority(MyThreadPrio prio) ALWAYS_INLINE;
INLINE MyThreadPrio myThreadGetPriority(void) ALWAYS_INLINE;
INLINE int myThreadStart(MyThread* th, char const* tag, MyThreadProc func, MyThreadArg arg) ALWAYS_INLINE;
INLINE int myThreadStartEx(MyThread* th, char const* tag, MyThreadProc func, MyThreadArg arg, MyThreadPrio prio, size_t stack) ALWAYS_INLINE;
INLINE MyThreadVal myThreadJoin(MyThread* th) ALWAYS_INLINE;
INLINE Bool myThreadIsValid(MyThread* th) ALWAYS_INLINE;

INLINE void sleep(SInt msec){
	if(msec <= 0){
		return;
	}
	SCECHK(sceKernelDelayThread((SceUInt32)msec));
}

INLINE void yield(void){
	sleep(1);
}

INLINE UInt32 system_time(void){
	return sceKernelGetProcessTimeLow();
}

INLINE UInt64 system_time_l(void){
	return sceKernelGetProcessTimeWide();
}

INLINE void myAtomic32Inc(MyAtomic32* p){
	__builtin_dmb();
	sceAtomicIncrement32((volatile int32_t*)p);
	__builtin_dmb();
}

INLINE void myAtomic32Dec(MyAtomic32* p){
	__builtin_dmb();
	sceAtomicDecrement32((volatile int32_t*)p);
	__builtin_dmb();
}

INLINE MyAtomic32 myAtomic32Swap(MyAtomic32* p, MyAtomic32 v){
	MyAtomic32 val;
	__builtin_dmb();
	val = (MyAtomic32)sceAtomicExchange32((volatile int32_t*)p, (int32_t)v);
	__builtin_dmb();
	return val;
}

INLINE MyAtomic32 myAtomic32Get(MyAtomic32 const* p){
	MyAtomic32 val = *p;
	__builtin_dmb();
	return val;
}

INLINE MyAtomic32 myAtomic32CompareAndSwap(MyAtomic32* p, MyAtomic32 c, MyAtomic32 v){
	MyAtomic32 val;
	__builtin_dmb();
	val = (MyAtomic32)sceAtomicCompareAndSwap32((volatile int32_t*)p, (int32_t)c, (int32_t)v);
	__builtin_dmb();
	return val;
}

INLINE int myEventCreate(MyEvent* evt){
	SceInt32 res;
	SCECHK(res = sceKernelCreateEventFlag(__FUNCTION__, SCE_KERNEL_EVF_ATTR_TH_FIFO | SCE_KERNEL_EVF_ATTR_MULTI, 0, NULL));
	*evt = res;
	return (int)res;
}
INLINE void myEventRelease(MyEvent* evt){
	SCECHK(sceKernelDeleteEventFlag(*evt));
	*evt = -1;
}

INLINE void myEventRaise(MyEvent* evt){
	SCECHK(sceKernelSetEventFlag(*evt, 1));
}

INLINE void myEventClear(MyEvent* evt){
	SCECHK(sceKernelClearEventFlag(*evt, 0));
}

INLINE Bool myEventWait(MyEvent* evt, SInt msec){
	SceInt32 res;
	if(msec == 0){
		res = sceKernelPollEventFlag(*evt, 1, SCE_KERNEL_EVF_WAITMODE_OR | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL);
		ASSERT((res == SCE_OK) || (res == SCE_KERNEL_ERROR_EVF_COND));
	}else if(msec < 0){
		res = sceKernelWaitEventFlag(*evt, 1, SCE_KERNEL_EVF_WAITMODE_OR | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL, NULL);
		ASSERT(res == SCE_OK);
	}else{
		SceUInt32 timeout;
		timeout = (SceUInt32)msec * 1000;
		res = sceKernelWaitEventFlag(*evt, 1, SCE_KERNEL_EVF_WAITMODE_OR | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL, &timeout);
		ASSERT((res == SCE_OK) || (res == SCE_KERNEL_ERROR_WAIT_TIMEOUT));
	}
	return (res == SCE_OK) ? TRUE : FALSE;
}

INLINE Bool myEventIsValid(MyEvent* evt){
	return (*evt >= SCE_OK) ? TRUE : FALSE;
}

INLINE int myEventFlagCreate(MyEventFlag* flg){
	SceInt32 res;
	SCECHK(res = sceKernelCreateEventFlag(__FUNCTION__, SCE_KERNEL_EVF_ATTR_TH_FIFO | SCE_KERNEL_EVF_ATTR_MULTI, 0, NULL));
	*flg = res;
	return (int)res;
}
INLINE void myEventFlagRelease(MyEventFlag* flg){
	SCECHK(sceKernelDeleteEventFlag(*flg));
	*flg = -1;
}
INLINE void myEventFlagSet(MyEventFlag* flg, UInt32 pattern){
	SCECHK(sceKernelSetEventFlag(*flg, pattern));
}
INLINE void myEventFlagClear(MyEventFlag* flg, UInt32 pattern){
	SCECHK(sceKernelClearEventFlag(*flg, pattern));
}
INLINE Bool myEventFlagWait(MyEventFlag* flg, UInt32 pattern, UInt32 waitmode, SInt msec){
	SceInt32 res;
	if(msec == 0){
		res = sceKernelPollEventFlag(*flg, pattern, waitmode | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL);
		ASSERT((res == SCE_OK) || (res == SCE_KERNEL_ERROR_EVF_COND));
	}else if(msec < 0){
		res = sceKernelWaitEventFlag(*flg, pattern, waitmode | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL, NULL);
		ASSERT((res == SCE_OK));
	}else{
		SceUInt32 timeout;
		timeout = (SceUInt32)msec * 1000;
		res = sceKernelWaitEventFlag(*flg, pattern, waitmode | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL, &timeout);
	ASSERT((res == SCE_OK) || (res == SCE_KERNEL_ERROR_WAIT_TIMEOUT));
	}
	return (res == SCE_OK) ? TRUE : FALSE;
}

INLINE Bool myEventFlagIsValid(MyEventFlag* flg){
	return (*flg >= SCE_OK) ? TRUE : FALSE;
}

INLINE int myMutexCreate(MyMutex* mtx){
#if defined(USE_TINY_MUTEX)
	SceInt32 res;
	SCECHK(res = sceKernelCreateEventFlag(__FUNCTION__, SCE_KERNEL_EVF_ATTR_TH_FIFO | SCE_KERNEL_EVF_ATTR_MULTI, 0, NULL));
	mtx->evt = (SceUID)res;
	mtx->lock = 0;
	mtx->ref = 0;
	return res;
#else
	SceInt32 res;
	SCECHK(res = sceKernelCreateLwMutex(&mtx->lwMutex, __FUNCTION__, SCE_KERNEL_LW_MUTEX_ATTR_TH_FIFO, 0, NULL));
	mtx->res = res;
	return res;
#endif
}

INLINE void myMutexRelease(MyMutex* mtx){
#if defined(USE_TINY_MUTEX)
	ASSERT(mtx->lock == 0);
	ASSERT(mtx->ref == 0);
	ASSERT(mtx->evt >= 0);
	SCECHK(sceKernelDeleteEventFlag(mtx->evt));
	mtx->evt = -1;
#else
	ASSERT(mtx->res >= 0);
	SCECHK(sceKernelDeleteLwMutex(&mtx->lwMutex));
	mtx->res = -1;
#endif
}

INLINE void myMutexLock(MyMutex* mtx){
#if defined(USE_TINY_MUTEX)
	MyAtomic32 lock = myAtomic32Swap(&mtx->lock, 1);
	if(lock != 0){
		myAtomic32Inc(&mtx->ref);
		for(;;){
			lock = myAtomic32Swap(&mtx->lock, 1);
			if(lock == 0){
				break;
			}
			SCECHK(sceKernelWaitEventFlag(mtx->evt, 1, SCE_KERNEL_EVF_WAITMODE_OR | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL, NULL));
		}
		myAtomic32Dec(&mtx->ref);
	}
#else
	SCECHK(sceKernelLockLwMutex(&mtx->lwMutex, 1, NULL));
#endif
}

INLINE void myMutexUnlock(MyMutex* mtx){
#if defined(USE_TINY_MUTEX)
	myAtomic32Swap(&mtx->lock, 0);
	if(myAtomic32Get(&mtx->ref) != 0){
		SCECHK(sceKernelSetEventFlag(mtx->evt, 1));
	}
#else
	SCECHK(sceKernelUnlockLwMutex(&mtx->lwMutex, 1));
#endif
}

INLINE Bool myMutexTryLock(MyMutex* mtx, SInt msec){
#if defined(USE_TINY_MUTEX)
	myAtomic32Inc(&mtx->ref);
	for(;;){
		SceInt32 res;
		MyAtomic32 lock = myAtomic32Swap(&mtx->lock, 1);
		if(lock == 0){
			break;
		}
		if(msec == 0){
			myAtomic32Dec(&mtx->ref);
			return FALSE;
		}else if(msec < 0){
			res = sceKernelWaitEventFlag(mtx->evt, 1, SCE_KERNEL_EVF_WAITMODE_OR | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL, NULL);
		}else{
			SceUInt32 timeout = (SceUInt32)msec * 1000;
			res = sceKernelPollEventFlag(mtx->evt, 1, SCE_KERNEL_EVF_WAITMODE_OR | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL);
			ASSERT((res == SCE_OK) || (res == SCE_KERNEL_ERROR_WAIT_TIMEOUT));
			res = sceKernelWaitEventFlag(mtx->evt, 1, SCE_KERNEL_EVF_WAITMODE_OR | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, NULL, &timeout);
			if(res == SCE_KERNEL_ERROR_WAIT_TIMEOUT){
				myAtomic32Dec(&mtx->ref);
				return FALSE;
			}
		}
		ASSERT(res == SCE_OK);
	}
	return TRUE;
#else
	SceInt32 res;
	if(msec == 0){
		res = sceKernelTryLockLwMutex(&mtx->lwMutex, 1);
		ASSERT((res == SCE_OK) || (res == SCE_KERNEL_ERROR_LW_MUTEX_FAILED_TO_OWN));
	}else if(msec < 0){
		res = sceKernelLockLwMutex(&mtx->lwMutex, 1, NULL);
		ASSERT((res == SCE_OK));
	}else{
		SceUInt32 timeout;
		timeout = (SceUInt32)msec * 1000;
		res = sceKernelLockLwMutex(&mtx->lwMutex, 1, &timeout);
		ASSERT((res == SCE_OK) || (res == SCE_KERNEL_ERROR_WAIT_TIMEOUT));
	}
	return (res == SCE_OK) ? TRUE : FALSE;
#endif
}

INLINE Bool myMutexIsValid(MyMutex* mtx){
#if defined(USE_TINY_MUTEX)
	return (mtx->evt >= SCE_OK) ? TRUE : FALSE;
#else
	return (mtx->res >= SCE_OK) ? TRUE : FALSE;
#endif
}

INLINE int mySemaphoreCreate(MySemaphore* smp, UInt init, UInt max){
	SceInt32 res;
	SCECHK(res = sceKernelCreateSema(__FUNCTION__, SCE_KERNEL_SEMA_ATTR_TH_FIFO, (int)init, (int)max, NULL));
	*smp = res;
	return (int)res;
}

INLINE void mySemaphoreRelease(MySemaphore* smp){
	SCECHK(sceKernelDeleteSema(*smp));
	*smp = -1;
}

INLINE void mySemaphoreSignal(MySemaphore* smp, UInt count){
	SCECHK(sceKernelSignalSema(*smp, (int)count));
}

INLINE Bool mySemaphoreWait(MySemaphore* smp, SInt msec){
	int res;
	int count = 1;
	if(msec == 0){
		res = sceKernelPollSema(*smp, count);
	}else if(msec < 0){
		res = sceKernelWaitSema(*smp, count, NULL);
	}else{
		SceUInt32 timeout = msec * 1000;
		res = sceKernelWaitSema(*smp, count, &timeout);
	}
	ASSERT((res == SCE_OK) || (res == SCE_KERNEL_ERROR_WAIT_TIMEOUT) || (res == SCE_KERNEL_ERROR_SEMA_ZERO));
	return (res == SCE_OK) ? TRUE : FALSE;
}

INLINE Bool mySemaphoreIsValid(MySemaphore* smp){
	return (*smp >= SCE_OK) ? TRUE : FALSE;
}

INLINE void myThreadSetPriority(MyThreadPrio prio){
	SCECHK(sceKernelChangeThreadPriority(SCE_KERNEL_THREAD_ID_SELF, prio));
}

INLINE MyThreadPrio myThreadGetPriority(void){
	MyThreadPrio prio;
	prio = sceKernelGetThreadCurrentPriority();
	SCECHK(prio);
	return prio;
}

INLINE int myThreadStart(MyThread* th, char const* tag, MyThreadProc func, MyThreadArg arg){
	SceUID uID = sceKernelCreateThread(tag, func, THREAD_DEFAULT_PRIORITY, THREAD_DEFAULT_STACK_SIZE, 0, SCE_KERNEL_THREAD_CPU_AFFINITY_MASK_DEFAULT, NULL);
	ASSERT(uID >= SCE_OK);
	if(uID < SCE_OK){
		return (int)uID;
	}
	SCECHK(sceKernelStartThread(uID, sizeof(arg), &arg));
	*th = uID;
	return SCE_OK;
}

INLINE int myThreadStartEx(MyThread* th, char const* tag, MyThreadProc func, MyThreadArg arg, MyThreadPrio prio, size_t stack){
	SceUID uID = sceKernelCreateThread(tag, func, prio, stack, 0, SCE_KERNEL_THREAD_CPU_AFFINITY_MASK_DEFAULT, NULL);
	ASSERT(uID >= SCE_OK);
	if(uID < SCE_OK){
		return (int)uID;
	}
	SCECHK(sceKernelStartThread(uID, sizeof(arg), &arg));
	*th = uID;
	return SCE_OK;
}

INLINE MyThreadVal myThreadJoin(MyThread* th){
	SceUID uID = *th;
	SceInt32 val;
	SCECHK(sceKernelWaitThreadEnd(uID, &val, NULL));
	SCECHK(sceKernelDeleteThread(uID));
	*th = -1;
	return (MyThreadVal)val;
}

INLINE Bool myThreadIsValid(MyThread* th){
	return (*th >= SCE_OK) ? TRUE : FALSE;
}

#if defined(__cplusplus)
} //extern "C"{
#endif


#endif // COMMON_H
