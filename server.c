#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#define IP "0.0.0.0"
#define MAX_LENGTH 80
#define WIN32_LEAN_AND_MEAN

//Gets a connection socket and orchestrates communication between server and client
void com (int conn) {
    char buffer[MAX_LENGTH];

    while (1) {
        memset(buffer, '\0', MAX_LENGTH);
        if (recv(conn, buffer, MAX_LENGTH, 0) <= 0) {
            printf("Connection lost.\n");
            break;
        }
        printf("RX: %s\nTX: ", buffer);

        memset(buffer, '\0', MAX_LENGTH);
        scanf("%s", buffer);
        send(conn, buffer, MAX_LENGTH, 0);
    }
}

void free_heap (struct sockaddr_in *sock_addr, struct WSAData *wsadata) {
    free(sock_addr);
    free(wsadata);
}

int main (int argc, char *argv[]) {
    int bind_status = -1, listen_status = -1, wsa_status = -1;
    struct in_addr ip_addr;
    SOCKET sock, conn;
    struct sockaddr_in *sock_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    struct WSAData *wsadata = (struct WSAData *)malloc(sizeof(struct WSAData));

    if (argc != 2) {
        printf("Usage ./server <PORT>\n");
        free_heap(sock_addr, wsadata);
        return(-1);
    }

    //Initialising Winsock v2.2
    wsa_status = WSAStartup(MAKEWORD(2,2), wsadata);
    if (wsa_status != 0) {
        printf("Error initialising Winsock: %d\n", wsa_status);
        return(wsa_status);
    }

    //Creating TCP socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //Defining IP in the structure
    ip_addr.S_un.S_addr = inet_addr(IP);
    //Setting IP, PORT, SOCKET TYPE
    sock_addr->sin_family = AF_INET;
    sock_addr->sin_addr = ip_addr;
    sock_addr->sin_port = htons(atoi(argv[1]));

    //Binding address
    bind_status = bind(sock, (struct sockaddr *)sock_addr, sizeof(*sock_addr));
    if (bind_status != 0) {
        printf("Error binding address: %d\n", bind_status);
        return(bind_status);
    }

    //Starting listener with no queue
    listen_status = listen(sock, -1);
    if (listen_status != 0) {
        printf("Error starting listener: %d\n", listen_status);
        return(listen_status);
    }

    //Accepting incoming connection
    conn = accept(sock, NULL, NULL);

    if (conn == INVALID_SOCKET) {
        printf("Client not accepted\n");
    } else {
        printf("Client accepted\n");
        com(conn);
    }

    shutdown(sock, SD_BOTH);
    closesocket(sock);
    closesocket(conn);

    free_heap(sock_addr, wsadata);

    return(0);
}