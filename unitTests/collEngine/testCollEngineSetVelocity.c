#include <stdio.h>
#include <jTypes.h>
#include "../../collDetect.h"
#include "mockCollisionFrameCalculate.h"
#include "testMallocTracker.h"

#include <assert.h>

#define UNORDERED_MATCH(a1, a2, b1, b2) \
    ((a1 == b1 && a2 == b2)||(a1 == b2 && a2 == b1))

typedef struct retMapNode
{
    const collActor * ca1;
    const collActor * ca2;

    COLL_FRAME_CALC_RET ret;
    jint collFrame;
} retMapNode;

static retMapNode * retMap;
static jint retMapNum = 0;

static COLL_FRAME_CALC_RET testCNCF(
        jint * collFrame, const collActor * ca1, const collActor * ca2,
        void * context)
{
    jint i;
    for (i = 0; i < retMapNum; i++)
    {
        if (UNORDERED_MATCH(
                    retMap[i].ca1, retMap[i].ca2,
                    ca1, ca2))
        {
            *collFrame = retMap[i].collFrame;
            return retMap[i].ret;
        }
    }

    return COLL_FRAME_CALC_NO_COLLISION;
}

static collActor * caExpected1 = NULL;
static collActor * caExpected2 = NULL;
static bool expectedReceived = false;
static void testHandler(collActor * ca1, collActor * ca2, const jintVec * norm)
{
    if (UNORDERED_MATCH(
                ca1, ca2,
                caExpected1, caExpected2))
    {
        expectedReceived = true; 
        jint i;
        for (i = 0; i < retMapNum; i++)
        {
            if (UNORDERED_MATCH(
                        retMap[i].ca1, retMap[i].ca2,
                        ca1, ca2))
            {
                retMap[i].ret = COLL_FRAME_CALC_NO_COLLISION;
            }
        }
        return;
    }

    assert(false);
}

// have three actors, with 1,2 colliding as the next collision. Upon call to
// 'SetVelocity' have 1,3 colliding as next collision, and ensure that
// collision handler is called as expected
void testCollEngineSetVelocityPrevCollision()
{
    collEngine * eng = createCollEngine();
    setMockCalculateNextCollisionFrame(testCNCF);

    jint catNumber = 1;
    collEngineUpsertCollGroup(eng, catNumber, catNumber, testHandler);

    // add three objects, have 1,2 collide before 1,3
    collActor ca1, ca2, ca3;
    ca1.categoryNumber = catNumber;
    ca2.categoryNumber = catNumber;
    ca3.categoryNumber = catNumber;

    retMapNode initialRets[2] = {
        {.ca1 = &ca1, .ca2 = &ca2, .ret = COLL_FRAME_CALC_OK, .collFrame = 4},
        {.ca1 = &ca1, .ca2 = &ca3, .ret = COLL_FRAME_CALC_OK, .collFrame = 6}
    };
    retMap = initialRets;
    retMapNum = 2;

    collEngineRegisterCollActor(eng, &ca1);
    collEngineRegisterCollActor(eng, &ca2);
    collEngineRegisterCollActor(eng, &ca3);

    // switch testCNCF to have 1,3 collide before 1,2
    retMapNode secondRets[2] = {
        {.ca1 = &ca1, .ca2 = &ca2, .ret = COLL_FRAME_CALC_OK, .collFrame = 6},
        {.ca1 = &ca1, .ca2 = &ca3, .ret = COLL_FRAME_CALC_OK, .collFrame = 4}
    };
    retMap = secondRets;

    // zero memory checker
    allocatorTrackerReset();

    // set velocity to some arbitrary velocity, this will cause the 
    // collisions to be recalculated
    jintVecScaled vTest = {.v = {{0,0}}, .s = 1};
    collEngineCollActorSetVelocity(eng, &ca1, &vTest);

    jint allocatedAfterCheckPoint = allocatorTrackerGetBytesAllocated() - \
                                    allocatorTrackerGetBytesFreed();

    if (allocatedAfterCheckPoint != 0)
    {
        printf("testCollEngineSetVelocity(): memory allocated after modifying "
                "velocity, when no memory should be allocated\n");
        assert(0);
    }

    caExpected1 = &ca1;
    caExpected2 = &ca3;

    expectedReceived = false;
    // ensure that the correct collision is detected at the right time
    int i = 0;
    for (i = 0; i <= 4; i++)
    {
        assert(!expectedReceived);
        collEngineProcessFrame(eng);
    }

    assert(expectedReceived);

    destroyCollEngine(eng);
}

// have two actors, with 1,2 not colliding. Upon call to 'SetVelocity' have 1,2
// colliding, and ensure that collision handler is called as expected
void testCollEngineSetVelocityNoPrevCollision()
{
    collEngine * eng = createCollEngine();
    setMockCalculateNextCollisionFrame(testCNCF);

    jint catNumber = 1;
    collEngineUpsertCollGroup(eng, catNumber, catNumber, testHandler);

    collActor ca1, ca2;

    ca1.categoryNumber = catNumber;
    ca2.categoryNumber = catNumber;

    retMapNode initialRets[1] = {
        {.ca1 = &ca1, .ca2 = &ca2, .ret = COLL_FRAME_CALC_NO_COLLISION, .collFrame = 0}
    };
    retMap = initialRets;
    retMapNum = 1;

    collEngineRegisterCollActor(eng, &ca1);
    collEngineRegisterCollActor(eng, &ca2);

    // switch testCNCF to have 1,2 collide
    retMapNode secondRets[1] = {
        {.ca1 = &ca1, .ca2 = &ca2, .ret = COLL_FRAME_CALC_OK, .collFrame = 4}
    };
    retMap = secondRets;

    // set velocity to some arbitrary velocity, this will cause the 
    // collisions to be recalculated
    jintVecScaled vTest = {.v = {{0,0}}, .s = 1};
    collEngineCollActorSetVelocity(eng, &ca1, &vTest);

    caExpected1 = &ca1;
    caExpected2 = &ca2;

    expectedReceived = false;
    // ensure that the correct collision is detected at the right time
    int i = 0;
    for (i = 0; i <= 4; i++)
    {
        assert(!expectedReceived);
        collEngineProcessFrame(eng);
    }

    assert(expectedReceived);

    destroyCollEngine(eng);
}
