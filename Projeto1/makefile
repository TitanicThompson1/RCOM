CC=gcc
CFLAGS=-Wall
src = $(wildcard *.c)
obj = $(src:.c=.o)

all: reader writer

reader: reader.c application.o
	$(CC) $(CFLAGS) -o reader application.o dataLinker.o reader.c

writer: writer.c application.o 
	$(CC) $(CFLAGS) -o writer application.o dataLinker.o writer.c

application.o: application.c application.h dataLinker.o
	$(CC) $(CFLAGS) -c application.c dataLinker.o

dataLinker.o: dataLinker.c dataLinker.h
	$(CC) $(CFLAGS) -c dataLinker.c

.PHONY: clean
clean:
	rm -f $(obj) reader writer 
