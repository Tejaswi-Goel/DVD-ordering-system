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

/**
 * The server statistics counters.
 */
typedef struct ServerStats {
    unsigned long numConns;    /* # of client connections */
    unsigned long numRequests; /* # of client requests */
    unsigned long numOrders;   /* # of orders */
    unsigned long orderedDVDs; /* # of DVD copies being sold */
} ServerStats;

static ServerStats svrStats;

/**
 * The entire collection of DVDs.
 */
static DVD allDVDs[] = {
    { 1, 10, "Birdman" },
    { 2, 20, "Twelve Years a Slave" },
    { 3, 30, "Argo" },
    { 4, 40, "The Artist" },
    { 5, 50, "The King's Speech" },
};


/**
 **************************************************************************
 *
 * \brief Show the usage message and exit the program.
 *
 **************************************************************************
 */
static void
Usage(const char *prog) // IN
{
    Log("Usage:\n");
    Log("    %s <port>\n", prog);
    exit(EXIT_FAILURE);
}


/**
 **************************************************************************
 *
 * \brief Parse command line arguments.
 *
 **************************************************************************
 */
void
ParseArgs(int argc,            // IN
          char *argv[],        // IN
          ServerArgs *svrArgs) // OUT
{
    if (argc != 2) {
        Usage(argv[0]);
    }
    svrArgs->listenPort = atoi(argv[1]);
    if (svrArgs->listenPort == 0) {
        Usage(argv[0]);
    }
}


/**
 **************************************************************************
 *
 * \brief Handler for MSG_QUERY_INVENTORY.
 *
 **************************************************************************
 */
static Bool
ProcessInventory(int sd,              // IN
                 const Msg *req,      // IN
                 const char *cliName) // IN
{
    Msg reply;

    PrintMsg(req, cliName);

    memset(&reply, 0, sizeof reply);
    reply.type                 = MSG_INVENTORY;
    reply.u.inventory.numItems = ARRAYSIZE(allDVDs);
    
    /* Send the inventory message header */
    if (WriteFully(sd, &reply, sizeof reply) <= 0) {
        return FALSE;
    }
    /* Send the entire DVD inventory */
    if (WriteFully(sd, allDVDs, sizeof allDVDs) <= 0) {
        return FALSE;
    }

    PrintMsg(&reply, cliName);
    return TRUE;
}


/**
 **************************************************************************
 *
 * \brief Handler for MSG_ORDER_REQUEST.
 *
 **************************************************************************
 */
static Bool
ProcessOrder(int sd,              // IN
             const Msg *req,      // IN
             const char *cliName) // IN
{
    Msg reply;
    int orderQty    = req->u.order.qty;
    int orderStatus = ORDER_DVD_NOT_FOUND;

    PrintMsg(req, cliName);

    svrStats.numOrders++;

    for (int i = 0; i < ARRAYSIZE(allDVDs); i++) {
        if (allDVDs[i].id == req->u.order.id) {
            if (allDVDs[i].qty < orderQty) {
                orderStatus = ORDER_QTY_NOT_ENOUGH;
            } else {
                allDVDs[i].qty -= orderQty;
                svrStats.orderedDVDs += orderQty;
                orderStatus = ORDER_SUCCESS;
            }
            break;
        }
    }

    memset(&reply, 0, sizeof reply);
    reply.type           = MSG_ORDER_STATUS;
    reply.u.order.id     = req->u.order.id;
    reply.u.order.qty    = orderQty;
    reply.u.order.status = orderStatus;

    if (WriteFully(sd, &reply, sizeof reply) <= 0) {
        return FALSE;
    }

    PrintMsg(&reply, cliName);
    return TRUE;
}


/**
 **************************************************************************
 *
 * \brief The server loop to handle requests from a particular client.
 *
 **************************************************************************
 */
void
Server(int sd,               // IN
       const char *cliName)  // IN
{
    Bool running = TRUE;

    Log("\nClient %s connected\n", cliName);
    svrStats.numConns++;

    while (running) {
        Msg req;
        if (ReadFully(sd, &req, sizeof(req)) <= 0) {
            running = FALSE;
            break;
        }
        svrStats.numRequests++;
        switch (req.type) {
            case MSG_QUERY_INVENTORY:
                running = ProcessInventory(sd, &req, cliName);
                break;
            case MSG_ORDER_REQUEST:
                running = ProcessOrder(sd, &req, cliName);
                break;
            default:
                Error("   [%s] Unknown message type %d\n", cliName, req.type);
                running = FALSE;
        }
    }
    close(sd);
    Log("Client %s disconnected\n\n", cliName);
}


/**
 **************************************************************************
 *
 * \brief Print the server statistics.
 *
 **************************************************************************
 */
void
PrintServerStats(void)
{
    Log("**************************************************************\n");
    Log("Summary:\n");
    Log("    Total number of connections   : %ld\n", svrStats.numConns);
    Log("    Total number of requests      : %ld\n", svrStats.numRequests);
    Log("    Total number of orders        : %ld\n", svrStats.numOrders);
    Log("    Total number of DVD being sold: %ld\n", svrStats.orderedDVDs);
    Log("**************************************************************\n");
}

