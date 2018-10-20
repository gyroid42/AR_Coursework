/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#define ENABLE_REPLACE_MALLOC


#if defined(ENABLE_REPLACE_MALLOC)

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>
#include <libdbg.h>

#include <kernel.h>
#include <kernel/backtrace.h>

//#define ENABLE_DBGPRINTF
#include "common.h"

#define USE_MYSPACE

#if defined(USE_MYSPACE)
#	include "myspace.h"
#else
#	include <mspace.h>
#endif

//#define ENABLE_MEMCHK

#if defined(_DEBUG) && !defined(ENABLE_MEMCHK)
#	define ENABLE_MEMCHK
#endif


#if defined(ENABLE_MEMCHK)
#define ENBALE_LOGCHK

//#define ENBALE_HEAVYCHK

//#define DISABLE_FILLING

//#define ENBALE_COUNTING
#endif

extern "C" size_t sceLibcHeapSize				= 0;

#define MY_HEAP_MAX				(2)

#define LOG_DEPTH				(128 * 1024)

typedef struct{
	MyMutex mtx;

	struct{
#if defined(ENABLE_MEMCHK)
		size_t used;
		size_t max;
		MyAtomic32 counts;
		uint32_t units;
#if defined(ENBALE_COUNTING)
		MyAtomic32 countx[32];
#endif
#endif
#if defined(USE_MYSPACE)
		MySpaceState mss;
		MySpaceState* msp;
#else
		mspace msp;
#endif
		SceUID volatile tid;
		SceUID uid;
		size_t len;
		void* top;
		void* end;
#if defined(ENABLE_MEMCHK) && defined(ENBALE_LOGCHK)
		uint32_t logcnt;
		uint32_t logmod;
#endif
	}heap[MY_HEAP_MAX];

#if defined(ENABLE_MEMCHK) && defined(ENBALE_LOGCHK)
	uint32_t logpos;
	void* logptr[LOG_DEPTH];
#endif

}MyMemState;

static MyMemState s_myMemState = { 0 };


#if defined(ENABLE_MEMCHK)

void _myMemValidate(void* ptr);

#if defined(ENBALE_LOGCHK)
uintptr_t _myMemLogAlloc(UInt idx, void* ptr){
	MyMemState* const s = &s_myMemState;
	uint32_t pos = 0;
	if(s->heap[idx].logmod == 0){
		return LOG_DEPTH;
	}
#if defined(ENBALE_HEAVYCHK)
	for(int i = 0; i < LOG_DEPTH; i++){
		if(s->logptr[i] != NULL){
			_myMemValidate(s->logptr[i]);
		}
	}
#endif
	pos = s->logpos;
	while(s->logptr[pos] != NULL){
		pos++;
		pos %= LOG_DEPTH;
		ASSERT(pos != s->logpos);
	}
	s->logptr[pos] = ptr;
#if 0
	s->logpos = pos;
#endif
	s->heap[idx].logcnt++;
	return pos;
}

void _myMemLogFree(UInt idx, uintptr_t pos){
	MyMemState* const s = &s_myMemState;
	if(s->heap[idx].logmod == 0){
		return;
	}
	if(pos == LOG_DEPTH){
		return;
	}
	ASSERT(pos < LOG_DEPTH);
	ASSERT(s->logptr[pos] != NULL);
	s->logptr[pos] = NULL;
#if defined(ENBALE_HEAVYCHK)
	for(int i = 0; i < LOG_DEPTH; i++){
		if(s->logptr[i] != NULL){
			_myMemValidate(s->logptr[i]);
		}
	}
#endif
	s->heap[idx].logcnt--;
}
#endif //defined(ENBALE_LOGCHK)

#define BOUNDARY_SIZE			(sizeof(uintptr_t) * 4)
#define PADDING_SIZE_MIN		(BOUNDARY_SIZE * 2)

#define MM(a)	((((a) & 0x000000ffU) << 24) | (((a) & 0x0000ff00U) << 8) | (((a) & 0x00ff0000U) >> 8) | (((a) & 0xff000000U) >> 24))



