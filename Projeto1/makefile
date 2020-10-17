CC=gcc
CFLAGS=-Wall
src = $(wildcard *.c)
obj = $(src:.c=.o)

all: reader writer

reader: nc3.c message.o message.h 
	$(CC) $(CFLAGS) -o reader message.o nc3.c

writer: wnc3.c message.o message.h 
	$(CC) $(CFLAGS) -o writer message.o wnc3.c

message.o: message.c message.h
	gcc -c message.c

.PHONY: clean
clean:
	rm -f $(obj) reader writer 
