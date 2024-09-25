#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/select.h>
#define PORT 8083
#define LOSS_PROB 0.1
#define TIMEOUT 1

void start_server(int num_packets)
{
    int socketfd, newsocket;
    struct sockaddr_in serveradd, client;
    char message[25];
    char ack[25];

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd == -1)
    {
        printf("Socket creation failed\n");
        exit(1);
    }

    printf("Socket Craeted sucessfully\n");

    bzero(&serveradd, sizeof(serveradd));

    serveradd.sin_family = AF_INET;
    serveradd.sin_addr.s_addr = htonl(INADDR_ANY);
    serveradd.sin_port = htons(PORT);

    // bind the socket

    if ((bind(socketfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) != 0))
    {
        printf("binding Failed\n");
        exit(1);
    }

    printf("binding sucessfull\n");

    // Listen

    if (listen(socketfd, 3) != 0)
    {
        printf("Listen failed\n");
        exit(1);
    }

    printf("Serever listening on port %d\n", PORT);

    // Accept the client

    int len = sizeof(client);

    newsocket = accept(socketfd, (struct sockaddr *)&client, &len);
    if (newsocket < 0)
    {
        printf("Client acceptance failed\n");
    }

    printf("Client accepted by the server\n");

    for (int i = 0; i < num_packets; i++)
    {
        snprintf(message, sizeof(message), "Packet %d", i + 1);
        printf("sending : %s\n", message);

        if ((rand() / (double)RAND_MAX) < LOSS_PROB)
        {
            printf("Simulated packet loss for %s\n", message);
            continue; // Skip sending this packet
        }

        send(newsocket, message, strlen(message), 0);

        fd_set readfds;
        struct timeval timeout;
        int activity;

        while (1)
        {
            timeout.tv_sec = TIMEOUT;
            timeout.tv_usec = 0;

            FD_ZERO(&readfds);
            FD_SET(newsocket, &readfds);

            activity = select(newsocket + 1, &readfds, NULL, NULL, &timeout);

            if (activity > 0)
            {
                // ACK Received
                int valread = read(newsocket, ack, 25);
                ack[valread] = '\0';
                printf("Received: %s\n", ack);
                break; // Exit loop after receiving ACK
            }

            else if (activity == 0)
            {
                // Timeout occurred, retransmit
                printf("Timeout! Retransmitting: %s\n", message);
                send(newsocket, message, strlen(message), 0);
            }
        }
    }

    close(newsocket);
    close(socketfd);
}

int main()
{
    int num_packets;
    srand(time(NULL));
    printf("Enter the number of packets to be transmitted: ");
    scanf("%d", &num_packets);
    start_server(num_packets);

    return 0;
}
