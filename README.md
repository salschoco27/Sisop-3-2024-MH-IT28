# SOAL SHIFT SISTEM OPERASI MODUL 3
Kelompok IT 28:
- Salsabila Rahmah (5027271005)
- Fadlillah Cantika Sari H (5027271042)
- I Dewa Made Satya Raditya (5027271051)
## Soal 1
Buat direktori baru untuk mengerjakan soal ini dengan perintah:
```
mkdir soal-shift-modul-3
cd soal-shift-modul-3
```
Buat file-file C yang diperlukan dengan perintah:
```
touch auth.c rate.c db.c
```
Buat direktori microservices dan new-data dengan perintah:
```
mkdir microservices
mkdir database
mkdir new-data
```
Di dalam direktori new-data, buat beberapa file CSV sesuai dengan soal, belobog_trashcan.csv, osaka_parkinglot.csv, dan ikn.csv.

Buka file auth.c 
```
nano auth.c
```
Tulis kodenya untuk mengecek setiap file di direktori new-data. Jika file bukan CSV dengan akhiran _trashcan.csv atau _parkinglot.csv, maka hapus file tersebut.

Untuk file yang valid, lakukan proses selanjutnya (bagian b dan c pada soal).

Buka file rate.c 
```
nano rate.c
```
Buka file db.c 
```
nano db.c
```
Tulis kode untuk memindahkan file yang valid dari new-data ke microservices/database menggunakan shared memory.

Buat file log db.log di dalam direktori microservices/database.

Log setiap file yang dipindahkan ke db.log 

Kompilasi file-file C dengan perintah:
```
gcc -o auth auth.c
gcc -o rate rate.c
gcc -o db db.c
```
Jalankan masing masing program dengan perintah:
```
./auth
./rate
./db
```

## Soal 2
### dudududu.c
## Soal 3
### actions.c
```c
#include <stdio.h>

//function untuk ketentuan DISTANCE
char* handle_gap(float distance) {
    if (distance < 3.5)
        return "Gogogo";
    else if (distance >= 3.5 && distance <= 10)
        return "Push";
    else
        return "Stay out of trouble";
}


//function untuk ketentuan FUEL
char* handle_fuel(int fuel_percent) {
    if (fuel_percent > 80)
        return "Push Push Push";
    else if (fuel_percent >= 50 && fuel_percent <= 80)
        return "You can go";
    else
        return "Conserve Fuel";
}

//function untuk ketentuan TIRE
char* handle_tire(int tire_wear) {
    if (tire_wear > 80)
        return "Go Push Go Push";
    else if (tire_wear >= 50 && tire_wear <= 80)
        return "Good Tire Wear";
    else if (tire_wear > 30 && tire_wear < 50)
        return "Conserve Your Tire";
    else
        return "Box Box Box";
}

//function untuk TIRE CHANGE
char* handle_tire_change(char* tire_type) {
    if (strcmp(tire_type, "Soft") == 0)
        return "Mediums Ready";
    else if (strcmp(tire_type, "Medium") == 0)
        return "Box for Softs";
    else
        return "Invalid tire type";
}
```
### driver.c
```c
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
```
### paddock.c
```c
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
```


Setelah itu menjalankan command

```
gcc driver.c -o driver
gcc paddock.c -o paddock
```

Dokumentasi pengerjaan
<img width="1080" alt="image" src="https://github.com/salschoco27/Sisop-3-2024-MH-IT28/assets/151063684/93292b2b-c476-47ec-9d7b-3fe32c967bb3">



## Soal 4
download file csv terlebih dahulu

```export fileid=10p_kzuOgaFY3WT6FVPJIXFbkej2s9f50 && export filename=myanimelist.csv && wget -O $filename 'https://docs.google.com/uc?export=download&id='$fileid```
