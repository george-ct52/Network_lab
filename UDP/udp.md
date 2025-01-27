

### **UDP Server (server.c)**
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

        // Send response to client
        const char *response = "Message received by server";
        sendto(sock_fd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sock_fd);
    return 0;
}
```

### **UDP Client (client.c)**
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

### Explanation of UDP Client and Server Code:

#### **UDP Server (`server.c`):**
1. **Socket Creation**: 
   - The server creates a **UDP socket** with `socket(AF_INET, SOCK_DGRAM, 0)`. Note that `SOCK_DGRAM` specifies that it is a **UDP** socket (as opposed to `SOCK_STREAM` for TCP).
   
2. **Setting up Address**: 
   - The server fills in the `sockaddr_in` structure (`server_addr`) with the **IPv4 address** (using `INADDR_ANY` to bind to any available network interface) and the **port number** (using `htons()` to ensure it's in network byte order).

3. **Binding the Socket**:
   - The server binds the socket to a specific port using the `bind()` function. This tells the server to listen for **UDP datagrams** on the specified port.

4. **Receiving and Sending Data**:
   - The server uses `recvfrom()` to receive data from any client. The client’s address is also returned via the `client_addr` structure.
   - After receiving data, the server sends a response back to the client using `sendto()`. Since UDP is connectionless, the server doesn't need to maintain a connection with the client; it simply sends the response back to the client's address.

#### **UDP Client (`client.c`):**
1. **Socket Creation**:
   - Similar to the server, the client creates a **UDP socket** with `socket(AF_INET, SOCK_DGRAM, 0)`.

2. **Setting up Address**:
   - The client fills in the `sockaddr_in` structure (`server_addr`) with the **server's IP address** (`127.0.0.1` for localhost) and the **port number**.

3. **Sending Data**:
   - The client uses `sendto()` to send data (a message entered by the user) to the server. Since UDP is connectionless, there's no need for a handshake; the client just sends the message to the server's IP and port.

4. **Receiving Response**:
   - The client waits for a response using `recvfrom()`. Once it receives the response, it displays it to the user.
   - If the user types `"exit"`, the client will stop the communication and close the socket.

### Key Differences between TCP and UDP in Code:
- **UDP**: 
  - **Connectionless**: There’s no need for a connection setup or teardown. Data is sent as individual datagrams.
  - The functions used for sending and receiving data are **`sendto()`** and **`recvfrom()`** (since UDP doesn’t maintain a persistent connection).
  
- **TCP**:
  - **Connection-oriented**: The server needs to `listen()` and `accept()` connections from clients. Communication is maintained over a connection.
  - The functions used for sending and receiving data are **`send()`** and **`recv()`**.

### Example Output:

#### **Server Output (UDP Server)**:
```bash
UDP Server is listening on port 8080...
Received from client: Hello Server!
Received from client: exit
```

#### **Client Output (UDP Client)**:
```bash
Connected to server.
Enter message to send to server (type 'exit' to quit): Hello Server!
Received from server: Message received by server
Enter message to send to server (type 'exit' to quit): exit
Exiting communication.
```

### Summary:
- **UDP** does not require the connection setup and teardown that **TCP** does.
- **Server** listens on a port and **receives** and **sends** datagrams without establishing a connection.
- The **client** simply sends a datagram to the server and optionally waits for a response.
- The process is similar to TCP, but simpler and without the overhead of connection management.
