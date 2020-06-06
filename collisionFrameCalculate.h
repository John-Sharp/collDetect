#ifndef COLLISION_FRAME_CALCULATE_H
#define COLLISION_FRAME_CALCULATE_H

#include "collDetect.h"
#include <jTypes.h>

typedef enum COLL_FRAME_CALC_RET
{
    COLL_FRAME_CALC_OK,
    COLL_FRAME_CALC_UNHANDLED_TYPES,
    COLL_FRAME_CALC_NO_COLLISION
} COLL_FRAME_CALC_RET;


COLL_FRAME_CALC_RET calculateNextCollisionFrame(
        jint * collFrame, jintVec * norm, 
        const collActor * ca1, const collActor * ca2);

#endif
