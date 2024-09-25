// Selective Repeat ARQ Sender (Client)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<sys/time.h>

#define PORT 8080
#define TIMEOUT 8 // Timeout for ACK in seconds

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd, windowSize, totalFrames, frame = 0, ack;
    struct sockaddr_in servaddr;
    socklen_t addr_len = sizeof(servaddr);
    char buffer[1024];
    struct timeval timeout;

    // Variables to track packet statistics
    int transmittedPackets = 0;
    int lostPackets = 0;
    int retransmittedPackets = 0;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error("Socket creation failed");
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Set socket timeout option for receiving ACKs
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        error("Error setting socket options");
    }

    // Ask user for the total number of frames and window size
    printf("Enter the total number of frames to send: ");
    scanf("%d", &totalFrames);
    printf("Enter the window size: ");
    scanf("%d", &windowSize);

    int base = 0; // Tracks the base of the sliding window
    int sent[totalFrames]; // Array to track if frames are sent or acknowledged

    // Initialize all frames as unsent
    memset(sent, 0, sizeof(sent));

    while (base < totalFrames) {
        // Send frames in the window
        for (int i = 0; i < windowSize && base + i < totalFrames; i++) {
            if (sent[base + i] == 0) { // Only send if not yet acknowledged
                sprintf(buffer, "Frame %d", base + i);
                sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, addr_len);
                printf("Sender: Sent %s\n", buffer);
                transmittedPackets++; // Increment transmitted packets count
                sent[base + i] = 1; // Mark frame as sent
            }
        }

        // Wait for ACKs or timeout
        for (int i = 0; i < windowSize && base < totalFrames; i++) {
            int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&servaddr, &addr_len);
            if (n < 0) {
                // Timeout occurred
                printf("Sender: Timeout occurred. Retransmitting lost frames.\n");
                lostPackets += windowSize - i; // Count the number of lost packets
                break; // Retransmit the entire window from the base frame
            } else {
                buffer[n] = '\0';
                sscanf(buffer, "ACK %d", &ack);
                printf("Sender: Acknowledgment for frame %d received.\n", ack);

                // Mark the frame as acknowledged
                if (ack >= base && ack < base + windowSize) {
                    sent[ack] = 2; // 2 means acknowledged
                }

                // Slide the window if all frames in the base are acknowledged
                while (base < totalFrames && sent[base] == 2) {
                    base++;
                }
            }
        }

        // Retransmit any unacknowledged frames in the window
        for (int i = 0; i < windowSize && base + i < totalFrames; i++) {
            if (sent[base + i] == 1) { // If sent but not acknowledged
                sprintf(buffer, "Frame %d", base + i);
                sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, addr_len);
                printf("Sender: Retransmitted %s\n", buffer);
                retransmittedPackets++; // Increment retransmitted packets count
            }
        }
    }

    // Send END signal to indicate completion
    strcpy(buffer, "END");
    sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, addr_len);
    printf("Sender: Sent END signal. Transmission complete.\n");

    // Print packet statistics
    printf("\nTransmission Summary:\n");
    printf("Total packets transmitted: %d\n", transmittedPackets);
    printf("Total packets lost: %d\n", lostPackets);
    printf("Total packets retransmitted: %d\n", retransmittedPackets);

    close(sockfd);
    return 0;
}