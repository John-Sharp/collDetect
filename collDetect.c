#include "collDetect.h"

#include <stdio.h>
#include <string.h>

typedef enum RELATION
{
    GT,
    EQ,
    LT
} RELATION;

RELATION pointLineRelation(jint frame, juint axis, const jintVecScaled * vel, const jintVec * point, const jintAxPlLine * line)
{
    jint scaledUpPoint = point->v[axis] * vel->s + frame * vel->v.v[axis];
    jint scaledUpLineBottom = line->rStart.v[axis] * vel->s;
    jint scaledUpLineTop = scaledUpLineBottom + line->length * vel->s;

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

RELATION_PAIR lineLineRelation(jint frame, juint axis, const jintVecScaled * vel, const jintAxPlLine * line1, const jintAxPlLine * line2)
{
   RELATION_PAIR ret;
   ret.pair[0] = pointLineRelation(frame, axis, vel, &line1->rStart, line2);
   jintVec temp = line1->rStart;
   temp.v[axis] += line1->length;
   ret.pair[1] = pointLineRelation(frame, axis, vel, &temp, line2);

   return ret;
}

COLL_FRAME_CALC_RET CNCFPointLine(
        jint * collFrame, juint axis, const jintVecScaled * vRel,
        const jintVec * point, const jintAxPlLine * line);
COLL_FRAME_CALC_RET CNCFLineLine(
        jint * collFrame, juint axis, const jintVecScaled * vRel, // velocity of 1 relative to 2
        const jintAxPlLine * line1, const jintAxPlLine * line2);
static COLL_FRAME_CALC_RET CNCFPointRect(
        jint * collFrame, const jintVecScaled * vrel, 
        const jintVec * point, const jintRect * rect);

static COLL_FRAME_CALC_RET calculateNextCollisionFrameOrderedInput(
        jint * collFrame, const collActor * ca1, const collActor * ca2);

COLL_FRAME_CALC_RET calculateNextCollisionFrame(
        jint * collFrame, const collActor * ca1, const collActor * ca2)
{
    if (ca1->type <= ca2->type)
        return calculateNextCollisionFrameOrderedInput(
                collFrame, ca1, ca2);
    return calculateNextCollisionFrameOrderedInput(
            collFrame, ca2, ca1);
}

static COLL_FRAME_CALC_RET calculateNextCollisionFrameOrderedInput(
        jint * collFrame, const collActor * ca1, const collActor * ca2)
{
    jintVecScaled vrel = jintVecScaledSub(&ca1->vel, &ca2->vel);
    switch (ca1->type | ca2->type)
    {
        case (COLL_ACTOR_TYPE_POINT | COLL_ACTOR_TYPE_V_LINE):
        {
            return CNCFPointLine(collFrame, 0, &vrel, &ca1->shape.point, &ca2->shape.line);
        }
        case (COLL_ACTOR_TYPE_POINT | COLL_ACTOR_TYPE_H_LINE):
        {
            return CNCFPointLine(collFrame, 1, &vrel, &ca1->shape.point, &ca2->shape.line);
        }
        case (COLL_ACTOR_TYPE_POINT | COLL_ACTOR_TYPE_RECT):
        {
            return CNCFPointRect(collFrame, &vrel, &ca1->shape.point, &ca2->shape.rect);
        }
        case (COLL_ACTOR_TYPE_V_LINE | COLL_ACTOR_TYPE_V_LINE):
        {
            return CNCFLineLine(collFrame, 0, &vrel, &ca1->shape.line, &ca2->shape.line);
        }
        case (COLL_ACTOR_TYPE_H_LINE | COLL_ACTOR_TYPE_H_LINE):
        {
            return CNCFLineLine(collFrame, 1, &vrel, &ca1->shape.line, &ca2->shape.line);
        }
        default:
        {
            return COLL_FRAME_CALC_UNHANDLED_TYPES;
        }
    }
}

jint CNCFPointInfiniteLine(juint axis, const jintVecScaled * vel, const jintVec * point, const jintVec * linePoint)
{
    return (linePoint->v[axis] - point->v[axis])  * vel->s / vel->v.v[axis];
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
        jint * collFrame, juint axis, const jintVecScaled * vrel, const jintVec * point, const jintAxPlLine * line)
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
        int c = vrel->v.v[(axis+1)%2];
        int d = vrel->v.v[axis];

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
        int c = vrel->v.v[(axis+1)%2];
        int d = vrel->v.v[axis];

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

static jint getLeadingEdge(
        const jintRect * rect, jint axis,
        const jintVecScaled * v,
        jintAxPlLine * leadingEdge)
{
    if (v->v.v[axis] == 0)
        return 0;

    jintVec leadingEdgeLocation;
    jint widthHeight[2] = {jintRectGetWidth(rect), jintRectGetHeight(rect)};

    if (v->v.v[axis] > 0)
    {
        leadingEdgeLocation = rect->bl;
    }
    else if (v->v.v[axis] < 0)
    {
        leadingEdgeLocation.v[axis] = rect->bl.v[axis] + widthHeight[axis];
        leadingEdgeLocation.v[(axis+1)%2] = rect->bl.v[(axis+1)%2];
    }
    leadingEdge->direction = (axis == 0 ? AX_PL_DIR_Y : AX_PL_DIR_X);
    leadingEdge->rStart = leadingEdgeLocation;
    leadingEdge->length = widthHeight[(axis+1)%2];

    return 1;
}

static COLL_FRAME_CALC_RET CNCFPointRect(
        jint * collFrame, const jintVecScaled * vrel, 
        const jintVec * point, const jintRect * rect)
{
    jint axis;
    for (axis = 0; axis < 2; axis++)
    {
        jintAxPlLine leadingEdge;
        if (getLeadingEdge(rect, axis, vrel, &leadingEdge))
        {
            jint collFrameTemp;
            COLL_FRAME_CALC_RET res = CNCFPointLine(
                    &collFrameTemp,
                    axis,
                    vrel,
                    point,
                    &leadingEdge);

            if (res == COLL_FRAME_CALC_OK)
            {
                *collFrame = collFrameTemp;
                return COLL_FRAME_CALC_OK;
            }
        }
    }

    return COLL_FRAME_CALC_NO_COLLISION;
}

COLL_FRAME_CALC_RET CNCFLineLine(
        jint * collFrame, juint axis, const jintVecScaled * vrel, // velocity of 1 relative to 2
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
        int c = vrel->v.v[(axis+1)%2];
        int d = vrel->v.v[axis];

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
        int c = vrel->v.v[(axis+1)%2];
        int d = vrel->v.v[axis];

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
