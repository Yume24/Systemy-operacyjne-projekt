#include "pracownik_ciezarowka_utils.h"
#include "utils.h"
void signal_handler(int signum)
{
    if (signum == SIGUSR2)
    {
        running = 0;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Brak argumentow\n");
        exit(EXIT_FAILURE);
    }

    int worker_id = atoi(argv[1]);
    key_t queue_key = atoi(argv[2]);
    printf("Jestem pracownikiem %d\n", worker_id);
    queue_id = create_message_queue(queue_key);
    // Obsługa sygnału
    signal(SIGUSR2, signal_handler);
    srand(time(NULL) * worker_id);
    while (running)
    {
        place_brick(worker_id, worker_id, 1, queue_id);
        sleep(rand() % 5 + 1);
    }

    printf("Pracownik %d konczy prace\n", worker_id);
    return 0;
}
