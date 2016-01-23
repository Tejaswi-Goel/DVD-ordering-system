##****************************************************************************
## CMPE 207 (Network Programming and Applications) Sample Program.
##
## San Jose State University, Copyright (2015) Reserved.
##
## DO NOT REDISTRIBUTE WITHOUT THE PERMISSION OF THE INSTRUCTOR.
##****************************************************************************

CC=gcc
CCFLAGS=-g -std=c99 -D_BSD_SOURCE -Wall

TARGETS=server4 client4

all: $(TARGETS)

server4: server4_main.o server.o common.o common.h server.h
	$(CC) $(CCFLAGS) -o $@ $^

server4_main.o: server4_main.c common.h server.h
	$(CC) $(CCFLAGS) -c $<

server.o: server.c common.h server.h
	$(CC) $(CCFLAGS) -c $<

client4: client4_main.o client.o common.o common.h client.h
	$(CC) $(CCFLAGS) -o $@ $^

client4_main.o: client4_main.c common.h client.h
	$(CC) $(CCFLAGS) -c $<

client.o: client.c common.h client.h
	$(CC) $(CCFLAGS) -c $<

common.o: common.c common.h
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -rf *.o $(TARGETS)