void* _myMemChkAlloc(UInt idx, size_t size, size_t alignment, int clear){
	MyMemState* const s = &s_myMemState;
	void* ptr;
	size_t padding = alignment;
	if(padding < PADDING_SIZE_MIN){
		padding = PADDING_SIZE_MIN;
	}
	myAtomic32Inc(&s->heap[idx].counts);
#if defined(ENBALE_COUNTING)
	if(size != 0){
		//(int)ceil(log2(size))
		size_t cl2 = 0;
		size_t t = size - 1;
		while(t != 0){
			t >>= 1;
			cl2++;
		}
		myAtomic32Inc(&s->heap[idx].countx[cl2]);
		ASSERT((1 << cl2) >= size);
		ASSERT((cl2 == 0) || ((1 << (cl2 - 1)) < size));
	}
#endif
#if 0
	padding += padding * (rand() % 5);
#endif

#if defined(USE_MYSPACE)
	ptr = mySpaceMemalign(s->heap[idx].msp, alignment, size + padding * 2);
#else
	ptr = mspace_memalign(s->heap[idx].msp, alignment, size + padding * 2);
#endif
	if(ptr == NULL){
		printf("out of memory!\n");
		ABORT();
		return NULL;
	}

	memset((void*)((uintptr_t)ptr + 0), 0xbb, padding);
	memset((void*)((uintptr_t)ptr + padding + size), 0xdd, padding);

#if defined(DISABLE_FILLING)
	clear = 1;
#endif
	memset((void*)((uintptr_t)ptr + padding), (clear == 0) ? 0xdb : 0x00, size);

	myMutexLock(&s->mtx);
	{
		uintptr_t* v0 = (uintptr_t*)((uintptr_t)ptr + padding - BOUNDARY_SIZE);
		uintptr_t* v1 = (uintptr_t*)((uintptr_t)ptr + padding + size);
		uintptr_t bt0;
		uintptr_t bt1;
		uintptr_t bt2;
		uintptr_t bt3;
#if 0
		{
			SceKernelCallFrame cf[8] = { { 0 } };
			SceUID tid = sceKernelGetThreadId();
			SceUInt32 num;
			SceInt32 mode = SCE_KERNEL_BACKTRACE_MODE_USER;
			SceInt32 ret;
			ASSERT(tid >= SCE_OK);
			ret = sceKernelBacktrace(tid, cf, sizeof(cf), &num, mode);
			if(ret >= SCE_OK){
				// Save backtrace.
				int b = 3;//5;
				bt0 = cf[0 + b].pc;
				bt1 = cf[1 + b].pc;
				bt2 = cf[2 + b].pc;
				bt3 = cf[3 + b].pc;
			}else{

				bt0 = __builtin_return_address();
				bt1 = bt0;
				bt2 = bt0;
				bt3 = bt0;
			}
		}
#else
		bt0 = __builtin_return_address();
		bt1 = bt0;
		bt2 = bt0;
		bt3 = bt0;
#endif
		// Marker
		v0[0] = bt0;
		v0[1] = bt1;
		v0[2] = size;
		v0[3] = MM(0xcafeabcbU);
		v1[0] = MM(0xcafeabcbU);
		v1[1] = padding;
		v1[2] = bt2;
		v1[3] = bt3;

#if 1
		s->heap[idx].units++;
		s->heap[idx].used += size;
		if(s->heap[idx].used > s->heap[idx].max){
			s->heap[idx].max = s->heap[idx].used;
		}

#if defined(ENBALE_LOGCHK)
		v1[3] = (uintptr_t)_myMemLogAlloc(idx, (void*)((uintptr_t)ptr + padding));
#endif
#endif
	}
	myMutexUnlock(&s->mtx);
	return (void*)((uintptr_t)ptr + padding);
}


