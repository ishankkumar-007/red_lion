#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 2900

// Function for chat between server and client
void chat(int newsocketfd)
{
    char msg[1024];

    int n;

    while (1)
    {
        printf("reading message form client\n");
        read(newsocketfd, msg, sizeof(msg));
        printf("Message from client : %s\n", msg);

        bzero(msg, 1024);

        printf("\nEnter a message to send from server:");
        scanf(" %[^\n]", msg);

        write(newsocketfd, msg, sizeof(msg));
        printf("Message sent to client\n");

        if (strncmp("exit", msg, 4) == 0)
        {
            printf("Server Exiting...\n\n");
            break;
        }
    }
}

int main()
{
    int socketfd;
    struct sockaddr_in serveraddr;
    char buffer[1024], message[1024];

    // Create socket

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    // AF_INET indicates socket will use IPv4 Address
    // Sock_stream ensures data realiability and integrity

    if (socketfd == -1)
    {
        printf("Socket Creation Failed!");
        exit(1);
    }
    printf("Socket Creation sucessful\n");

    bzero(&serveraddr, sizeof(serveraddr)); // Assigns zero in the structure which stores IP Address and port number

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);

    // Bind socket to the given IP Address

    if ((bind(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) != 0))
    {
        printf("Socket bind failed\n");
        exit(1);
    }
    printf("Socket binded Sucessfully\n");

    if (listen(socketfd, 3) != 0)
    {
        printf("Socket is not listening\n");
        exit(1);
    }
    printf("Socket listening on port %d \n", PORT);

    // Accept data packet from client

    struct sockaddr_in clientaddr;

    int len = sizeof(clientaddr);

    int clientfd = accept(socketfd, (struct sockaddr *)&clientaddr, &len);

    if (clientfd < 0)
    {
        printf("server accept failed!\n");
        exit(1);
    }

    printf("Server accepted the client\n");

    // Receive data from the client

    // read(clientfd,buffer,1024);
    // printf("Message from client : %s\n",buffer);

    // // Send data to client

    // printf("Enter a message to send to the client : ");
    // scanf("%[^\n]",message);

    // send(clientfd,message,strlen(message),0);
    // printf("Message sent to client\n ");

    chat(clientfd);

    close(clientfd);
    close(socketfd);

    return 0;
}