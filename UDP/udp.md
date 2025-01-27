### **Two-Way UDP Communication**

#### **UDP Server (`server.c`)**:
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // Create socket
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set server address information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);  // Convert port to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept from any address

    // Bind the socket to the address and port
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(sock_fd);
        exit(1);
    }

    printf("UDP Server is listening on port %d...\n", PORT);

    while (1) {
        // Receive message from client
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len == -1) {
            perror("Receive failed");
            continue;
        }

        printf("Received from client: %s\n", buffer);

        // Send response to client (two-way communication)
        printf("Enter message to send to client: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character
        
        // Send response back to client
        sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sock_fd);
    return 0;
}
```

#### **UDP Client (`client.c`)**:
```c
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
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set server address information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);  // Convert port to network byte order
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    printf("Connected to server.\n");

    while (1) {
        // Send message to server
        printf("Enter message to send to server (type 'exit' to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Remove the newline character captured by fgets
        buffer[strcspn(buffer, "\n")] = '\0';

        // If the user types "exit", break out of the loop and close the socket
        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting communication.\n");
            break;
        }

        // Send message to server
        sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        // Receive response from the server
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (recv_len <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Received from server: %s\n", buffer);
    }

    close(sock_fd);
    return 0;
}
```

---

### **Key Changes to Achieve Two-Way Communication**:

1. **Server Side**:
   - The server continuously **receives** messages from the client using `recvfrom()`.
   - After receiving a message, the server sends a response back to the client by using `sendto()`. 
   - The server asks for user input (using `fgets()`) to send a message back to the client, allowing **two-way communication**.

2. **Client Side**:
   - The client **sends** messages to the server using `sendto()`.
   - After sending the message, the client waits for the server’s response using `recvfrom()`.
   - The client can send multiple messages, and the server will respond each time, enabling continuous interaction.

---

### **How the Two-Way Communication Works**:

1. **Server**:
   - The server receives a message from the client, prints it, and then prompts the user to type a response. The server then sends this response back to the client.
   
2. **Client**:
   - The client sends a message to the server and waits for a response. It prints the server's response, then prompts the user to send another message.
   
3. The process continues until the client sends the message `"exit"`, which breaks out of the loop.

---

### **Example Output**:

#### **Server Output** (UDP Server):
```bash
UDP Server is listening on port 8080...
Received from client: Hello Server!
Enter message to send to client: Hello Client!
Received from client: How are you?
Enter message to send to client: I'm doing great!
```

#### **Client Output** (UDP Client):
```bash
Connected to server.
Enter message to send to server (type 'exit' to quit): Hello Server!
Received from server: Hello Client!
Enter message to send to server (type 'exit' to quit): How are you?
Received from server: I'm doing great!
Enter message to send to server (type 'exit' to quit): exit
Exiting communication.
```

---

### **Summary**:
- **UDP** allows both the server and the client to **send and receive messages independently**.
- Both the **client** and the **server** in this example send and receive messages in a **two-way communication loop**.
- The **server** doesn't just receive messages from the client; it also sends responses back after receiving the client’s message.
- Similarly, the **client** sends messages to the server and waits for the server's response.

By using `sendto()` and `recvfrom()` on both sides (client and server), we achieve the desired two-way communication in a **UDP** context!
