#include "collDetect.h"

#include <stdio.h>

typedef enum RELATION
{
    GT,
    EQ,
    LT
} RELATION;

RELATION pointLineRelation(jint frame, const jintLine * point, const jintAxPlLine * line)
{
    jint scaledUpPointY = point->rStart.y * point->tScale + frame * point->sTarg.y;
    jint scaledUpLineBottom = line->rStart.y * point->tScale;
    jint scaledUpLineTop = scaledUpLineBottom + line->length * point->tScale;

    if (scaledUpPointY > scaledUpLineTop)
        return GT;

    if (scaledUpPointY < scaledUpLineBottom)
        return LT;

    return EQ;
}

COLL_FRAME_CALC_RET calculateNextCollisionFrame(jint * collFrame, const collActor * ca1, const collActor * ca2)
{
    bool handled_collision = 
        (ca1->type == COLL_ACTOR_TYPE_POINT && ca2->type == COLL_ACTOR_TYPE_V_LINE);

    if (!handled_collision)
    {
        return COLL_FRAME_CALC_UNHANDLED_TYPES;
    }

    const jintLine * point = &ca1->shape.point; 
    const jintAxPlLine * line = &ca2->shape.line; 

    int frame_coll = (line->rStart.x - point->rStart.x)  * point->tScale / point->sTarg.x;
    printf("(%d - %d) * %d / %d = %d\n\n", line->rStart.x, point->rStart.x, point->tScale, point->sTarg.x, frame_coll);

    // check that point actually lies inbetween the end-points of the line
    // if both positions at point `frame_coll` and `frame_coll` + 1 
    // are above or below the line then we know it doesn't hit, else it does
    RELATION relationBefore = pointLineRelation(frame_coll, point, line);
    RELATION relationAfter = pointLineRelation(frame_coll + 1, point, line);

    if (relationBefore == GT && relationAfter == GT)
    {
        return COLL_FRAME_CALC_NO_COLLISION;
    }
        
    if (relationBefore == LT && relationAfter == LT)
    {
        return COLL_FRAME_CALC_NO_COLLISION;
    }

    *collFrame = frame_coll;
    return COLL_FRAME_CALC_OK;
}


