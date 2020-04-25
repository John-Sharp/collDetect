#ifndef COLL_DETECT_H
#define COLL_DETECT_H

#include <jTypes.h>

typedef enum COLL_ACTOR_TYPE
{
    COLL_ACTOR_TYPE_POINT = 1,
    COLL_ACTOR_TYPE_V_LINE = 1<<1,
    COLL_ACTOR_TYPE_H_LINE = 1<<2,
    COLL_ACTOR_TYPE_RECT = 1<<3,
} COLL_ACTOR_TYPE;

typedef union collShape 
{
    jintVec point;
    jintAxPlLine line;
    jintRect rect;
} collShape;

typedef struct collInfo collInfo;

#include "./listHeaders/collInfoList.h"

typedef struct collActor {
    COLL_ACTOR_TYPE type;

    collShape shape;
    jintVecScaled vel; // velocity
    
    juint categoryNumber;
    collInfoList * nextScheduledCollision;

    jint frameStart; // frame when was position described by `shape`
    jint collFrame; // frame before collision
} collActor;

typedef struct collEngine
{
} collEngine;
typedef void (*collHandler)(collActor * ca1, collActor * ca2);

collEngine * createCollEngine();
void destroyCollEngine();

void collEngineUpsertCollGroup(collEngine * collEngine,
        juint categoryNumber1, juint categoryNumber2, 
        collHandler handler);
void collEngineCollGroupRm(collEngine * eng,
        juint categoryNumber1, juint categoryNumber2);
void collEngineCollGroupRmCat(collEngine * eng,
        juint categoryNumber);

void collEngineRegisterCollActor(collEngine * eng,
        collActor * actor);
void collEngineDeregisterCollActor(collEngine * eng,
        collActor * actor);

void collEngineProcessFrame(collEngine * eng);

#endif
