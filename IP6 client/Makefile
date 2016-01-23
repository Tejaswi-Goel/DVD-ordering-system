##****************************************************************************
## CMPE 207 (Network Programming and Applications) Sample Program.
##
## San Jose State University, Copyright (2015) Reserved.
##
## DO NOT REDISTRIBUTE WITHOUT THE PERMISSION OF THE INSTRUCTOR.
##****************************************************************************

CC=gcc
CCFLAGS=-g -std=c99 -D_BSD_SOURCE -Wall -std=gnu99 -D_POSIX_SOURCE 

TARGETS=client4

all: $(TARGETS)


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

