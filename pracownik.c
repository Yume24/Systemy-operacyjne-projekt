#include "pracownik_ciezarowka_utils.h"

volatile sig_atomic_t running = 1;
int queue_id;

void signal_handler(int signum)
{
    if (signum == SIGTERM)
    {
        running = 0;
    }
}

void place_brick(int worker_id)
{
    struct message msg; // Struktura wiadomości

    // Ustawienie danych w wiadomości
    msg.mtype = 1;                     // Typ wiadomości (priorytet, np. 1)
    msg.worker_id = worker_id;         // ID pracownika
    msg.brick_weight = worker_id;   // Masa cegły

    // Wysłanie wiadomości do kolejki
    if (msgsnd(queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("Błąd wysyłania cegły do kolejki komunikatów");
        exit(EXIT_FAILURE);
    }

    printf("Pracownik %d dodał cegłę o masie %d do kolejki.\n", worker_id, worker_id);
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
    queue_id = link_to_queue(queue_key);
    // Obsługa sygnału
    signal(SIGTERM, signal_handler);
    srand(time(NULL));
    while (running)
    {
        place_brick(worker_id);
        sleep(rand() % 5 + 1);
    }

    printf("Pracownik %d konczy prace\n", worker_id);
    return 0;
}
