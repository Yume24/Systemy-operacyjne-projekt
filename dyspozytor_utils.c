#include "dyspozytor_utils.h"

pid_t workers[3];  // Przechowuje PID pracowników
pid_t trucks[NUMBER_OF_TRUCKS]; // Przechowuje PID ciężarówek
int message_queue_id; // Identyfikator kolejki komunikatów
key_t queue_key;


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

void remove_message_queue(int msgid)
{
    // Usunięcie kolejki komunikatów
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("Blad podczas usuwania kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    printf("Kolejka komunikatów o ID %d została usunięta.\n", msgid);
}

void create_workers()
{
    for (int i = 1; i <= 3; i++)
    {
        switch (workers[i] = fork())
        {
        case 0: // Kod potomka
            char key_str[16];
            sprintf(key_str, "%d", queue_key);

            // Wywołanie execl w if, aby obsłużyć błąd
            if (execl("./pracownik", "pracownik", (char[2]){i + '0', '\0'}, key_str, NULL) == -1)
            {
                perror("Blad przy wywolaniu execl");
                exit(EXIT_FAILURE);
            }
            break;

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

void create_trucks()
{
    for (int i = 1; i < NUMBER_OF_TRUCKS + 1; i++)
    {
        switch (trucks[i] = fork())
        {
        case 0: // Kod potomka
            char key_str[16];
            sprintf(key_str, "%d", queue_key);
            // Wywołanie execl w if, aby obsłużyć błąd
            if (execl("./ciezarowka", "ciezarowka", (char[2]){i + '0', '\0'}, key_str, NULL) == -1)
            {
                perror("Blad przy wywolaniu execl");
                exit(EXIT_FAILURE);
            }
            break;

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