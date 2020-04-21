CC = gcc

CFLAGS = -g -Wall -lm -I../../jTypes --coverage

generateCollDetectListCode: ../../dataStructures/templates/listTemplate.h ../../dataStructures/templates/listTemplate.inc
	mkdir -p ../listHeaders ../listCode
	../../dataStructures/generateList.py ../listHeaders ../listCode collGroup:collGroup
	../../dataStructures/generateList.py ../listHeaders ../listCode collActor:collActor
	../../dataStructures/generateList.py ../listHeaders ../listCode collActorCategoryNumberGrouped:collActorCategoryNumberGrouped
	../../dataStructures/generateList.py ../listHeaders ../listCode collInfo:collInfo

all: generateCollDetectListCode main.c pointVLine.h pointVLine.c pointHLine.h pointHLine.c pointRect.h pointRect.c lineRect.h lineRect.c rectRect.h rectRect.c VLineVLine.h VLineVLine.c HLineHLine.c ../../jTypes/jTypes.h ../../jTypes/jTypes.c ../collDetect.h ../collDetect.c
	$(CC) $(CFLAGS) -include  ../../studiousBroccoli/utils/testMallocTracker/malloc_redefinition.h main.c ../collDetect.c pointVLine.c pointHLine.c pointRect.c lineRect.c rectRect.c VLineVLine.c HLineHLine.c ../../jTypes/jTypes.c -L ../../studiousBroccoli/utils/testMallocTracker  -Wl,-rpath=$(shell pwd)/../../studiousBroccoli/utils/testMallocTracker -ltestMallocTracker -I../../studiousBroccoli/utils/testMallocTracker  -o unitTests
	./unitTests
	gcov collDetect.c

clean:
	rm unitTests *.gcov *.gcno *.gcda
