#include "pracownik_ciezarowka_utils.h"

volatile sig_atomic_t running = 1;
int queue_id;
int semaphore_id;

// Umieszczanie cegly na tasmie
void place_brick(int id, int mass, int type, int queue_id)
{
    struct message msg; // Struktura wiadomosci

    // Ustawienie danych w wiadomosci
    msg.mtype = type;        // Typ wiadomosci (0 - zwykla cegla, 1 - cegla pozstawiona)
    msg.worker_id = id;      // ID pracownika
    msg.brick_weight = mass; // Masa cegly

    // Wysłanie wiadomości do kolejki
    if (msgsnd(queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("Błąd wysyłania cegły do kolejki komunikatów");
        exit(EXIT_FAILURE);
    }
    if (id > 0)
    {
        printf(GREEN "\t\t\t\t\t\t\t\t\t\tPracownik %d dodal cegle o masie %d.\n" RESET, id, mass);
    }
    else
    {
        printf(RED "Ciezarowka %d zwrocila cegle o masie %d.\n" RESET, -id, mass);
    }
}

// Ladowanie ciezarowki
void get_bricks(int truck_id, int queue_id, int semaphore_id, int *current_load, int *are_there_bricks, int *is_interrupted)
{

    int space_available = 1;
    struct message msg;

    // Sprawdzenie czy sa jakies pozostawione cegly
    if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 2, IPC_NOWAIT) == -1)
    {
        perror("Brak zostawionych cegiel");
    }
    else
    {
        safe_sleep(TRUCK_LOADING_TIME);
        *current_load += msg.brick_weight;
        printf(RED "Ciezarowka %d odebrala zostawiona cegle o masie %d\n" RESET, truck_id, msg.brick_weight);
    }
    printf(RED "Obecny ladunek %d/%d\n" RESET, *current_load, TRUCK_MAX_LOAD);

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
            else
                sem_op(semaphore_id, (int[]){msg.brick_weight, 1}, 2); // Zwolnienie miejsca na tasmie
        }
        else
        {
            // Gdy pracownicy zakoncza prace, ciezarowka konczy, gdy nie ma juz zadnych cegiel
            if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 1, IPC_NOWAIT) == -1)
            {
                perror("Brak zostawionych cegiel");
                *are_there_bricks = 0;
                break;
            }
            else
                sem_op(semaphore_id, (int[]){msg.brick_weight, 1}, 2);
        }
        if (*current_load + msg.brick_weight > TRUCK_MAX_LOAD)
        {
            // Jezeli odebrana cegla przekracza ladownosc ciezarowki, to jest zwracana z powrotem
            // i jest odbierana jako pierwsza przez nastepna ciezarowke
            place_brick(-truck_id, msg.brick_weight, 2, queue_id);
            space_available = 0;
        }
        else
        {
            printf(RED "Ciezarowka %d odebrala cegle o masie %d\n" RESET,
                   truck_id, msg.brick_weight);

            // Zwiekszenie aktualnego ladunku
            safe_sleep(TRUCK_LOADING_TIME);
            *current_load += msg.brick_weight;
        }
        printf(RED "Obecny ladunek %d/%d\n" RESET, *current_load, TRUCK_MAX_LOAD);
    }
    if (*is_interrupted) // Na polecenie dyspozytora ciezarowka moze odjechac wczesniej
    {
        printf(RESET "Ciezarowka %d odjazd na polecenie dyspozytora\n" RESET, truck_id);
        *is_interrupted = 0;
    }
}

void safe_sleep(int seconds) // Funkcja podobna do sleep(), ale kontynuuje "spanie" po obsluzeniu sygnalu
{
    struct timespec req, rem;
    req.tv_sec = seconds;
    req.tv_nsec = 0;

    while (nanosleep(&req, &rem) == -1 && errno == EINTR)
    {
        req = rem; // Kontynuuje pozostaly czas, jesli zostal przerwany
    }
}

void sem_op(int semid, int val[], int num_sems)
{
    struct sembuf operations[num_sems]; // Tablica struktur sembuf

    // Inicjalizacja tablicy operacji semaforowych do wykonania
    for (int i = 0; i < num_sems; i++)
    {
        operations[i].sem_num = i;     // Numer semafora
        operations[i].sem_op = val[i]; // Wartość operacji dla semafora
        operations[i].sem_flg = 0;     // Flagi operacji
    }
    // Proba opuszczenia semafora
    while (semop(semid, operations, num_sems) == -1)
    {
        if (errno == EINTR)
        {             // Przerwanie przez sygnal
            continue; // Powtorz operację
        }
        else
        {
            perror("Nie mozna opuscic semafora");
            exit(EXIT_FAILURE);
        }
    }
}