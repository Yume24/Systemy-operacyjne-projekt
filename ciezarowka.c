#include "pracownik_ciezarowka_utils.h"

volatile sig_atomic_t running = 1;
int queue_id;
int curent_load = 0;

void signal_handler(int signum)
{
    if (signum == SIGTERM)
    {
        running = 0;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Brak argumentow\n");
        exit(EXIT_FAILURE);
    }

    int truck_id = atoi(argv[1]);
    key_t queue_key = atoi(argv[2]);
    printf("Jestem ciezarowka %d\n", truck_id);
    queue_id = link_to_queue(queue_key);
    // Obsługa sygnału
    signal(SIGTERM, signal_handler);

    while (running)
    {
        get_bricks(truck_id, queue_id, &curent_load);
        printf("Ciezarowka %d rozwozi cegly...", truck_id);
        sleep(TRUCK_GONE_TIME);
    }

    printf("Ciezarowka %d konczy prace\n", truck_id);
    return 0;
}
