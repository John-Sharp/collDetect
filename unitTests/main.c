#include "../collDetect.h"
#include <stdio.h>
#include <stdlib.h>

#include "testMallocTracker.h"

#include "pointVLine.h"
#include "pointHLine.h"
#include "pointRect.h"

#include "lineRect.h"
#include "VLineVLine.h"
#include "HLineHLine.h"

#include "rectRect.h"

void testUnhandledTypes()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .vel = {.v = {{1,1}}, .s = 1}
    };
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_UNHANDLED_TYPES)
    {
        printf("failure calling `calculateNextCollisionFrame` with unhandled collision types - "
                "did not return COLL_FRAME_CALC_UNHANDLED_TYPES");
        exit(1);
    }
}

static bool mallocShouldFailOnFirstAttempt()
{
    return true;
}

void testCreateCollEngine()
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

    allocatorTrackerReset();
}

int main()
{
    // tests of collEngine
    testCreateCollEngine();

    // tests of collision calculateNextCollisionFrame()
    testUnhandledTypes();
    subTestsPointVLine();
    subTestsPointHLine();
    subTestsPointRect();
    subTestsVLineVLine();
    subTestsLineRect();
    subTestsHLineHLine();
    subTestsRectRect();
}
