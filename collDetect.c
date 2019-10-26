#include "collDetect.h"

COLL_FRAME_CALC_RET calculateNextCollisionFrame(jint * collFrame, const collActor * ca1, const collActor * ca2)
{
    bool handled_collision = 
        (ca1->type == COLL_ACTOR_TYPE_POINT && ca2->type == COLL_ACTOR_TYPE_V_LINE);

    if (!handled_collision)
    {
        return COLL_FRAME_CALC_UNHANDLED_TYPES;
    }

    jintLine * point = &ca1->shape.point; 
    jintAxPlLine * line = &ca1->shape.line; 

    int frame_coll = line->rStart.x * point->tScale / (point->sTarg.x - point->rStart.x);

    // TODO check that point actually lies inbetween the end-points of the line

    *collFrame = frame_coll;
    return COLL_FRAME_CALC_OK;
}

