#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_FILENAME_LENGTH 512

int is_valid_filename(char *filename) {
    // Memeriksa apakah file adalah file CSV dan berakhiran 'trashcan.csv' atau 'parkinglot.csv'
    int len = strlen(filename);
    if (len < 12) // Panjang minimal file adalah "x_trashcan.csv" atau "x_parkinglot.csv"
        return 0;
    if (strcmp(&filename[len - 11], "_trashcan.csv") == 0 || strcmp(&filename[len - 13], "_parkinglot.csv") == 0)
        return 1;
    return 0;
}

int main(int argc, char *argv[]) {
    // Memeriksa jumlah argumen yang diberikan
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <folder_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    DIR *dir;
    struct dirent *entry;

    char *folder_path = argv[1];

    // Membuka folder
    if ((dir = opendir(folder_path)) == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Membaca setiap entri di dalam folder
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Memeriksa apakah entri adalah file regular
            char filename[MAX_FILENAME_LENGTH];
            snprintf(filename, MAX_FILENAME_LENGTH, "%s/%s", folder_path, entry->d_name);

            // Memeriksa apakah file tersebut valid
            if (!is_valid_filename(entry->d_name)) {
                fprintf(stderr, "Invalid filename format. Filename must end with 'trashcan.csv' or 'parkinglot.csv'. Deleting file: %s\n", filename);
                // Menghapus file yang tidak valid
                unlink(filename);
            } else {
                printf("File %s successfully verified.\n", filename);
            }
        }
    }

    // Menutup folder
    closedir(dir);
    
    return 0;
}
