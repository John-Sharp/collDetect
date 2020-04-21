#include "collDetect.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static COLL_FRAME_CALC_RET calculateNextCollisionFrameOrderedInput(
        jint * collFrame, const collActor * ca1, const collActor * ca2);

typedef struct collGroup 
{
    juint categoryNumber1;
    juint categoryNumber2;
    collHandler handler;
} collGroup;

#include "./listHeaders/collActorList.h"
#include "./listCode/collActorList.inc"

typedef struct collActorCategoryNumberGrouped
{
    juint categoryNumber;
    collActorList * actorList;
} collActorCategoryNumberGrouped;

#include "./listHeaders/collActorCategoryNumberGroupedList.h"
#include "./listCode/collActorCategoryNumberGroupedList.inc"

#include "./listHeaders/collGroupList.h"
#include "./listCode/collGroupList.inc"

typedef struct collInfo
{
    collActor * actor1;
    collActor * actor2;

    jint collFrame;

    collGroup * collGroup;
} collInfo;

#include "./listHeaders/collInfoList.h"
#include "./listCode/collInfoList.inc"

typedef struct collEngineInternal
{
    collEngine collEngine;

    jint frame;

    collGroupList * collGroupList;

    collActorCategoryNumberGroupedList * registeredActors;
    collInfoList * scheduledCollisions;
} collEngineInternal;

collEngine * createCollEngine()
{
    collEngineInternal * eng = malloc(sizeof(*eng));
    if (!eng)
        return NULL;

    eng->frame = 0;
    eng->collGroupList = NULL;
    eng->registeredActors = NULL;
    eng->scheduledCollisions = NULL;

    return &eng->collEngine;
}

static void freeCollGroupList(collGroupList * list);
static void freeCollActorCategoryNumberGroupedList(
    collActorCategoryNumberGroupedList * list);
void destroyCollEngine(collEngine * eng)
{
    collEngineInternal * this = (collEngineInternal *)eng;

    freeCollGroupList(this->collGroupList);
    freeCollActorCategoryNumberGroupedList(
            this->registeredActors);

    free(this);
}

static bool collGroupKeyCmp(const collGroup * a, const collGroup * b);
static bool collGroupCatCmp(const collGroup * a, const collGroup * b);
void collEngineUpsertCollGroup(collEngine * eng,
        juint categoryNumber1, juint categoryNumber2, 
        collHandler handler)
{
    collEngineInternal * this = (collEngineInternal *)eng;

    collGroup * cg = malloc(sizeof(*cg));
    cg->categoryNumber1 = categoryNumber1 <= categoryNumber2 ? \
                            categoryNumber1 : categoryNumber2;
    cg->categoryNumber2 = categoryNumber1 <= categoryNumber2 ? \
                            categoryNumber2 : categoryNumber1;
    cg->handler = handler;

    collGroup * cgOld;
    this->collGroupList = collGroupListUpsert(
            this->collGroupList, cg, collGroupKeyCmp, &cgOld);
    if (cgOld)
        free(cgOld);
}

void collEngineCollGroupRm(collEngine * eng,
        juint categoryNumber1, juint categoryNumber2)
{
    collEngineInternal * this = (collEngineInternal *)eng;
    collGroup valToRemove = {
        .categoryNumber1 = categoryNumber1 <= \
                         categoryNumber2 ? \
                            categoryNumber1 :\
                            categoryNumber2,
        .categoryNumber2 = categoryNumber1 <= \
                           categoryNumber2 ? \
                        categoryNumber2 :\
                            categoryNumber1,
        .handler = NULL
    };

    collGroup * removedVal = NULL;
    this->collGroupList = collGroupListRm(
            this->collGroupList, &valToRemove,
            collGroupKeyCmp, &removedVal);

    if (removedVal)
    {
        free(removedVal);
    }
}

void collEngineCollGroupRmCat(collEngine * eng,
        juint categoryNumber)
{
    collEngineInternal * this = (collEngineInternal *)eng;
    collGroup valToRemove = {
        .categoryNumber1 = categoryNumber,
        .categoryNumber2 = 0,
        .handler = NULL
    };

    collGroup * removedVal = NULL;
    do
    {
        this->collGroupList = collGroupListRm(
                this->collGroupList, &valToRemove,
                collGroupCatCmp, &removedVal);

        if (removedVal)
        {
            free(removedVal);
        }
    } while (removedVal);
}

