/*****************************************************************************
 * CMPE 207 (Network Programming and Applications) Sample Program.
 *
 * San Jose State University, Copyright (2015) Reserved.
 *
 * DO NOT REDISTRIBUTE WITHOUT THE PERMISSION OF THE INSTRUCTOR.
 *****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "common.h"
#include "server.h"

static int            listenSocket    = -1;
static volatile Bool  listenerRunning = TRUE;


/**
 **************************************************************************
 *
 * \brief Signal handler to stop the server with Ctrl-C.
 *
 **************************************************************************
 */
static void
SignalHandler(int signo)
{
    if (signo == SIGINT) {
        if (listenSocket > 0) {
            shutdown(listenSocket, SHUT_RDWR);
        }
        listenerRunning = FALSE;
    }
}


/**
 **************************************************************************
 *
 * \brief Create a TCP listen socket on the given port.
 *
 **************************************************************************
 */
static int
CreatePassiveTCP(unsigned port)
{
    int msock;
    struct sockaddr_in svrAddr;

    msock = socket(AF_INET, SOCK_STREAM, 0);
    if (msock < 0) {
        perror("Failed to allocate the listen socket");
        exit(EXIT_FAILURE);
    }

    memset(&svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family      = AF_INET;
    svrAddr.sin_port        = htons(port);
    svrAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(msock, (struct sockaddr *)&svrAddr, sizeof svrAddr) < 0) {
        perror("Failed to bind IP address and port to the listen socket");
        exit(EXIT_FAILURE);
    }

    if (listen(msock, 5) < 0) {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    return msock;
}


/**
 **************************************************************************
 *
 * \brief The server loop to accept new client connections.
 *
 **************************************************************************
 */
static void
ServerListenerLoop(void)
{
    while (listenerRunning) {
        int ssock;
        struct sockaddr_in cliAddr;
        struct sockaddr_in localAddr;
        socklen_t cliAddrLen;
        socklen_t localAddrLen;
        char cliName[INET_ADDRSTRLEN + PORT_STRLEN];
        char svrName[INET_ADDRSTRLEN + PORT_STRLEN];

        cliAddrLen = sizeof cliAddr;
        ssock = accept(listenSocket, (struct sockaddr *)&cliAddr, &cliAddrLen);
        if (ssock < 0) {
            if (listenerRunning && ssock != EINTR) {
                perror("Failed to accept a connection");
                listenerRunning = FALSE;
            }
            continue;
        }

        localAddrLen = sizeof localAddr;
        if (getsockname(ssock,
                        (struct sockaddr *)&localAddr,
                        &localAddrLen) < 0) {
            perror("Failed to get server address info for new connection");
            close(ssock);
            continue;
        }
        SocketAddrToString(&localAddr, svrName, sizeof svrName);
        SocketAddrToString(&cliAddr, cliName, sizeof cliName);
        Log("Accepted client %s at server %s\n", cliName, svrName);

        Server(ssock, cliName);
    }
}


/**
 **************************************************************************
 *
 * \brief Main entry point.
 *
 **************************************************************************
 */
int
main(int argc,      // IN
     char *argv[])  // IN
{
    ServerArgs svrArgs;

    signal(SIGINT, SignalHandler);

    ParseArgs(argc, argv, &svrArgs);

    listenSocket = CreatePassiveTCP(svrArgs.listenPort);

    Log("\nServer started listening at *:%u\n", svrArgs.listenPort);
    Log("Press Ctrl-C to stop the server.\n\n");

    ServerListenerLoop();

    close(listenSocket);
    Log("Server stopped listening at *:%u\n", svrArgs.listenPort);

    PrintServerStats();
    return 0;
}

