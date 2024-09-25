#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/select.h>
#define PORT 8083

int main()
{
    int socketfd, newsocket;
    struct sockaddr_in client;
    char message[25];

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd == -1)
    {
        printf("Socket creation failed\n");
        exit(1);
    }

    printf("Socket Craeted sucessfully\n");

    bzero(&client, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    client.sin_port = htons(PORT);

    // Connect to the server

    if (connect(socketfd, (struct sockaddr *)&client, sizeof(client)) != 0)
    {
        printf("Connection with server failed\n");
        exit(1);
    }

    printf("Client connected to the server\n");

    while (1)
    {
        bzero(message, 25);
        int valread = read(socketfd, message, sizeof(message));

        if (valread <= 0)
        {
            printf("Server closed the connection or an error occurred.\n");
            break;
        }

        printf("Received %s\n", message);

        // Prepare ACK message

        char ACK[25];
        char *ack_prefix = "ACK for ";

        // size_t max_len = sizeof(ACK) - strlen(ack_prefix) - 1;
        snprintf(ACK, sizeof(ACK), "%s%s", ack_prefix, message);

        if (strlen(ACK) >= sizeof(ACK))
        {
            ACK[sizeof(ACK) - 1] = '\0'; // Null terminate if it exceeds
        }

        // Send acknowledgment back to server
        send(socketfd, ACK, strlen(ACK), 0);
        printf("Sent: %s\n", ACK);
    }

    close(socketfd);
}