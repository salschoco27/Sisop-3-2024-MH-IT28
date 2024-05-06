#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN 100

void operasi(char *op, int num1, int num2, int pipe_fd[]);
void ubah_kata(int pipe_fd[], char *op);
void log_history(char *op, char *message);

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
        // Child process
        close(pipe1[1]);  // Tutup pipe1 untuk menulis
        close(pipe2[0]);  // Tutup pipe2 untuk membaca

        ubah_kata(pipe1, argv[1]);

        close(pipe1[0]);
        close(pipe2[1]);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
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
