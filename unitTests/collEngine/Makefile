CC = gcc

CFLAGS = -g -Wall -lm -I../../../jTypes --coverage

generateCollDetectListCode: ../../../dataStructures/templates/listTemplate.h ../../../dataStructures/templates/listTemplate.inc
	mkdir -p ../../listHeaders ../../listCode
	../../../dataStructures/generateList.py ../../listHeaders ../../listCode collGroup:collGroup
	../../../dataStructures/generateList.py ../../listHeaders ../../listCode collActor:collActor
	../../../dataStructures/generateList.py ../../listHeaders ../../listCode collActorCategoryNumberGrouped:collActorCategoryNumberGrouped
	../../../dataStructures/generateList.py ../../listHeaders ../../listCode collInfo:collInfo

all: generateCollDetectListCode main.c ../../../jTypes/jTypes.h ../../../jTypes/jTypes.c ../../collDetect.h ../../collDetect.c\
	subTestObjA1ObjB1FrameX.c\
	subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5.c\
	../../collisionFrameCalculate.h mockCollisionFrameCalculate.c
	$(CC) $(CFLAGS) -include  ../../../studiousBroccoli/utils/testMallocTracker/malloc_redefinition.h main.c ../../collDetect.c\
		subTestObjA1ObjB1FrameX.c\
		subTestObjA1ObjB1Frame12ObjA1ObjC1Frame5.c\
		mockCollisionFrameCalculate.c ../../../jTypes/jTypes.c -L ../../../studiousBroccoli/utils/testMallocTracker  -Wl,-rpath=$(shell pwd)/../../../studiousBroccoli/utils/testMallocTracker -ltestMallocTracker -I../../../studiousBroccoli/utils/testMallocTracker  -o unitTests
	./unitTests
	gcov collDetect.c

clean:
	rm unitTests *.gcov *.gcno *.gcda
