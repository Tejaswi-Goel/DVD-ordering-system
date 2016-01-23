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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"
#include "client.h"


/**
 **************************************************************************
 *
 * \brief Print the usage message and exit the program.
 *
 **************************************************************************
 */
static void
Usage(const char *prog) // IN
{
    Log("Usage:\n");
    Log("    %s <server_ip> <server_port> list\n", prog);
    Log("    %s <server_ip> <server_port> order <dvd_id> <qty>\n", prog);
    exit(EXIT_FAILURE);
}


/**
 **************************************************************************
 *
 * \brief Parse the client command line arguments.
 *
 **************************************************************************
 */
void
ParseArgs(int argc,             // IN
          char *argv[],         // IN
          ClientArgs *cliArgs)  // OUT
{
    if (argc < 4) {
        Usage(argv[0]);
    }

    memset(cliArgs, 0, sizeof *cliArgs);

    cliArgs->svrHost = argv[1];
    cliArgs->svrPort = atoi(argv[2]);
    if (cliArgs->svrPort == 0) {
        Usage(argv[0]);
    }

    if (strcasecmp(argv[3], "list") == 0) {
        cliArgs->command = MSG_QUERY_INVENTORY;
    } else if (strcasecmp(argv[3], "order") == 0) {
        if (argc < 6) {
            Usage(argv[0]);
        }
        cliArgs->command  = MSG_ORDER_REQUEST;
        cliArgs->orderDVD = atoi(argv[4]);
        cliArgs->orderQty = atoi(argv[5]);
    } else {
        Usage(argv[0]);
    }
}


/**
 **************************************************************************
 *
 * \brief Handler to send MSG_QUERY_INVENTORY and process the reply.
 *
 **************************************************************************
 */
static void
HandleQueryInventory(int sd) // IN
{
    Msg req, reply;

    memset(&req, 0, sizeof req);
    req.type = MSG_QUERY_INVENTORY;

    PrintMsg(&req, "");

    /* Send the query request */
    if (WriteFully(sd, &req, sizeof req) <= 0) {
        return;
    }

    /* Read the inventory message header */
    if (ReadFully(sd, &reply, sizeof reply) <= 0) {
        return;
    }
    if (reply.type != MSG_INVENTORY) {
        Error("Unexpected reply message type %d\n", reply.type);
        return;
    }

    PrintMsg(&reply, "");

    /* Read the DVD inventory */
    Log("***********************************************************\n");
    Log("%6s    %-32s  %8s\n", "DVD_ID", "TITLE", "QTY");
    for (int i = 0; i < reply.u.inventory.numItems; i++) {
        DVD dvd;
        if (ReadFully(sd, &dvd, sizeof dvd) <= 0) {
            return;
        }
        Log("%6d    %-32s  %8d\n", dvd.id, dvd.title, dvd.qty);
    }
    Log("***********************************************************\n");
}


/**
 **************************************************************************
 *
 * \brief Handler to send MSG_ORDER_REQUEST and process the reply.
 *
 **************************************************************************
 */
static void
HandleOrderRequest(int sd,             // IN
                   unsigned orderDVD,  // IN
                   unsigned orderQty)  // IN
{
    Msg req, reply;

    memset(&req, 0, sizeof req);
    req.type        = MSG_ORDER_REQUEST;
    req.u.order.id  = orderDVD;
    req.u.order.qty = orderQty;

    PrintMsg(&req, "");

    /* Send the order request */
    if (WriteFully(sd, &req, sizeof req) <= 0) {
        return;
    }

    /* Read the order status */
    if (ReadFully(sd, &reply, sizeof reply) <= 0) {
        return;
    }

    if (reply.type != MSG_ORDER_STATUS) {
        Error("Unexpected reply message type %d\n", reply.type);
        return;
    }

    PrintMsg(&reply, "");
    Log("\n");
}


/**
 **************************************************************************
 *
 * \brief Dispatch client commands.
 *
 **************************************************************************
 */
void
Client(int sock,                   // IN
       const ClientArgs *cliArgs)  // IN
{
    switch (cliArgs->command) {
        case MSG_QUERY_INVENTORY:
            HandleQueryInventory(sock);
            break;
        case MSG_ORDER_REQUEST:
            HandleOrderRequest(sock, cliArgs->orderDVD, cliArgs->orderQty);
            break;
        default:
            Error("Unexpected command type %d\n", cliArgs->command);
    }
}

