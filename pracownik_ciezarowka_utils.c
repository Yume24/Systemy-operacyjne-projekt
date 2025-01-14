#include "pracownik_ciezarowka_utils.h"

volatile sig_atomic_t running = 1;
int queue_id;
int semaphore_id;

void place_brick(int id, int mass, int type, int queue_id)
{
    struct message msg; // Struktura wiadomości

    // Ustawienie danych w wiadomości
    msg.mtype = type;        // Typ wiadomości (priorytet, np. 1)
    msg.worker_id = id;      // ID pracownika
    msg.brick_weight = mass; // Masa cegły

    // Wysłanie wiadomości do kolejki
    if (msgsnd(queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("Błąd wysyłania cegły do kolejki komunikatów");
        exit(EXIT_FAILURE);
    }
    if (id > 0)
    {
        // printf("Pracownik %d dodał cegłę o masie %d do kolejki.\n", id, mass);
    }
    else
    {
        // printf("Ciezarowka %d zwrocila cegle o masie %d do kolejki.\n", -id, mass);
    }
}

void get_bricks(int truck_id, int queue_id, int semaphore_id, int *current_load, int *are_there_bricks, int *is_interrupted)
{

    int space_available = 1;
    struct message msg;
    if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 2, IPC_NOWAIT) == -1)
    {
        perror("Brak zostawionych cegiel");
    }
    else
    {
        safe_sleep(TRUCK_LOADING_TIME);
        *current_load += msg.brick_weight;
        printf("Ciezarowka %d odebrala cegle zostawiona przez ciezarowke %d o masie %d\n", truck_id, -msg.worker_id, msg.brick_weight);
    }
    printf("Obecny ladunek %d/%d\n", *current_load, TRUCK_MAX_LOAD);
    while (space_available && !(*is_interrupted))
    {
        if (running == 1)
        {
            // Oczekiwanie na wiadomosc z cegla
            if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 1, 0) == -1)
            {
                perror("Brak zostawionych cegiel");
                if (*is_interrupted)
                    break;
            }
        }
        else
        {
            if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 1, IPC_NOWAIT) == -1)
            {
                perror("Brak zostawionych cegiel");
                *are_there_bricks = 0;
                break;
            }
        }
        if (*current_load + msg.brick_weight > TRUCK_MAX_LOAD)
        {
            place_brick(-truck_id, msg.brick_weight, 2, queue_id);
            space_available = 0;
        }
        else
        {
            
            printf("Ciezarowka %d odebrala cegle o masie: %d od pracownika %d\n",
                   truck_id, msg.brick_weight, msg.worker_id);

            // Zwiekszenie aktualnego ladunku
            safe_sleep(TRUCK_LOADING_TIME);
            *current_load += msg.brick_weight;
        }
        printf("Obecny ladunek %d/%d\n", *current_load, TRUCK_MAX_LOAD);
    }
    if (*is_interrupted)
    {
        printf("Ciezarowka %d odjazd na polecenie dyspozytora\n", truck_id);
        *is_interrupted = 0;
    }
}

void safe_sleep(int seconds)
{
    struct timespec req, rem;
    req.tv_sec = seconds;
    req.tv_nsec = 0;

    while (nanosleep(&req, &rem) == -1 && errno == EINTR)
    {
        req = rem; // Kontynuuje pozostały czas, jeśli został przerwany
    }
}

void sem_op(int semid, int val[], int num_sems)
{
    struct sembuf operations[num_sems]; // Tablica struktur sembuf

    // Inicjalizacja tablicy operations
    for (int i = 0; i < num_sems; i++)
    {
        operations[i].sem_num = i;     // Numer semafora
        operations[i].sem_op = val[i]; // Wartość operacji dla semafora
        operations[i].sem_flg = 0;     // Flagi operacji
    }
    // Proba opuszczenia semafora z obsluga sygnalow
    while (semop(semid, operations, num_sems) == -1)
    {
        if (errno == EINTR)
        {             // Przerwanie przez sygnał
            continue; // Powtórz operację
        }
        else
        {
            perror("Nie mozna opuscic semafora");
            exit(EXIT_FAILURE);
        }
    }
}