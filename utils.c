#include "utils.h"

int create_message_queue(key_t key)
{
    int msgid;

    // Tworzenie lub otwieranie kolejki komunikatów
    msgid = msgget(key, IPC_CREAT | 0600);
    if (msgid == -1)
    {
        perror("Blad przy tworzeniu kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    return msgid; // Zwraca identyfikator kolejki
}

int create_semaphore(key_t key, int nsems, int init_val[]) {
    int semid = semget(key, nsems, IPC_CREAT | 0600);
    if (semid == -1) {
        perror("Blad przy tworzeniu semafora");
        exit(EXIT_FAILURE);
    }

    // Ustawienie wartości początkowych semaforów
    if (init_val != NULL) {
        for (int i = 0; i < nsems; i++) {
            if (semctl(semid, i, SETVAL, init_val[i]) == -1) {
                perror("Blad przy ustawianiu semafora");
                exit(EXIT_FAILURE);
            }
        }
    }

    return semid;
}
