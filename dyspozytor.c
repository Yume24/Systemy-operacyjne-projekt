#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define TRUCK_MAX_LOAD 100
#define CONVEYOR_MAX_NUMBER 20
#define CONVEYOR_MAX_LOAD 30
#define NUMBER_OF_TRUCKS 3
#define TRUCK_GONE_TIME 5

pid_t workers[3];

void create_workers()
{
    for (int i = 0; i < 3; i++)
    {
        switch (workers[i] = fork())
        {
        case 0: // Kod potomka
            execl("./pracownik", "pracownik", (char[2]){i + '0', '\0'}, NULL);
            perror("execl error");
            exit(EXIT_FAILURE);

        case -1: // Błąd
            perror("fork error");
            exit(EXIT_FAILURE);

        default: // Kod rodzica
            printf("Stworzono pracownika %d\n", i + 1);
            break;
        }
    }
}

void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("Sygnał do odjazdu ciężarówki\n");
    }
    else if (signum == SIGUSR2)
    {
        printf("Zatrzymywanie pracy\n");
        for (int i = 0; i < 3; i++)
        {
            kill(workers[i], SIGTERM);
        }
        while (wait(NULL) > 0);
        exit(0);
    }
}

int main()
{
    // Obsługa sygnałów
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    // Tworzenie pracowników
    create_workers();

    // Oczekiwanie na sygnały
    while (1)
    {
        pause(); // Czeka na sygnał
    }

    return 0;
}
