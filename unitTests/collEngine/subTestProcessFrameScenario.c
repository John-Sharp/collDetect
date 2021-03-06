#include "../../collDetect.h"
#include "mockCollisionFrameCalculate.h"
#include "../../listHeaders/collActorList.h"
#include <stdio.h>
#include <assert.h>

#include "listHeaders/intList.h"
#include "listCode/intList.inc"

// collisionCalcResult: actor pair, with list of frames when collision is
// calculated for
typedef struct collisionCalcResult
{
    collActor * a1;
    collActor * a2;
    intList * resultFrames;
} collisionCalcResult;

#include "listHeaders/collisionCalcResultList.h"
#include "listCode/collisionCalcResultList.inc"

typedef struct scenario
{
    collActorList * actorList;
    int categories;

    // list of results that will be provided by testCNCF
    collisionCalcResultList * collisionResults;

    // expected collisions, ordered by frame
    collisionCalcResultList * expectedCollisionResults;

    // what has been received since the last time process frame was called
    collisionCalcResultList * receivedCollisionResults;
} scenario;

#define UNORDERED_MATCH(a1, a2, b1, b2) \
    ((a1 == b1 && a2 == b2)||(a1 == b2 && a2 == b1))

static struct scenario * scenario_global = NULL;
static void testHandler(collActor * ca1, collActor * ca2, const jintVec * norm)
{
    // set the next result for this pair (by popping off
    // the pair's `resultFrames` list)
    // add record of this collision to `receivedCollisionResults`
    collisionCalcResultList * l;
    for (l = scenario_global->collisionResults; l != NULL; l = l->next)
    {
        if (l->val->a1 == ca1 || l->val->a1 == ca2
                || l->val->a2 == ca1 || l->val->a2 == ca2)
        {
            if (l->val->resultFrames)
            {
                intList * resultFrameOld = l->val->resultFrames;
                l->val->resultFrames = l->val->resultFrames->next;
                if (l->val->resultFrames)
                    l->val->resultFrames->prev = NULL;
                if (resultFrameOld->val)
                    free(resultFrameOld->val);
                free(resultFrameOld);
            }
        }
    }

    collisionCalcResult * r = malloc(sizeof(*r));
    assert(r);
    r->a1 = ca1;
    r->a2 = ca2;
    scenario_global->receivedCollisionResults = collisionCalcResultListAddToEnd(
        scenario_global->receivedCollisionResults,
        r);
}

static COLL_FRAME_CALC_RET testCNCF(
        jint * collFrame, const collActor * ca1, const collActor * ca2,
        void * context)
{
    // look through scenario collisionResults for ca1, ca2
    // if found, set collFrame appropriately
    // else return no collision

    collisionCalcResultList * l;
    for (l = scenario_global->collisionResults; l != NULL; l = l->next)
    {
        if (UNORDERED_MATCH(l->val->a1, l->val->a2, ca1, ca2))
        {
            if (l->val->resultFrames && l->val->resultFrames->val)
            {
                *collFrame = *l->val->resultFrames->val;
                return COLL_FRAME_CALC_OK;
            }
            break;
        }
    }

    return COLL_FRAME_CALC_NO_COLLISION;
}

static const char * parseAndCreateScenarioSetup(
        struct scenario * scenario,
        const char * scenarioDescription, collEngine * eng);

static const char * parseAndCreateScenarioBody(
        struct scenario * scenario,
        const char * scenarioBody);

static const char * parseAndCreateScenarioExpectations(
        struct scenario * scenario,
        const char * scenarioExpectations);

static void registerActors(
        struct scenario * scenario,
        collEngine * eng);

