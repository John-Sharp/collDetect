#include "pointVLine.h"
#include "../../collisionFrameCalculate.h"
#include <stdlib.h>
#include <stdio.h>

static void testMissesGTGT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{5, 15}}
        },
        .vel = {.v = {{20,-3}}, .s = 1}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{15,0}},
                .length = 11
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("testMissesGTGT: `calculateNextCollisionFrame` fails when point passes above vertical line");
        exit(1);
    }
}

static void testMissesGTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{5, 15}}
        },
        .vel = {.v = {{20,-6}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{15,0}},
                .length = 11
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("testMissesGTEQ: `calculateNextCollisionFrame` fails when point passes above vertical line");
        exit(1);
    }

}

static void testMissesEQGT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{5, 5}}
        },
        .vel = {.v = {{17, 13}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{15,0}},
                .length = 11
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("testMissesEQGT: `calculateNextCollisionFrame` fails when point passes above vertical line");
        exit(1);
    }
}

static void testMissesLTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{7, -5}}
        },
        .vel = {.v = {{18, 9}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{15,0}},
                .length = 11
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("testMissesLTEQ: `calculateNextCollisionFrame` fails when point passes below vertical line");
        exit(1);
    }
}

static void testMissesEQLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{8, 4}}
        },
        .vel = {.v = {{12, -11}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{15,0}},
                .length = 11
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("testMissesEQLT: `calculateNextCollisionFrame` fails when point passes below vertical line");
        exit(1);
    }
}

static void testMissesLTLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{7, -1}}
        },
        .vel = {.v = {{13, -6}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{15,0}},
                .length = 11
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("testMissesLTLT: `calculateNextCollisionFrame` fails when point passes below vertical line");
        exit(1);
    }
}

static void testCalculateFrame()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{10, 10}}
        },
        .vel = {.v = {{5,2}}, .s = 6}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{15,10}},
                .length = 11
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
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

static void testHitsGTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {{7, 14}}
        },
        .vel = {.v = {{13, -11}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{15,0}},
                .length = 11
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret == COLL_FRAME_CALC_NO_COLLISION || f != 0)
    {
        printf("testHitsGTEQ: `calculateNextCollisionFrame` fails to calculate correct frame of collision");
        exit(1);
    }
}

void subTestsPointVLine()
{
    testMissesGTGT();
    testMissesGTEQ();
    testMissesEQGT();
    testMissesLTEQ();
    testMissesEQLT();
    testMissesLTLT();
    testCalculateFrame();
    testHitsGTEQ();
}
