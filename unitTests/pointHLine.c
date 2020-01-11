#include "pointHLine.h"
#include "../collDetect.h"
#include <stdlib.h>
#include <stdio.h>

static void testMissesGTGT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .v = {31, 20},
                .scale = 1
            }
        },
        .vel = {.v = {5,-17}, .scale = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{15,11}},
                .length = 15
            }
        },
        .vel = {.v = {0,0}, .scale = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesGTGT` fails when point passes to right of horizontal line");
        exit(1);
    }

}

static void testMissesGTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .v = {38, 17},
                .scale = 1
            }
        },
        .vel = {.v = {-13,-14}, .scale = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{15,11}},
                .length = 15
            }
        },
        .vel = {.v = {0,0}, .scale = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesGTEQ` fails when noooo point passes to right of horizontal line");
        exit(1);
    }

}

static void testMissesEQGT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .v = {26, 17},
                .scale = 1
            }
        },
        .vel = {.v = {11,-14}, .scale = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{15,11}},
                .length = 15
            }
        },
        .vel = {.v = {0,0}, .scale = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesEQGT` fails when point passes to right of horizontal line");
        exit(1);
    }

}

static void testMissesLTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .v = {2, 17},
                .scale = 1
            }
        },
        .vel = {.v = {23,-14}, .scale = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{15,11}},
                .length = 15
            }
        },
        .vel = {.v = {0,0}, .scale = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesLTEQ` fails when point passes to right of horizontal line");
        exit(1);
    }

}

static void testMissesEQLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .v = {20, 15},
                .scale = 1
            }
        },
        .vel = {.v = {-16,-9}, .scale = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{15,11}},
                .length = 15
            }
        },
        .vel = {.v = {0,0}, .scale = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesEQLT` fails when point passes to right of horizontal line");
        exit(1);
    }

}

static void testMissesLTLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_POINT,
        .shape = {
            .point = {
                .v = {14, 16},
                .scale = 1
            }
        },
        .vel = {.v = {-10,-10}, .scale = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{15,11}},
                .length = 15
            }
        },
        .vel = {.v = {0,0}, .scale = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_NO_COLLISION)
    {
        printf("`testMissesLTLT` fails when point passes to right of horizontal line");
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
                .v = {10, 10},
                .scale = 1
            }
        },
        .vel = {.v = {2,5}, .scale = 6}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{10,15}},
                .length = 11
            }
        },
        .vel = {.v = {0,0}, .scale = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

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
            .point = {
                .v = {35, 16},
                .scale = 1
            }
        },
        .vel = {.v = {-15,-9}, .scale = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{15,11}},
                .length = 15
            }
        },
        .vel = {.v = {0,0}, .scale = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret == COLL_FRAME_CALC_NO_COLLISION || f != 0)
    {
        printf("`testHitsGTEQ` fails to calculate correct frame of collision");
        exit(1);
    }

}

void subTestsPointHLine()
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
