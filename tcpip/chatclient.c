#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util.h"

int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];

int handle_stdin(){

}

int handle_client_socket(){

}

int main(int argc, char* argv[]){
    //Check validity of arguments
    if(argc != 3){
        printf("Usage: %s <server IP> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    int port;
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0){
        printf("Error: Invalid IP address '%s'.\n", argv[1]);
        return EXIT_FAILURE;
    }
    if(!parse_int(argv[2], &port, "port number")){
        return EXIT_FAILURE;
    }
    if(!(port >= 1024 && port <= 65535)){
        fprintf(stderr, "Error: Port must be in range [1024, 65535].\n");
        return EXIT_FAILURE;
    }

    char name[BUFLEN];
    printf("Hello, what would you like to be called?\n");
    while(true){
        fflush(stdin);
        int br = read(STDIN_FILENO, name, BUFLEN);
        name[br - 1] = '\0';
        if(strlen(name) > MAX_NAME_LEN){
            printf("Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
        }
        else if(strlen(name) <= 0){
            printf("Username must be at least 1 character.\n");
        }
        else{
            printf("Hello, %s. Let's try to connect to the server.\n", name);
            break;
        }
    }

    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "Error: Failed to create socket. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) != 0){
        fprintf(stderr, "Error: Failed to connect to server. %s.\n", strerror(errno));
        goto EXIT;
    }
    else{
        int bytes_recvd = recv(client_socket, inbuf, MAX_MSG_LEN-1, 0);
        if(bytes_recvd > 0){
            inbuf[bytes_recvd] = '\0';
        }
        else if(bytes_recvd == 0){
            fprintf(stderr, "Failed to receive message from server. %s.\n", strerror(errno));
            goto EXIT;
        }
        else{
            printf("All connections are busy. Try again later.\n");
            goto EXIT;
        }
    }
    printf("Welcome message: %s\n\n", inbuf);
EXIT:
    if(fcntl(client_socket, F_GETFD) >= 0){
        close(client_socket);
    }
    return EXIT_SUCCESS;
}