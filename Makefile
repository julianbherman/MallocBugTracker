CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined

run: mymalloc.o err.o
	$(CC) $(CFLAGS) -o program $^

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

err.o: err.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f program *.o