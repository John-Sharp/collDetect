#include "HLineHLine.h"
#include "../collDetect.h"
#include <stdlib.h>
#include <stdio.h>

static void testMissesLTLTLTLT()
{
   jint f;
   collActor ca1 = {
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{1, 17}},
               .length = 10 
           }
       },
       .vel = {.v={{6,-11}}, .s = 1}
   };

   collActor ca2 = {
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
           }
       },
       .vel = {.v = {{0,0}}, .s = 1}
   };

   COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

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
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{43, 17}},
               .length = 10 
           }
       },
       .vel = {.v={{-5,-19}}, .s = 1}
   };

   collActor ca2 = {
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
           }
       },
       .vel = {.v = {{0,0}}, .s = 1}
   };

   COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

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
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{27, 17}},
                .length = 9 
            }
        },
        .vel = {.v={{-4,-12}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK)
    {
        printf("`testHitsEQEQEQEQ` fails when should calculate collision");
        exit(1);
    }

    if (f != 0)
    {
        printf("`testHitsEQEQEQEQ` fails to calclate correct frame of collision");
        exit(1);
    }
}

static void testHitsLTEQLTEQ()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{17, 17}},
                .length = 9 
            }
        },
        .vel = {.v={{-4,-12}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK)
    {
        printf("`testHitsLTEQLTEQ` fails when should calculate collision");
        exit(1);
    }

    if (f != 0)
    {
        printf("`testHitsLTEQLTEQ` fails to calclate correct frame of collision");
        exit(1);
    }
}

static void testMissesGTGTEQGT()
{
   jint f;
   collActor ca1 = {
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{41, 17}},
               .length = 9 
           }
       },
       .vel = {.v={{-7,-17}}, .s = 1}
   };

   collActor ca2 = {
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
           }
       },
       .vel = {.v = {{0,0}}, .s = 1}
   };

   COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

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
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{34, 0}},
               .length = 9 
           }
       },
       .vel = {.v={{7,17}}, .s = 1}
   };

   collActor ca2 = {
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
           }
       },
       .vel = {.v = {{0,0}}, .s = 1}
   };

   COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

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
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{6, 16}},
               .length = 9 
           }
       },
       .vel = {.v={{5,-15}}, .s = 1}
   };

   collActor ca2 = {
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
           }
       },
       .vel = {.v = {{0,0}}, .s = 1}
   };

   COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

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
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{11, 1}},
               .length = 9 
           }
       },
       .vel = {.v={{-5,15}}, .s = 1}
   };

   collActor ca2 = {
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
           }
       },
       .vel = {.v = {{0,0}}, .s = 1}
   };

   COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

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
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{24, 21}},
                .length = 9 
            }
        },
        .vel = {.v={{-9,-27}}, .s = 9}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

    if (ret != COLL_FRAME_CALC_OK)
    {
        printf("`testCalculateFrame` fails when should calculate collision");
        exit(1);
    }

    if (f != 3)
    {
        printf("`testCalculateFrame` fails to calclate correct frame of collision");
        exit(1);
    }
}

static void testHitsLTEQLTLT()
{
    jint f;
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
                .rStart = {{14, 14}},
                .length = 9 
            }
        },
        .vel = {.v={{-7,-10}}, .s = 1}
    };
    
    collActor ca2 = {
        .type = COLL_ACTOR_TYPE_H_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,
               .rStart = {{18,10}},
               .length = 19 
            }
        },
        .vel = {.v = {{0,0}}, .s = 1}
    };

    COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

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

void subTestsHLineHLine()
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