void subTestProcessFrameScenario(const char * scenarioDescription)
{
    struct scenario scenario;
    scenario_global = &scenario;
    collEngine * eng = createCollEngine();
    setMockCalculateNextCollisionFrame(testCNCF);

    const char * scenarioBody = parseAndCreateScenarioSetup(
            &scenario, scenarioDescription, eng);

    const char * scenarioExpectations = parseAndCreateScenarioBody(
            &scenario, scenarioBody);

    registerActors(&scenario, eng);

    parseAndCreateScenarioExpectations(
            &scenario, scenarioExpectations);

    scenario.receivedCollisionResults = NULL;
    int nextRunEngineUntil = -1;
    collisionCalcResultList * l;
    int framesProcessed = 0;
    for (l = scenario.expectedCollisionResults; l != NULL; l = l->next)
    {
        if (*l->val->resultFrames->val > nextRunEngineUntil)
        {
            nextRunEngineUntil = *l->val->resultFrames->val;
            for (; framesProcessed < nextRunEngineUntil; framesProcessed++)
            {
                collEngineProcessFrame(eng);
                // check nothing has happened
                if (scenario.receivedCollisionResults != NULL)
                {
                    printf("collision handler called when not expected\n");
                    assert(0);
                }
            }

            collEngineProcessFrame(eng);
            framesProcessed++;
        }

        // check that expected actors' collision handler called
        if (!scenario.receivedCollisionResults)
        {
            printf("collision handler not called when expected\n");
            assert(0);
        }

        if (!UNORDERED_MATCH(l->val->a1, l->val->a2,
            scenario.receivedCollisionResults->val->a1,
            scenario.receivedCollisionResults->val->a2))
        {
            printf("expected collision handler not called\n");
            assert(0);
        }

        collisionCalcResultList * listOld = scenario.expectedCollisionResults;
        scenario.expectedCollisionResults = scenario.expectedCollisionResults->next;
        if (scenario.expectedCollisionResults)
            scenario.expectedCollisionResults->prev = NULL;
        free(listOld->val);
        free(listOld);

        listOld = scenario.receivedCollisionResults;
        scenario.receivedCollisionResults = scenario.receivedCollisionResults->next;
        if (scenario.receivedCollisionResults)
            scenario.receivedCollisionResults->prev = NULL;
        free(listOld->val);
        free(listOld);
    }
}

#define CHAR_IS_DIGIT(x) ((x) >= '0' && (x) <= '9')

static void createActorsForCollGroup(
        struct scenario * scenario, collEngine * eng,
        int numActorsInCategory);
static const char * parseAndCreateScenarioSetup(
        struct scenario * scenario, const char * scenarioDescription, collEngine * eng)
{
    scenario->actorList = NULL;
    scenario->categories = 0;

    const char * c;
    int placeMultiplier = 10;
    int numActorsInCategory = 0;
    for (c = scenarioDescription; *c != ':'; c++)
    {
        if (*c == ',')
        {
            assert(placeMultiplier>10);
            assert(CHAR_IS_DIGIT(*(c+1))); 
            createActorsForCollGroup(scenario, eng, numActorsInCategory);
            placeMultiplier = 10;
            numActorsInCategory = 0;
            continue;
        }
        assert(CHAR_IS_DIGIT(*c));

        int digit = *c - '0';
        numActorsInCategory += numActorsInCategory * placeMultiplier + digit;
        placeMultiplier *= 10;
    }
    createActorsForCollGroup(scenario, eng, numActorsInCategory);

    return ++c;
}

static void createActorsForCollGroup(
        struct scenario * scenario, collEngine * eng,
        int numActorsInCategory)
{
    int i;
    for (i = 0; i < numActorsInCategory; i++)
    {
        collActor * a = malloc(sizeof(*a));
        a->categoryNumber = scenario->categories;
        scenario->actorList = collActorListAdd(scenario->actorList, a); 
    }

    for (i = 0; i < scenario->categories; i++)
    {
        collEngineUpsertCollGroup(eng, i, scenario->categories, testHandler);
    }
    scenario->categories++;
}

static void registerActors(
        struct scenario * scenario,
        collEngine * eng)
{
    collActorList * l;
    for (l = scenario->actorList; l != NULL; l = l->next)
    {
        collEngineRegisterCollActor(eng, l->val);
    }
}


static collisionCalcResult * parseCollisionCalcResult(
        struct scenario * scenario, const char ** c);

