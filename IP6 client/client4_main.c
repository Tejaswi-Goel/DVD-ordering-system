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
#include <netdb.h>

#include "common.h"
#include "client.h"


/**
 **************************************************************************
 *
 * \brief Create a client TCP socket and connect to the server.
 *
 **************************************************************************
 */
int
CreateClientTCP(const char *svrHost,     // IN
                unsigned short svrPort,  // IN
                char *svrName,           // OUT
                int svrNameLen)          // IN
{

	struct addrinfo hints, *result, *rp;

    int sock,s;
    struct sockaddr_in svrAddr;
//struct sockaddr_in6 svrAddr1;
 int con;

	memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET6;    /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
        hints.ai_flags = 0;

        hints.ai_protocol = 0; 
	
	con= svrPort;
    	char sport[50]; //= (const char *)svrPort;
        snprintf(sport, 50, "%d",con);       


	s= getaddrinfo((const char *)svrHost, (const char *)sport, &hints, &result);
if(s == 0)
{	for (rp = result; rp != NULL; rp = rp->ai_next) {
               sock= socket(rp->ai_family, rp->ai_socktype,
                            rp->ai_protocol);

               if (sock == -1)
                   continue;

               if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
/*{
 if (inet_pton(AF_INET6, svrHost, &svrAddr.sin_addr.s_addr) <= 0) {
        perror("Failed to convert IP address\n");
        exit(EXIT_FAILURE);
    }


    
    Log("Attempting %s\n", svrName);
}*/
	SocketAddrToString(&svrAddr, svrName, svrNameLen);
                   break;                  /* Success */
    
           }
	return sock;
	freeaddrinfo(result);
}
else 
{

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Failed to allocate the client socket");
        exit(EXIT_FAILURE);
    }

    memset(&svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_port   = htons(svrPort);

    if (inet_pton(AF_INET, svrHost, &svrAddr.sin_addr.s_addr) <= 0) {
        perror("Failed to convert IP address\n");
        exit(EXIT_FAILURE);
    }

    SocketAddrToString(&svrAddr, svrName, svrNameLen);
    Log("Attempting %s\n", svrName);

    if (connect(sock, (struct sockaddr *)&svrAddr, sizeof(svrAddr)) < 0) {
        perror("Failed to connect to the server");
        exit(EXIT_FAILURE);
    }
return sock;
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
main(int argc, char *argv[])
{
    int sock;
    ClientArgs cliArgs;
    char svrName[INET6_ADDRSTRLEN + PORT_STRLEN];

    ParseArgs(argc, argv, &cliArgs);

    sock = CreateClientTCP(cliArgs.svrHost, cliArgs.svrPort,
                           svrName, sizeof svrName);

    Log("Connected to server at %s\n", svrName);

    Client(sock, &cliArgs);

    close(sock);
    Log("Disconnected from server at %s\n", svrName);
    return 0;


}

