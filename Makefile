CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined

run: mymalloc.o err.o
	$(CC) $(CFLAGS) -o program $^

debug: mymalloc-debug.o err.o
	$(CC) $(CFLAGS) -o $@ $^

memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

test: test.o
	$(CC) $(CFLAGS) -o test $^

test.o: test.c mymalloc.c
	$(CC) $(CFLAGS) -c $<

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c $< 

mymalloc-debug.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -D DEBUG=1 -o mymalloc-debug.o -c $<

err.o: err.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c $<	

clean:
	rm -f program test debug memgrind *.o