static const char * parseAndCreateScenarioBody(
        struct scenario * scenario,
        const char * scenarioBody)
{
    scenario->collisionResults = NULL;
    const char * c;
    for (c = scenarioBody; *c != '_';)
    {
        scenario->collisionResults = collisionCalcResultListAdd(
            scenario->collisionResults,
            parseCollisionCalcResult(scenario, &c));
    }

    return ++c;
}

static collActor * findActor(struct scenario * scenario, int catNum, int actorNum);

static collisionCalcResult * parseCollisionCalcResult(
        struct scenario * scenario, const char ** c)
{
    int catNum1, catNum2, actorNum1, actorNum2;
    int pos;

    int res = sscanf(*c, "(%d,%d)(%d,%d)%n",
            &catNum1,
            &actorNum1,
            &catNum2,
            &actorNum2,
            &pos
            );

    assert(res == 4);

    *c = *c + pos;

    collisionCalcResult * ret = malloc(sizeof(*ret));
    assert(ret);

    ret->a1 = findActor(scenario, catNum1, actorNum1);
    assert(ret->a1);

    ret->a2 = findActor(scenario, catNum2, actorNum2);
    assert(ret->a2);

    ret->resultFrames = NULL;

    if (!CHAR_IS_DIGIT(**c) && **c != ',')
    {
        return ret;
    }


    while (1)
    {
        int * frameP = NULL;
        int frame;
        if (sscanf(*c, "%d%n", &frame, &pos))
        {
            frameP = malloc(sizeof(*frameP));
            *frameP = frame;
            *c += pos;
        }
        ret->resultFrames =  intListAddToEnd(ret->resultFrames, frameP);
        if (**c == ',')
        {
            *c += 1;
        }
        else
        {
            break;
        }
    }

    return ret;
}

static collActor * findActor(struct scenario * scenario, int catNum, int actorNum)
{
    collActorList * l;
    for (l = scenario->actorList; l!= NULL; l = l->next)
    {
        if (l->val->categoryNumber == catNum)
            break;
    }
    assert(l);

    int i = 0;
    for (; l!= NULL; l = l->next)
    {
        assert(l->val->categoryNumber == catNum);
        if (i == actorNum)
            return l->val;
        i++;
    }
    assert(false);
    return NULL;
}

static collisionCalcResult * parseExpectedCollisionResult(
        const char ** c,
        struct scenario * scenario,
        int frame);

static const char * parseAndCreateScenarioExpectations(
        struct scenario * scenario,
        const char * scenarioExpectations)
{
    scenario->expectedCollisionResults = NULL;
    const char * c;
    int frame = -1;
    assert(CHAR_IS_DIGIT(*scenarioExpectations));

    for (c = scenarioExpectations; *c != '\0'; c++)
    {
        if (CHAR_IS_DIGIT(*c))
        {
            int pos;
            int frameOld = frame;
            int res = sscanf(c, "%d%n", &frame, &pos);
            assert(res==1);
            assert(frame >= frameOld);
            c += pos - 1;
            continue;
        }
        if (*c == ',')
        {
            continue;
        }

        scenario->expectedCollisionResults = collisionCalcResultListAddToEnd(
            scenario->expectedCollisionResults,
            parseExpectedCollisionResult(&c, scenario, frame));

    }

    return c;
}

static collisionCalcResult * parseExpectedCollisionResult(
        const char ** c,
        struct scenario * scenario,
        int frame)
{
    int catNum1, catNum2, actorNum1, actorNum2;
    int pos;
    int res = sscanf(*c, "(%d,%d)(%d,%d)%n",
            &catNum1, &actorNum1,
            &catNum2, &actorNum2,
            &pos);

    assert(res == 4);

    *c = *c + pos - 1;

    collisionCalcResult * ret = malloc(sizeof(*ret));
    assert(ret);

    ret->a1 = findActor(scenario, catNum1, actorNum1);
    assert(ret->a1);

    ret->a2 = findActor(scenario, catNum2, actorNum2);
    assert(ret->a2);

    int * frameP = malloc(sizeof(*frameP));
    *frameP = frame;
    ret->resultFrames = intListAdd(ret->resultFrames, frameP);

    return ret;
}
