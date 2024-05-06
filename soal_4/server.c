//server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define PORT 8080

//untuk menambahkan log perubahan ke change.log
void addChangeLog(const char *type, const char *message) {
    FILE *fp;
    //agar bisa menampilkan current time
    time_t now;
    struct tm *local_time;

    time(&now);
    local_time = localtime(&now);

    fp = fopen("change.log", "a");
    if (fp == NULL) {
        perror("Error opening change.log");
        return;
    }

    fprintf(fp, "[%02d/%02d/%04d %02d:%02d:%02d] [%s] %s\n",
            local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year + 1900,
            local_time->tm_hour, local_time->tm_min, local_time->tm_sec,
            type, message);

    fclose(fp);
}

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // membuat socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding socket ke alamat dan port tertentu
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // listen ke incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // menerima koneksi dari client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Handle incoming messages
    while (1) {
        valread = read(new_socket, buffer, 1024);
        if (valread <= 0) {
            break; // Koneksi terputus
        }

        // Proses perintah dari client
        buffer[valread] = '\0'; // Tambahkan null terminator

        // Lakukan aksi berdasarkan perintah dari client
        // Contoh: menampilkan perintah dari client di server console
        printf("Received command from client: %s\n", buffer);

        // Kirim balasan ke client (contoh: echo kembali pesan yang diterima)
        send(new_socket, buffer, strlen(buffer), 0);
    }

    printf("Client disconnected. Closing server...\n");
    close(server_fd);
    return 0;
}
