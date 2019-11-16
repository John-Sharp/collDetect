#include "pointVLine.h"
#include "../collDetect.h"
#include <stdlib.h>
#include <stdio.h>

void testMissesAbove()
{
    collActor ca1 = {
        .type = COLL_ACTOR_TYPE_V_LINE,
        .shape = {
            .line = {
                .direction = AX_PL_DIR_X,

}
