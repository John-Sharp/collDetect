#include "../../collisionFrameCalculate.h"

#include <stdio.h>
#include <stdlib.h>

#include "pointVLine.h"
#include "pointHLine.h"
#include "pointRect.h"

#include "lineRect.h"
#include "VLineVLine.h"
#include "HLineHLine.h"

#include "rectRect.h"

#include <assert.h>

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
    jintVec norm;

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &norm, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_UNHANDLED_TYPES)
    {
        printf("failure calling `calculateNextCollisionFrame` with unhandled collision types - "
                "did not return COLL_FRAME_CALC_UNHANDLED_TYPES");
        exit(1);
    }
}

int main()
{
    testUnhandledTypes();
    subTestsPointVLine();
    subTestsPointHLine();
    subTestsPointRect();
    subTestsVLineVLine();
    subTestsLineRect();
    subTestsHLineHLine();
    subTestsRectRect();
}
