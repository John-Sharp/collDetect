CC = gcc

CFLAGS = -g -Wall -lm -I../../jTypes --coverage

all: main.c pointVLine.h pointVLine.c pointHLine.h pointHLine.c VLineVLine.h VLineVLine.c ../collDetect.h ../collDetect.c
	$(CC) $(CFLAGS) main.c ../collDetect.c pointVLine.c pointHLine.c VLineVLine.c -o unitTests
	./unitTests
	gcov collDetect.c

clean:
	rm unitTests *.gcov *.gcno *.gcda
