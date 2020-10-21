CC=gcc
CFLAGS=-Wall
src = $(wildcard *.c)
obj = $(src:.c=.o)

all: reader writer

reader: reader.c dataLinker.o dataLinker.h 
	$(CC) $(CFLAGS) -o reader dataLinker.o reader.c

writer: writer.c dataLinker.o dataLinker.h 
	$(CC) $(CFLAGS) -o writer dataLinker.o writer.c

dataLynker.o: dataLinker.c dataLinker.h
	gcc -c dataLinker.c

.PHONY: clean
clean:
	rm -f $(obj) reader writer 
