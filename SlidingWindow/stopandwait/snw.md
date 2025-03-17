Here's an implementation of the **Stop-and-Wait Sliding Window Protocol** in C using sockets. This consists of a **sender** and a **receiver** program.

---

### **Implementation Overview**
- **Sender**:
  - Sends a frame.
  - Waits for an acknowledgment (ACK).
  - If no ACK is received within a timeout, retransmits the frame.
  
- **Receiver**:
  - Receives a frame.
  - Sends an ACK back to the sender.

---

### **Stop-and-Wait Protocol Implementation in C**
#### **1. Sender (sender.c)**
```c
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
```

---

#### **2. Receiver (receiver.c)**
```c
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
```

---

### **How to Run**
1. **Compile both programs:**
   ```sh
   gcc sender.c -o sender
   gcc receiver.c -o receiver
   ```
2. **Run the receiver first:**
   ```sh
   ./receiver
   ```
3. **Run the sender:**
   ```sh
   ./sender
   ```

---

### **Explanation**
- The **sender** sends a frame and waits for an acknowledgment.
- The **receiver** receives the frame and sends back an ACK.
- If the sender does not receive an ACK within a timeout, it **resends** the frame.
- The receiver **randomly drops ACKs** sometimes to simulate network issues.

This implementation demonstrates the **Stop-and-Wait** mechanism effectively. 🚀