static void getOrCreateRegisteredActorCategory(collEngineInternal * this,
        juint categoryNumber, collActorCategoryNumberGrouped ** category);
static bool collActorListPtrCmp(const collActor * a, const collActor * b)
{
    if (a == b)
        return true;

    return false;
}

static void collEngineCollActorCalculateNextCollision(
        collEngineInternal * this, collActor * actor,
        collActor ** orphanedActor);
void collEngineRegisterCollActor(collEngine * eng,
        collActor * actor)
{
    collEngineInternal * this = (collEngineInternal *)eng;
    collActorCategoryNumberGrouped * category;

    getOrCreateRegisteredActorCategory(this,
        actor->categoryNumber, &category);

    if (!collActorListSearch(
                category->actorList, actor, collActorListPtrCmp))
    {
        category->actorList = collActorListAdd(
                category->actorList, actor);
    }

    actor->nextScheduledCollision = NULL;
    collActor * orphanedActor = NULL;
    collEngineCollActorCalculateNextCollision(
            this, actor, &orphanedActor);

    while (orphanedActor)
    {
        collActor * oldOrphanedActor = orphanedActor;
        orphanedActor = NULL;
        collEngineCollActorCalculateNextCollision(
                this, oldOrphanedActor, &orphanedActor);
    }
}

static bool collActorCategoryNumberGroupedCatNumCmp(
    const collActorCategoryNumberGrouped * a,
    const collActorCategoryNumberGrouped * b);
static void rmRegisteredActorCategory(
        collEngineInternal * this, juint categoryNumber);

void collEngineDeregisterCollActor(collEngine * eng,
        collActor * actor)
{
    collEngineInternal * this = (collEngineInternal *)eng;

    collActorCategoryNumberGrouped target = {.categoryNumber = 
        actor->categoryNumber};
    collActorCategoryNumberGroupedList * node = \
        collActorCategoryNumberGroupedListSearch(
                this->registeredActors,
                &target,
                collActorCategoryNumberGroupedCatNumCmp);

    if (!node)
        return;

    collActorCategoryNumberGrouped * category = node->val;

    category->actorList = collActorListRm(category->actorList,
            actor, collActorListPtrCmp, NULL);

    if (!category->actorList)
        rmRegisteredActorCategory(this, actor->categoryNumber);

    if (!actor->nextScheduledCollision)
        return;

    collInfo * nsInfo = actor->nextScheduledCollision;
    collActor * orphanedActor;
    if (actor->categoryNumber == nsInfo->actor1->categoryNumber)
        orphanedActor = nsInfo->actor2;
    else
        orphanedActor = nsInfo->actor1;

    while (orphanedActor)
    {
        collActor * oldOrphanedActor = orphanedActor;
        orphanedActor = NULL;
        collEngineCollActorCalculateNextCollision(
                this, oldOrphanedActor, &orphanedActor);
    }
}

COLL_FRAME_CALC_RET calculateNextCollisionFrame(
        jint * collFrame, const collActor * ca1, const collActor * ca2)
{
    if (ca1->type <= ca2->type)
        return calculateNextCollisionFrameOrderedInput(
                collFrame, ca1, ca2);
    return calculateNextCollisionFrameOrderedInput(
            collFrame, ca2, ca1);
}

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
static COLL_FRAME_CALC_RET CNCFLineRect(
        jint * collFrame, jint axis, const jintVecScaled * vrel, 
        const jintAxPlLine * line, const jintRect * rect);
