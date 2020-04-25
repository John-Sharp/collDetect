#ifndef MOCK_COLLISION_FRAME_CALCULATE_H
#define MOCK_COLLISION_FRAME_CALCULATE_H
#include "../../collisionFrameCalculate.h"

void setMockCalcuateNextCollisionFrameContext(void * context);

typedef COLL_FRAME_CALC_RET (*calculateNextCollisionFrameCallHandler)(
        jint * collFrame, const collActor * ca1, const collActor * ca2,
        void * mockCalculateNextCollisionFrameContext);
void setMockCalculateNextCollisionFrame(calculateNextCollisionFrameCallHandler handler);

#endif
