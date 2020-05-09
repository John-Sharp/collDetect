#include "../../collDetect.h"
#include <jTypes.h>
#include "mockCollisionFrameCalculate.h"
#include <stdio.h>
#include <assert.h>

typedef struct CNCFCallInfo
{
    jint called;
    const collActor * ca1;
    const collActor * ca2;

    jint collFrameToReturn;
} CNCFCallInfo;

static COLL_FRAME_CALC_RET testCNCF(
        jint * collFrame, const collActor * ca1, const collActor * ca2,
        void * context)
{
    CNCFCallInfo * info = (CNCFCallInfo *)context;

    info->called++;
    info->ca1 = ca1;
    info->ca2 = ca2;

    *collFrame = info->collFrameToReturn;
    return COLL_FRAME_CALC_OK;
}

typedef struct testHandlerCallInfo
{
    jint called;
    collActor * ca1;
    collActor * ca2;
} testHandlerCallInfo;
testHandlerCallInfo tHCallInfo;

static void testHandler(collActor * ca1, collActor * ca2)
{
    tHCallInfo.called++;
    tHCallInfo.ca1 = ca1;
    tHCallInfo.ca2 = ca2;
    setMockCalculateNextCollisionFrame(NULL);
}

static void testTemplateObjA1ObjB1FrameX(
        const char * testName, jint frame)
{
    // Create object of categoryNumber 'A', object of categoryNumber 'B',
    // calculate their collision frame for frame `frame`. Verify that collision
    // frame calculation function is called and that handler is called on
    // `frame+1`th call of process frame
    collEngine * eng = createCollEngine();
    juint catNumA = 1;
    juint catNumB = 2;

    collActor actorA = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {.point = {{234,234}}},
        .vel = {.s = 1, .v = {{1,1}}},
        .categoryNumber = catNumA,
        .frameStart = 0,
        .collFrame =1
    };
    
    collActor actorB = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {.point = {{567,567}}},
        .vel = {.s = 1, .v = {{1,1}}},
        .categoryNumber = catNumB,
        .frameStart = 0,
        .collFrame =1
    };

    CNCFCallInfo info = {.called = 0, .collFrameToReturn=frame};
    setMockCalculateNextCollisionFrame(testCNCF);
    setMockCalcuateNextCollisionFrameContext(&info);

    collEngineUpsertCollGroup(eng, catNumA, catNumB, testHandler);
    collEngineRegisterCollActor(eng, &actorA);
    collEngineRegisterCollActor(eng, &actorB);

    if (info.called != 1)
    {
        printf("%s: calculate next collision frame "
                "not called expected number of times\n",
                testName);
        assert(0);
    }

    jint expectedArguments = 0;
    if (info.ca1 == &actorA)
    {
        if (info.ca2 == &actorB)
        {
            expectedArguments = 1;
        }
    }
    else if (info.ca1 == &actorB)
    {
        if (info.ca2 == &actorA)
        {
            expectedArguments = 1;
        }
    }

    if (!expectedArguments)
    {
        printf("%s: calculate next collision frame "
                "not called with expected arguments\n",
                testName);
        assert(0);
    }

    tHCallInfo.called = 0;

    int i;
    for (i = 0; i < frame; i++)
    {
        collEngineProcessFrame(eng);

        if (tHCallInfo.called != 0)
        {
            printf("%s: collision handler "
                    "called when not expected\n",
                    testName);
            assert(0);
        }
    }
    collEngineProcessFrame(eng);

    if (tHCallInfo.called != 1)
    {
        printf("%s: collision handler "
                "not called when expected\n",
                testName);
        assert(0);
    }

    expectedArguments = 0;
    if (tHCallInfo.ca1 == &actorA)
    {
        if (tHCallInfo.ca2 == &actorB)
        {
            expectedArguments = 1;
        }
    }
    else if (tHCallInfo.ca1 == &actorB)
    {
        if (tHCallInfo.ca2 == &actorA)
        {
            expectedArguments = 1;
        }
    }

    if (!expectedArguments)
    {
        printf("%s: collision handler "
                "not called with expected arguments\n",
                testName);
        assert(0);
    }

    tHCallInfo.called = 0;
    collEngineProcessFrame(eng);
    collEngineProcessFrame(eng);
    collEngineProcessFrame(eng);

    if (tHCallInfo.called != 0)
    {
        printf("%s: collision handler "
                "called after collision shoud've been resolved\n",
                testName);
        assert(0);
    }
}

void subTestObjA1ObjB1Frame0()
{
    // Create object of categoryNumber 'A', object of categoryNumber 'B',
    // calculate their collision frame for frame 0. Verify that collision frame
    // calculation function is called and that handler is called on 1st call
    // of process frame
    testTemplateObjA1ObjB1FrameX("subTestObjA1ObjB1Frame0()", 0);
}

void subTestObjA1ObjB1Frame12()
{
    // Create object of categoryNumber 'A', object of categoryNumber 'B',
    // calculate their collision frame for frame 12. Verify that collision frame
    // calculation function is called and that handler is called on 13th call
    // of process frame
    testTemplateObjA1ObjB1FrameX("subTestObjA1ObjB1Frame12()", 12);
}
