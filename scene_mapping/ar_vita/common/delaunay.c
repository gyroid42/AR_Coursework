/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2011 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdlib.h>
#include <string.h>
#include <float.h>

//#define ENABLE_DBGPRINTF
#include "common.h"

#include "delaunay.h"

typedef struct DelaunayNode{
	float x;
	float y;
}DelaunayNode;

typedef struct DelaunayEdge{
	UInt p0;
	UInt p1;
	SInt t0;					// p0->p1->t0 cw
	SInt t1;					// p0->p1->t1 ccw
}DelaunayEdge;

typedef struct DelaunayTriangle{
	UInt p0;
	UInt p1;
	UInt p2;					// p0->p1->p2 ccw
	UInt e0;					// p0-p1
	UInt e1;					// p1-p2
	UInt e2;					// p2-p0
}DelaunayTriangle;

struct DelaunayState{
	UInt max;
	UInt pad;					// padding
	UInt tpos;
	UInt epos;
	UInt num_work;
	UInt num_node;
	UInt num_edge;
	UInt num_tri;
	UInt* work;					// n(* 3)
	DelaunayNode* node;			// n
	DelaunayEdge* edge;			// n * 3
	DelaunayTriangle* tri;		// n * 2
};

static void* _incptr(void** p, size_t s){
	void* r = *p;
	*p = (void*)((UIntPtr)*p + s);
	return r;
}
void delaunayClear(DelaunayState* state){
	state->tpos = 0;
	state->epos = 0;
	state->num_node = 0;
	state->num_edge = 0;
	state->num_work = 0;
	state->num_tri = 0;
}
DelaunayState* delaunayInitialize(int max){
	DelaunayState* state;
	void* ptr;
	size_t len;
	if(max < 4){
		return NULL;
	}
	if(max >= (1 << 24)){
		return NULL;
	}
	len = sizeof(DelaunayState);
	len += sizeof(UInt) * max;				// work
	len += sizeof(DelaunayNode) * max;		// node
	len += sizeof(DelaunayEdge) * max * 3;	// edge
	len += sizeof(DelaunayTriangle) * max * 2;	// tri
	ptr = malloc(len);
	if(ptr == NULL){
		return NULL;
	}
	memset(ptr, 0, len);
	state = (DelaunayState*)_incptr(&ptr, sizeof(DelaunayState));
	state->max = max;
	state->work = (UInt*)_incptr(&ptr, sizeof(UInt) * max);
	state->node = (DelaunayNode*)_incptr(&ptr, sizeof(DelaunayNode) * max);
	state->edge = (DelaunayEdge*)_incptr(&ptr, sizeof(DelaunayEdge) * max * 3);
	state->tri = (DelaunayTriangle*)_incptr(&ptr, sizeof(DelaunayTriangle) * max * 2);
	delaunayClear(state);
	return state;
}
void delaunayRelease(DelaunayState* state){
	if(state == NULL){
		return;
	}
	memset(state, 0, sizeof(DelaunayState));
	free(state);
}
// p0->p1->p2: CCW=1,line=0,CW=-1
static SInt _delaunayCCW(DelaunayNode const* n0, DelaunayNode const* n1, DelaunayNode const* n2){
	float x1 = n1->x - n0->x;
	float y1 = n1->y - n0->y;
	float x2 = n2->x - n0->x;
	float y2 = n2->y - n0->y;
	float z = x1 * y2 - x2 * y1;
	return (z > +0.0f) ? +1 : ((z < -0.0f) ? -1 : 0);
}
// flippable(n0-n2 diagonal)=1,other=0
static UInt _delaunayFlippable(DelaunayNode const* n0, DelaunayNode const* n1, DelaunayNode const* n2, DelaunayNode const* n3){
	SInt ccw =  _delaunayCCW(n1, n2, n3);
	if(ccw != _delaunayCCW(n3, n0, n1)){
		return 0;
	}
#if 0
	if(ccw != _delaunayCCW(n2, n3, n0)){
		return 0;
	}
	if(ccw != _delaunayCCW(n0, n1, n2)){
		return 0;
	}
#endif
	return 1;
}

