#include "pracownik_ciezarowka_utils.h"

volatile sig_atomic_t running = 1;
int queue_id;

int link_to_queue(key_t key)
{
    int msgid;

    msgid = msgget(key, IPC_CREAT | 0600);
    if (msgid == -1)
    {
        perror("Blad przy dolaczaniu do kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    printf("Dolaczono do kolejki komunikatow o ID: %d\n", msgid);
    return msgid;
}

void place_brick(int id, int mass, int type, int queue_id)
{
    struct message msg; // Struktura wiadomości

    // Ustawienie danych w wiadomości
    msg.mtype = type;                     // Typ wiadomości (priorytet, np. 1)
    msg.worker_id = id;         // ID pracownika
    msg.brick_weight = mass;   // Masa cegły

    // Wysłanie wiadomości do kolejki
    if (msgsnd(queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("Błąd wysyłania cegły do kolejki komunikatów");
        exit(EXIT_FAILURE);
    }
    if (id > 0) {}//printf("Pracownik %d dodał cegłę o masie %d do kolejki.\n", id, mass);
    else printf("Ciezarowka %d zwrocila cegle o masie %d do kolejki.\n", -id, mass);
}

void get_bricks(int truck_id, int queue_id, int* current_load)
{
    int space_available = 1;
    struct message msg;
    if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 2, IPC_NOWAIT) == -1)
        {
            perror("Blad odbioru cegly");
            //exit(EXIT_FAILURE);
        }
    else 
    {
        *current_load += msg.brick_weight;
        printf("Odebrano cegle zostawiona przez ciezarowke %d o masie %d\n", -msg.worker_id, msg.brick_weight);
    }

    while (space_available)
    {
        // Oczekiwanie na wiadomosc z cegla
       if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 1, 0) == -1)
        {
            perror("Brak zostawionych cegiel");
            exit(EXIT_FAILURE);
        }
        if (*current_load + msg.brick_weight > TRUCK_MAX_LOAD)
        {
            place_brick(-truck_id, msg.brick_weight, 2, queue_id);
            printf("Zwrocono cegle o masie %d\n", msg.brick_weight);
            space_available = 0;
        }
        else
        {
            printf("Ciezarowka %d odebrala cegle o masie: %d od pracownika %d\n",
                truck_id, msg.brick_weight, msg.worker_id);

            // Zwiekszenie aktualnego ladunku
            *current_load += msg.brick_weight;
        }
        printf("Obecny ladunek %d/%d\n", *current_load, TRUCK_MAX_LOAD);
        sleep(TRUCK_LOADING_TIME);
    }
}

void safe_sleep(int seconds) 
{
    struct timespec req, rem;
    req.tv_sec = seconds;
    req.tv_nsec = 0;

    while (nanosleep(&req, &rem) == -1 && errno == EINTR) {
        req = rem;  // Kontynuuje pozostały czas, jeśli został przerwany
    }
}