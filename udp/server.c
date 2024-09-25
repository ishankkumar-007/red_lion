#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 1234
#define MAX 20

void chat(int socktfd, struct sockaddr_in serveradd)
{
    char buffer[MAX];
    char message[MAX];
    int len = sizeof(serveradd);

    while (1)
    {
        bzero(buffer, MAX);

        printf("Reading message from client..\n");
        int n = recvfrom(socktfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serveradd, &len);
        buffer[n] = '\0';

        printf("Message received from Client: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0)
        {
            printf("Client exited the chat.\n");
            break;
        }

        bzero(message, MAX);

        printf("Enter message to send to client: ");
        scanf(" %[^\n]", message);

        sendto(socktfd, message, MAX, 0, (struct sockaddr *)&serveradd, len);
        printf("message sent to client\n");

        if (strcmp(message, "exit") == 0)
        {
            printf("Server exited the chat.\n");
            break;
        }
    }
}

int main()
{
    int sockfd, len;
    struct sockaddr_in serveradd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1)
    {
        printf("Socket Craetion Failed!\n");
    }

    printf("Socket Created sucessfully\n");

    bzero(&serveradd, sizeof(serveradd));

    serveradd.sin_family = AF_INET;
    serveradd.sin_addr.s_addr = htonl(INADDR_ANY);
    serveradd.sin_port = htons(PORT);

    if ((bind(sockfd, (struct sockaddr *)&serveradd, sizeof(serveradd))) != 0)
    {
        printf("Bind failed\n");
        exit(1);
    }

    printf("Socket bind sucessful\n");

    chat(sockfd, serveradd);

    return 0;
}