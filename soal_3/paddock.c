#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "actt.c"
#include <time.h>
#define PORT 8080

void log_message(char* source, char* command, char* additional_info) {
    FILE *fp;
    fp = fopen("race.log", "a+");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    //untuk localtime
    time_t current_time;
    time(&current_time);
    struct tm* local_time = localtime(&current_time);

    //date and time
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", local_time);

    // Menulis message log ke file
    fprintf(fp, "[%s] [%s]: [%s] [%s]\n", source, timestamp, command, additional_info);
    fclose(fp);
}

void handle_client_request(int client_socket) {
    char buffer[1024] = {0};
    char response[1024] = {0};


    //breakdown request/command
    char *command = strtok(buffer, " ");
    char *info = strtok(NULL, "\n");

    if (strcmp(command, "Gap") == 0) {
        float distance = atof(info);
        sprintf(response, "%s", handle_gap(distance));
    } else if (strcmp(command, "Fuel") == 0) {
        int fuel_percent = atoi(info);
        sprintf(response, "%s", handle_fuel(fuel_percent));
    } else if (strcmp(command, "Tire") == 0) {
        int tire_wear = atoi(info);
        sprintf(response, "%s", handle_tire(tire_wear));
    } else if (strcmp(command, "TireChange") == 0) {
        sprintf(response, "%s", handle_tire_change(info));
    } else {
        sprintf(response, "Invalid command");
    }

    send(client_socket, response, strlen(response), 0);
    log_message("Paddock", command, response);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        
        handle_client_request(new_socket);
        close(new_socket);
    }
    
    return 0;
}