void _myMemValidate(void* ptr){
	uintptr_t* v0 = (uintptr_t*)((uintptr_t)ptr - BOUNDARY_SIZE);
	size_t size = v0[2];
	uintptr_t* v1 = (uintptr_t*)((uintptr_t)ptr + size);
	size_t padding = v1[1];
	// Double free.
	if(v0[3] == MM(0xdeadbeefU)){
		ABORT();
	}
	if(v1[0] == MM(0xdeadbeefU)){
		ABORT();
	}
	// Memory corruption.
	if(v0[3] != MM(0xcafeabcbU)){
		ABORT();
	}
	if(v1[0] != MM(0xcafeabcbU)){
		ABORT();
	}
	{
		uint8_t const* b0 = (uint8_t const*)((uintptr_t)ptr - padding);
		uint8_t const* b1 = (uint8_t const*)((uintptr_t)ptr + size + BOUNDARY_SIZE);
		for(uintptr_t i = 0; i < padding - BOUNDARY_SIZE; i++){
			if(b0[i] != 0xbbU){
				ABORT();
			}
			if(b1[i] != 0xddU){
				ABORT();
			}
		}
	}
}

void _myMemChkFree(UInt idx, void* ptr){
	MyMemState* const s = &s_myMemState;
	if(ptr == NULL){
		return;
	}

	_myMemValidate(ptr);

	myMutexLock(&s->mtx);
	{
		uintptr_t* v0 = (uintptr_t*)((uintptr_t)ptr - BOUNDARY_SIZE);
		size_t size = v0[2];
		uintptr_t* v1 = (uintptr_t*)((uintptr_t)ptr + size);
		size_t padding = v1[1];

		memset(ptr, 0xbc, size);

		//v0[0] = 0;
		//v0[1] = 0;
		//v0[2] = size;
		v0[3] = MM(0xdeadbeefU);
		v1[0] = MM(0xdeadbeefU);
		//v1[1] = padding;
		//v1[2] = 0;
		//v1[3] = 0;


#if 1
#if defined(ENBALE_LOGCHK)
		_myMemLogFree(idx, v1[3]);
#endif

		if(s->heap[idx].units == 0){
			ABORT();
		}
		if(s->heap[idx].used < size){
			ABORT();
		}
		s->heap[idx].units -= 1;
		s->heap[idx].used -= size;
#endif

		ptr = (void*)((uintptr_t)ptr - padding);
	}
	myMutexUnlock(&s->mtx);

#if defined(USE_MYSPACE)
	mySpaceFree(s->heap[idx].msp, ptr);
#else
	mspace_free(s->heap[idx].msp, ptr);
#endif
}

size_t _myMemGetSize(void* ptr){
	uintptr_t* v0 = (uintptr_t*)((uintptr_t)ptr - BOUNDARY_SIZE);
	size_t size = v0[2];
	return size;
}
#endif



extern "C"{
void user_malloc_init(void);
void user_malloc_finalize(void);
void *user_malloc(size_t size);
void user_free(void *ptr);
void *user_calloc(size_t nelem, size_t size);
void *user_realloc(void *ptr, size_t size);
void *user_memalign(size_t boundary, size_t size);
void *user_reallocalign(void *ptr, size_t size, size_t boundary);
int user_malloc_stats(struct malloc_managed_size *mmsize);
int user_malloc_stats_fast(struct malloc_managed_size *mmsize);
size_t user_malloc_usable_size(void *ptr);
}



UInt _myMemGetHeapIdx(void){
	MyMemState* const s = &s_myMemState;
	SceUID tid = sceKernelGetThreadId();
	UInt i;
	for(i = MY_HEAP_MAX - 1; i > 0; i--){
		if(s->heap[i].tid == tid){
			break;
		}
	}
	ASSERT(s->heap[i].msp != NULL);
	return i;
}

