#include "pracownik_ciezarowka_utils.h"

volatile sig_atomic_t running = 1;

void signal_handler(int signum)
{
    if (signum == SIGTERM)
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

    int truck_id = atoi(argv[1]);
    key_t queue_key = atoi(argv[2]);
    printf("Jestem ciezarowka %d\n", truck_id);
    int queue_id = link_to_queue(queue_key);
    // Obsługa sygnału
    signal(SIGTERM, signal_handler);

    while (running)
    {
        //printf("Pracownik %d układa cegłę\n", truck_id);
        sleep(1); // Symulacja pracy
    }

    printf("Ciezarowka %d konczy prace\n", truck_id);
    return 0;
}
