#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define SHM_SIZE 1024  // Ukuran shared memory
#define LOG_FILE "microservices/database/db.log" // Lokasi file log

// Fungsi untuk menulis log
void write_log(const char *type, const char *filename) {
    time_t now;
    struct tm *tm_info;
    char timestamp[20];
    FILE *file;

    // Mendapatkan timestamp saat ini
    time(&now);
    tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", tm_info);

    // Membuka file log
    file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    // Menulis log ke file
    fprintf(file, "[%s] [%s] [%s]\n", timestamp, type, filename);

    // Menutup file
    fclose(file);
}

int main() {
    // Memindahkan file yang valid dari new-data ke database
    if (rename("new-data/belobog_trashcan.csv", "microservices/database/belobog_trashcan.csv") == 0) {
        write_log("Trash Can", "belobog_trashcan.csv");
    } else {
        perror("Failed to move file");
        exit(EXIT_FAILURE);
    }

    if (rename("new-data/osaka_parkinglot.csv", "microservices/database/osaka_parkinglot.csv") == 0) {
        write_log("Parking Lot", "osaka_parkinglot.csv");
    } else {
        perror("Failed to move file");
        exit(EXIT_FAILURE);
    }

    printf("Files moved successfully and logged.\n");

    return 0;
}

