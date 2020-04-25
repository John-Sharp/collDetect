#include "collDetect.h"
#include "collisionFrameCalculate.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

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
        collInfoList ** scheduledCollisionList);
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
    collEngineCollActorCalculateNextCollision(this, actor,
            &this->scheduledCollisions);
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

    collInfo * nsInfo = actor->nextScheduledCollision->val;
    collActor * orphanedActor;
    if (actor->categoryNumber == nsInfo->actor1->categoryNumber)
        orphanedActor = nsInfo->actor2;
    else
        orphanedActor = nsInfo->actor1;

    collEngineCollActorCalculateNextCollision(
            this, orphanedActor, &this->scheduledCollisions);
}

void collEngineProcessFrame(collEngine * eng)
{
    collEngineInternal * this = (collEngineInternal *)eng;

    // search through this->scheduledCollisions for collisions that happen at
    // this->frame
    collInfoList * schedCollN;
    collInfoList * schedCollNPrev = NULL;
    for (schedCollN = this->scheduledCollisions; schedCollN != NULL;)
    {
        if (schedCollN->val->collFrame != this->frame)
        {
            schedCollNPrev = schedCollN;
            schedCollN = schedCollN->next;
            continue;
        }

        // call collision handler
        schedCollN->val->collGroup->handler(
            schedCollN->val->actor1,
            schedCollN->val->actor2
                );

        schedCollN->val->actor1->nextScheduledCollision = NULL;
        schedCollN->val->actor2->nextScheduledCollision = NULL;

        // calculate next collision for actors putting them on list at/after
        // schedCollN->next
        collEngineCollActorCalculateNextCollision(
                this, schedCollN->val->actor1, &schedCollN->next);

        // TODO rather than dynamically allocating/releasing memory for
        // collInfo, have a pool of collInfo objects
        if (schedCollNPrev)
            schedCollNPrev->next = schedCollN->next;
        schedCollN = schedCollN->next;
        free(schedCollN);
    }

    this->frame++;
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
        const collEngineInternal * this, collActor * actor,
        collGroup * collGroup,
        collInfo * collInfo);


static void collEngineCollActorCalculateNextCollisionReturnOrphan(
        collEngineInternal * this, collActor * actor,
        collInfoList ** scheduledCollisionList,
        collActor ** orphanedActor);

static void collEngineCollActorCalculateNextCollision(
        collEngineInternal * this, collActor * actor,
        collInfoList ** scheduledCollisionList)
{
    // it is not the job of this function to deal
    // with the previous collision of the actor
    assert(actor->nextScheduledCollision == NULL);

    collActor * orphanedActor = actor;
    while (orphanedActor)
    {
        collActor * oldOrphanedActor = orphanedActor;
        orphanedActor = NULL;
        collEngineCollActorCalculateNextCollisionReturnOrphan(
                this, oldOrphanedActor, scheduledCollisionList,
                &orphanedActor);
    }
}

static void collEngineCollActorCalculateNextCollisionReturnOrphan(
        collEngineInternal * this,
        collActor * actor,
        collInfoList ** scheduledCollisionList,
        collActor ** orphanedActor)
{
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
        struct collInfo * nsColl = collInfo.actor2->nextScheduledCollision->val;
        if (nsColl->actor1 == collInfo.actor2)
        {
            *orphanedActor = nsColl->actor1;
        }
        else
        {
            *orphanedActor = collInfo.actor2;
        }

        *nsColl = collInfo;
        actor->nextScheduledCollision = collInfo.actor2->nextScheduledCollision;
        (*orphanedActor)->nextScheduledCollision = NULL;
        *scheduledCollisionList  = collInfoListNodeMv(
                *scheduledCollisionList,
                &this->scheduledCollisions,
                actor->nextScheduledCollision);
        return;
    }

    struct collInfo * nextScheduledCollision = malloc(sizeof(*nextScheduledCollision));
    assert(nextScheduledCollision);

    *nextScheduledCollision = collInfo;

    *scheduledCollisionList = collInfoListAdd(
            *scheduledCollisionList, nextScheduledCollision);

    actor->nextScheduledCollision = *scheduledCollisionList;
    collInfo.actor2->nextScheduledCollision  = *scheduledCollisionList;

}

static void collEnginePopulateCollisionInfoForActorWithGroup(
        const collEngineInternal * this, collActor * actor,
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
            actorList->val->nextScheduledCollision->val->collFrame <= collFrame)
            continue;

        collInfo->actor2 = actorList->val;
        collInfo->collFrame = collFrame;
        collInfo->collGroup = collGroup;
    }
}
