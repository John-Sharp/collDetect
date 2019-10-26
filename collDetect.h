#ifndef COLL_DETECT_H
#define COLL_DETECT_H

typedef enum COLL_ACTOR_TYPE
{
    COLL_ACTOR_TYPE_POINT,
    COLL_ACTOR_TYPE_V_LINE,
    COLL_ACTOR_TYPE_H_LINE,
} COLL_ACTOR_TYPE;

typedef union collShape 
{
    jintLine point;
    jintAxPlLine line;
} collShape;

typedef struct collActor {
    COLL_ACTOR_TYPE type;

    collShape shape;

    jint collFrame; // frame before collision
} collActor;

typedef enum COLL_FRAME_CALC_RET
{
    COLL_FRAME_CALC_OK,
    COLL_FRAME_CALC_UNHANDLED_TYPES,
} COLL_FRAME_CALC_RET;

COLL_FRAME_CALC_RET calculateNextCollisionFrame(
        jint * collFrame, const collActor * ca1, const collActor * ca2);

#endif
