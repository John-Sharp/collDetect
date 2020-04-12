#include "pointRect.h"
#include "../collDetect.h"
#include <stdlib.h>
#include <stdio.h>

static void testHitsXLTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{20,20}},
        },
        .vel = {.v = {{65, 50}}, .s=65}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_RECT,
        .shape = {
            .rect = {
                .bl = {{ 55, 40}},
                .tr = {{125, 95}}
            },
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret == COLL_FRAME_CALC_NO_COLLISION || f != 35)
    {
        printf("`testHitsXLTEQ` fails to calculate correct frame of collision\n");
        exit(1);
    }
}

static void testHitsXYLTGT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{20,20}},
        },
        .vel = {.v = {{95, 95}}, .s=95}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_RECT,
        .shape = {
            .rect = {
                .bl = {{ 55, 40}},
                .tr = {{125, 95}}
            },
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret == COLL_FRAME_CALC_NO_COLLISION || f != 35)
    {
        printf("`testHitsXYLTGT` fails to calculate correct frame of collision\n");
        exit(1);
    }
}

static void testHitsYEQLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{105,110}},
        },
        .vel = {.v = {{-90, -55}}, .s=55}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_RECT,
        .shape = {
            .rect = {
                .bl = {{ 55, 40}},
                .tr = {{125, 95}}
            },
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret == COLL_FRAME_CALC_NO_COLLISION || f != 15)
    {
        printf("`testHitsXLTEQ` fails to calculate correct frame of collision\n");
        exit(1);
    }

    ret = calculateNextCollisionFrame(&f, &ca2, &ca1);

    if (ret == COLL_FRAME_CALC_NO_COLLISION || f != 15)
    {
        printf("`testHitsXLTEQ` fails to calculate correct frame of collision when"
                " order swapped\n");
        exit(1);
    }
}

void subTestsPointRect()
{
    testHitsXLTEQ();
    testHitsXYLTGT();
    testHitsYEQLT();
}