static COLL_FRAME_CALC_RET CNCFRectRect(
        jint * collFrame, const jintVecScaled * vrel,
        const jintRect * rect1, const jintRect * rect2);

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
        case (COLL_ACTOR_TYPE_V_LINE | COLL_ACTOR_TYPE_RECT):
        {
            return CNCFLineRect(collFrame, 0, &vrel, &ca1->shape.line, &ca2->shape.rect);
        }
        case (COLL_ACTOR_TYPE_H_LINE | COLL_ACTOR_TYPE_H_LINE):
        {
            return CNCFLineLine(collFrame, 1, &vrel, &ca1->shape.line, &ca2->shape.line);
        }
        case (COLL_ACTOR_TYPE_H_LINE | COLL_ACTOR_TYPE_RECT):
        {
            return CNCFLineRect(collFrame, 1, &vrel, &ca1->shape.line, &ca2->shape.rect);
        }
        case (COLL_ACTOR_TYPE_RECT | COLL_ACTOR_TYPE_RECT):
        {
            return CNCFRectRect(collFrame, &vrel, &ca1->shape.rect, &ca2->shape.rect);
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

static COLL_FRAME_CALC_RET CNCFLineRect(
        jint * collFrame, jint axis, const jintVecScaled * vrel, 
        const jintAxPlLine * line, const jintRect * rect)
{
    jintAxPlLine leadingEdge;
    if (getLeadingEdge(rect, axis, vrel, &leadingEdge))
    {
        return CNCFLineLine(
                collFrame, axis, vrel,
                line, &leadingEdge);
    }
    return COLL_FRAME_CALC_NO_COLLISION;
}

static COLL_FRAME_CALC_RET CNCFRectRect(
        jint * collFrame, const jintVecScaled * vrel,
        const jintRect * rect1, const jintRect * rect2)
{
    jint axis;

    jintVecScaled vrelReversed = *vrel;
    vrelReversed.v.v[0] *= -1;
    vrelReversed.v.v[1] *= -1;

    for (axis = 0; axis < 2; axis++)
    {
        jintAxPlLine leadingEdge[2];
        if (!getLeadingEdge(rect2, axis, vrel, &leadingEdge[1]))
            continue;
        if (!getLeadingEdge(rect1, axis, &vrelReversed, &leadingEdge[0]))
            continue;
        if (CNCFLineLine(collFrame, axis, vrel,
                    &leadingEdge[0], &leadingEdge[1]) == COLL_FRAME_CALC_OK)
        {
            return COLL_FRAME_CALC_OK;
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

static bool collGroupKeyCmp(const collGroup * a, const collGroup * b)
{
    if (a->categoryNumber1 != b->categoryNumber1)
        return false;
    if (a->categoryNumber2 != b->categoryNumber2)
        return false;

    return true;
}

static bool collGroupCatCmp(const collGroup * a, const collGroup * b)
{
    if (a->categoryNumber1 == b->categoryNumber1)
        return true;
    if (a->categoryNumber1 == b->categoryNumber2)
        return true;

    return false;
}

static void freeCollGroupList(collGroupList * list)
{
    collGroupList * lp;
    for (lp = list;lp != NULL;)
    {
        collGroupList * lp2 = lp->next;
        free(lp->val);
        free(lp);
        lp = lp2;
    }
}

static void freeCollActorCategoryNumberGrouped(
    collActorCategoryNumberGrouped * group);
static void freeCollActorCategoryNumberGroupedList(
    collActorCategoryNumberGroupedList * list)
{
    collActorCategoryNumberGroupedList * lp;
    for (lp = list;lp != NULL;)
    {
        collActorCategoryNumberGroupedList * lp2 = lp->next;

        freeCollActorCategoryNumberGrouped(lp->val);
        free(lp);
        lp = lp2;
    }
}

static void freeCollActorCategoryNumberGrouped(
    collActorCategoryNumberGrouped * group)
{
    collActorList * alp;

    for (alp = group->actorList; alp != NULL;)
    {
        collActorList * alp2 = alp->next;

        free(alp);
        alp = alp2;
    }

    free(group);
}

static void getOrCreateRegisteredActorCategory(collEngineInternal * this,
        juint categoryNumber, collActorCategoryNumberGrouped ** category)
{
    collActorCategoryNumberGrouped target = {.categoryNumber = 
        categoryNumber};
    collActorCategoryNumberGroupedList * node = \
        collActorCategoryNumberGroupedListSearch(
            this->registeredActors,
            &target,
            collActorCategoryNumberGroupedCatNumCmp);

    if (node)
    {
        *category = node->val;
        return;
    }

    collActorCategoryNumberGrouped * newCategory = \
        malloc(sizeof(*newCategory));
    assert(newCategory);

    newCategory->categoryNumber = categoryNumber;
    newCategory->actorList = NULL;

    this->registeredActors = collActorCategoryNumberGroupedListAdd(
        this->registeredActors, newCategory);

    *category = newCategory;
    return;
}

static void rmRegisteredActorCategory(
        collEngineInternal * this, juint categoryNumber)
{
    collActorCategoryNumberGrouped target = {.categoryNumber = 
        categoryNumber};

    collActorCategoryNumberGrouped * oldNode;
    this->registeredActors = collActorCategoryNumberGroupedListRm(
            this->registeredActors, 
            &target,
            collActorCategoryNumberGroupedCatNumCmp,
            &oldNode);

    if (oldNode)
        freeCollActorCategoryNumberGrouped(oldNode);
}

static bool collActorCategoryNumberGroupedCatNumCmp(
    const collActorCategoryNumberGrouped * a,
    const collActorCategoryNumberGrouped * b)
{
    if (a->categoryNumber == b->categoryNumber)
        return true;
    return false;
}

static void collEnginePopulateCollisionInfoForActorWithGroup(
        collEngineInternal * this, collActor * actor,
        collGroup * collGroup,
        collInfo * collInfo);

static void collEngineCollActorCalculateNextCollision(
        collEngineInternal * this, collActor * actor,
        collActor ** orphanedActor)
{
    assert(actor->nextScheduledCollision == NULL);
    *orphanedActor = NULL;

    // partially initialise a collInfo struct.
    // collEnginePopulateCollisionInfoForActorWithGroup will
    // only fill this in if frame is >= the engine's
    // frame
    collInfo collInfo = {
        .actor1 = actor,
        .actor2 = NULL,
        .collFrame = this->frame - 1,
        .collGroup = NULL
    };

    // search and retrieve all collGroups containing actor->categoryNumber
    collGroup target = {
        .categoryNumber1 = actor->categoryNumber,
        .categoryNumber2 = 0,
        .handler = NULL
    };
    collGroupList * searchResult = this->collGroupList;
    while (
        (searchResult = collGroupListSearch(
                searchResult, &target, collGroupCatCmp)))
    {
        collEnginePopulateCollisionInfoForActorWithGroup(
                this, actor, searchResult->val,
                &collInfo);
        searchResult = searchResult->next;
    }

    if (!collInfo.actor2)
    {
        return;
    }

    if (collInfo.actor2->nextScheduledCollision)
    {
        struct collInfo * nsColl = collInfo.actor2->nextScheduledCollision;
        if (nsColl->actor1 == collInfo.actor2)
        {
            *orphanedActor = nsColl->actor1;
        }
        else
        {
            *orphanedActor = collInfo.actor2;
        }

        *nsColl = collInfo;
        actor->nextScheduledCollision = nsColl;
        (*orphanedActor)->nextScheduledCollision = NULL;
        // TODO probably want to reinsert this in list
        // in ordered way
        return;
    }

    struct collInfo * nextScheduledCollision = malloc(sizeof(*nextScheduledCollision));
    assert(nextScheduledCollision);

    *nextScheduledCollision = collInfo;
    actor->nextScheduledCollision = nextScheduledCollision;
    collInfo.actor2->nextScheduledCollision  = nextScheduledCollision;

    this->scheduledCollisions = collInfoListAdd(this->scheduledCollisions, nextScheduledCollision);
}

static void collEnginePopulateCollisionInfoForActorWithGroup(
        collEngineInternal * this, collActor * actor,
        collGroup * collGroup,
        collInfo * collInfo)
{
    collActorCategoryNumberGrouped target;
    if (actor->categoryNumber == collGroup->categoryNumber1)
        target.categoryNumber = collGroup->categoryNumber2;
    else
        target.categoryNumber = collGroup->categoryNumber1;

    collActorCategoryNumberGroupedList * actorGroupList = \
        collActorCategoryNumberGroupedListSearch(
            this->registeredActors, &target, 
            collActorCategoryNumberGroupedCatNumCmp);
    if (!actorGroupList)
        return;

    collActorList * actorList = actorGroupList->val->actorList;

    for (; actorList; actorList = actorList->next)
    {
        if (actorList->val == actor)
            continue;

        // for each of these, CNCF, keep a record of lowest CF,
        // that is still greater or equal to current frame of engine
        // and where partner in collision isn't due to collide with anything
        // before CF
        // if it is due to collide with something afterwards, then use that 
        // objects' collisionInfo, and return orphaned
        // actor
        jint collFrame;
        COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(
                &collFrame, actor, actorList->val);
        if (ret == COLL_FRAME_CALC_NO_COLLISION)
            continue;
        if (ret == COLL_FRAME_CALC_UNHANDLED_TYPES)
        {
            printf("warning! Ignoring collision between unhandled types\n");
            continue;
        }
        if (collFrame <= collInfo->collFrame)
            continue;

        if (actorList->val->nextScheduledCollision && \
            actorList->val->nextScheduledCollision->collFrame <= collFrame)
            continue;

        collInfo->actor2 = actorList->val;
        collInfo->collFrame = collFrame;
        collInfo->collGroup = collGroup;
    }
}
