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
#include <stdarg.h>
#include <arpa/inet.h>

#include "common.h"


/**
 **************************************************************************
 *
 * \brief Log a message.
 *
 **************************************************************************
 */
void
Log(const char *fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    vfprintf(stdout, fmt, arg);
    va_end(arg);
}


/**
 **************************************************************************
 *
 * \brief Log an error message.
 *
 **************************************************************************
 */
void
Error(const char *fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
}


/**
 **************************************************************************
 *
 * \brief Read/write the entire buffer from/to a socket.
 *
 **************************************************************************
 */
static int
ReadOrWriteFully(int sd,      // IN
                 void *buf,   // IN (write), OUT (read)
                 int nbytes,  // IN
                 int isRead)  // IN
{
    int bytesLeft = nbytes;

    while (bytesLeft > 0) {
        int n = isRead ? read(sd, buf, bytesLeft) :
                         write(sd, buf, bytesLeft);
        if (n <= 0) {
            if (n < 0) {
                Error("%s error: %d\n", isRead ? "read" : "write", n);
            }
            return n;
        }
        bytesLeft -= n;
        buf += n;
    }
    return nbytes;
}


/**
 **************************************************************************
 *
 * \brief Read the entire buffer from the socket.
 *
 **************************************************************************
 */
int
ReadFully(int sd,      // IN
          void *buf,   // OUT
          int nbytes)  // IN
{
    return ReadOrWriteFully(sd, buf, nbytes, TRUE);
}


/**
 **************************************************************************
 *
 * \brief Write the entire buffer to the socket.
 *
 **************************************************************************
 */
int
WriteFully(int sd,     // IN
           void *buf,  // IN
           int nbytes) // IN
{
    return ReadOrWriteFully(sd, buf, nbytes, FALSE);
}


/**
 **************************************************************************
 *
 * \brief Convert a socket endpoint address to a string of "ip:port".
 *
 **************************************************************************
 */
void
SocketAddrToString(const struct sockaddr_in6 *addr,  // IN
                   char *addrStr,                   // OUT
                   int addrStrLen)                  // IN
{
    char ipAddrStr[INET6_ADDRSTRLEN];

    inet_ntop(AF_INET6, &addr->sin6_addr, ipAddrStr, sizeof ipAddrStr);
    snprintf(addrStr, addrStrLen, "%s:%u", ipAddrStr, ntohs(addr->sin6_port));
}


/**
 **************************************************************************
 *
 * \brief Print out a given protocol message.
 *
 **************************************************************************
 */
void
PrintMsg(const Msg *msg,      // IN
         const char *prefix)  // IN
{
    static char *orderStatusStr[] = {
        "OK",
        "DVD not found",
        "Not enough DVDs",
    };

    switch (msg->type) {
        case MSG_QUERY_INVENTORY:
            Log("   %s Request: QUERY_INVENTORY\n", prefix);
            break;
        case MSG_INVENTORY:
            Log("   %s Reply:   INVENTORY (numItems %u)\n",
                prefix, msg->u.inventory.numItems);
            break;
        case MSG_ORDER_REQUEST:
            Log("   %s Request: ORDER_REQUEST (id %d qty %d)\n",
                prefix, msg->u.order.id, msg->u.order.qty);
            break;
        case MSG_ORDER_STATUS:
            if (msg->u.order.status <= ORDER_STATUS_LAST) {
                Log("   %s Reply:   ORDER_STATUS  "
                    "(id %d qty %d status %d '%s')\n",
                    prefix, msg->u.order.id, msg->u.order.qty,
                    msg->u.order.status, orderStatusStr[msg->u.order.status]);
            } else {
                Log("   %s Reply:   ORDER_STATUS  "
                    "(id %d qty %d status %d)\n",
                    prefix, msg->u.order.id, msg->u.order.qty,
                    msg->u.order.status);
            }
            break;
        default:
            Log("   %s Unknown message type %d\n", prefix, msg->type);
    }
}


