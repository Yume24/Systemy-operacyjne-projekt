#include "dyspozytor_utils.h"
#include "utils.h"

int running = 1;

void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        for (int i = 0; i < NUMBER_OF_TRUCKS; i++)
        {
            kill(trucks[i], SIGUSR1);
        }
    }
    else if (signum == SIGUSR2)
    {
        printf("Zatrzymywanie pracy\n");
        for (int i = 0; i < 3; i++)
        {
            kill(workers[i], SIGUSR2);
        }
        for (int i = 0; i < NUMBER_OF_TRUCKS; i++)
        {
            kill(trucks[i], SIGUSR2);
        }
        while (wait(NULL) > 0)
            ;
        remove_message_queue(message_queue_id);
        remove_semaphore(truck_semaphore_id);
        running = 0;
    }
}

int main()
{
    // printf("Dyspozytor PID: %d\n", getpid());

    // Obsługa sygnałów
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    if ((queue_key = ftok(".", 0)) == -1)
    {
        perror("Blad generowania klucza");
        exit(EXIT_FAILURE);
    }
    if ((truck_semaphore_key = ftok(".", 1)) == -1)
    {
        perror("Blad generowania klucza semafora");
        exit(EXIT_FAILURE);
    }

    truck_semaphore_id = create_semaphore(truck_semaphore_key, 1);
    message_queue_id = create_message_queue(queue_key);

    char queue_key_string[16];
    char truck_semaphore_key_string[16];
    sprintf(queue_key_string, "%d", queue_key);
    sprintf(truck_semaphore_key_string, "%d", truck_semaphore_key);
    // Tworzenie pracowników
    create_workers(queue_key_string);
    // Tworzenie ciężarówek
    create_trucks(queue_key_string, truck_semaphore_key_string);
    // Oczekiwanie na sygnały
    while (running)
    {
        pause();
    }

    return 0;
}
