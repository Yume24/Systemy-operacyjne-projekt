#include "pracownik_ciezarowka_utils.h"
#include "utils.h"

int worker_id;

void signal_handler(int signum)
{
    running = 0;
    printf("Pracownik %d konczy prace\n", worker_id);
    exit(EXIT_SUCCESS);
}

void setup_signal_handler()
{
    if (signal(SIGUSR2, signal_handler) == -1)
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

    worker_id = atoi(argv[1]);
    key_t queue_key = atoi(argv[2]);
    key_t semaphore_key = atoi(argv[3]);
    queue_id = create_message_queue(queue_key);
    semaphore_id = create_semaphore(semaphore_key, 2, NULL);
    // Obsługa sygnału
    setup_signal_handler();
    srand(time(NULL) * worker_id);
    while (running)
    {
        sleep(rand() % 5 + 1);
        sem_op(semaphore_id, (int[]){-worker_id, -1}, 2);
        place_brick(worker_id, worker_id, 1, queue_id);
    }

    printf("Pracownik %d konczy prace\n", worker_id);
    return 0;
}
