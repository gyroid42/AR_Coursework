/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2011 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef DELAUNAY_H
#define DELAUNAY_H


#if defined(__cplusplus)
extern "C"{
#endif

struct DelaunayState;
typedef struct DelaunayState DelaunayState;

DelaunayState* delaunayInitialize(int max);

void delaunayRelease(DelaunayState* state);

void delaunayClear(DelaunayState* state);

int delaunayInsertNode(DelaunayState* state, float x, float y);

UInt delaunayGetNodeCount(DelaunayState* state);

void delaunaySetTrianglesTop(DelaunayState* state);

UInt delaunayGetTriangles(DelaunayState* state, UInt* idx, UInt num);

void delaunaySetEdgesTop(DelaunayState* state);

UInt delaunayGetEdges(DelaunayState* state, UInt* idx, UInt num);

#if defined(__cplusplus)
}//extern "C"
#endif

#endif /* DELAUNAY_H */
