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

    printf("Utworzono kolejke komunikatow o ID: %d\n", msgid);
    return msgid; // Zwraca identyfikator kolejki
}

int create_semaphore(key_t key, int init_val)
{
    int semid = semget(key, 1, IPC_CREAT | 0600);
    if (semid == -1)
    {
        perror("Blad przy tworzeniu semafora");
        exit(EXIT_FAILURE);
    }
    printf("Utworzono zbior semaforow o ID: %d\n", semid);
    // Ustawienie początkowej wartości semafora na 1
    if (init_val != NULL)
    {
        if (semctl(semid, 0, SETVAL, init_val) == -1)
        {
            perror("Blad przy ustawianiu semafora");
            exit(EXIT_FAILURE);
        }
    }

    return semid;
}