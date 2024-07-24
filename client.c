#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void upload_file(int socket, const char *filename) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "UPLOAD %s", filename);
    write(socket, buffer, strlen(buffer));

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(socket, buffer, bytes_read);
    }

    fclose(file);
    printf("File %s uploaded\n", filename);
}

void download_file(int socket, const char *filename) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "DOWNLOAD %s", filename);
    write(socket, buffer, strlen(buffer));

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    size_t bytes_received;
    while ((bytes_received = read(socket, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    printf("File %s downloaded\n", filename);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <upload/download> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    const char *operation = argv[2];
    const char *filename = argv[3];

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (strcmp(operation, "upload") == 0) {
        upload_file(socket_fd, filename);
    } else if (strcmp(operation, "download") == 0) {
        download_file(socket_fd, filename);
    } else {
        fprintf(stderr, "Invalid operation: %s\n", operation);
    }

    close(socket_fd);
    return 0;
}

