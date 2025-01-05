#include "pracownik_ciezarowka_utils.h"
#include "utils.h"

int curent_load = 0;

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
    //signal(SIGUSR2, signal_handler);
    setup_signal_handler();

    while (running)
    {
        printf("Ciezarowka %d czeka na semafor\n", truck_id);
        sem_wait(semaphore_id, 1);
        printf("Ciezarowka %d przeszla przez semafor\n", truck_id);
        get_bricks(truck_id, queue_id, &curent_load);
        sem_signal(semaphore_id, 1);
        printf("Ciezarowka %d rozwozi cegly...\n", truck_id);
        safe_sleep(TRUCK_GONE_TIME);
        curent_load = 0;
    }

    printf("Ciezarowka %d konczy prace\n", truck_id);
    return 0;
}
