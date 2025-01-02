#include "dyspozytor_utils.h"
int running = 1;
void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("Sygnal do odjazdu ciezarowki\n");
    }
    else if (signum == SIGUSR2)
    {
        printf("Zatrzymywanie pracy\n");
        for (int i = 0; i < 3; i++)
        {
            kill(workers[i], SIGTERM);
        }
        for (int i = 0; i < NUMBER_OF_TRUCKS; i++)
        {
            kill(trucks[i], SIGTERM);
        }
        while(wait(NULL) > 0);
        printf("test\n");
        remove_message_queue(message_queue_id);
        running = 0;
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
    sprintf(queue_key_string, "%d", queue_key);
    // Tworzenie pracowników
    create_workers();
    // Tworzenie ciężarówek
    create_trucks();
    // Oczekiwanie na sygnały
    while (running)
    {
        pause();
    }

    return 0;
}
