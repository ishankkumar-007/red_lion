#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define PORT 2900

void chat(int newsocketfd)
{
    char msg[1024];

    int n;

    while (1)
    {
        printf("Reading message from the server\n");
        bzero(msg, 1024);
        printf("\nEnter a message to send from client: ");
        scanf(" %[^\n]", msg);

        write(newsocketfd, msg, sizeof(msg));
        printf("Message sent to server\n");

        bzero(msg, 1024);
        read(newsocketfd, msg, sizeof(msg));
        printf("Message from server : %s\n",msg);

        if (strncmp("exit", msg, 4) == 0)
        {
            printf("client  Exit...\n");
            break;
        }
    }
}

int main()
{
    int sockfd;
    char msg1[1024], msg2[1024];

    struct sockaddr_in clientadd;

    // Create socket

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("Socket Creation failed\n");
        exit(1);
    }

    printf("Socket created sucessfully\n");

    bzero(&clientadd, sizeof(clientadd));

    clientadd.sin_family = AF_INET;
    clientadd.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientadd.sin_port = htons(PORT);

    // Connect client to server

    if (connect(sockfd, (struct sockaddr *)&clientadd, sizeof(clientadd)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(1);
    }
    printf("Client Connected to the server\n");

    chat(sockfd);

    // printf("Enter a message to send to the server : ");
    // scanf("%[^\n]", msg1);

    // send(sockfd, msg2, strlen(msg2), 0);

    // printf("Message sent to server!\n");

    // bzero(msg1, sizeof(msg1));
    // read(sockfd, msg1, strlen(msg1));

    // printf("Message from Server : %s\n", msg1);

    close(sockfd);

    return 0;
}