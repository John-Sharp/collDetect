#include "collDetect.h"

#include <stdio.h>

typedef enum RELATION
{
    GT,
    EQ,
    LT
} RELATION;

RELATION pointLineRelationV(jint frame, const jintLine * point, const jintAxPlLine * line)
{
    jint scaledUpPointY = point->rStart.v[1] * point->tScale + frame * point->sTarg.v[1];
    jint scaledUpLineBottom = line->rStart.v[1] * point->tScale;
    jint scaledUpLineTop = scaledUpLineBottom + line->length * point->tScale;

    if (scaledUpPointY > scaledUpLineTop)
        return GT;

    if (scaledUpPointY < scaledUpLineBottom)
        return LT;

    return EQ;
}

RELATION pointLineRelationH(jint frame, const jintLine * point, const jintAxPlLine * line)
{
    jint scaledUpPointX = point->rStart.v[0] * point->tScale + frame * point->sTarg.v[0];
    jint scaledUpLineBottom = line->rStart.v[0] * point->tScale;
    jint scaledUpLineTop = scaledUpLineBottom + line->length * point->tScale;

    if (scaledUpPointX > scaledUpLineTop)
        return GT;

    if (scaledUpPointX < scaledUpLineBottom)
        return LT;

    return EQ;
}

COLL_FRAME_CALC_RET CNCFPointVline(jint * collFrame, const jintLine * point, const jintAxPlLine * line);
COLL_FRAME_CALC_RET CNCFPointHline(jint * collFrame, const jintLine * point, const jintAxPlLine * line);

COLL_FRAME_CALC_RET calculateNextCollisionFrame(jint * collFrame, const collActor * ca1, const collActor * ca2)
{
    switch (ca1->type | ca2->type)
    {
        case (COLL_ACTOR_TYPE_POINT | COLL_ACTOR_TYPE_V_LINE):
        {
            return CNCFPointVline(collFrame, &ca1->shape.point, &ca2->shape.line);
        }
        case (COLL_ACTOR_TYPE_POINT | COLL_ACTOR_TYPE_H_LINE):
        {
            return CNCFPointHline(collFrame, &ca1->shape.point, &ca2->shape.line);
        }
        default:
        {
            return COLL_FRAME_CALC_UNHANDLED_TYPES;
        }
    }
}

COLL_FRAME_CALC_RET CNCFPointVline(jint * collFrame, const jintLine * point, const jintAxPlLine * line)
{
    int frame_coll = (line->rStart.v[0] - point->rStart.v[0])  * point->tScale / point->sTarg.v[0];
    printf("(%d - %d) * %d / %d = %d\n\n", line->rStart.v[0], point->rStart.v[0], point->tScale, point->sTarg.v[0], frame_coll);

    // check that point actually lies inbetween the end-points of the line
    // if both positions at point `frame_coll` and `frame_coll` + 1 
    // are above or below the line then we know it doesn't hit, else it does
    RELATION relationBefore = pointLineRelationV(frame_coll, point, line);
    RELATION relationAfter = pointLineRelationV(frame_coll + 1, point, line);

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

COLL_FRAME_CALC_RET CNCFPointHline(jint * collFrame, const jintLine * point, const jintAxPlLine * line)
{
    int frame_coll = (line->rStart.v[1] - point->rStart.v[1])  * point->tScale / point->sTarg.v[1];
    printf("(%d - %d) * %d / %d = %d\n\n", line->rStart.v[1], point->rStart.v[1], point->tScale, point->sTarg.v[1], frame_coll);

    // check that point actually lies inbetween the end-points of the line
    // if both positions at point `frame_coll` and `frame_coll` + 1 
    // are above or below the line then we know it doesn't hit, else it does
    RELATION relationBefore = pointLineRelationH(frame_coll, point, line);
    RELATION relationAfter = pointLineRelationH(frame_coll + 1, point, line);

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
