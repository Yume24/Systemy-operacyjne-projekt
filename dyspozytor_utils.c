#include "dyspozytor_utils.h"

pid_t workers[3];               // Przechowuje PID pracownikow
pid_t trucks[NUMBER_OF_TRUCKS]; // Przechowuje PID ciezarowek
int message_queue_id;
int truck_semaphore_id;
int worker_semaphore_id;
key_t queue_key;
key_t truck_semaphore_key;
key_t worker_semaphore_key;

void remove_message_queue(int msgid) // Usuwanie kolejki komunikatow
{
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("Blad podczas usuwania kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    printf("Kolejka komunikatów o ID %d zostala usunieta.\n", msgid);
}

void remove_semaphore(int semid) // Usuwanie zbioru semaforow
{
    if (semctl(semid, 0, IPC_RMID, NULL) == -1)
    {
        perror("Blad podczas usuwania semafora");
        exit(EXIT_FAILURE);
    }

    printf("Zbior semaforow o ID %d zostal usuniety.\n", semid);
}

// Tworzenie pracownikow poprzez fork i exec
void create_workers(char *queue_key_string, char *semaphore_key_string)
{
    for (int i = 1; i <= 3; i++)
    {
        switch (workers[i - 1] = fork())
        {
        case 0: // Kod potomka
        {
            if (execl("./pracownik", "pracownik", (char[2]){i + '0', '\0'}, queue_key_string, semaphore_key_string, NULL) == -1)
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

// Tworzenie ciezarowek poprzez fork i exec
void create_trucks(char *queue_key_string, char *semaphore_key_string, char *truck_semaphore_key_string)
{
    for (int i = 1; i < NUMBER_OF_TRUCKS + 1; i++)
    {
        switch (trucks[i - 1] = fork())
        {
        case 0: // Kod potomka
        {
            char truck_number[12];
            snprintf(truck_number, sizeof(truck_number), "%d", i);
            if (execl("./ciezarowka", "ciezarowka", truck_number, queue_key_string, semaphore_key_string, truck_semaphore_key_string, NULL) == -1)
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

void initialize_log_file(char *file_name)
{
    // Otwórz plik w trybie "write"
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        // Obsluga bledu otwierania pliku
        fprintf(stderr, "Blad tworzenia pliku %s: %s\n", file_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Zamknij plik
    if (fclose(file) != 0)
    {
        // Obsluga bledu zamykania pliku
        fprintf(stderr, "Blad zamykania pliku %s: %s\n", file_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
}