UInt _delaunayCheck(DelaunayNode const* n0, DelaunayNode const* n1, DelaunayNode const* n2, DelaunayNode const* n3){
	float n3x = n3->x;
	float n3y = n3->y;
	float x0m = n0->x - n3x;
	float x1m = n1->x - n3x;
	float x2m = n2->x - n3x;
	float y0m = n0->y - n3y;
	float y1m = n1->y - n3y;
	float y2m = n2->y - n3y;
	float x0p = n0->x + n3x;
	float x1p = n1->x + n3x;
	float x2p = n2->x + n3x;
	float y0p = n0->y + n3y;
	float y1p = n1->y + n3y;
	float y2p = n2->y + n3y;
	float v =
		+ (x0m * x0p + y0m * y0p) * (x1m * y2m - x2m * y1m)
		+ (x1m * x1p + y1m * y1p) * (x2m * y0m - x0m * y2m)
		+ (x2m * x2p + y2m * y2p) * (x0m * y1m - x1m * y0m);
	return (v > 0.0f) ? 0 : 1;
}
static void _delaunaySetTriangle(DelaunayState* state, UInt te, UInt p0, UInt p1, UInt p2, UInt e0, UInt e1, UInt e2){
	ASSERT(te < state->max * 2);
	state->tri[te].p0 = p0;
	state->tri[te].p1 = p1;
	state->tri[te].p2 = p2;
	state->tri[te].e0 = e0;
	state->tri[te].e1 = e1;
	state->tri[te].e2 = e2;
#if 0
	{
		DelaunayNode const* n0 = &state->node[state->tri[te].p0];
		DelaunayNode const* n1 = &state->node[state->tri[te].p1];
		DelaunayNode const* n2 = &state->node[state->tri[te].p2];
		SInt ccw = _delaunayCCW(n0, n1, n2);
		if(ccw < 0){
			PRINTF("(%f, %f), (%f, %f), (%f, %f)\n", n0->x, n0->y, n1->x, n1->y, n2->x, n2->y);
		}
		ASSERT(ccw >= 0);
	}
#endif
}
static UInt _delaunayAddEdge(DelaunayState* state, UInt p0, UInt p1, SInt t0, SInt t1){
	UInt ee = state->num_edge;
	ASSERT(ee < state->max * 3);
	state->num_edge = ee + 1;
	state->edge[ee].p0 = p0;
	state->edge[ee].p1 = p1;
	state->edge[ee].t0 = t0;
	state->edge[ee].t1 = t1;
	return ee;
}
static void _delaunayPushEdge(DelaunayState* state, UInt ee){
	UInt we = state->num_work;
	ASSERT(we < state->max);
	if(we >= state->max){
		return;
	}
	state->work[we] = ee;
	state->num_work = ++we;
}
static UInt _delaunayPopEdge(DelaunayState* state){
	UInt we = state->num_work;
	ASSERT(we > 0);
	state->num_work = --we;
	return state->work[we];
}
static UInt _delaunayIsEmptyEdges(DelaunayState* state){
	return (state->num_work == 0) ? 1 : 0;
}
static SInt _delaunayFindTriangle(DelaunayState* state, DelaunayNode const* nn){
	SInt te = (SInt)state->num_tri - 1;
	UInt ep = state->num_edge;
	for(;;){
		DelaunayNode const* n0 = &state->node[state->tri[te].p0];
		DelaunayNode const* n1 = &state->node[state->tri[te].p1];
		DelaunayNode const* n2 = &state->node[state->tri[te].p2];
#if 1
		UInt ex = ep;
		SInt ccw0;
		SInt ccw1;
		SInt ccw2;
		{
			float x0 = n0->x - nn->x;
			float y0 = n0->y - nn->y;
			float x1 = n1->x - nn->x;
			float y1 = n1->y - nn->y;
			float x2 = n2->x - nn->x;
			float y2 = n2->y - nn->y;
			float z0 = x0 * y1 - x1 * y0;
			float z1 = x1 * y2 - x2 * y1;
			float z2 = x2 * y0 - x0 * y2;
			if(((x0 > -FLT_EPSILON) && (x0 < +FLT_EPSILON)) || ((y0 > -FLT_EPSILON) && (y0 < +FLT_EPSILON))){
				break;
			}
			if(((x1 > -FLT_EPSILON) && (x1 < +FLT_EPSILON)) || ((y1 > -FLT_EPSILON) && (y1 < +FLT_EPSILON))){
				break;
			}
			if(((x2 > -FLT_EPSILON) && (x2 < +FLT_EPSILON)) || ((y2 > -FLT_EPSILON) && (y2 < +FLT_EPSILON))){
				break;
			}
			ccw0 = (z0 < +0.0f) ? -1 : 0;
			ccw1 = (z1 < +0.0f) ? -1 : 0;
			ccw2 = (z2 < +0.0f) ? -1 : 0;
		}
		if(ccw0 < 0){
			ex = state->tri[te].e0;
		}
		if(ccw1 < 0){
			ex = state->tri[te].e1;
		}
		if(ccw2 < 0){
			ex = state->tri[te].e2;
		}
		if(ex != ep){
			DelaunayEdge const* edge = &state->edge[ex];
			ep = ex;
			te = (edge->t0 != te) ? edge->t0 : edge->t1;
			if(te < 0){
				break;
			}
			continue;
		}
#else
		if((state->tri[te].e0 != ep) && (0 > _delaunayCCW(n0, n1, nn))){
			DelaunayEdge const* edge = &state->edge[state->tri[te].e0];
			ep = state->tri[te].e0;
			te = (edge->t0 != te) ? edge->t0 : edge->t1;
			if(te < 0){
				break;
			}
			continue;
		}
		if((state->tri[te].e1 != ep) && (0 > _delaunayCCW(n1, n2, nn))){
			DelaunayEdge const* edge = &state->edge[state->tri[te].e1];
			ep = state->tri[te].e1;
			te = (edge->t0 != te) ? edge->t0 : edge->t1;
			if(te < 0){
				break;
			}
			continue;
		}
		if((state->tri[te].e2 != ep) && (0 > _delaunayCCW(n2, n0, nn))){
			DelaunayEdge const* edge = &state->edge[state->tri[te].e2];
			ep = state->tri[te].e2;
			te = (edge->t0 != te) ? edge->t0 : edge->t1;
			if(te < 0){
				break;
			}
			continue;
		}
#endif
		return te;
	}
	return -1;
}

