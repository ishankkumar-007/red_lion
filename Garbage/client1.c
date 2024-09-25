#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#define PORT 8086
#define BUFFER_SIZE 1024
#define TIMEOUT 2     // Timeout for ACK in seconds
#define WINDOW_SIZE 4 // Define the sender's window size
int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len;
    char buffer[BUFFER_SIZE];
    int total_packets, window_base = 1; // First packet in the window
    int window[WINDOW_SIZE];            // Track packets in the current window
    int acked[WINDOW_SIZE];             // Track acknowledgments of packets in the window
    int num_packets;                    // Total number of packets to send
    // Initialize window and acknowledgment tracker
    memset(window, -1, sizeof(window));
    memset(acked, 0, sizeof(acked));
    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Initialize server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    len = sizeof(servaddr);
    // Get total number of packets and window size from user
    printf("Enter the number of packets to send: ");
    scanf("%d", &num_packets);
    // Initialize random seed for simulating packet loss
    srand(time(0));
    // Set socket timeout for ACK
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    int sent_packets = 0; // Counter to keep track of how many packets have been sent
    int all_acked = 0;    // Flag to check if all packets are acknowledged
    while (!all_acked)
    {
        // Send all packets in the current window that are not yet sent or lost
        for (int i = 0; i < WINDOW_SIZE && (window_base + i) <= num_packets; i++)
        {
            if (!acked[i])
            {
                // Send packet if not already acknowledged
                int packet_number = window_base + i;
                sprintf(buffer, "%d", packet_number);
                sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, len);
                printf("Packet %d sent\n", packet_number);
            }
        }
        // Wait for ACKs
        for (int i = 0; i < WINDOW_SIZE && (window_base + i) <=
                                               num_packets;
             i++)
        {
            if (acked[i])
            {
                continue; // Skip if already acknowledged
            }
            memset(buffer, 0, BUFFER_SIZE);
            int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&servaddr, &len);
            if (n > 0)
            {
                // Parse ACK received from server
                int ack_number;
                sscanf(buffer, "ACK %d", &ack_number);
                printf("ACK for packet %d received\n", ack_number);
                if (ack_number >= window_base && ack_number < window_base +
                                                                  WINDOW_SIZE)
                {
                    int ack_index = ack_number - window_base;
                    acked[ack_index] = 1; // Mark packet as acknowledged
                }
            }
            else
            {
                printf("Timeout occurred, resending window starting from packet %d\n", window_base);
                break; // Timeout occurred, will retry the entire window
            }
        }
        // Slide the window if the base packet is acknowledged
        while (acked[0] && window_base <= num_packets)
        {
            printf("Sliding window, base packet %d acknowledged\n",
                   window_base);
            // Slide the window to the right
            for (int i = 1; i < WINDOW_SIZE; i++)
            {
                window[i - 1] = window[i];
                acked[i - 1] = acked[i];
            }
            window[WINDOW_SIZE - 1] = -1; // Clear the last window position
            acked[WINDOW_SIZE - 1] = 0;
            window_base++; // Move the base forward
        }
        // Check if all packets are acknowledged
        if (window_base > num_packets)
        {
            all_acked = 1;
        }
    }
    printf("All packets sent and acknowledged.\n");
    close(sockfd);
    return 0;
}