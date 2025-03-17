#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define TIMEOUT 2  // Timeout in seconds
#define MAX_TRIES 5

void send_frame(int sockfd, struct sockaddr_in *receiver_addr, socklen_t addr_len, int frame) {
    int attempts = 0;
    int ack;
    struct timeval tv;
    fd_set readfds;

    while (attempts < MAX_TRIES) {
        // Send frame
        printf("Sender: Sending frame %d\n", frame);
        sendto(sockfd, &frame, sizeof(frame), 0, (struct sockaddr *)receiver_addr, addr_len);

        // Set timeout
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        // Wait for ACK
        if (select(sockfd + 1, &readfds, NULL, NULL, &tv) > 0) {
            recvfrom(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)receiver_addr, &addr_len);
            if (ack == frame) {
                printf("Sender: Received ACK for frame %d\n", ack);
                return;
            }
        }

        printf("Sender: Timeout, resending frame %d\n", frame);
        attempts++;
    }
    printf("Sender: Max retries reached, giving up on frame %d\n", frame);
}

int main() {
    int sockfd;
    struct sockaddr_in receiver_addr;
    socklen_t addr_len = sizeof(receiver_addr);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define receiver address
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(PORT);
    receiver_addr.sin_addr.s_addr = INADDR_ANY;

    // Sending frames
    for (int frame = 1; frame <= 5; frame++) {
        send_frame(sockfd, &receiver_addr, addr_len, frame);
        sleep(1);
    }

    close(sockfd);
    return 0;
}

