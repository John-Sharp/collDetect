#include "collDetect.h"

#include <stdio.h>
#include <string.h>

typedef enum RELATION
{
    GT,
    EQ,
    LT
} RELATION;

RELATION pointLineRelation(jint frame, juint axis, const jintVec * vel, const jintVec * point, const jintAxPlLine * line)
{
    jint scaledUpPoint = point->v[axis] * vel->scale + frame * vel->v[axis];
    jint scaledUpLineBottom = line->rStart.v[axis] * vel->scale;
    jint scaledUpLineTop = scaledUpLineBottom + line->length * vel->scale;

    if (scaledUpPoint > scaledUpLineTop)
        return GT;

    if (scaledUpPoint < scaledUpLineBottom)
        return LT;

    return EQ;
}

typedef struct RELATION_PAIR
{
    RELATION pair[2];
} RELATION_PAIR;

RELATION_PAIR lineLineRelation(jint frame, juint axis, const jintVec * vel, const jintAxPlLine * line1, const jintAxPlLine * line2)
{
   RELATION_PAIR ret;
   ret.pair[0] = pointLineRelation(frame, axis, vel, &line1->rStart, line2);
   jintVec temp = line1->rStart;
   temp.v[axis] += line1->length;
   ret.pair[1] = pointLineRelation(frame, axis, vel, &temp, line2);

   return ret;
}

COLL_FRAME_CALC_RET CNCFPointLine(
        jint * collFrame, juint axis, const jintVec * vRel,
        const jintVec * point, const jintAxPlLine * line);
COLL_FRAME_CALC_RET CNCFLineLine(
        jint * collFrame, juint axis, const jintVec * vRel, // velocity of 1 relative to 2
        const jintAxPlLine * line1, const jintAxPlLine * line2);

// TODO pass relative velocity into this function
COLL_FRAME_CALC_RET calculateNextCollisionFrame(
        jint * collFrame, const jintVec * vrel, const collActor * ca1, const collActor * ca2)
{
    switch (ca1->type | ca2->type)
    {
        case (COLL_ACTOR_TYPE_POINT | COLL_ACTOR_TYPE_V_LINE):
        {
            return CNCFPointLine(collFrame, 0, vrel, &ca1->shape.point, &ca2->shape.line);
        }
        case (COLL_ACTOR_TYPE_POINT | COLL_ACTOR_TYPE_H_LINE):
        {
            return CNCFPointLine(collFrame, 1, vrel, &ca1->shape.point, &ca2->shape.line);
        }
        case (COLL_ACTOR_TYPE_V_LINE | COLL_ACTOR_TYPE_V_LINE):
        {
            return CNCFLineLine(collFrame, 0, vrel, &ca1->shape.line, &ca2->shape.line);
        }
        default:
        {
            return COLL_FRAME_CALC_UNHANDLED_TYPES;
        }
    }
}

jint CNCFPointInfiniteLine(juint axis, const jintVec * vel, const jintVec * point, const jintVec * linePoint)
{
    return (linePoint->v[axis] - point->v[axis])  * vel->scale / vel->v[axis];
}

COLL_FRAME_CALC_RET CNCFPointLine(
        jint * collFrame, juint axis, const jintVec * vrel, const jintVec * point, const jintAxPlLine * line)
{
    // TODO neaten this up
    // TODO pass relative velocity into this function
    int frame_coll = CNCFPointInfiniteLine(axis, vrel, point, &line->rStart);
    printf("(%d - %d) * %d / %d = %d\n\n", line->rStart.v[axis], point->v[axis], vrel->scale, vrel->v[axis],
            frame_coll);

    // check that point actually lies inbetween the end-points of the line
    // if both positions at point `frame_coll` and `frame_coll` + 1 
    // are above or below the line then we know it doesn't hit, else it does
    RELATION relationBefore = pointLineRelation(frame_coll, (axis+1)%2, vrel, point, line);
    RELATION relationAfter = pointLineRelation(frame_coll + 1, (axis+1)%2, vrel, point, line);

    if (relationBefore == GT && relationAfter == GT)
    {
        return COLL_FRAME_CALC_NO_COLLISION;
    }
        
    if (relationBefore == LT && relationAfter == LT)
    {
        return COLL_FRAME_CALC_NO_COLLISION;
    }

    // TODO in cases where GT->LT and visa versa, need to compare the angle between the 
    // point before collision and after collision and the (axis+1)%2 (theta1) and the 
    // angle between the point before collision and the end of the line that is
    // passed during the collision and the (axis+1)%2 (theta2). If theta1 >
    // theta2, then there is no collision

    *collFrame = frame_coll;
    return COLL_FRAME_CALC_OK;
}

COLL_FRAME_CALC_RET CNCFLineLine(
        jint * collFrame, juint axis, const jintVec * relVel, // velocity of 1 relative to 2
        const jintAxPlLine * line1, const jintAxPlLine * line2)
{
    int frame_coll = CNCFPointInfiniteLine(axis, relVel, &line1->rStart, &line2->rStart);

    // (b, i, a)
    // bb -> bb => no collision
    // bb -> bb => no collision

    RELATION_PAIR relationBefore = lineLineRelation(
            frame_coll, (axis+1)%2, relVel, line1, line2);
    RELATION_PAIR relationAfter = lineLineRelation(
            frame_coll + 1, (axis+1)%2, relVel, line1, line2);

    RELATION_PAIR LT_LT = {{LT, LT}};
    if (memcmp(&relationBefore, &LT_LT, sizeof(RELATION_PAIR)) == 0 && memcmp(&relationAfter, &LT_LT, sizeof(RELATION_PAIR)) == 0)
    {
        return COLL_FRAME_CALC_NO_COLLISION;
    }
    RELATION_PAIR GT_GT = {{GT, GT}};
    if (memcmp(&relationBefore, &GT_GT, sizeof(RELATION_PAIR)) == 0 && memcmp(&relationAfter, &GT_GT, sizeof(RELATION_PAIR)) == 0)
    {
        return COLL_FRAME_CALC_NO_COLLISION;
    }

    return COLL_FRAME_CALC_OK;
}
