CC = gcc

CFLAGS = -g -Wall -lm -I../../jTypes --coverage

all: main.c ../collDetect.h ../collDetect.c
	$(CC) $(CFLAGS) main.c ../collDetect.c -o unitTests
	./unitTests
	gcov collDetect.c

clean:
	rm unitTests *.gcov *.gcno *.gcda
