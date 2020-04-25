#include "mockCollisionFrameCalculate.h"

static void * mockCalculateNextCollisionFrameContext = NULL;
static calculateNextCollisionFrameCallHandler \
                    mockCalculateNextCollisionFrame = NULL;

void setMockCalcuateNextCollisionFrameContext(void * context)
{
    mockCalculateNextCollisionFrameContext = context;
}

void setMockCalculateNextCollisionFrame(calculateNextCollisionFrameCallHandler h)
{
    mockCalculateNextCollisionFrame = h;
}

COLL_FRAME_CALC_RET calculateNextCollisionFrame(
        jint * collFrame, const collActor * ca1, const collActor * ca2)
{
    if (mockCalculateNextCollisionFrame)
    {
        return mockCalculateNextCollisionFrame(
                collFrame, ca1, ca2, mockCalculateNextCollisionFrameContext);
    }
    return COLL_FRAME_CALC_NO_COLLISION;
}
