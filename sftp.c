/************************************************************
 * This is a dangerous program. DO NOT try it in the        *
 * directory where you store your important files.          *
 *                                                          *
 * compile: gcc -g -o sftp sftp.c                           *
 * execute: ./sftp [port number]                            *
 ************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sftp.h"

void lsfile(Packet *p) {
    FILE *fp = NULL;

    system(_LS_FILES_);

    if ((fp = fopen(_TMP_FILE_, "rb")) == NULL) {
        strcpy(p->buf, "file list fail\n");
        return;
    }
    if (fread(p->buf, 1, BUFFER_SIZE, fp) < 0) {
        strcpy(p->buf, "no file\n");
        return;
    }
    if(fclose(fp)) {
        printf("file close fail\n");
        exit(1);
    }
}

void txfile(Packet *p) {
    FILE *fp = NULL;

    p->type = _TYPE_PUT_;

    if ((fp = fopen(p->filename, "rb")) == NULL) {
        strcpy(p->buf, "file read fail\n");
        return;
    }
    if (fread(p->buf, 1, BUFFER_SIZE, fp) < 0) {
        strcpy(p->buf, "no file\n");
        return;
    }
    if(fclose(fp)) {
        printf("file close fail\n");
        exit(1);
    }
}

void rxfile(Packet *p) {
    FILE *fp = NULL;
    p->type = _TYPE_GET_;

    if ((fp = fopen(p->filename, "wb")) == NULL) {
        strcpy(p->buf, "file read fail\n");
        return;
    }
    printf("perform write\n");
    if (fwrite(p->buf, 1, BUFFER_SIZE, fp) < 0) {
        strcpy(p->buf, "write fails\n");
        return;
    }
    if(fclose(fp)) {
        printf("file close fail\n");
        exit(1);
    }
}

void DoParse(Packet *p) {
    char cmd[32];

#ifdef PRINTSTACK
    int i, *ptr;
    ptr = cmd;
    for (i = 26; i >= 0; i--) {
        printf("%8X : %8X\n", &ptr[i], ptr[i]);
    }
    printf("\n");
#endif

    strcpy(cmd, p->filename);
    if(!strcmp(cmd, "ls")) {
        lsfile(p);
    }

#ifdef PRINTSTACK
    ptr = cmd;
    for (i = 26; i >= 0; i--) {
        printf("%8X : %8X\n", &ptr[i], ptr[i]);
    }
#endif
}

unsigned long get_ESP_address(void) {
    __asm__("movl %esp, %eax");
}


int main(int argc, char *argv[]) {
    int create_socket, new_socket, addrlen, port = 15000;
    Packet recvPacket;
    struct sockaddr_in address;

    printf("Current stack address: %8X\n", &address);
    printf("system() call address: %8X\n", system);	

    if (argc > 1) {
        port = atoi(argv[1]);
    }
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        printf("socket created\n");
    } else {
        printf("Fail to create socket\n");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
 
    if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0) {
        printf("Socket binded to port %d\n", port);
    } else {
        printf("Fail to bind socket\n");
        return 1;
    }

    listen(create_socket, 3);
    addrlen = sizeof(struct sockaddr_in);
    new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen);

    if (new_socket > 0) {
        printf("The Client %s is connected...\n", inet_ntoa(address.sin_addr));
    }

    do {
        bzero((char *) &recvPacket, sizeof(Packet));
        recv(new_socket, (char *) &recvPacket, sizeof(Packet), 0);
        switch (recvPacket.type) {
            case _TYPE_CMD_:
                printf("user issues command:");
                printf(recvPacket.filename);
                printf("\n");
                DoParse(&recvPacket);
                break;
            case _TYPE_GET_:
                printf("user get file:");
                printf(recvPacket.filename);
                printf("\n");
                txfile(&recvPacket);
                break;
            case _TYPE_PUT_:
                printf("user put file:");
                printf(recvPacket.filename);
                printf("\n");
                rxfile(&recvPacket);
                break;
        }
        send(new_socket, (char *) &recvPacket, sizeof(Packet), 0);
    } while(recvPacket.type != _TYPE_END_); 

    close(new_socket);
    close(create_socket); 
}
