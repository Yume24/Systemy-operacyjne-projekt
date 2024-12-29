#include "dyspozytor_utils.h"

void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("Sygnal do odjazdu ciezarowki\n");
    }
    else if (signum == SIGUSR2)
    {
        printf("Zatrzymywanie pracy\n");
        remove_message_queue(message_queue_id);
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
    printf("Dyspozytor PID: %d\n", getpid());

    // Obsługa sygnałów
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    if ((queue_key = ftok(".", 0)) == -1) 
    {
        perror("Blad generowania klucza");
        exit(EXIT_FAILURE);
    }
    message_queue_id = create_message_queue(queue_key);

    // Tworzenie pracowników
    create_workers();

    // Oczekiwanie na sygnały
    while (1)
    {
        pause(); // Czeka na sygnał
    }
    remove_message_queue(message_queue_id);
    return 0;
}
