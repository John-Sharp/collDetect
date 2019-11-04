#include "pointHLine.h"
#include "../collDetect.h"
#include <stdlib.h>
#include <stdio.h>

void testMissesRight()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .rStart = {{10, 10}},
                .sTarg = {{2,5}},
                .tScale = 6
            }
        }
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{0,15}},
                .length = 11
            }
        }
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`calculateNextCollisionFrame` fails when point passes to right of horizontal line");
        exit(1);
    }

}

void testMissesLeft()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .rStart = {{10, 10}},
                .sTarg = {{2,5}},
                .tScale = 6
            }
        }
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{16,15}},
                .length = 11
            }
        }
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`calculateNextCollisionFrame` fails when point passes to left of horizontal line");
        exit(1);
    }

}

static void testCalculateFrame()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .rStart = {{10, 10}},
                .sTarg = {{2,5}},
                .tScale = 6
            }
        }
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{10,15}},
                .length = 11
            }
        }
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK)
    {
        printf("`calculateNextCollisionFrame` fails when should calculate collision");
        exit(1);
    }

    if (f != 6)
    {
        printf("`calculateNextCollisionFrame` fails to calclate correct frame of collision");
        exit(1);
    }
}

void subTestsPointHLine()
{
    testMissesRight();
    testMissesLeft();
    testCalculateFrame();
}
