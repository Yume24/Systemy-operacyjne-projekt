#include "dyspozytor_utils.h"
#include "utils.h"

int running = 1;

// Obsluga sygnalow
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
        printf(RESET "Zatrzymywanie pracy\n");
        for (int i = 0; i < 3; i++)
        {
            kill(workers[i], SIGUSR2);
        }
        for (int i = 0; i < NUMBER_OF_TRUCKS; i++)
        {
            kill(trucks[i], SIGUSR2);
        }
        semctl(worker_semaphore_id, 2, SETVAL, 1);
        while (wait(NULL) > 0)
        {
            int m_value = semctl(worker_semaphore_id, 0, GETVAL);
            int n_value = semctl(worker_semaphore_id, 1, GETVAL);
            if (n_value == -1 || m_value == -1)
            {
                perror("Blad przy odczycie semafora");
            }
            printf(BLUE "\t\t\t\t\t\tLiczba cegiel: %d/%d\n\t\t\t\t\t\tMasa cegiel: %d/%d\n\n" RESET, CONVEYOR_MAX_NUMBER - n_value, CONVEYOR_MAX_NUMBER, CONVEYOR_MAX_LOAD - m_value, CONVEYOR_MAX_LOAD);
        }
        // Usuwanie kolejki komunikatow oraz zbiorow semaforow
        remove_message_queue(message_queue_id);
        remove_semaphore(truck_semaphore_id);
        remove_semaphore(worker_semaphore_id);
        running = 0;
    }
}

int main()
{
    if (NUMBER_OF_TRUCKS <= 0 || NUMBER_OF_TRUCKS > 1000)
    {
        fprintf(stderr, "Nieprawidlowa ilosc ciezarowek!\n");
        exit(EXIT_FAILURE);
    }

    if (CONVEYOR_MAX_LOAD <= 0 || CONVEYOR_MAX_NUMBER <= 0 || CONVEYOR_MAX_LOAD > 10000 || CONVEYOR_MAX_LOAD > 1000)
    {
        fprintf(stderr, "Nieprawidlowy udzwig tasmy!\n");
        exit(EXIT_FAILURE);
    }

    initialize_log_file(FILE_NAME_TRUCKS);
    initialize_log_file(FILE_NAME_WORKERS);

    // Obsługa sygnaloww
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    // Generowanie kluczy
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

    if ((worker_semaphore_key = ftok(".", 2)) == -1)
    {
        perror("Blad generowania klucza semafora");
        exit(EXIT_FAILURE);
    }

    // Tworzenie semaforow oraz kolejki komunikatow
    truck_semaphore_id = create_semaphore(truck_semaphore_key, 1, (int[]){1});
    if (truck_semaphore_id == NULL)
    {
        printf("Niepoprawne tworzenie semafora\n");
        exit(EXIT_FAILURE);
    }
    worker_semaphore_id = create_semaphore(worker_semaphore_key, 3, (int[]){CONVEYOR_MAX_LOAD, CONVEYOR_MAX_NUMBER, 1});
    if (worker_semaphore_id == NULL)
    {
        printf("Niepoprawne tworzenie semafora\n");
        exit(EXIT_FAILURE);
    }
    message_queue_id = create_message_queue(queue_key);

    char queue_key_string[16];
    char truck_semaphore_key_string[16];
    char worker_semaphore_key_string[16];
    sprintf(queue_key_string, "%d", queue_key);
    sprintf(truck_semaphore_key_string, "%d", truck_semaphore_key);
    sprintf(worker_semaphore_key_string, "%d", worker_semaphore_key);

    // Tworzenie pracowników
    create_workers(queue_key_string, worker_semaphore_key_string);

    // Tworzenie ciężarówek
    create_trucks(queue_key_string, worker_semaphore_key_string, truck_semaphore_key_string);

    int old_m_value = 0;
    int old_n_value = 0;

    while (running)
    {
        int m_value = semctl(worker_semaphore_id, 0, GETVAL);
        int n_value = semctl(worker_semaphore_id, 1, GETVAL);
        if (n_value == -1 || m_value == -1)
        {
            perror("Blad przy odczycie semafora");
        }
        if (old_m_value != m_value || old_n_value != n_value)
        {
            printf(BLUE "\t\t\t\t\t\tLiczba cegiel: %d/%d\n\t\t\t\t\t\tMasa cegiel: %d/%d\n\n" RESET, CONVEYOR_MAX_NUMBER - n_value, CONVEYOR_MAX_NUMBER, CONVEYOR_MAX_LOAD - m_value, CONVEYOR_MAX_LOAD);
        }

        old_m_value = m_value;
        old_n_value = n_value;
    }

    return 0;
}
