#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#define MAX_LENGTH 80
#define WIN32_LEAN_AND_MEAN

//Gets a connection socket and orchestrates communication between server and client
void com (int conn) {
    char buffer[MAX_LENGTH];

    while (1) {
        memset(buffer, '\0', MAX_LENGTH);
        printf("TX: ");
        scanf("%s", buffer);
        send(conn, buffer, MAX_LENGTH, 0);

        memset(buffer, '\0', MAX_LENGTH);
        if (recv(conn, buffer, MAX_LENGTH, 0) <= 0) {
            printf("Connection lost.\n");
            break;
        }
        printf("RX: %s\n", buffer);
    }
}

void free_heap (struct sockaddr_in *sock_addr, struct WSAData *wsadata) {
    free(sock_addr);
    free(wsadata);
}

int main (int argc, char *argv[]) {
    int conn_status = -1, wsa_status = -1;
    struct in_addr ip_addr;
    SOCKET sock;
    struct sockaddr_in *sock_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    struct WSAData *wsadata = (struct WSAData *)malloc(sizeof(struct WSAData));

    if (argc != 3) {
        printf("Usage ./client <IP> <PORT>\n");
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
    ip_addr.S_un.S_addr = inet_addr(argv[1]);
    //Setting IP, PORT, SOCKET TYPE
    sock_addr->sin_family = AF_INET;
    sock_addr->sin_addr = ip_addr;
    sock_addr->sin_port = htons(atoi(argv[2]));

    //Accepting incoming connection
    conn_status = connect(sock, (struct sockaddr *)sock_addr, sizeof(*sock_addr));

    if (conn_status == SOCKET_ERROR) {
        printf("Can't connect\n");
    } else {
        printf("Connected\n");
        com(sock);
    }

    shutdown(sock, SD_BOTH);
    closesocket(sock);

    WSACleanup();

    free_heap(sock_addr, wsadata);

    return(0);
}