#include "rectRect.h"
#include "../../collisionFrameCalculate.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static void testHitsX()
{
    jint f;
    jintVec norm;

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

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &norm, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK || f != 25)
    {
        printf("`testHitsX` fails\n");
        exit(1);
    }

    if (norm.v[0] != -1 || norm.v[1] != 0)
    {
        printf("`testHitsX` fails\n");
        assert(0);
    }
}

static void testHitsY()
{
    jint f;
    jintVec norm;

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

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &norm, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK || f != 40)
    {
        printf("`testHitsY` fails\n");
        exit(1);
    }
    
    if (norm.v[0] != 0 || norm.v[1] != 1)
    {
        printf("`testHitsY` fails\n");
        assert(0);
    }
}

void subTestsRectRect()
{
    testHitsX();
    testHitsY();
}
