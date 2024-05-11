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

1. import library yang diperlukan
```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN 100
```
2. Definisikan fungsi yg akan dipakai

```
void operasi(char *op, int num1, int num2, int pipe_fd[]); //detailnya dibawah//
void ubah_kata(int pipe_fd[], char *op); //detailnya dibawah//
void log_history(char *op, char *message); //detailnya dibawah//
```


3. `int main()`, parent & child process
   * `int main()` membuat dua pipe `pipe1` dan `pipe2`  untuk hubungan             antara  parent process dan child process, Meminta input dua bilangan         dari pengguna, Melakukan `fork()` untuk membuat child process.
   * parent process:
     Menutup ujung pipa yang tidak digunakan `(pipe1 baca, pipe2 tulis)`,         Jika ada argumen baris perintah `(operasi)` maka akan Memanggil fungsi 
     `operasi()` dengan argumen operasi, dua bilangan input, dan pipe2.
   * child process: 
     Menutup ujung pipa yang tidak digunakan `(pipe1 write, pipe2 read)`,         Memanggil fungsi `ubah_kata()` dengan argumen pipe1 dan operasi dari         argumen baris perintah `(argv[1])`, lalu Menutup ujung pipa yang masih 
     terbuka `(pipe1 baca, pipe2 tulis)`.

```
int main(int argc, char *argv[]) {
    int pipe1[2], pipe2[2];
    pid_t pid;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    char num1[MAX_LEN], num2[MAX_LEN];
    printf("Masukkan dua angka: ");
    scanf("%s %s", num1, num2);

    pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child 
        close(pipe1[1]);  // Tutup pipe1 untuk menulis
        close(pipe2[0]);  // Tutup pipe2 untuk membaca

        ubah_kata(pipe1, argv[1]);

        close(pipe1[0]);
        close(pipe2[1]);
        exit(EXIT_SUCCESS);
    } else {
        // Parent 
        close(pipe1[0]);  // Tutup pipe1 untuk membaca
        close(pipe2[1]);  // Tutup pipe2 untuk menulis

        if (argc > 1) {
            operasi(argv[1], atoi(num1), atoi(num2), pipe2);
        } else {
            printf("Penggunaan: %s <operasi>\n", argv[0]);
            printf("Operasi yang tersedia: -kali, -tambah, -kurang, -bagi\n");
            exit(EXIT_FAILURE);
        }

        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);
    }

    return 0;
}
```
4. Fungsi `operasi()`: 
   bertugas melakukan operasi yang diminta soal seperti         
   perkalian, penjumlahan, pengurangan, atau pembagian terhadap dua bilangan 
   bulat yang diberikan.
   
```
void operasi(char *op, int num1, int num2, int pipe_fd[]) {
    int hasil;
    char message[MAX_LEN];

    if (strcmp(op, "-kali") == 0) {
        hasil = num1 * num2;
        sprintf(message, "%d", hasil);
        write(pipe_fd[1], message, strlen(message) + 1);
    } else if (strcmp(op, "-tambah") == 0) {
        hasil = num1 + num2;
        sprintf(message, "%d", hasil);
        write(pipe_fd[1], message, strlen(message) + 1);
    } else if (strcmp(op, "-kurang") == 0) {
        hasil = num1 - num2;
        if (hasil < 0) {
            strcpy(message, "ERROR");
        } else {
            sprintf(message, "%d", hasil);
        }
        write(pipe_fd[1], message, strlen(message) + 1);
    } else if (strcmp(op, "-bagi") == 0) {
        if (num2 == 0) {
            strcpy(message, "ERROR");
        } else {
            hasil = num1 / num2;
            sprintf(message, "%d", hasil);
        }
        write(pipe_fd[1], message, strlen(message) + 1);
    } else {
        printf("Operasi tidak valid\n");
        exit(EXIT_FAILURE);
    }
}
```
5. Fungsi `ubah_kata()`:
   mengubah hasil operasi matematika menjadi kalimat deskriptif, fungsi ini     akan memanggil fungsi `log_history()` untuk mencatat operasi yang 
   dilakukan beserta kalimat deskriptifnya ke dalam file log.
```
void ubah_kata(int pipe_fd[], char *op) {
    char hasil[MAX_LEN];
    char message[MAX_LEN];
    read(pipe_fd[0], hasil, MAX_LEN);

    if (strcmp(hasil, "ERROR") == 0) {
        sprintf(message, "ERROR pada %s.", op + 1);
    } else {
        int num = atoi(hasil);
        char kata[MAX_LEN];
        sprintf(kata, "%d", num);

        if (strcmp(op, "-kali") == 0) {
            sprintf(message, "hasil perkalian %s adalah %s.", hasil, kata);
        } else if (strcmp(op, "-tambah") == 0) {
            sprintf(message, "hasil penjumlahan %s adalah %s.", hasil, kata);
        } else if (strcmp(op, "-kurang") == 0) {
            sprintf(message, "hasil pengurangan %s adalah %s.", hasil, kata);
        } else if (strcmp(op, "-bagi") == 0) {
            sprintf(message, "hasil pembagian %s adalah %s.", hasil, kata);
        }
    }

    write(pipe_fd[1], message, strlen(message) + 1);
    log_history(op, message);
}
```
6. Fungsi `log_history()`:
   Fungsi ini bertugas mencatat informasi hasil fungsi `operasi()` 
   yang diteruskan ke `ubah_kata` yang selanjutnya dicatat ke dalam file log.
```
void log_history(char *op, char *message) {
    FILE *log_file = fopen("histori.log", "a");
    if (log_file == NULL) {
        perror("Gagal membuka file log");
        exit(EXIT_FAILURE);
    }

    time_t rawtime;
    struct tm *timeinfo;
    char date_str[MAX_LEN];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(date_str, MAX_LEN, "%d/%m/%y %H:%M:%S", timeinfo);

    char type[MAX_LEN];
    if (strcmp(op, "-kali") == 0) {
        strcpy(type, "KALI");
    } else if (strcmp(op, "-tambah") == 0) {
        strcpy(type, "TAMBAH");
    } else if (strcmp(op, "-kurang") == 0) {
        strcpy(type, "KURANG");
    } else if (strcmp(op, "-bagi") == 0) {
        strcpy(type, "BAGI");
    }

    fprintf(log_file, "[%s] [%s] %s\n", date_str, type, message);
    fclose(log_file);
}
```



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
#include "actions.c"
#include <time.h>
#define PORT 8080
```
Membuat log file dan ketentuan message log nya
```c
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
```

```c
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
Command untuk menjalankan program:
```
./driver -c Tire -i 51%
notes: Tire dapat diganti
TireChange : untuk check pergantian ban
Fuel : untuk check respon paddock terhadap bensin
Gap : untuk check respon paddock terhadap jarak antara driver dengan mobil di depannya
```

Dokumentasi pengerjaan
<img width="1080" alt="image" src="https://github.com/salschoco27/Sisop-3-2024-MH-IT28/assets/151063684/93292b2b-c476-47ec-9d7b-3fe32c967bb3">



## Soal 4
download file csv terlebih dahulu

```export fileid=10p_kzuOgaFY3WT6FVPJIXFbkej2s9f50 && export filename=myanimelist.csv && wget -O $filename 'https://docs.google.com/uc?export=download&id='$fileid```

###  client.c
```c
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
```

### server.c
```c
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
```
