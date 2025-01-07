#include "pracownik_ciezarowka_utils.h"
#include "utils.h"

int current_load = 0;
int are_there_bricks = 1;
volatile sig_atomic_t is_interrupted = 0;

void sigusr1_handler(int signum)
{
    signal(SIGUSR1, sigusr1_handler);
    is_interrupted = 1;
}

void setup_additional_signal_handler()
{
    if (signal(SIGUSR1, sigusr1_handler) == -1)
    {
        perror("Blad ustawiania obslugi sygnalu");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Brak argumentow\n");
        exit(EXIT_FAILURE);
    }

    int truck_id = atoi(argv[1]);
    key_t queue_key = atoi(argv[2]);
    key_t semaphore_key = atoi(argv[3]);
    queue_id = create_message_queue(queue_key);
    semaphore_id = create_semaphore(semaphore_key, NULL);
    // Obsługa sygnału
    setup_signal_handler();
    setup_additional_signal_handler();

    while (are_there_bricks)
    {
        printf("Ciezarowka %d czeka na semafor\n", truck_id);
        sem_wait(semaphore_id, 1);
        printf("Ciezarowka %d przeszla przez semafor\n", truck_id);
        is_interrupted = 0;
        get_bricks(truck_id, queue_id, &current_load, &are_there_bricks, &is_interrupted);
        sem_signal(semaphore_id, 1);
        if (current_load > 0)
        {
            printf("Ciezarowka %d rozwozi cegly...\n", truck_id);
            safe_sleep(TRUCK_GONE_TIME);
            current_load = 0;
        }
    }

    printf("Ciezarowka %d konczy prace\n", truck_id);
    return 0;
}
