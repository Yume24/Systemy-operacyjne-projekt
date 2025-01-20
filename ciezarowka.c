#include "pracownik_ciezarowka_utils.h"
#include "utils.h"

int current_load = 0; // Obecny stopien zaladowania ciezarowki
int are_there_bricks = 1;
volatile sig_atomic_t is_interrupted = 0;
int semaphore_id_trucks;

// Obsluga sygnalu SIGUSR1 - odjazd ciezarowki
void sigusr1_handler(int signum)
{
    signal(SIGUSR1, sigusr1_handler);
    is_interrupted = 1;
}

// Obsluga sygnalu SIGUSR2 - koniec pracy
void signal_handler(int signum)
{
    running = 0;
}

void setup_signal_handler()
{
    if (signal(SIGUSR1, sigusr1_handler) == -1)
    {
        perror("Blad ustawiania obslugi sygnalu");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR2, signal_handler) == -1)
    {
        perror("Blad ustawiania obslugi sygnalu");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, "Brak argumentow\n");
        exit(EXIT_FAILURE);
    }
    
    if (TRUCK_MAX_LOAD < 3)
    {
        fprintf(stderr, "Za mala pojemnosc ciezarowki\n");
        exit(EXIT_FAILURE);
    }
    if (TRUCK_LOADING_TIME < 0)
    {
        fprintf(stderr, "Nieprawidlowy czas zaladunku\n");
        exit(EXIT_FAILURE);
    }
    if (TRUCK_GONE_TIME < 3)
    {
        fprintf(stderr, "Nieprawidlowy czas rozwozenia cegiel\n");
        exit(EXIT_FAILURE);
    }

    setvbuf(stdout, NULL, _IONBF, 0); // Wylaczenie buforowania

    int truck_id = atoi(argv[1]);
    key_t queue_key = atoi(argv[2]);
    key_t semaphore_key = atoi(argv[3]);
    key_t semaphore_key_trucks = atoi(argv[4]);

    // Podlaczanie sie do kolejki oraz semaforow
    queue_id = create_message_queue(queue_key);
    semaphore_id_trucks = create_semaphore(semaphore_key_trucks, 1, NULL);
    semaphore_id = create_semaphore(semaphore_key, 3, NULL);

    setup_signal_handler();

    while (are_there_bricks) // Petla dziala dopoki w kolejce znajduja sie cegly
    {
        // Obnizenie semafora aby tylko 1 ciezarowka na raz mogla odbierac cegly
        sem_op(semaphore_id_trucks, (int[]){-1}, 1);
        is_interrupted = 0;
        get_bricks(truck_id, queue_id, semaphore_id, &current_load, &are_there_bricks, &is_interrupted);
        sem_op(semaphore_id_trucks, (int[]){1}, 1);
        if (current_load > 0)
        {
            printf(RED "Ciezarowka %d rozwozi cegly...\n" RESET, truck_id);
            // safe_sleep(TRUCK_GONE_TIME);
            current_load = 0;
        }
    }

    printf(RED "Ciezarowka %d konczy prace\n" RESET, truck_id);
    return 0;
}