static int _delaunaySetup(DelaunayState* state){
	UInt p0 = 0;
	UInt p1 = 1;
	UInt p2 = 2;
	UInt p3 = 3;
	UInt q = 1;
	ASSERT(state->num_node == 4);
	{
		DelaunayNode const* n0 = &state->node[p0];
		DelaunayNode const* n1 = &state->node[p1];
		DelaunayNode const* n2 = &state->node[p2];
		DelaunayNode const* n3 = &state->node[p3];
		if(0 > _delaunayCCW(n0, n1, n2)){
			UInt pt = p1;
			p1 = p2;
			p2 = pt;
			n1 = &state->node[p1];
			n2 = &state->node[p2];
		}
		if(0 > _delaunayCCW(n0, n1, n3)){
			UInt pt = p2;
			p2 = p0;
			p0 = p1;
			p1 = pt;
		}else if(0 > _delaunayCCW(n1, n2, n3)){
			UInt pt = p0;
			p0 = p2;
			p2 = p1;
			p1 = pt;
		}else if(0 > _delaunayCCW(n2, n0, n3)){
			// nop
		}else{
			q = 0;
		}
	}

	if(q == 0){
		_delaunayAddEdge(state, p0, p1, -1, 0);
		_delaunayAddEdge(state, p1, p2, -1, 0);
		_delaunayAddEdge(state, p2, p0, -1, 0);
		_delaunaySetTriangle(state, 0, p0, p1, p2, 0, 1, 2);
		state->num_tri = 1;
	}else{
		_delaunayAddEdge(state, p0, p1, -1, 0);
		_delaunayAddEdge(state, p1, p2, -1, 0);
		_delaunayAddEdge(state, p0, p2, 0, 1);
		_delaunayAddEdge(state, p2, p3, -1, 1);
		_delaunayAddEdge(state, p3, p0, -1, 1);
		_delaunaySetTriangle(state, 0, p0, p1, p2, 0, 1, 2);
		_delaunaySetTriangle(state, 1, p0, p2, p3, 2, 3, 4);
		state->num_tri = 2;
	}
	return q;
}

