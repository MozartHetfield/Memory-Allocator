CC=gcc
CFLAGS=-Wall -Wextra -c
EXE=memory_allocator

build: memory_allocator.o
	$(CC) $^ -o $(EXE)

alcator.o: memory_allocator.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf memory_allocator.o $(EXE)


