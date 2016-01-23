/*****************************************************************************
 * CMPE 207 (Network Programming and Applications) Sample Program.
 *
 * San Jose State University, Copyright (2015) Reserved.
 *
 * DO NOT REDISTRIBUTE WITHOUT THE PERMISSION OF THE INSTRUCTOR.
 *****************************************************************************
 */

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"

/**
 * The client command line arguments.
 */
typedef struct ClientArgs {
    const char     *svrHost;
    unsigned short  svrPort;
    MsgType         command;
    unsigned int    orderDVD;
    unsigned int    orderQty;
} ClientArgs;

void ParseArgs(int argc, char *argv[], ClientArgs *cliArgs);
void Client(int sock, const ClientArgs *cliArgs);

#endif