int myMemGetInfo(MyMemInfo* info){
#if defined(ENABLE_MEMCHK)
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	ASSERT(info != NULL);
	info->counts = myAtomic32Swap(&s->heap[select].counts, 0);
	info->units = s->heap[select].units;
	info->used = s->heap[select].used;
	info->max = s->heap[select].max;
#if defined(ENBALE_COUNTING)
	for(int t = 0; t < MY_HEAP_MAX; t++){
		myDebugPrintf(__FUNCTION__, __LINE__, "%d:\n", t);
		for(int i = 0; i < 31; i++){
			int c = myAtomic32Swap(&s->heap[t].countx[i], 0);
			if(c != 0){
				myDebugPrintf(__FUNCTION__, __LINE__, "%d:%d\n", 1 << i, c);
			}
		}
	}
#endif
	return 0;
#else
	if(info == NULL){
		return -1;
	}
	info->counts = 0;
	info->units = 0;
	info->used = 0;
	info->max = 0;
	return -1;
#endif
}

static int _myMemHeapCreate(UInt idx, size_t size, SceUID tid){
	MyMemState* const s = &s_myMemState;
	int res;
	SceUID uid;
	void* base = NULL;
	uid = sceKernelAllocMemBlock("MyMemory", SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA, size, SCE_NULL);
	if(uid < SCE_OK){
		return -1;
	}
	res = sceKernelGetMemBlockBase(uid, &base);
	if(res < SCE_OK){
		SCECHK(sceKernelFreeMemBlock(uid));
		return -1;
	}
	ASSERT(s->heap[idx].msp == NULL);
#if defined(USE_MYSPACE)
	if(0 > mySpaceCreate(&s->heap[idx].mss, base, size)){
		SCECHK(sceKernelFreeMemBlock(uid));
		return - 1;
	}
	s->heap[idx].msp = &s->heap[idx].mss;
#else
	s->heap[idx].msp = mspace_create(base, size);
	if(s->heap[idx].msp == NULL){
		SCECHK(sceKernelFreeMemBlock(uid));
		return - 1;
	}
#endif
	s->heap[idx].uid = uid;
	s->heap[idx].top = base;
	s->heap[idx].end = (void*)((uintptr_t)base + size);
	s->heap[idx].len = size;
	s->heap[idx].tid = tid;
	return 0;
}
static void _myMemHeapRelease(UInt idx){
	MyMemState* const s = &s_myMemState;
	ASSERT(s->heap[idx].msp != NULL);
	ASSERT(s->heap[idx].uid != -1);
#if defined(USE_MYSPACE)
	mySpaceRelease(s->heap[idx].msp);
#else
	mspace_destroy(s->heap[idx].msp);
#endif
	s->heap[idx].msp = NULL;
	SCECHK(sceKernelFreeMemBlock(s->heap[idx].uid));
	s->heap[idx].uid = -1;
	s->heap[idx].tid = -1;
	s->heap[idx].top = NULL;
	s->heap[idx].end = NULL;
}
int myMemThreadLocalHeapInit(size_t size){
	MyMemState* const s = &s_myMemState;
	int res;
	UInt idx;

	UInt select = _myMemGetHeapIdx();
	ASSERT(select == 0);
	if(select != 0){
		return -1;
	}

	myMutexLock(&s->mtx);

	for(idx = 1; idx < bulkof(s->heap); idx++){
		if(s->heap[idx].msp == NULL){
			break;
		}
	}
	ASSERT(idx < bulkof(s->heap));
	if(idx == bulkof(s->heap)){
		myMutexUnlock(&s->mtx);
		return -1;
	}

	res = _myMemHeapCreate(idx, size, sceKernelGetThreadId());
	if(res < 0){
		myMutexUnlock(&s->mtx);
		return -1;
	}

	myMutexUnlock(&s->mtx);
	return 0;
}

int myMemThreadLocalHeapRelease(void){
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	ASSERT(select != 0);
	if(select == 0){
		return -1;
	}
	myMutexLock(&s->mtx);
	_myMemHeapRelease(select);
	myMutexUnlock(&s->mtx);
	return 0;
}

