#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#define PORT 1082
#define MAX 1024
#define TIMEOUT 2

void handle_error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main()
{
    int num_packets, window;
    int base = 1, next = 1, retransmissions = 0, ack_num = 0;
    int sock;
    struct sockaddr_in serv_addr;
    char frame[MAX], buffer[MAX];

    printf("Enter the total number of packets to be transmitted: ");
    scanf("%d", &num_packets);

    printf("Enter the window size: ");
    scanf("%d", &window);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("Socket creation failed");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        handle_error("Connection failed");

    printf("Connected to the server!\n");

    srand(time(NULL));

    while (base <= num_packets)
    {
        while (next < base + window && next <= num_packets)
        {
            snprintf(frame, sizeof(frame), "Frame %d ", next);
            if (send(sock, frame, strlen(frame), 0) < 0)
            {
                handle_error("Send failed");
            }
            printf("Sent: %s\n", frame);
            next++;
        }

        // Set socket timeout
        struct timeval tv;
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        // Wait for acknowledgment
        int n = recv(sock, buffer, MAX, 0);
        if (n > 0)
        {
            buffer[n] = '\0';
            sscanf(buffer, "%d", &ack_num);
            printf("Acknowledged: %d\n", ack_num);
            if (ack_num >= base)
            {
                base = ack_num + 1; // Update base based on the last acknowledged frame
            }
        }
        else
        {
            printf("Timeout or lost packet. Retransmitting from packet %d...\n", base);
            next = base;
            retransmissions++;
        }
    }
    printf("Transmission complete. Retransmissions: %d\n", retransmissions);
    close(sock);
    return 0;
}