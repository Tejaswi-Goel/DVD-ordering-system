/*****************************************************************************
 * CMPE 207 (Network Programming and Applications) Sample Program.
 *
 * San Jose State University, Copyright (2015) Reserved.
 *
 * DO NOT REDISTRIBUTE WITHOUT THE PERMISSION OF THE INSTRUCTOR.
 *****************************************************************************
 */

#ifndef _SERVER_H_
#define _SERVER_H_

/**
 * The server command line arguments.
 */
typedef struct ServerArgs {
    unsigned short listenPort;
} ServerArgs;

void ParseArgs(int argc, char *argv[], ServerArgs *svrArgs);
void Server(int sd, const char *cliName);
void PrintServerStats(void);

#endif

