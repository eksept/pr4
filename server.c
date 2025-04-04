#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_CLIENTS 5
#define LOG_FILE "server.log"
#define BUFFER_SIZE 256

void log_message(const char *client_ip, const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        fprintf(log_file, "%s: %s\n", client_ip, message);
        fclose(log_file);
    }
    printf("%s: %s\n", client_ip, message);
}

void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    int number_to_guess = rand() % 100 + 1;
    int attempts = 0;
    char client_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    log_message(client_ip, "Connected");

    while (1) {
        ssize_t received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) {
            log_message(client_ip, "Disconnected");
            break;
        }
        buffer[received] = '\0';
        int guess = atoi(buffer);
        attempts++;

        if (guess == number_to_guess) {
            snprintf(buffer, sizeof(buffer), "Correct! Attempts: %d", attempts);
            send(client_sock, buffer, strlen(buffer), 0);
            log_message(client_ip, "Guessed correctly!");
            break;
        } else if (guess < number_to_guess) {
            send(client_sock, "Higher", 6, 0);
        } else {
            send(client_sock, "Lower", 5, 0);
        }
        log_message(client_ip, buffer);
    }
    close(client_sock);
}

int main() {
    srand(time(NULL));

    int server_sock, client_sock, port;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    printf("Enter port number: ");
    if (scanf("%d", &port) != 1) {
        perror("Invalid port input");
        return 1;
    }

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_sock);
        return 1;
    }
    printf("Server listening on port %d\n", port);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) {
            close(server_sock);
            handle_client(client_sock, client_addr);
            exit(0);
        }
        close(client_sock);
    }

    close(server_sock);
    return 0;
}
