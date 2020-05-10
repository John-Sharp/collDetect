#include "../../collDetect.h"

#include <stdio.h>
#include <stdlib.h>

#include "testMallocTracker.h"

#include <assert.h>

static bool mallocShouldFailOnFirstAttempt()
{
    return true;
}

static void testCreateCollEngine();
static void testCollEngineCollGroupAddRm();
static void testCollEngineCollGroupRmCat();
static void testCollEngineCollActorRegisterDeregister();
static void testCollEngineProcessFrame();

int main()
{
    // tests of collEngine
    testCreateCollEngine();
    testCollEngineCollGroupAddRm();
    testCollEngineCollGroupRmCat();
    testCollEngineCollActorRegisterDeregister();

    testCollEngineProcessFrame();
}

static void populateCollList(collEngine * eng);
static void testCreateCollEngine()
{
    allocatorTrackerReset();
    collEngine * eng = createCollEngine();

    if (!eng)
    {
        printf("failed to return pointer when call to create coll engine\n");
        exit(1);
    }

    if (allocatorTrackerGetBytesAllocated() == 0)
    {
        printf("no memory allocated for creation of coll engine\n");
        exit(1);
    }

    populateCollList(eng);

    destroyCollEngine(eng);

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("memory freed upon destruction of collEngine (%u)"
                " != memory allocated upon creation of collEngine (%u)\n",
                allocatorTrackerGetBytesFreed(),
                allocatorTrackerGetBytesAllocated());
        exit(1);
    }

    allocatorTrackerReset();

    mallocShouldFail = mallocShouldFailOnFirstAttempt; 
    eng = createCollEngine();

    if (eng != NULL)
    {
        printf("when malloc fails createCollEngine returns a non-NULL pointer\n");
        exit(1);
    }

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("when malloc fails createFrameRateTracker still allocates memory\n");
        exit(1);
    }

    mallocShouldFail = NULL;
    allocatorTrackerReset();
}

static void testCollEngineCollGroupAddRm()
{
    allocatorTrackerReset();
    collEngine * eng = createCollEngine();

    jint allocatedAtCheckPointA = allocatorTrackerGetBytesAllocated() - \
                                  allocatorTrackerGetBytesFreed();

    juint categoryNumber1a = 45;
    juint categoryNumber2a = 67;
    collHandler handlerx = (void *)0xbeef;
    collEngineUpsertCollGroup(eng,
            categoryNumber1a, categoryNumber2a, 
            handlerx);

    juint categoryNumber1b = 84;
    juint categoryNumber2b = 67;
    collHandler handlerb = (void *)0xdeed;
    collEngineUpsertCollGroup(eng,
            categoryNumber1b, categoryNumber2b, 
            handlerb);

    jint allocatedAtCheckPointB = allocatorTrackerGetBytesAllocated() - \
                                 allocatorTrackerGetBytesFreed();

    collHandler handlera = (void *)0xabcd;
    collEngineUpsertCollGroup(eng,
            categoryNumber2a, categoryNumber1a, 
            handlera);

    jint allocatedAfterCheckPoint = allocatorTrackerGetBytesAllocated() - \
                                    allocatorTrackerGetBytesFreed();
    if (allocatedAfterCheckPoint != allocatedAtCheckPointB)
    {
        printf("testCollEngineCollGroupAddRm(): memory allocated when "
                "modification to coll group list should be just "
                "an update\n");
        assert(0);
    }

    collEngineCollGroupRm(eng,
            categoryNumber2b, categoryNumber1b);
    collEngineCollGroupRm(eng,
            categoryNumber1a, categoryNumber2a);

    allocatedAfterCheckPoint = allocatorTrackerGetBytesAllocated() - \
                                    allocatorTrackerGetBytesFreed();
    if (allocatedAfterCheckPoint != allocatedAtCheckPointA)
    {
        printf("testCollEngineCollGroupAddRm(): memory allocated when "
                "coll group list should be empty\n");
        assert(0);
    }

    destroyCollEngine(eng);
}

static void testCollEngineCollGroupRmCat()
{
    collEngine * eng = createCollEngine();
    juint catToRm = 67;

    allocatorTrackerReset();

    collEngineUpsertCollGroup(eng,
            catToRm, 45, (void *)0xbeef);

    collEngineUpsertCollGroup(eng,
            12, catToRm, (void *)0xbeef);

    jint allocatedAtCheckPoint = allocatorTrackerGetBytesAllocated();

    collEngineUpsertCollGroup(eng,
            11, 14, (void *)0xbeef);

    jint expectedMemoryLeftAllocated = allocatorTrackerGetBytesAllocated() \
                                       - allocatedAtCheckPoint;

    collEngineUpsertCollGroup(eng,
            catToRm, 84, (void *)0xbaef);

    jint expectedMemoryFreed = allocatorTrackerGetBytesAllocated() - \
                           expectedMemoryLeftAllocated;

    collEngineCollGroupRmCat(eng, catToRm);

    if (allocatorTrackerGetBytesFreed() != expectedMemoryFreed)
    {
        printf("testCollEngineCollGroupRmCat(): memory freed when removing "
                "category does not equal expected\n");
        assert(0);
    }
}

