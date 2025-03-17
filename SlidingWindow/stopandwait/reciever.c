#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in receiver_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    int frame, ack;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure receiver address
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(PORT);
    receiver_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(sockfd, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Receiver: Waiting for frames...\n");

    while (1) {
        // Receive frame
        recvfrom(sockfd, &frame, sizeof(frame), 0, (struct sockaddr *)&sender_addr, &addr_len);
        printf("Receiver: Received frame %d\n", frame);

        // Simulate ACK loss (Randomly drop ACKs for testing)
        if (rand() % 4 == 0) {
            printf("Receiver: ACK for frame %d lost!\n", frame);
            continue;
        }

        // Send ACK
        ack = frame;
        sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&sender_addr, addr_len);
        printf("Receiver: Sent ACK %d\n", ack);
    }

    close(sockfd);
    return 0;
}

