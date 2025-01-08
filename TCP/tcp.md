To implement two-way communication using TCP in C, you need to create both a TCP server and a TCP client. The server will listen for incoming connections, and the client will connect to the server. Once the connection is established, both sides can send and receive data.

Below are the steps and examples for creating a simple two-way communication in C using TCP sockets.

### Steps:

1. **Create a TCP server:**
   - The server listens on a specific port for incoming connections.
   - Once a connection is established, it can send and receive data.

2. **Create a TCP client:**
   - The client connects to the server's IP address and port.
   - After the connection is established, the client can send and receive data.

3. **Use `send()` and `recv()` functions for data transfer:**
   - `send()` sends data to the server or client.
   - `recv()` receives data from the server or client.

### Example Code:

#### 1. TCP Server in C:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set server address information
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept a client connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) == -1) {
        perror("Accept failed");
        close(server_fd);
        exit(1);
    }

    printf("Client connected.\n");

    // Two-way communication loop
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from client
        int recv_len = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (recv_len <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        printf("Received from client: %s\n", buffer);

        // Send response to client
        printf("Enter message to send to client: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(client_fd, buffer, strlen(buffer), 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
```

#### 2. TCP Client in C:

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
        printf("Enter message to send to server: ");
        fgets(buffer, BUFFER_SIZE, stdin);
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
```

### How It Works:

1. **Server:**
   - The server creates a socket and binds it to a specific port (`8080` in this case).
   - It listens for incoming connections and accepts one connection.
   - After accepting the connection, it enters a loop where it:
     - Receives a message from the client using `recv()`.
     - Sends a response back to the client using `send()`.

2. **Client:**
   - The client creates a socket and connects to the server at the specified IP address and port.
   - Once connected, it enters a loop where it:
     - Sends a message to the server using `send()`.
     - Receives the server's response using `recv()`.

### Notes:
- The server and client use a blocking communication model. If you want non-blocking communication or timeouts, you can use `select()`, `poll()`, or set the socket to non-blocking mode using `fcntl()`.
- The buffer size is set to `1024` bytes in this example, but you can adjust it depending on your needs.
- Error handling is important in real-world applications. This example includes basic error handling, but you should handle different error cases more robustly depending on your application's requirements.

### Compile and Run:

1. **Compile the server:**

```bash
gcc -o server server.c
```

2. **Compile the client:**

```bash
gcc -o client client.c
```

3. **Run the server:**

```bash
./server
```

4. **Run the client (in another terminal or machine):**

```bash
./client
```

Now you have a simple two-way communication between a TCP server and client using C. You can send messages back and forth as long as the connection is active.
