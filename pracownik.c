#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

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
    printf("Jestem pracownikiem %d\n", worker_id);

    // Obsługa sygnału
    signal(SIGTERM, signal_handler);

    while (running)
    {
        printf("Pracownik %d układa cegłę\n", worker_id);
        sleep(1); // Symulacja pracy
    }

    printf("Pracownik %d kończy pracę\n", worker_id);
    return 0;
}
