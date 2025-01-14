#include "dyspozytor_utils.h"

pid_t workers[3];               // Przechowuje PID pracowników
pid_t trucks[NUMBER_OF_TRUCKS]; // Przechowuje PID ciężarówek
int message_queue_id;
int truck_semaphore_id;
int worker_semaphore_id;
key_t queue_key;
key_t truck_semaphore_key;
key_t worker_semaphore_key;

void remove_message_queue(int msgid)
{
    // Usunięcie kolejki komunikatów
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("Blad podczas usuwania kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    printf("Kolejka komunikatów o ID %d zostala usunieta.\n", msgid);
}

void remove_semaphore(int semid)
{
    // Usunięcie semafora
    if (semctl(semid, 0, IPC_RMID, NULL) == -1)
    {
        perror("Blad podczas usuwania semafora");
        exit(EXIT_FAILURE);
    }

    printf("Zbior semaforow o ID %d zostal usuniety.\n", semid);
}

void create_workers(char *queue_key_string, char *semaphore_key_string)
{
    for (int i = 1; i <= 3; i++)
    {
        switch (workers[i - 1] = fork())
        {
        case 0:
        { // Kod potomka
            // Wywołanie execl w if, aby obsłużyć błąd
            if (execl("./pracownik", "pracownik", (char[2]){i + '0', '\0'}, queue_key_string, semaphore_key_string,NULL) == -1)
            {
                perror("Blad przy wywolaniu execl");
                exit(EXIT_FAILURE);
            }
            break;
        }

        case -1: // Błąd
            perror("fork error");
            exit(EXIT_FAILURE);
            break;

        default: // Kod rodzica
            printf("Stworzono pracownika %d\n", i);
            break;
        }
    }
}

void create_trucks(char *queue_key_string, char *semaphore_key_string, char *truck_semaphore_key_string)
{
    for (int i = 1; i < NUMBER_OF_TRUCKS + 1; i++)
    {
        switch (trucks[i - 1] = fork())
        {
        case 0:
        { // Kod potomka
            // Wywołanie execl w if, aby obsłużyć błąd
            if (execl("./ciezarowka", "ciezarowka", (char[2]){i + '0', '\0'}, queue_key_string, semaphore_key_string, truck_semaphore_key_string,NULL) == -1)
            {
                perror("Blad przy wywolaniu execl");
                exit(EXIT_FAILURE);
            }
            break;
        }

        case -1: // Błąd
            perror("fork error");
            exit(EXIT_FAILURE);
            break;

        default: // Kod rodzica
            printf("Stworzono ciezarowke %d\n", i);
            break;
        }
    }
}
