#include "../../collDetect.h"
#include <jTypes.h>
#include "mockCollisionFrameCalculate.h"
#include <stdio.h>
#include <assert.h>

typedef struct CNCFCallInfo
{
    jint called;
    jint calledA1B1;
    jint calledA1C1;

    collActor * actorA1;
    collActor * actorB1;
    collActor * actorC1;
    jint collFrameToReturnA1B1;
    COLL_FRAME_CALC_RET returnA1B1;
    jint collFrameToReturnA1C1;
    COLL_FRAME_CALC_RET returnA1C1;
} CNCFCallInfo;


#define UNORDERED_MATCH(a1, a2, b1, b2) \
    ((a1 == b1 && a2 == b2)||(a1 == b2 && a2 == b1))

static COLL_FRAME_CALC_RET testCNCF(
        jint * collFrame, const collActor * ca1, const collActor * ca2,
        void * context)
{
    CNCFCallInfo * info = (CNCFCallInfo *)context;

    info->called++;

    if (UNORDERED_MATCH(ca1, ca2, info->actorA1, info->actorB1))
    {
        info->calledA1B1++;
        *collFrame = info->collFrameToReturnA1B1;
        return info->returnA1B1;
    }
    if (UNORDERED_MATCH(ca1, ca2, info->actorA1, info->actorC1))
    {
        info->calledA1C1++;
        *collFrame = info->collFrameToReturnA1C1;
        return info->returnA1C1;
    }

    return COLL_FRAME_CALC_NO_COLLISION;
}

typedef struct testHandlerCallInfo
{
    jint called;
    collActor * ca1;
    collActor * ca2;

    collActor * actorA1;
    collActor * actorB1;
    collActor * actorC1;
    CNCFCallInfo * info;
} testHandlerCallInfo;
testHandlerCallInfo tHCallInfoAB;
testHandlerCallInfo tHCallInfoAC;

static void testHandlerAB(collActor * ca1, collActor * ca2)
{
    tHCallInfoAB.called++;
    tHCallInfoAB.ca1 = ca1;
    tHCallInfoAB.ca2 = ca2;

    if (UNORDERED_MATCH(ca1, ca2,
                tHCallInfoAB.actorA1, tHCallInfoAB.actorB1))
    {
        tHCallInfoAB.info->returnA1B1 = COLL_FRAME_CALC_NO_COLLISION;
    }
}

static void testHandlerAC(collActor * ca1, collActor * ca2)
{
    tHCallInfoAC.called++;
    tHCallInfoAC.ca1 = ca1;
    tHCallInfoAC.ca2 = ca2;

    if (UNORDERED_MATCH(ca1, ca2,
                tHCallInfoAB.actorA1, tHCallInfoAB.actorC1))
    {
        tHCallInfoAB.info->returnA1C1 = COLL_FRAME_CALC_NO_COLLISION;
    }
}

void subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5()
{
    collEngine * eng = createCollEngine();
    juint catNumA = 1;
    juint catNumB = 2;
    juint catNumC = 3;

    collActor actorA1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {.point = {{234,234}}},
        .vel = {.s = 1, .v = {{1,1}}},
        .categoryNumber = catNumA,
        .frameStart = 0,
        .collFrame =1
    };
    
    collActor actorB1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {.point = {{567,567}}},
        .vel = {.s = 1, .v = {{1,1}}},
        .categoryNumber = catNumB,
        .frameStart = 0,
        .collFrame =1
    };

    collActor actorC1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {.point = {{127,122}}},
        .vel = {.s = 1, .v = {{1,1}}},
        .categoryNumber = catNumC,
        .frameStart = 0,
        .collFrame =1
    };

    CNCFCallInfo info = {
        .called = 0,
        .calledA1B1 = 0,
        .calledA1C1 = 0,
        .actorA1 = &actorA1,
        .actorB1 = &actorB1,
        .actorC1 = &actorC1,
        .collFrameToReturnA1B1 = 12,
        .returnA1B1 = COLL_FRAME_CALC_OK,
        .collFrameToReturnA1C1 = 5,
        .returnA1B1 = COLL_FRAME_CALC_OK
    };
    setMockCalculateNextCollisionFrame(testCNCF);
    setMockCalcuateNextCollisionFrameContext(&info);

    tHCallInfoAB.called = 0;
    tHCallInfoAB.ca1 = NULL;
    tHCallInfoAB.ca2 = NULL;
    tHCallInfoAB.actorA1 = &actorA1;
    tHCallInfoAB.actorB1 = &actorB1;
    tHCallInfoAB.actorC1 = &actorC1;
    tHCallInfoAB.info = &info;
    collEngineUpsertCollGroup(eng, catNumA, catNumB, testHandlerAB);

    tHCallInfoAC.called = 0;
    tHCallInfoAC.ca1 = NULL;
    tHCallInfoAC.ca2 = NULL;
    tHCallInfoAC.actorA1 = &actorA1;
    tHCallInfoAC.actorB1 = &actorB1;
    tHCallInfoAC.actorC1 = &actorC1;
    tHCallInfoAC.info = &info;
    collEngineUpsertCollGroup(eng, catNumA, catNumC, testHandlerAC);

    collEngineRegisterCollActor(eng, &actorA1);
    collEngineRegisterCollActor(eng, &actorB1);
    collEngineRegisterCollActor(eng, &actorC1);

    int i;
    for (i = 0; i < 5; i++)
    {
        collEngineProcessFrame(eng);
    }

    if (tHCallInfoAB.called != 0 || tHCallInfoAC.called != 0)
    {
        printf("subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5(): "
                "collision handler called when not expected\n");
        assert(0);
    }

    i++;
    collEngineProcessFrame(eng);

    if (tHCallInfoAB.called != 0)
    {
        printf("subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5(): "
                "collision handler called when not expected\n");
        assert(0);
    }

    if (tHCallInfoAC.called != 1 &&
            !(UNORDERED_MATCH(tHCallInfoAC.ca1,  tHCallInfoAC.ca2,
                    &actorA1, &actorC1)))
    {
        printf("subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5(): "
                "collision handler not called with "
                "expected arguments\n");
        assert(0);
    }
    tHCallInfoAC.called = 0;

    for (; i<12; i++)
    {
        collEngineProcessFrame(eng);
    }

    if (tHCallInfoAB.called != 0 || tHCallInfoAC.called != 0)
    {
        printf("subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5(): "
                "collision handler called when not expected\n");
        assert(0);
    }

    collEngineProcessFrame(eng);

    if (tHCallInfoAC.called != 0)
    {
        printf("subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5(): "
                "collision handler called when not expected\n");
        assert(0);
    }

    if (tHCallInfoAB.called != 1 &&
            !(UNORDERED_MATCH(tHCallInfoAB.ca1,  tHCallInfoAB.ca2,
                    &actorA1, &actorB1)))
    {
        printf("subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5(): "
                "collision handler not called with "
                "expected arguments\n");
        assert(0);
    }
    tHCallInfoAB.called = 0;
}
