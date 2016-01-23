/*****************************************************************************
 * CMPE 207 (Network Programming and Applications) Sample Program.
 *
 * San Jose State University, Copyright (2015) Reserved.
 *
 * DO NOT REDISTRIBUTE WITHOUT THE PERMISSION OF THE INSTRUCTOR.
 *****************************************************************************
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ARRAYSIZE(_x)    (sizeof(_x) / sizeof((_x)[0]))

#define FALSE  0
#define TRUE   1

typedef unsigned char Bool;

#define PORT_STRLEN      6
#define MAX_TITLE_LEN    32

/**
 *  Data type for each DVD.
 */
typedef struct DVD {
    int  id;
    int  qty;
    char title[MAX_TITLE_LEN];
} DVD;

/**
 *  Message type exchanged between client/server.
 */
typedef enum MsgType {
    MSG_UNKNOWN            = 0,
    /* Client -> Server */
    MSG_QUERY_INVENTORY    = 1,
    MSG_ORDER_REQUEST      = 2,
    /* Server -> Client */
    MSG_INVENTORY          = 3,
    MSG_ORDER_STATUS       = 4,
} MsgType;

typedef enum OrderStatus {
    ORDER_SUCCESS          = 0,
    ORDER_DVD_NOT_FOUND    = 1,
    ORDER_QTY_NOT_ENOUGH   = 2,
    ORDER_STATUS_LAST      = 2,
} OrderStatus;

/**
 * Data type for messages exchanged between client/server.
 */
typedef struct Msg {
    int type;
    union {
        struct Inventory {
            unsigned int numItems;
        } inventory;
        struct Order {
            unsigned int id;
            unsigned int qty;
            unsigned int status;
        } order;
    } u;
} Msg;


void Log(const char *fmt, ...);
void Error(const char *fmt, ...);

int ReadFully(int sd, void *buf, int nbytes);
int WriteFully(int sd, void *buf, int nbytes);

void SocketAddrToString(const struct sockaddr_in *addr, char *addrStr,
                        int addrStrLen);

void SocketAddrToString1(const struct sockaddr_in6 *addr, char *addrStr,
                        int addrStrLen);
void PrintMsg(const Msg *msg, const char *prefix); 

#endif

