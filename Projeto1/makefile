CC=gcc
CFLAGS=-Wall
src = $(wildcard *.c)
obj = $(src:.c=.o)

all: reader writer

reader: nc3.c dataLynker.o dataLynker.h 
	$(CC) $(CFLAGS) -o reader dataLynker.o nc3.c

writer: wnc3.c dataLynker.o dataLynker.h 
	$(CC) $(CFLAGS) -o writer dataLynker.o wnc3.c

dataLynker.o: dataLynker.c dataLynker.h
	gcc -c dataLynker.c

.PHONY: clean
clean:
	rm -f $(obj) reader writer 
