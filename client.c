#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char message[1024];

    // Buat socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //mengatur server address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect ke server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Loop untuk mengirim pesan ke server
    while (1) {
        printf("Enter command (type 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);

        // Hentikan loop jika pengguna mengetikkan "exit"
        if (strncmp(message, "exit", 4) == 0) {
            break;
        }

        // Kirim pesan ke server
        send(sock, message, strlen(message), 0);

        // Terima balasan dari server
        valread = read(sock, buffer, 1024);
        printf("Server response: %s\n", buffer);
        memset(buffer, 0, sizeof(buffer)); // Bersihkan buffer
    }

    printf("Closing client...\n");
    close(sock);
    return 0;
}