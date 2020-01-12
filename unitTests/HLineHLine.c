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
               .rStart = {{1, 17},1},
               .length = 10 
           }
       },
       .vel = {.v={6,-11}, .scale = 1}
   };

   collActor ca2 = {
       .type = COLL_ACTOR_TYPE_H_LINE,
       .shape = {
           .line = {
               .direction = AX_PL_DIR_X,
               .rStart = {{18,10},1},
               .length = 19 
           }
       },
       .vel = {.v = {0,0}, .scale = 1}
   };

   COLL_FRAME_CALC_RET ret = calculateNextCollisionFrame(&f, &ca1.vel, &ca1, &ca2);

   if (ret != COLL_FRAME_CALC_NO_COLLISION)
   {
       printf("`testMissesLTLTLTLT` fails\n");
       exit(1);
   }
}

void subTestsHLineHLine()
{
    testMissesLTLTLTLT();
    // testMissesGTGTGTGT();
    // testHitsEQEQEQEQ();
    // testHitsLTEQLTEQ();
    // testMissesGTGTEQGT();
    // testMissesEQGTGTGT();
    // testMissesLTLTLTEQ();
    // testMissesLTEQLTLT();
    // testCalculateFrame();
    // testHitsLTEQLTLT();
}
