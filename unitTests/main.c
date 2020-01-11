#include "../collDetect.h"
#include <stdio.h>
#include <stdlib.h>

#include "pointVLine.h"
#include "pointHLine.h"

#include "VLineVLine.h"

void testUnhandledTypes()
{
    jint f;
    collActor ca1 = {.type = COLL_ACTOR_TYPE_POINT};
    collActor ca2 = {.type = COLL_ACTOR_TYPE_POINT};
    jintVec vrel = {.v = {1,1}, .scale = 1};

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &vrel, &ca1, &ca2);

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
    // subTestsPointVLine();
    subTestsPointHLine();
    // TODO
    subTestsVLineVLine();
}
