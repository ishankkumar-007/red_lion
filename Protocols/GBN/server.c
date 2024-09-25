#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 1082
#define MAX 1024
#define LOSS_RATE 10 // Percentage of packet loss

int simulate_packet_loss()
{
    return rand() % 100 < LOSS_RATE; // Simulate packet loss based on LOSS_RATE
}


void handle_error(const char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[MAX] = {0};
    int expected = 1; // Sequence number expected by the receiver
    int valread;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        handle_error("Socket creation failed");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        handle_error("Bind failed");
    }

    printf("Socket bind sucessful\n");

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        handle_error("Listen failed");
    }

    printf("Server listening on port %d\n",PORT);

    // Accept a connection
    int addrlen = sizeof(address);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0)
    {
        handle_error("Accept failed");
    }

    printf("Client connected to the server\n");

    srand(time(NULL));

    while (1)
    {
        valread = recv(new_socket, buffer, MAX-1, 0);

        if (valread > 0)
        {
            buffer[valread] = '\0';

            if (strcmp(buffer, "DONE") == 0)
            {
                printf("Transmission complete.\n");
                break;
            }

            if (!simulate_packet_loss())
            {
                int received;
                sscanf(buffer, "Frame %d ", &received);

                if (received == expected)
                {
                    printf("Received : %s\n", buffer);

                    // Send ACK

                    char ack[20];
                    snprintf(ack, sizeof(ack), "%d\n", received);
                    if (send(new_socket, ack, strlen(ack), 0) < 0)
                    {
                        handle_error("Acknowledgment failed");
                    }
                    printf("Acknowledged: Frame %d\n", received);
                    expected++;
                }

                else
                {
                    printf("Out of order Frame %d received expected %d\n", received, expected);
                }
            }

            else
            {
                printf("Frame Loss\n");
            }
        }

        else
        {
            printf("Connection closed or error\n");
            break;
        }
    }

    close(new_socket);
    close(server_fd);

    return 0;
}