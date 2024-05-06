#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024

int main() {
    // Mengambil data dari shared memory
    key_t key = ftok("shared_memory_key", 'R');
    int shmid = shmget(key, SHM_SIZE, 0666);
    char *shmaddr = shmat(shmid, (void *)0, 0);

    // Menampilkan tempat sampah dan parkiran dengan rating tertinggi
    printf("Type : Trash Can\n");
    printf("Filename : belobog_trashcan.csv\n");
    printf("------------------------\n");
    printf("%s\n", shmaddr);

    // Melepaskan shared memory
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
