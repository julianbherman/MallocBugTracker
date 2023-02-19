CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined

run: mymalloc.o err.o
	$(CC) $(CFLAGS) -o program $^

test: test.o
	$(CC) $(CFLAGS) -o test $^

test.o: test.c mymalloc.c
	$(CC) $(CFLAGS) -c $<

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c $< 

err.o: err.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f program test *.o