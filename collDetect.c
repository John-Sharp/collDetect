#include "collDetect.h"

#include <stdio.h>

typedef enum RELATION
{
    GT,
    EQ,
    LT
} RELATION;

RELATION pointLineRelation(jint frame, juint axis, const jintLine * point, const jintAxPlLine * line)
{
    jint scaledUpPoint = point->rStart.v[axis] * point->tScale + frame * point->sTarg.v[axis];
    jint scaledUpLineBottom = line->rStart.v[axis] * point->tScale;
    jint scaledUpLineTop = scaledUpLineBottom + line->length * point->tScale;

    if (scaledUpPoint > scaledUpLineTop)
        return GT;

    if (scaledUpPoint < scaledUpLineBottom)
        return LT;

    return EQ;
}

COLL_FRAME_CALC_RET CNCFPointLine(jint * collFrame, juint axis, const jintLine * point, const jintAxPlLine * line);

COLL_FRAME_CALC_RET calculateNextCollisionFrame(jint * collFrame, const collActor * ca1, const collActor * ca2)
{
    switch (ca1->type | ca2->type)
    {
        case (COLL_ACTOR_TYPE_POINT | COLL_ACTOR_TYPE_V_LINE):
        {
            return CNCFPointLine(collFrame, 0, &ca1->shape.point, &ca2->shape.line);
        }
        case (COLL_ACTOR_TYPE_POINT | COLL_ACTOR_TYPE_H_LINE):
        {
            return CNCFPointLine(collFrame, 1, &ca1->shape.point, &ca2->shape.line);
        }
        default:
        {
            return COLL_FRAME_CALC_UNHANDLED_TYPES;
        }
    }
}

COLL_FRAME_CALC_RET CNCFPointLine(jint * collFrame, juint axis, const jintLine * point, const jintAxPlLine * line)
{
    int frame_coll = (line->rStart.v[axis] - point->rStart.v[axis])  * point->tScale / point->sTarg.v[axis];
    printf("(%d - %d) * %d / %d = %d\n\n", line->rStart.v[axis], point->rStart.v[axis], point->tScale, point->sTarg.v[axis], frame_coll);

    // check that point actually lies inbetween the end-points of the line
    // if both positions at point `frame_coll` and `frame_coll` + 1 
    // are above or below the line then we know it doesn't hit, else it does
    RELATION relationBefore = pointLineRelation(frame_coll, (axis+1)%2, point, line);
    RELATION relationAfter = pointLineRelation(frame_coll + 1, (axis+1)%2, point, line);

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
