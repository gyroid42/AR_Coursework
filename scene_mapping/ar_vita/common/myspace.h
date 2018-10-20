/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2014 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef MYSPACE_H
#define MYSPACE_H

#include "common.h"


#if defined(__cplusplus)
extern "C" {
#endif

#if 0
#undef ALWAYS_INLINE
#define ALWAYS_INLINE

#undef INLINE
#define INLINE static
#endif

#define MYSPACE_MINIMUM_ALIGN		(32)
#define MYSPACE_MINIMUM_UNIT		(32)
#define MYSPACE_EXP_STEP			(4)

//#define MYSPACE_DEBUG_ENABLE

typedef struct MySpaceNode{
	struct{
		struct MySpaceNode* prev;
		struct MySpaceNode* next;
	}queue;
	struct{
		struct MySpaceNode* prev;
		struct MySpaceNode* next;
	}neighbor;
	size_t capacity;
	size_t engaged;
	size_t padding;
	struct MySpaceNode* tag;
}MySpaceNode;

typedef struct MySpaceState{
	void* base;
	size_t capacity;
#if defined(MYSPACE_DEBUG_ENABLE)
	size_t remain;
#endif
	MyMutex mtx;
	MySpaceNode exp[32 / MYSPACE_EXP_STEP];
}MySpaceState;

INLINE int mySpaceCreate(MySpaceState* myspace, void* base, size_t capacity) ALWAYS_INLINE;
INLINE int mySpaceRelease(MySpaceState* myspace) ALWAYS_INLINE;

INLINE void* mySpaceMalloc(MySpaceState* myspace, size_t size) ALWAYS_INLINE;
INLINE void* mySpaceMemalign(MySpaceState* myspace, size_t align, size_t size) ALWAYS_INLINE;
INLINE void mySpaceFree(MySpaceState* myspace, void* ptr) ALWAYS_INLINE;

INLINE size_t mySpaceGetAllocSize(MySpaceState* myspace, void* ptr) ALWAYS_INLINE;

INLINE void _mySpaceRegisterQueue(MySpaceState* myspace, MySpaceNode* node) ALWAYS_INLINE;
INLINE void _mySpaceRemoveQueue(MySpaceState* myspace, MySpaceNode* node) ALWAYS_INLINE;
INLINE MySpaceNode* _mySpaceCombineNode(MySpaceState* myspace, MySpaceNode* nodel, MySpaceNode* nodeh) ALWAYS_INLINE;

INLINE void _mySpaceRegisterQueue(MySpaceState* myspace, MySpaceNode* node){
	UInt i;
	size_t mask = ~((MYSPACE_MINIMUM_UNIT << 1) - 1);
	for(i = 0; i < (32 / MYSPACE_EXP_STEP); i++, mask <<= MYSPACE_EXP_STEP){
		if((node->capacity & mask) == 0){
			node->engaged = 0;
			node->queue.prev = &myspace->exp[i];
			node->queue.next = myspace->exp[i].queue.next;
			if(myspace->exp[i].queue.next != NULL){
				myspace->exp[i].queue.next->queue.prev = node;
			}
			myspace->exp[i].queue.next = node;
#if defined(MYSPACE_DEBUG_ENABLE)
			myspace->remain += node->capacity;
#endif
			break;
		}
	}
	ASSERT(i < (32 / MYSPACE_EXP_STEP));
}

INLINE void _mySpaceRemoveQueue(MySpaceState* myspace, MySpaceNode* node){
	UNUSED(myspace);
	node->queue.prev->queue.next = node->queue.next;
	if(node->queue.next != NULL){
		node->queue.next->queue.prev = node->queue.prev;
	}
#if defined(MYSPACE_DEBUG_ENABLE)
	myspace->remain -= node->capacity;
#endif
}

INLINE MySpaceNode* _mySpaceCombineNode(MySpaceState* myspace, MySpaceNode* nodel, MySpaceNode* nodeh){
	UNUSED(myspace);
	nodel->capacity += nodeh->capacity + sizeof(MySpaceNode);
	nodel->neighbor.next = nodeh->neighbor.next;
	if(nodeh->neighbor.next != NULL){
		nodeh->neighbor.next->neighbor.prev = nodel;
	}
	return nodel;
}

INLINE int mySpaceCreate(MySpaceState* myspace, void* base, size_t capacity){
	ASSERT((sizeof(MySpaceNode) & (MYSPACE_MINIMUM_ALIGN - 1)) == 0);
	ASSERT(MYSPACE_MINIMUM_UNIT >= MYSPACE_MINIMUM_ALIGN);
	memset(myspace, 0, sizeof(MySpaceState));
	{
		size_t offset = (-(UIntPtr)base) & (MYSPACE_MINIMUM_ALIGN - 1);
		myspace->base = (void*)((UIntPtr)base + offset);
		myspace->capacity = capacity - offset;
#if defined(MYSPACE_DEBUG_ENABLE)
		myspace->remain = 0;
#endif
	}
	if(0 > myMutexCreate(&myspace->mtx)){
		return -1;
	}
	{
		MySpaceNode* node = (MySpaceNode*)base;
		node->capacity = capacity - sizeof(MySpaceNode);
		node->engaged = 0;
		node->neighbor.prev = NULL;
		node->neighbor.next = NULL;
		node->queue.prev = NULL;
		node->queue.next = NULL;
		_mySpaceRegisterQueue(myspace, node);
	}
	return 0;
}

INLINE int mySpaceRelease(MySpaceState* myspace){
	if(myMutexIsValid(&myspace->mtx) != FALSE){
		myMutexRelease(&myspace->mtx);
	}
#if defined(MYSPACE_DEBUG_ENABLE)
	ASSERT(myspace->remain + sizeof(MySpaceNode) == myspace->capacity);
	{
		UInt i;
		size_t mask = ~((MYSPACE_MINIMUM_UNIT << 1) - 1);
		for(i = 0; i < (32 / MYSPACE_EXP_STEP); i++, mask <<= MYSPACE_EXP_STEP){
			if((myspace->remain & mask) == 0){
				break;
			}
			ASSERT(myspace->exp[i].queue.next == NULL);
		}
		ASSERT(myspace->exp[i].queue.next != NULL);
		ASSERT(myspace->exp[i].queue.next->capacity == myspace->remain);
		ASSERT(myspace->exp[i].queue.next->queue.next == NULL);
		i++;
		for(; i < (32 / MYSPACE_EXP_STEP); i++){
			ASSERT(myspace->exp[i].queue.next == NULL);
		}
	}
#endif
	memset(myspace, 0, sizeof(MySpaceState));
	return 0;
}

INLINE void* mySpaceMemalign(MySpaceState* myspace, size_t align, size_t size){
	void* ptr = NULL;
	if(size <= MYSPACE_MINIMUM_UNIT){
		size = MYSPACE_MINIMUM_UNIT;
	}else{
		size = (size + MYSPACE_MINIMUM_ALIGN - 1) & ~(MYSPACE_MINIMUM_ALIGN - 1);
	}
	if(align <= MYSPACE_MINIMUM_ALIGN){
		align = MYSPACE_MINIMUM_ALIGN;
	}else{
		size_t cl2 = 0;
		size_t t = align - 1;
		while(t != 0){
			t >>= 1;
			cl2++;
		}
		align = 1 << cl2;
	}
	myMutexLock(&myspace->mtx);
	{
		UInt i;
		size_t mask = ~((MYSPACE_MINIMUM_UNIT << 1) - 1);
		for(i = 0; i < (32 / MYSPACE_EXP_STEP); i++, mask <<= MYSPACE_EXP_STEP){
			MySpaceNode* node;
			if((size & mask) != 0){
				continue;
			}
			for(node = myspace->exp[i].queue.next; node != NULL; node = node->queue.next){
				size_t offset = (-(UIntPtr)(node + 1)) & (align - 1);
				if((node->capacity - offset) >= size){
					// allocate
					ptr = (void*)((UIntPtr)(node + 1) + offset);
					// set tag
					*(MySpaceNode**)((UIntPtr)ptr - sizeof(UIntPtr)) = node;
					// remove from queue
					_mySpaceRemoveQueue(myspace, node);
					// engage!
					node->engaged = 1;
					if((node->capacity - offset - size) >= (sizeof(MySpaceNode) + MYSPACE_MINIMUM_UNIT)){
						// split
						MySpaceNode* newnode = (MySpaceNode*)((UIntPtr)ptr + size);
						newnode->capacity = node->capacity - offset - size - sizeof(MySpaceNode);
						newnode->engaged = 0;
						newnode->neighbor.prev = node;
						newnode->neighbor.next = node->neighbor.next;
						if(node->neighbor.next != NULL){
							node->neighbor.next->neighbor.prev = newnode;
						}
						node->neighbor.next = newnode;
						node->capacity = offset + size;
						_mySpaceRegisterQueue(myspace, newnode);
					}
					break;
				}
			}
			if(node != NULL){
				break;
			}
		}
	}
	myMutexUnlock(&myspace->mtx);
	return ptr;
}

INLINE void* mySpaceMalloc(MySpaceState* myspace, size_t size){
	return mySpaceMemalign(myspace, 0, size);
}

INLINE size_t mySpaceGetAllocSize(MySpaceState* myspace, void* ptr){
	MySpaceNode* node = *(MySpaceNode**)((UIntPtr)ptr - sizeof(UIntPtr));
	UNUSED(myspace);
	return node->capacity - ((UIntPtr)ptr - (UIntPtr)(node + 1));
}

INLINE void mySpaceFree(MySpaceState* myspace, void* ptr){
	MySpaceNode* node = *(MySpaceNode**)((UIntPtr)ptr - sizeof(UIntPtr));

	myMutexLock(&myspace->mtx);
	{
		if(node->neighbor.prev != NULL){
			MySpaceNode* prev = node->neighbor.prev;
			if(prev->engaged == 0){
				// remove from queue
				_mySpaceRemoveQueue(myspace, prev);
				// combine
				node = _mySpaceCombineNode(myspace, prev, node);
			}
		}
		if(node->neighbor.next != NULL){
			MySpaceNode* next = node->neighbor.next;
			if(next->engaged == 0){
				// remove from queue
				_mySpaceRemoveQueue(myspace, next);
				// combine
				node = _mySpaceCombineNode(myspace, node, next);
			}
		}
		_mySpaceRegisterQueue(myspace, node);
	}
	myMutexUnlock(&myspace->mtx);
}


#if defined(__cplusplus)
}
#endif

#endif /* MYSPACE_H */
