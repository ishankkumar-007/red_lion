// Selective Repeat ARQ Receiver (Server)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[1024];
    int expectedFrame = 0, receivedFrame, ackLoss, totalFrames = 10;
    int received[totalFrames]; // Array to track which frames have been received

    // Initialize the received frames array
    memset(received, 0, sizeof(received));

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error("Socket creation failed");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the port
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        error("Bind failed");
    }

    len = sizeof(cliaddr); // Length of client address

    while (1) {
        // Receive frame
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            error("Receiver: Error in receiving frame");
        }

        buffer[n] = '\0';

        // Check for end of transmission
        if (strcmp(buffer, "END") == 0) {
            printf("Receiver: End of transmission. Exiting.\n");
            break;
        }

        // Parse the received frame number
        sscanf(buffer, "Frame %d", &receivedFrame);
        printf("Receiver: Received %s\n", buffer);

        if (receivedFrame < totalFrames && received[receivedFrame] == 0) {
            // Mark frame as received
            received[receivedFrame] = 1;

            // Simulate ACK loss
            printf("Simulate ACK loss for frame %d? (0 for No, 1 for Yes): ", receivedFrame);
            scanf("%d", &ackLoss);

            if (ackLoss == 0) {
                // Send ACK for received frame
                sprintf(buffer, "ACK %d", receivedFrame);
                sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&cliaddr, len);
                printf("Receiver: Sent acknowledgment for frame %d\n", receivedFrame);
            } else {
                // Simulate ACK loss
                printf("Receiver: Simulating ACK loss for frame %d\n", receivedFrame);
            }
        } else {
            // Send ACK for already received frame
            sprintf(buffer, "ACK %d", receivedFrame);
            sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&cliaddr, len);
            printf("Receiver: Sent duplicate ACK for frame %d\n", receivedFrame);
        }
    }

    close(sockfd);
    return 0;
}