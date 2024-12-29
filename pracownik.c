#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
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
        fprintf(stderr, "Brak argumentu ID pracownika.\n");
        exit(EXIT_FAILURE);
    }

    int worker_id = atoi(argv[1]);
    key_t queue_key = atoi(argv[2]);
    printf("Jestem pracownikiem %d\n", worker_id);
    int queue_id = link_to_queue(queue_key);
    // Obsługa sygnału
    signal(SIGTERM, signal_handler);

    while (running)
    {
        //printf("Pracownik %d układa cegłę\n", worker_id);
        sleep(1); // Symulacja pracy
    }

    printf("Pracownik %d konczy prace\n", worker_id);
    return 0;
}
