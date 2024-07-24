#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = read(client_socket, buffer, sizeof(buffer) - 1);
    buffer[bytes_received] = '\0';>

    if (strncmp(buffer, "UPLOAD", 6) == 0) {
        char filename[256];
        sscanf(buffer + 7, "%s", filename);

        FILE *file = fopen(filename, "wb");
        if (file == NULL) {
            perror("Failed to open file");
            close(client_socket);
            return;
        }

        while ((bytes_received = read(client_socket, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytes_received, file);
        }

        fclose(file);
        printf("File %s received\n", filename);
    } else if (strncmp(buffer, "DOWNLOAD", 8) == 0) {
        char filename[256];
        sscanf(buffer + 9, "%s", filename);

        FILE *file = fopen(filename, "rb");
        if (file == NULL) {
            perror("File not found");
            close(client_socket);
            return;
        }

        while ((bytes_received = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            write(client_socket, buffer, bytes_received);
        }

        fclose(file);
        printf("File %s sent\n", filename);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}