void myMemCountStart(void){
#if defined(ENABLE_MEMCHK) && defined(ENBALE_LOGCHK)
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	myMutexLock(&s->mtx);
	ASSERT(s->heap[select].logcnt == 0);
	ASSERT(s->heap[select].logmod == 0);
	s->heap[select].logmod = 1;
	myMutexUnlock(&s->mtx);
#endif
}
void myMemCountStop(void){
#if defined(ENABLE_MEMCHK) && defined(ENBALE_LOGCHK)
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	myMutexLock(&s->mtx);
	ASSERT(s->heap[select].logmod != 0);
	if(s->heap[select].logcnt != 0){
	UInt i;
		for(i = 0; i < LOG_DEPTH; i++){
			void* ptr = s->logptr[i];
			if(ptr == NULL){
				continue;
			}
			if((ptr < s->heap[select].top) || (ptr >= s->heap[select].end)){
				continue;
			}
			{
				uintptr_t* p0 = (uintptr_t*)((uintptr_t)ptr);
				uintptr_t* p1 = (uintptr_t*)((uintptr_t)ptr + p0[-2]);
				TOUCH(p0);
				TOUCH(p1);
				DEBUG_PRINTF("%08x: adr=%0p siz=%p bt0=%p bt1=%p bt2=%p\n", i, (uintptr_t)p0, p0[-2], p0[-4], p0[-3], p1[2], p1[3]);
			}
		}
		ABORT();
	}
	s->heap[select].logmod = 0;
	myMutexUnlock(&s->mtx);
#endif
}

void user_malloc_init(void){
	MyMemState* const s = &s_myMemState;
	UInt select = 0;

	memset(s, 0, sizeof(MyMemState));

	{
		UInt i;
		for(i = 0; i < bulkof(s->heap); i++){
			s->heap[i].uid = -1;
			s->heap[i].tid = -1;
		}
	}

	SCECHK(_myMemHeapCreate(select, MY_LIBC_HEAP_SIZE, NULL));

	myMutexCreate(&s->mtx);
}

void user_malloc_finalize(void){
	MyMemState* const s = &s_myMemState;

	{
		UInt i;
		for(i = 0; i < bulkof(s->heap); i++){
#if defined(ENABLE_MEMCHK)
			if(s->heap[i].units > 0){
				// Memory leak.
				ABORT();
			}
#endif
			if(i != 0){
				ASSERT(s->heap[i].msp == NULL);
			}else{
				ASSERT(s->heap[i].msp != NULL);
			}
			if(s->heap[i].msp != NULL){
				_myMemHeapRelease(i);
			}
		}
	}
	myMutexRelease(&s->mtx);

	memset(s, 0, sizeof(MyMemState));
}

void* user_malloc(size_t size){
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	TOUCH(s);
#if defined(ENABLE_MEMCHK)
	return _myMemChkAlloc(select, size, 0, 0);
#else
#if defined(USE_MYSPACE)
	return mySpaceMalloc(s->heap[select].msp, size);
#else
	return mspace_malloc(s->heap[select].msp, size);
#endif
#endif
}

void user_free(void* ptr){
	MyMemState* const s = &s_myMemState;
	if(ptr == NULL){
		return;
	}
	UInt select = bulkof(s->heap);
	for(UInt i = 0; i < bulkof(s->heap); i++){
		if((ptr >= s->heap[i].top) && (ptr < s->heap[i].end)){
			select = i;
			break;
		}
	}
	ASSERT(select < bulkof(s->heap));
#if defined(ENABLE_MEMCHK)
	_myMemChkFree(select, ptr);
#else
#if defined(USE_MYSPACE)
	mySpaceFree(s->heap[select].msp, ptr);
#else
	mspace_free(s->heap[select].msp, ptr);
#endif
#endif
}

void* user_calloc(size_t nelem, size_t size){
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	TOUCH(s);
#if defined(ENABLE_MEMCHK)
	return _myMemChkAlloc(select, size * nelem, 0, 1);
#else
#if defined(USE_MYSPACE)
	void* ptr = mySpaceMalloc(s->heap[select].msp, size);
	if(ptr != NULL){
		memset(ptr, 0, size);
	}
	return ptr;
#else
	return mspace_calloc(s->heap[select].msp, nelem, size);
#endif
#endif
}

