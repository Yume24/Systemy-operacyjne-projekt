#include "pracownik_ciezarowka_utils.h"
#include "utils.h"

int worker_id;

// Obsluga sygnalu SIGUSR2 (zakonczenie pracy)
void signal_handler(int signum)
{
    running = 0;
    printf("\t\t\t\t\t\t\t\t\t\tPracownik %d konczy prace\n", worker_id);
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

    setvbuf(stdout, NULL, _IONBF, 0); // Wylaczenie buforowania

    worker_id = atoi(argv[1]);
    key_t queue_key = atoi(argv[2]);
    key_t semaphore_key = atoi(argv[3]);

    // Uzyskanie dostepu do kolejki komunikatow i zbioru semaforow
    queue_id = create_message_queue(queue_key);
    semaphore_id = create_semaphore(semaphore_key, 3, NULL);

    // Inicjalizacja obslugi sygnalow
    setup_signal_handler();

    // Inicjalizacja generatora liczb losowych
    srand(time(NULL) * worker_id);

    while (running)
    {
        sleep(rand() % 5 + 1); // Spij przez losowa ilosc sekund 1-5

        // Sprobuj polozyc cegle na tasmie
        sem_op(semaphore_id, (int[]){-worker_id, -1, -1}, 3);
        place_brick(worker_id, worker_id, 1, queue_id);
        sem_op_one_sem(semaphore_id, 1, 2);
    }

    printf(GREEN "\t\t\t\t\t\t\t\t\t\tPracownik %d konczy prace\n" RESET, worker_id);
    return 0;
}
