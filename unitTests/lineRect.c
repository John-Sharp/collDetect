#include "VLineVLine.h"
#include "../collDetect.h"
#include <stdlib.h>
#include <stdio.h>

static void testHitsYLTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{20, 20}},
                .length = 55 
            }
        },
        .vel = {.v={{120,40}}, .s = 120}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_RECT,
        .shape = {
            .rect = {
                .bl = {{55, 40}},
                .tr = {{125, 95}}
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK || f != 35)
    {
        printf("`testHitsYLTEQ` fails\n");
        exit(1);
    }
}

static void testHitsXGTLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{30, 115}},
                .length = 135 
            }
        },
        .vel = {.v={{0,-100}}, .s = 100}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_RECT,
        .shape = {
            .rect = {
                .bl = {{55, 40}},
                .tr = {{125, 95}}
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK || f != 20)
    {
        printf("`testHitsXGTLT` fails\n");
        exit(1);
    }
}

void subTestsLineRect()
{
    testHitsYLTEQ();
    testHitsXGTLT();
}
