#include "pointVLine.h"
#include "../collDetect.h"
#include <stdlib.h>
#include <stdio.h>

static void testMissesAbove()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{10, 10},1},
                .length = 5
            }
        },
        .vel = {.v={5,2}, .scale = 6}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{15,0},1},
                .length = 11
            }
        },
        .vel = {.v = {0,0}, .scale = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`calculateNextCollisionFrame` fails when vertical line passes above vertical line");
        exit(1);
    }
}

void subTestsVLineVLine()
{
    testMissesAbove();
    // testMissesBelow();
    // testCalculateFrame();
}