void* user_realloc(void *ptr, size_t size){
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	TOUCH(s);
#if defined(ENABLE_MEMCHK)
	size_t size0 = size;
	size_t size1 = _myMemGetSize(ptr);
	void* ptr1 = ptr;
	void* ptr0 = _myMemChkAlloc(select, size0, 0, 0);
	memcpy(ptr0, ptr1, size0 < size1 ? size0 : size1);
	user_free(ptr1);
	return ptr0;
#else
#if defined(USE_MYSPACE)
	size_t size0 = size;
	size_t size1 = mySpaceGetAllocSize(s->heap[select].msp, ptr);
	void* ptr1 = ptr;
	void* ptr0 = mySpaceMalloc(s->heap[select].msp, size0);
	memcpy(ptr0, ptr1, size0 < size1 ? size0 : size1);
	mySpaceFree(s->heap[select].msp, ptr1);
	return ptr0;
#else
	return mspace_realloc(s->heap[select].msp, ptr, size);
#endif
#endif
}

void* user_memalign(size_t alignment, size_t size){
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	TOUCH(s);
#if defined(ENABLE_MEMCHK)
	return _myMemChkAlloc(select, size, alignment, 0);
#else
#if defined(USE_MYSPACE)
	return mySpaceMemalign(s->heap[select].msp, alignment, size);
#else
	return mspace_memalign(s->heap[select].msp, alignment, size);
#endif
#endif
}

void* user_reallocalign(void* ptr, size_t size, size_t alignment){
	MyMemState* const s = &s_myMemState;
#if defined(ENABLE_MEMCHK)
	UNUSED(s);
	UNUSED(ptr);
	UNUSED(size);
	UNUSED(alignment);
	ABORT();
	return NULL;
#else
#if defined(USE_MYSPACE)
	UNUSED(s);
	UNUSED(ptr);
	UNUSED(size);
	UNUSED(alignment);
	ABORT();
	return NULL;
#else
	UInt select = _myMemGetHeapIdx();
	return mspace_reallocalign(s->heap[select].msp, ptr, size, alignment);
#endif
#endif
}

int user_malloc_stats(struct malloc_managed_size* mmsize){
#if defined(USE_MYSPACE)
	UNUSED(mmsize);
	return 0;
#else
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	return mspace_malloc_stats(s->heap[select].msp, mmsize);
#endif
}

int user_malloc_stats_fast(struct malloc_managed_size* mmsize){
#if defined(USE_MYSPACE)
	UNUSED(mmsize);
	return 0;
#else
	MyMemState* const s = &s_myMemState;
	UInt select = _myMemGetHeapIdx();
	return mspace_malloc_stats_fast(s->heap[select].msp, mmsize);
#endif
}

size_t user_malloc_usable_size(void* ptr){
	//MyMemState* const s = &s_myMemState;
#if defined(USE_MYSPACE)
	UNUSED(ptr);
	return 0;
#else
	return mspace_malloc_usable_size(ptr);
#endif
}

#else

extern "C" size_t sceLibcHeapSize				= MY_LIBC_HEAP_SIZE;

int myMemGetInfo(MyMemInfo* info){
	if(info == NULL){
		return -1;
	}
	malloc_managed_size mms;
	malloc_stats(&mms);
	info->counts = mms.max_system_size;//0;
	info->units = mms.current_system_size;//0;
	info->used = mms.max_inuse_size;
	info->max = mms.current_inuse_size;
	return -1;
}
int myMemThreadLocalHeapInit(size_t size){
	UNUSED(size);
	// nop
	return 0;
}
int myMemThreadLocalHeapRelease(void){
	// nop
	return 0;
}
void myMemCountStart(void){
	// nop
}
void myMemCountStop(void){
	// nop
}

#endif // defined(ENABLE_REPLACE_MALLOC)
