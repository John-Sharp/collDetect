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
        case (COLL_ACTOR_TYPE_H_LINE | COLL_ACTOR_TYPE_H_LINE):
        {
            return CNCFLineLine(collFrame, 1, vrel, &ca1->shape.line, &ca2->shape.line);
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

// given two right angled triangles with orthogonal sides a,b and c,d
// respectively, return true if the angle between a and the hypotenuse is
// greater than the angle between d and the hypotenuse
static int angle1GTangle2(int a, int b, int c, int d)
{
    a = a > 0 ? a : -1*a;
    b = b > 0 ? b : -1*b;
    c = c > 0 ? c : -1*c;
    d = d > 0 ? d : -1*d;

    if (b*c == d*a)
        return 0;

    if (b*c > d*a)
        return 1;

    return -1;
}

COLL_FRAME_CALC_RET CNCFPointLine(
        jint * collFrame, juint axis, const jintVec * vrel, const jintVec * point, const jintAxPlLine * line)
{
    int frame_coll = CNCFPointInfiniteLine(axis, vrel, point, &line->rStart);

    // check that point actually lies inbetween the end-points of the line
    // if both positions at point `frame_coll` and `frame_coll` + 1 
    // are above or below the line then we know it doesn't hit, else it might
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

    if (relationBefore == EQ && relationAfter == EQ)
    {
        *collFrame = frame_coll;
        return COLL_FRAME_CALC_OK;
    }

    // in cases where GT->(LT||EQ) and LT->(GT||EQ), need to compare the angle
    // between the point before collision and after collision and the
    // (axis+1)%2 (:= theta1) and the angle between the point before collision
    // and the end of the line that is passed during the collision and the
    // (axis+1)%2 (:= theta2). If theta1 > theta2, then there is no collision
    if (relationBefore == GT || relationAfter == GT)
    {
        int a = (line->rStart.v[(axis+1)%2] + line->length) - point->v[(axis+1)%2];
        int b = line->rStart.v[axis] - point->v[axis];
        int c = vrel->v[(axis+1)%2];
        int d = vrel->v[axis];

        int theta1GTtheta2 = angle1GTangle2(a, b, c, d);

        if (relationBefore == GT)
        {
            if (theta1GTtheta2 < 0)
            {
                return COLL_FRAME_CALC_NO_COLLISION;
            }
        }
        else
        {
            if (theta1GTtheta2 > 0)
            {
                return COLL_FRAME_CALC_NO_COLLISION;
            }
        }
    }
    if (relationBefore == LT  || relationAfter == LT)
    {
        int a = line->rStart.v[(axis+1)%2] - point->v[(axis+1)%2];
        int b = line->rStart.v[axis] - point->v[axis];
        int c = vrel->v[(axis+1)%2];
        int d = vrel->v[axis];

        int theta1GTtheta2 = angle1GTangle2(a, b, c, d);

        if (relationBefore == LT)
        {
            if (theta1GTtheta2 < 0)
            {
                return COLL_FRAME_CALC_NO_COLLISION;
            }
        }
        else
        {
            if (theta1GTtheta2 > 0)
            {
                return COLL_FRAME_CALC_NO_COLLISION;
            }
        }
    }

    *collFrame = frame_coll;
    return COLL_FRAME_CALC_OK;
}

COLL_FRAME_CALC_RET CNCFLineLine(
        jint * collFrame, juint axis, const jintVec * vrel, // velocity of 1 relative to 2
        const jintAxPlLine * line1, const jintAxPlLine * line2)
{
    int frame_coll = CNCFPointInfiniteLine(axis, vrel, &line1->rStart, &line2->rStart);

    RELATION_PAIR relationBefore = lineLineRelation(
            frame_coll, (axis+1)%2, vrel, line1, line2);
    RELATION_PAIR relationAfter = lineLineRelation(
            frame_coll + 1, (axis+1)%2, vrel, line1, line2);

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

    if (relationBefore.pair[0] == EQ && relationAfter.pair[0] == EQ)
    {
        *collFrame = frame_coll;
        return COLL_FRAME_CALC_OK;
    }

    if (relationBefore.pair[1] == EQ && relationAfter.pair[1] == EQ)
    {
        *collFrame = frame_coll;
        return COLL_FRAME_CALC_OK;
    }

    // in cases where there is no edge collision between the bottom of line1
    // and the top of line2 there can be no collision at all
    if (relationBefore.pair[0] == GT || relationAfter.pair[0] == GT)
    {
        int a = (line2->rStart.v[(axis+1)%2] + line2->length) - line1->rStart.v[(axis+1)%2];
        int b = line2->rStart.v[axis] - line1->rStart.v[axis];
        int c = vrel->v[(axis+1)%2];
        int d = vrel->v[axis];

        int theta1GTtheta2 = angle1GTangle2(a, b, c, d);

        if (relationBefore.pair[0] == GT)
        {
            if (theta1GTtheta2 < 0)
            {
                return COLL_FRAME_CALC_NO_COLLISION;
            }
        }
        else
        {
            if (theta1GTtheta2 > 0)
            {
                return COLL_FRAME_CALC_NO_COLLISION;
            }
        }
    }

    // in cases where there is no edge collision between the top of line1
    // and the bottom of line2 there can be no collision at all
    if (relationBefore.pair[1] == LT || relationAfter.pair[1] == LT)
    {
        int a = line2->rStart.v[(axis+1)%2] - (line1->rStart.v[(axis+1)%2] + line1->length);
        int b = line2->rStart.v[axis] - line1->rStart.v[axis];
        int c = vrel->v[(axis+1)%2];
        int d = vrel->v[axis];

        int theta1GTtheta2 = angle1GTangle2(a, b, c, d);

        if (relationBefore.pair[1] == LT)
        {
            if (theta1GTtheta2 < 0)
            {
                return COLL_FRAME_CALC_NO_COLLISION;
            }
        }
        else
        {
            if (theta1GTtheta2 > 0)
            {
                return COLL_FRAME_CALC_NO_COLLISION;
            }
        }
    }

    *collFrame = frame_coll;
    return COLL_FRAME_CALC_OK;
}
