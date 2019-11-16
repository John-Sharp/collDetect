#ifndef COLL_DETECT_H
#define COLL_DETECT_H

#include <jTypes.h>

typedef enum COLL_ACTOR_TYPE
{
    COLL_ACTOR_TYPE_POINT = 1,
    COLL_ACTOR_TYPE_V_LINE = 1<<1,
    COLL_ACTOR_TYPE_H_LINE = 1<<2,
} COLL_ACTOR_TYPE;

typedef union collShape 
{
    jintVec point;
    jintAxPlLine line;
} collShape;

typedef struct collActor {
    COLL_ACTOR_TYPE type;

    collShape shape;
    jintVec vel; // velocity

    jint collFrame; // frame before collision
} collActor;

typedef enum COLL_FRAME_CALC_RET
{
    COLL_FRAME_CALC_OK,
    COLL_FRAME_CALC_UNHANDLED_TYPES,
    COLL_FRAME_CALC_NO_COLLISION
} COLL_FRAME_CALC_RET;

COLL_FRAME_CALC_RET calculateNextCollisionFrame(
        jint * collFrame, const jintVec * vrel, const collActor * ca1, const collActor * ca2);

#endif
