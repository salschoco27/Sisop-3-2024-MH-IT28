#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s -c <command> -i <info>\n", argv[0]);
        return 1;
    }

    char *command = NULL;
    char *info = NULL;

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-c") == 0) {
            command = argv[i + 1];
        } else if (strcmp(argv[i], "-i") == 0) {
            info = argv[i + 1];
        }
    }

    if (command == NULL || info == NULL) {
        printf("Invalid command or info\n");
        return 1;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return 1;
    }

    //membangun request
    char request[1024];
    sprintf(request, "%s %s\n", command, info);

    //mengirimkan request
    send(sock, request, strlen(request), 0);

    //menerima respon
    read(sock, buffer, 1024);
    printf("[Paddock]: %s\n", buffer);

    return 0;
}