int delaunayInsertNode(DelaunayState* state, float x, float y){
	UInt ne = state->num_node;
	SInt te;
	if(ne >= state->max){
		return -1;
	}
	{
		state->node[ne].x = x;
		state->node[ne].y = y;
		state->num_node = ne + 1;
	}
	if(state->num_node < 4){
		return (int)ne;
	}
	if(state->num_node == 4){
		if(0 != _delaunaySetup(state)){
			return (int)ne;
		}
	}
	{
		SInt tt = _delaunayFindTriangle(state, &state->node[ne]);
		if(tt < 0){
			return -1;
		}
		te = tt;
	}
	{
		UInt p0 = state->tri[te].p0;
		UInt p1 = state->tri[te].p1;
		UInt p2 = state->tri[te].p2;
		UInt e0 = state->tri[te].e0;	// p0-p1
		UInt e1 = state->tri[te].e1;	// p1-p2
		UInt e2 = state->tri[te].e2;	// p2-p0
		SInt t0 = te;
		SInt t1 = (SInt)state->num_tri++;
		SInt t2 = (SInt)state->num_tri++;
		UInt e3;
		UInt e4;
		UInt e5;
		if(state->edge[e0].t0 == te){
			state->edge[e0].t0 = t0;
		}
		if(state->edge[e0].t1 == te){
			state->edge[e0].t1 = t0;
		}
		if(state->edge[e1].t0 == te){
			state->edge[e1].t0 = t1;
		}
		if(state->edge[e1].t1 == te){
			state->edge[e1].t1 = t1;
		}
		if(state->edge[e2].t0 == te){
			state->edge[e2].t0 = t2;
		}
		if(state->edge[e2].t1 == te){
			state->edge[e2].t1 = t2;
		}
		e3 = _delaunayAddEdge(state, p0, ne, t0, t2);
		e4 = _delaunayAddEdge(state, p1, ne, t1, t0);
		e5 = _delaunayAddEdge(state, p2, ne, t2, t1);
		_delaunaySetTriangle(state, t0, p0, p1, ne, e0, e4, e3);
		_delaunaySetTriangle(state, t1, p1, p2, ne, e1, e5, e4);
		_delaunaySetTriangle(state, t2, p2, p0, ne, e2, e3, e5);
		_delaunayPushEdge(state, e0);
		_delaunayPushEdge(state, e1);
		_delaunayPushEdge(state, e2);
	}
	while(0 == _delaunayIsEmptyEdges(state)){
		UInt ee = _delaunayPopEdge(state);
		SInt t0 = state->edge[ee].t0;
		SInt t1 = state->edge[ee].t1;
		UInt e0;
		UInt e1;
		UInt e2;
		UInt e3;
		UInt p0 = state->edge[ee].p0;
		UInt p1;
		UInt p2 = state->edge[ee].p1;
		UInt p3;
		DelaunayNode const* n0;
		DelaunayNode const* n1;
		DelaunayNode const* n2;
		DelaunayNode const* n3;
		if((t0 < 0) || (t1 < 0)){
			continue;
		}
		if(state->tri[t0].e0 == ee){
			p1 = state->tri[t0].p2;
			e0 = state->tri[t0].e1;
			e1 = state->tri[t0].e2;
		}else if(state->tri[t0].e1 == ee){
			p1 = state->tri[t0].p0;
			e0 = state->tri[t0].e2;
			e1 = state->tri[t0].e0;
		}else{// if(state->tri[t0].e2 == ee){
			ASSERT(state->tri[t0].e2 == ee);
			p1 = state->tri[t0].p1;
			e0 = state->tri[t0].e0;
			e1 = state->tri[t0].e1;
		}
		if(state->tri[t1].e0 == ee){
			p3 = state->tri[t1].p2;
			e2 = state->tri[t1].e1;
			e3 = state->tri[t1].e2;
		}else if(state->tri[t1].e1 == ee){
			p3 = state->tri[t1].p0;
			e2 = state->tri[t1].e2;
			e3 = state->tri[t1].e0;
		}else/* if(state->tri[t1].e2 == ee)*/{
			ASSERT(state->tri[t1].e2 == ee);
			p3 = state->tri[t1].p1;
			e2 = state->tri[t1].e0;
			e3 = state->tri[t1].e1;
		}
		n0 = &state->node[p0];
		n1 = &state->node[p1];
		n2 = &state->node[p2];
		n3 = &state->node[p3];
		if(0 == _delaunayFlippable(n0, n1, n2, n3)){
			continue;
		}
		if(p1 == ne){
			if(0 != _delaunayCheck(n2, n3, n0, n1)){
				continue;
			}
		}else/* if(p3 == ne)*/{
			ASSERT(p3 == ne);
			if(0 != _delaunayCheck(n0, n1, n2, n3)){
				continue;
			}
		}
		{
			state->edge[ee].p0 = p1;
			state->edge[ee].p1 = p3;
			if(state->edge[e0].t0 == t0){
				state->edge[e0].t0 = t1;
			}
			if(state->edge[e0].t1 == t0){
				state->edge[e0].t1 = t1;
			}
			if(state->edge[e2].t0 == t1){
				state->edge[e2].t0 = t0;
			}
			if(state->edge[e2].t1 == t1){
				state->edge[e2].t1 = t0;
			}
			_delaunaySetTriangle(state, t0, p3, p1, p2, ee, e1, e2);
			_delaunaySetTriangle(state, t1, p1, p3, p0, ee, e3, e0);
		}
		if(p1 == ne){
			_delaunayPushEdge(state, e2);
			_delaunayPushEdge(state, e3);
		}else/* if(p3 == ne)*/{
			ASSERT(p3 == ne);
			_delaunayPushEdge(state, e0);
			_delaunayPushEdge(state, e1);
		}
	}
	return (int)ne;
}

UInt delaunayGetNodeCount(DelaunayState* state){
	return state->num_node;
}

void delaunaySetTrianglesTop(DelaunayState* state){
	state->tpos = 0;
}

UInt delaunayGetTriangles(DelaunayState* state, UInt* idx, UInt num){
	UInt tpos = state->tpos;
	UInt ret;
	UInt n = state->num_tri - tpos;
	if(n > num){
		n = num;
	}
	ret = n;
	while(n-- != 0){
		DelaunayTriangle const* tt = &state->tri[tpos++];
		*idx++ = tt->p0;
		*idx++ = tt->p1;
		*idx++ = tt->p2;
	}
	state->tpos = tpos;
	return ret;
}

void delaunaySetEdgesTop(DelaunayState* state){
	state->epos = 0;
}

UInt delaunayGetEdges(DelaunayState* state, UInt* idx, UInt num){
	UInt epos = state->epos;
	UInt ret;
	UInt n = state->num_edge - epos;
	if(n > num){
		n = num;
	}
	ret = n;
	while(n-- != 0){
		DelaunayEdge const* ee = &state->edge[epos++];
		*idx++ = ee->p0;
		*idx++ = ee->p1;
	}
	state->epos = epos;
	return ret;
}

