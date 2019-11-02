#include "../collDetect.h"
#include <stdio.h>
#include <stdlib.h>

void testUnhandledTypes()
{
    jint f;
    collActor ca1 = {.type = COLL_ACTOR_TYPE_POINT};
    collActor ca2 = {.type = COLL_ACTOR_TYPE_POINT};

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_UNHANDLED_TYPES)
    {
        printf("failure calling `calculateNextCollisionFrame` with unhandled collision types - "
                "did not return COLL_FRAME_CALC_UNHANDLED_TYPES");
        exit(1);
    }
}

void testMissesAbove()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .rStart = {10, 10},
                .sTarg = {5,2},
                .tScale = 6
            }
        }
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {15,0},
                .length = 11
            }
        }
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`calculateNextCollisionFrame` fails when point passes above vertical line");
        exit(1);
    }

}

void testMissesBelow()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .rStart = {10, 10},
                .sTarg = {5,2},
                .tScale = 6
            }
        }
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {15,16},
                .length = 11
            }
        }
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`calculateNextCollisionFrame` fails when point passes below vertical line");
        exit(1);
    }

}

void testCalculateFrame()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .rStart = {10, 10},
                .sTarg = {5,2},
                .tScale = 6
            }
        }
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {15,10},
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

int main()
{
    testUnhandledTypes();
    testMissesAbove();
    testMissesBelow();
    testCalculateFrame();
}
