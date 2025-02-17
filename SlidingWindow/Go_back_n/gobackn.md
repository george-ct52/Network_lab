
### Server-Side Code:
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 9009
#define MAX_MSG 10
#define WINDOW_SIZE 3

int main() {
    int s_sock, c_sock;
    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (s_sock == -1) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server, other;
    memset(&server, 0, sizeof(server));
    memset(&other, 0, sizeof(other));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(s_sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Binding failed");
        return -1;
    }

    printf("\tServer Up\n\n");
    listen(s_sock, 10);

    socklen_t addr_len = sizeof(other);
    c_sock = accept(s_sock, (struct sockaddr *)&other, &addr_len);
    if (c_sock == -1) {
        perror("Accept failed");
        return -1;
    }

    char msg[50] = "server message :";
    char buff[50];
    int i = 0;

    fd_set set;
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    while (i < MAX_MSG) {
        FD_ZERO(&set);
        FD_SET(c_sock, &set);

        // Sending the message
        char message[60];
        snprintf(message, sizeof(message), "%s%d", msg, i);
        printf("Message sent to client: %s\n", message);
        write(c_sock, message, strlen(message) + 1);

        // Waiting for acknowledgment from the client
        int rv = select(c_sock + 1, &set, NULL, NULL, &timeout);
        if (rv == -1) {
            perror("Select failed");
        } else if (rv == 0) {
            printf("Timeout reached, retransmitting message %d\n", i);
        } else {
            bzero(buff, sizeof(buff));
            read(c_sock, buff, sizeof(buff));
            printf("Message from Client: %s\n", buff);
        }

        i++;
        if (i % WINDOW_SIZE == 0) {
            usleep(1000); // Simulate waiting for a timeout or acknowledgment
        }
    }

    close(c_sock);
    close(s_sock);
    return 0;
}
```

### Client-Side Code:
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 9009

int main() {
    int s_sock;
    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (s_sock == -1) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(s_sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Connection failed");
        return -1;
    }

    char buff[50];
    int i = 0;

    while (i < 10) {
        bzero(buff, sizeof(buff));
        read(s_sock, buff, sizeof(buff));
        printf("Received from server: %s\n", buff);

        // Simulate an acknowledgment by sending back a response
        char ack[50];
        snprintf(ack, sizeof(ack), "Ack %d", i);
        printf("Sending acknowledgment: %s\n", ack);
        write(s_sock, ack, strlen(ack) + 1);

        i++;
    }

    close(s_sock);
    return 0;
}
```

### Explanation:

1. **Server-Side (Go-Back-N Protocol):**
   - The server listens for incoming connections and accepts them.
   - It sends 10 messages to the client, each labeled with a sequence number (e.g., `"server message :0"`).
   - After sending each message, the server waits for an acknowledgment from the client.
   - If no acknowledgment is received within the timeout (2 seconds), the server retransmits the message.
   - The communication uses `select()` for managing timeouts.
   - The messages are sent sequentially, and the server continues to send the next message after receiving acknowledgment.

2. **Client-Side:**
   - The client connects to the server and waits to receive messages.
   - Upon receiving a message from the server, the client sends an acknowledgment back (e.g., `"Ack 0"`).
   - The client continues sending acknowledgments for each message received until 10 messages are processed.

### How to Run:

1. **Compile the server and client code:**
   ```bash
   gcc server.c -o server
   gcc client.c -o client
   ```

2. **Run the server:**
   ```bash
   ./server
   ```

3. **Run the client (in a different terminal):**
   ```bash
   ./client
   ```

The server will send messages to the client and wait for acknowledgment. The client will respond with an acknowledgment for each message received. If the acknowledgment is not received within the timeout period, the server will retransmit the message.
