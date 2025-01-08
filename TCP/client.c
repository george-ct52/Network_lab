#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set server address information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    // Connect to the server
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server.\n");

    // Two-way communication loop
    while (1) {
        // Send message to server
        printf("Enter message to send to server (type 'exit' to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Remove the newline character that fgets() captures
        buffer[strcspn(buffer, "\n")] = '\0';

        // If the user types "exit", break out of the loop and close the connection
        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting communication.\n");
            break;
        }

        // Send the message to the server
        send(sock_fd, buffer, strlen(buffer), 0);

        // Receive message from server
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recv(sock_fd, buffer, BUFFER_SIZE, 0);
        if (recv_len <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        printf("Received from server: %s\n", buffer);
    }

    close(sock_fd);
    return 0;
}

