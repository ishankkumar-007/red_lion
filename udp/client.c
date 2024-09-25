#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 1234
#define MAX 20

void chat(int socktfd, struct sockaddr_in cliaddr)
{
    char buffer[MAX];
    char message[MAX];
    int len = sizeof(cliaddr);

    while (1)
    {

        bzero(message, MAX);

        printf("Enter message to send to Server: ");
        scanf(" %[^\n]", message);

        sendto(socktfd, message, MAX, 0, (struct sockaddr *)&cliaddr, len);
        printf("Message sent to server!\n");

        if (strcmp(message, "exit") == 0)
        {
            printf("Client exited the chat.\n");
            break;
        }

        bzero(buffer, MAX);

        printf("reading message from server...\n");
        int n = recvfrom(socktfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';

        printf("Message received from server: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0)
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
    serveradd.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveradd.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) < 0)
    {
        printf("Connection Failed\n");
        exit(0);
    }

    printf("Connected to server sucesfully\n");

    chat(sockfd, serveradd);

    return 0;
}