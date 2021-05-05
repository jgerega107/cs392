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
    fflush(stdin);
    int br = read(STDIN_FILENO, outbuf, MAX_MSG_LEN);
    outbuf[br - 1] = '\0';
    if(br-1 > strlen(outbuf)){
        printf("Sorry, limit your message to %d characters.\n", MAX_MSG_LEN);
        return 1;
    }
    if(strcmp(outbuf, "bye") == 0){
        printf("Goodbye.\n");
        return 0;
    }
    if(strcmp(outbuf, "") == 0){
        return 1;
    }
    ssize_t bytes_read;
    if((bytes_read = send(client_socket, outbuf, MAX_MSG_LEN, 0)) == -1){
        fprintf(stderr, "Error: Failed to send message to server. %s.\n", strerror(errno));
    }
    else{
        return 1;
    }
}

int handle_client_socket(){
    int bytes_recvd = recv(client_socket, inbuf, MAX_MSG_LEN, 0);
    if(bytes_recvd > 0){
        inbuf[bytes_recvd] = '\0';
    }
    else if(bytes_recvd == 0){
        printf("\nConnection to the server has been lost.\n");
        return -1;
    }
    else{
        fprintf(stderr, "Failed to receive incoming message. %s.\n", strerror(errno));
        return -1;
    }
    if(strcmp(inbuf, "bye") == 0){
        printf("Exit\n");
        return 0;
    }
    printf("\n%s\n", inbuf);
    return 1;
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
    fflush(stdout);
    printf("Enter your username: ");
    fflush(stdout);
    while(true){
        fflush(stdin);
        int br = read(STDIN_FILENO, username, MAX_NAME_LEN);
        username[br - 1] = '\0';
        if(br-1 > MAX_NAME_LEN){
            printf("Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
        }
        else if(strlen(username) <= 0){
            printf("Username must be at least 1 character.\n");
        }
        else{
            printf("Hello, %s. Let's try to connect to the server.\n", username);
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
        int bytes_recvd = recv(client_socket, inbuf, MAX_MSG_LEN, 0);
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
    printf("\n%s\n\n", inbuf);
    //Send username to server
    ssize_t bytes_read;
    if((bytes_read = send(client_socket, username, MAX_NAME_LEN, 0)) == -1){
        fprintf(stderr, "Error: Failed to send username to server. %s.\n", strerror(errno));
    }
    
    while(true){
        fflush(stdout);
        printf("[%s]: ", username);
        fflush(stdout);
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(client_socket, &fds);
        FD_SET(STDIN_FILENO, &fds);
        int retval = select(client_socket+1, &fds, NULL, NULL, NULL);
        if(retval < 0){
            fprintf(stderr, "Error: Select failed. %s.\n", strerror(errno));
        }
        else{
            if(FD_ISSET(client_socket, &fds)){
                int rval = handle_client_socket();
                if(rval != 1){
                    goto EXIT;
                }
            }
            else if(FD_ISSET(STDIN_FILENO, &fds)){
                int rval = handle_stdin();
                if(rval != 1){
                    goto EXIT;
                }
            }
        }
    }

EXIT:
    if(fcntl(client_socket, F_GETFD) >= 0){
        close(client_socket);
    }
    return EXIT_SUCCESS;
}