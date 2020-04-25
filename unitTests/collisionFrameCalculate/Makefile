CC = gcc

CFLAGS = -g -Wall -lm -I../../../jTypes --coverage

all:  main.c pointVLine.h pointVLine.c pointHLine.h pointHLine.c pointRect.h pointRect.c lineRect.h lineRect.c rectRect.h rectRect.c VLineVLine.h VLineVLine.c HLineHLine.c ../../../jTypes/jTypes.h ../../../jTypes/jTypes.c ../../collisionFrameCalculate.h ../../collisionFrameCalculate.c
	$(CC) $(CFLAGS) main.c ../../collisionFrameCalculate.c pointVLine.c pointHLine.c pointRect.c lineRect.c rectRect.c VLineVLine.c HLineHLine.c ../../../jTypes/jTypes.c -o unitTests
	./unitTests
	gcov collisionFrameCalculate.c

clean:
	rm unitTests *.gcov *.gcno *.gcda
