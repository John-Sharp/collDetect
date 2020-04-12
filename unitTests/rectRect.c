#include "rectRect.h"
#include "../collDetect.h"
#include <stdlib.h>
#include <stdio.h>

static void testHitsX()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_RECT,
        .shape = {
            .rect = {
                .bl = {{35, 60}},
                .tr = {{140,115}}
            }
        },
        .vel = {.v={{175,25}}, .s = 175}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_RECT,
        .shape = {
            .rect = {
                .bl = {{165, 100}},
                .tr = {{235, 155}}
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK || f != 25)
    {
        printf("`testHitsX` fails\n");
        exit(1);
    }
}

static void testHitsY()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_RECT,
        .shape = {
            .rect = {
                .bl = {{145, 140}},
                .tr = {{250,195}}
            }
        },
        .vel = {.v={{5,-110}}, .s = 110}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_RECT,
        .shape = {
            .rect = {
                .bl = {{115, 45}},
                .tr = {{185, 100}}
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK || f != 40)
    {
        printf("`testHitsX` fails\n");
        exit(1);
    }
}

void subTestsRectRect()
{
    testHitsX();
    testHitsY();
}