static void testCollEngineCollActorRegisterDeregister()
{
    collEngine * eng = createCollEngine();

    allocatorTrackerReset();

    collActor ca = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {.point = {{10,10}}},
        .vel = {.s = 1, .v = {{1,1}}},
        .categoryNumber = 28,
        .frameStart = 0,
        .collFrame =1
    };

    collEngineRegisterCollActor(eng, &ca);
    collEngineDeregisterCollActor(eng, &ca);

    if (allocatorTrackerGetBytesFreed() != allocatorTrackerGetBytesAllocated())
    {
        printf("testCollEngineCollActorRegisterDeregister(): after registering and "
                "deregistering collActor memory allocated does not equal memory freed\n");
        assert(0);
    }

    allocatorTrackerReset();

    collEngineRegisterCollActor(eng, &ca);
    collEngineRegisterCollActor(eng, &ca);

    collEngineDeregisterCollActor(eng, &ca);

    if (allocatorTrackerGetBytesFreed() != allocatorTrackerGetBytesAllocated())
    {
        printf("testCollEngineCollActorRegisterDeregister(): after registering same actor "
                "twice and then deregistering collActor memory allocated does not equal "
                "memory freed\n");
        assert(0);
    }

    // test deregistering non-registered actor
    collEngineDeregisterCollActor(eng, &ca);

    collActor ca2 = ca;

    collActor ca3 = ca;
    ca3.categoryNumber = 29;

    allocatorTrackerReset();

    collEngineRegisterCollActor(eng, &ca);
    collEngineRegisterCollActor(eng, &ca2);
    collEngineRegisterCollActor(eng, &ca3);
    collEngineDeregisterCollActor(eng, &ca3);
    collEngineDeregisterCollActor(eng, &ca2);
    collEngineDeregisterCollActor(eng, &ca);

    if (allocatorTrackerGetBytesFreed() != allocatorTrackerGetBytesAllocated())
    {
        printf("testCollEngineCollActorRegisterDeregister(): after registering multiple actors "
                "and then deregistering, collActor memory allocated does not equal "
                "memory freed\n");
        assert(0);
    }
    destroyCollEngine(eng);

    allocatorTrackerReset();

    eng = createCollEngine();

    collEngineRegisterCollActor(eng, &ca);
    collEngineRegisterCollActor(eng, &ca2);
    collEngineRegisterCollActor(eng, &ca3);

    destroyCollEngine(eng);

    if (allocatorTrackerGetBytesFreed() != allocatorTrackerGetBytesAllocated())
    {
        printf("testCollEngineCollActorRegisterDeregister(): after registering multiple actors "
                "and then destroying engine, memory allocated does not equal memory freed\n");
        assert(0);
    }
}

void subTestObjA1ObjB1Frame0();
void subTestObjA1ObjB1Frame12();
void subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5();
void subTestProcessFrameScenario(const char * scenario);
static void testCollEngineProcessFrame()
{
    subTestProcessFrameScenario("1,1:(0,0)(1,0)0_0(0,0)(1,0)");
    subTestProcessFrameScenario("1,1:(0,0)(1,0)12_12(0,0)(1,0)");
    subTestProcessFrameScenario("5,5:(0,0)(1,0)12_12(0,0)(1,0)");
    subTestProcessFrameScenario("5,5:(0,3)(1,2)12_12(0,3)(1,2)");
    subTestProcessFrameScenario("5,5,1:(0,0)(1,0)12_12(0,0)(1,0)");

    subTestProcessFrameScenario("1,1,1:(0,0)(1,0)12,12(0,0)(2,0)5,_5(0,0)(2,0)12(0,0)(1,0)");

    subTestProcessFrameScenario("1,1:(0,0)(1,0)12,15_12(0,0)(1,0)15(0,0)(1,0)");

    subTestProcessFrameScenario("1,1,1:(0,0)(1,0)12,,12(0,0)(2,0),12_12(0,0)(1,0)12(0,0)(2,0)12(0,0)(1,0)");

    subTestProcessFrameScenario("1,1,1,1:(0,0)(1,0)12(0,0)(2,0),12(0,0)(3,0),,12_12(0,0)(1,0)12(0,0)(2,0)12(0,0)(3,0)");
}

static void populateCollList(collEngine * eng)
{
    juint categoryNumber1 = 45;
    juint categoryNumber2 = 67;
    collHandler handler = (void *)0xbeef;
    collEngineUpsertCollGroup(eng,
            categoryNumber1, categoryNumber2, 
            handler);

    categoryNumber1 = 5;
    categoryNumber2 = 7;
    handler = (void *)0xbeff;
    collEngineUpsertCollGroup(eng,
            categoryNumber1, categoryNumber2, 
            handler);

    categoryNumber1 = 8;
    categoryNumber2 = 9;
    handler = (void *)0xbaff;
    collEngineUpsertCollGroup(eng,
            categoryNumber1, categoryNumber2, 
            handler);
}
