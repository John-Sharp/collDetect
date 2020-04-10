#include "VLineVLine.h"
#include "../collDetect.h"
#include <stdlib.h>
#include <stdio.h>

static void testMissesLTLTLTLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{12, -26}},
                .length = 16 
            }
        },
        .vel = {.v={{33,8}}, .s = 1}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesLTLTLTLT` fails\n");
        exit(1);
    }
}

static void testMissesGTGTGTGT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{18, 28}},
                .length = 16 
            }
        },
        .vel = {.v={{29,-4}}, .s = 1}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesGTGTGTGT` fails\n");
        exit(1);
    }
}

static void testHitsEQEQEQEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{17, 2}},
                .length = 9 
            }
        },
        .vel = {.v={{22,5}}, .s = 1}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK || f != 0)
    {
        printf("`testHitsEQEQEQEQ` fails\n");
        exit(1);
    }
}

static void testHitsLTEQLTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{19, -4}},
                .length = 10 
            }
        },
        .vel = {.v={{17,3}}, .s = 1}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK || f != 0)
    {
        printf("`testHitsLTEQLTEQ` fails\n");
        exit(1);
    }
}

static void testMissesGTGTEQGT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{19, 25}},
                .length = 9 
            }
        },
        .vel = {.v={{34,-10}}, .s = 1}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesGTGTEQGT` fails\n");
        exit(1);
    }
}

static void testMissesEQGTGTGT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{53, 15}},
                .length = 9 
            }
        },
        .vel = {.v={{-34,10}}, .s = 1}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesEQGTGTGT` fails\n");
        exit(1);
    }
}

static void testMissesLTLTLTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{17, -21}},
                .length = 13 
            }
        },
        .vel = {.v={{26,13}}, .s = 1}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesLTLTLTEQ` fails\n");
        exit(1);
    }
}

static void testMissesLTEQLTLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{43, -8}},
                .length = 13 
            }
        },
        .vel = {.v= {{-26,-13}}, .s = 1}
    };

    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesLTEQLTLT` fails\n");
        exit(1);
    }
}

static void testCalculateFrame()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{10, -5}},
                .length = 13 
            }
        },
        .vel = {.v= {{32,11}}, .s = 16}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
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

    if (f != 10)
    {
        printf("`calculateNextCollisionFrame` fails to calclate correct frame of collision");
        exit(1);
    }
}

static void testHitsLTEQLTLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{10, -5}},
                .length = 13 
            }
        },
        .vel = {.v={{35,-10}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_Y,
                .rStart = {{30,0}},
                .length = 20 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK)
    {
        printf("`testHitsLTEQLTLT` fails when should calculate collision");
        exit(1);
    }

    if (f != 0)
    {
        printf("`testHitsLTEQLTLT` fails to calclate correct frame of collision");
        exit(1);
    }
}
void subTestsVLineVLine()
{
    testMissesLTLTLTLT();
    testMissesGTGTGTGT();
    testHitsEQEQEQEQ();
    testHitsLTEQLTEQ();
    testMissesGTGTEQGT();
    testMissesEQGTGTGT();
    testMissesLTLTLTEQ();
    testMissesLTEQLTLT();
    testCalculateFrame();
    testHitsLTEQLTLT();
}
