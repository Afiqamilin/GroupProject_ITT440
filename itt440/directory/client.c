#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void main() {
    const char* host = "server-python";
    int port = 15001;

    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &server_address.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Main client loop
    char user[100];
    printf("Enter user name: ");
    if (fgets(user, sizeof(user), stdin) == NULL) {
        perror("Error reading user input");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    user[strcspn(user, "\n")] = '\0'; // Remove newline character from input

    while (1) {
        char message[100];
        sprintf(message, "%s:0", user);
        if (send(client_socket, message, strlen(message), 0) == -1) {
            perror("Message send failed");
            break;
        }

        char data[1024];
        int bytes_received = recv(client_socket, data, sizeof(data) - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0)
                printf("Server closed the connection.\n");
            else
                perror("Error receiving data");
            break;
        }
        data[bytes_received] = '\0';
        printf("User: %s, Points: %s\n", user, data);

        sleep(1); // Add a small delay to avoid spamming the server
    }

    close(client_socket);
}
