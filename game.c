#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

void interactive_mode(int sock) {
    char buffer[BUFFER_SIZE];
    while (1) {
        printf("Enter a number: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            perror("Error reading input");
            break;
        }
        send(sock, buffer, strlen(buffer), 0);

        int received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';
            printf("Server: %s\n", buffer);
            if (strstr(buffer, "Correct!")) break;
        } else {
            perror("Error receiving data");
            break;
        }
    }
}

void automatic_mode(int sock) {
    char buffer[BUFFER_SIZE];
    int low = 1, high = 100, guess;

    while (1) {
        guess = (low + high) / 2;
        snprintf(buffer, sizeof(buffer), "%d", guess);
        send(sock, buffer, strlen(buffer), 0);

        int received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';
            printf("Server: %s\n", buffer);
            if (strstr(buffer, "Correct!")) break;
            if (strstr(buffer, "Higher")) low = guess + 1;
            else high = guess - 1;
        } else {
            perror("Error receiving data");
            break;
        }
    }
}

int main() {
    char server_ip[16];
    int port;

    printf("Enter server IP: ");
    if (scanf("%15s", server_ip) != 1) {
        perror("Invalid IP input");
        return 1;
    }

    printf("Enter server port: ");
    if (scanf("%d", &port) != 1) {
        perror("Invalid port input");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("Connected to server\n");

    printf("Select mode: 1 - Interactive, 2 - Automatic\n");
    int mode;
    if (scanf("%d", &mode) != 1) {
        perror("Invalid mode input");
        close(sock);
        return 1;
    }

    if (mode == 1) {
        interactive_mode(sock);
    } else if (mode == 2) {
        automatic_mode(sock);
    } else {
        fprintf(stderr, "Invalid mode selected\n");
    }

    close(sock);
    return 0;
}
