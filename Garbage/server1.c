#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#define PORT 8086
#define BUFFER_SIZE 1024
#define WINDOW_SIZE 4 // Define the receiver's window size
int main()
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[BUFFER_SIZE];
    int base = 1;            // The first packet the receiver expects (window base)
    int window[WINDOW_SIZE]; // Buffer to store packets in the current window
    int acked[WINDOW_SIZE];  // Tracks acknowledgment for packets in the window
    int total_packets = 10;  // Set the total number of packets for the demo
    int packet_number;
    // Initialize window buffer and acknowledgment tracker
    memset(window, -1, sizeof(window));
    memset(acked, 0, sizeof(acked));
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Selective Repeat ARQ Server is ready to receive...\n");
    srand(time(0)); // Seed random number generator for packet loss simulation
    while (base <= total_packets)
    { // The receiver will stop after all packets are received
        len = sizeof(cliaddr);
        memset(buffer, 0, BUFFER_SIZE);
        // Receive packet
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0)
        {
            perror("Receive failed");
            continue;
        }
        packet_number = atoi(buffer);
        // Simulate packet loss
        if (rand() % 100 < 20)
        { // 20% chance of packet loss
            printf("Packet %d lost\n", packet_number);
            continue;
        }
        // Process the received packet
        if (packet_number >= base && packet_number < base + WINDOW_SIZE)
        {
            // Within window, store it
            int index = packet_number - base;
            if (!acked[index])
            { // If not already acknowledged
                window[index] = packet_number;
                acked[index] = 1;
                printf("Packet %d received and buffered\n", packet_number);
                // Send acknowledgment
                sprintf(buffer, "ACK %d", packet_number);
                sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, len);
                printf("ACK for packet %d sent\n", packet_number);
            }
        }
        else
        {
            printf("Packet %d is out of window (base=%d)\n", packet_number, base);
            continue; // Ignore packets outside the window
        }
        // Slide window if base packet is acknowledged
        while (acked[0] && base <= total_packets)
        {
            printf("Sliding window, base=%d acknowledged\n", base);
            // Slide window to the right
            for (int i = 1; i < WINDOW_SIZE; i++)
            {
                window[i - 1] = window[i];
                acked[i - 1] = acked[i];
            }
            window[WINDOW_SIZE - 1] = -1; // Reset last position
            acked[WINDOW_SIZE - 1] = 0;
            base++; // Move base forward
        }
    }
    printf("All packets received and acknowledged.\n");
    close(sockfd);
    return 0;
}