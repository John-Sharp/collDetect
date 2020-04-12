CC = gcc

CFLAGS = -g -Wall -lm -I../../jTypes --coverage

all: main.c pointVLine.h pointVLine.c pointHLine.h pointHLine.c pointRect.h pointRect.c lineRect.h lineRect.c VLineVLine.h VLineVLine.c HLineHLine.c ../../jTypes/jTypes.h ../../jTypes/jTypes.c ../collDetect.h ../collDetect.c
	$(CC) $(CFLAGS) main.c ../collDetect.c pointVLine.c pointHLine.c pointRect.c lineRect.c VLineVLine.c HLineHLine.c ../../jTypes/jTypes.c -o unitTests
	./unitTests
	gcov collDetect.c

clean:
	rm unitTests *.gcov *.gcno *.gcda
