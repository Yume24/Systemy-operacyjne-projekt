#include "pracownik_ciezarowka_utils.h"
#include "utils.h"

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
        printf(GREEN "\t\t\t\t\t\t\t\t\t\tPracownik %d dodal cegle o masie %d\n" RESET, id, mass);
        char log[50];
        snprintf(log, sizeof(log), "Pracownik %d dodal cegle o masie %d", id, mass);
        write_to_file(FILE_NAME_WORKERS, log);
    }
    else
    {
        printf(RED "Ciezarowka %d zwrocila cegle o masie %d\n" RESET, -id, mass);
    }
}

// Ladowanie ciezarowki
void get_bricks(int truck_id, int queue_id, int semaphore_id, int *current_load, int *are_there_bricks, int *is_interrupted)
{
    printf(RED "Ciezarowka %d podjezdza do tasmy\n" RESET, truck_id);
    int did_receive = 1;
    int space_available = 1;
    struct message msg;
    msg.brick_weight = 0;
    msg.mtype = 0;
    msg.worker_id = 0;
    // Sprawdzenie czy sa jakies pozostawione cegly
    if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 2, IPC_NOWAIT) == -1)
    {
        perror("Brak zostawionych cegiel");
        did_receive = 0;
    }
    else
    {
        sem_op(semaphore_id, (int[]){msg.brick_weight, 1}, 2);
        safe_sleep(TRUCK_LOADING_TIME);
        *current_load += msg.brick_weight;
        printf(RED "Ciezarowka %d odebrala zostawiona cegle o masie %d\n" RESET, truck_id, msg.brick_weight);
        char log[50];
        snprintf(log, sizeof(log), "Ciezarowka %d odebrala zostawiona cegle o masie %d", truck_id, msg.brick_weight);
        write_to_file(FILE_NAME_TRUCKS, log); // Zapisywanie logu do pliku
    }
    printf(RED "Obecny ladunek %d/%d\n" RESET, *current_load, TRUCK_MAX_LOAD);

    while (space_available && !(*is_interrupted))
    {
        if (running == 1)
        {
            // Oczekiwanie na wiadomosc z cegla
            sem_op_one_sem(semaphore_id, -1, 2);
            if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 1, IPC_NOWAIT) == -1)
            {
                did_receive = 0;
                if (*is_interrupted)
                    break;
            }
            else
            {
                did_receive = 1;
                sem_op(semaphore_id, (int[]){msg.brick_weight, 1}, 2); // Zwolnienie miejsca na tasmie
            }
        }
        else
        {
            // Gdy pracownicy zakoncza prace, ciezarowka konczy, gdy nie ma juz zadnych cegiel
            if (msgrcv(queue_id, &msg, sizeof(msg) - sizeof(long), 1, IPC_NOWAIT) == -1)
            {
                perror("Brak zostawionych cegiel");
                *are_there_bricks = 0;
                did_receive = 0;
                break;
            }
            else
            {
                sem_op(semaphore_id, (int[]){msg.brick_weight, 1}, 2);
                did_receive = 1;
            }
        }
        if (*current_load + msg.brick_weight > TRUCK_MAX_LOAD)
        {
            // Jezeli odebrana cegla przekracza ladownosc ciezarowki, to jest zwracana z powrotem
            // i jest odbierana jako pierwsza przez nastepna ciezarowke
            place_brick(-truck_id, msg.brick_weight, 2, queue_id);
            sem_op(semaphore_id, (int[]){-msg.brick_weight, -1}, 2);
            space_available = 0;
        }
        else if (did_receive)
        {
            printf(RED "Ciezarowka %d odebrala cegle o masie %d\n" RESET,
                   truck_id, msg.brick_weight);
            char log[50];
            snprintf(log, sizeof(log), "Ciezarowka %d odebrala zostawiona cegle o masie %d", truck_id, msg.brick_weight);
            write_to_file(FILE_NAME_TRUCKS, log);
            // Zwiekszenie aktualnego ladunku
            safe_sleep(TRUCK_LOADING_TIME);
            *current_load += msg.brick_weight;
        }
        sem_op_one_sem(semaphore_id, 1, 2);
        if (did_receive)
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
        if (errno == EINTR) // Przerwanie przez sygnal
        {
            continue; // Powtorz operację
        }
        else
        {
            perror("Nie mozna opuscic semafora");
            exit(EXIT_FAILURE);
        }
    }
}

void sem_op_one_sem(int semid, int val, int sem_num)
{
    struct sembuf operation; // Struktura sembuf

    // Inicjalizacja operacji semaforowej do wykonania
    operation.sem_flg = 0;
    operation.sem_num = sem_num;
    operation.sem_op = val;
    // Proba opuszczenia semafora
    while (semop(semid, &operation, 1) == -1)
    {
        if (errno == EINTR) // Przerwanie przez sygnal
        {
            continue; // Powtorz operację
        }
        else
        {
            perror("Nie mozna opuscic semafora");
            exit(EXIT_FAILURE);
        }
    }
}

void write_to_file(char *file_name, char *message)
{
    // Otwieranie pliku w trybie "append"
    FILE *file = fopen(file_name, "a");
    if (file == NULL)
    {
        // Obsługa błędów otwierania pliku
        fprintf(stderr, "Blad otwierania pliku %s: %s\n", file_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Zapisanie wiadomosci do pliku
    if (fprintf(file, "%s\n", message) < 0)
    {
        // Obsługa błędów podczas zapisu do pliku
        fprintf(stderr, "Blad zapisu do pliku %s: %s\n", file_name, strerror(errno));
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Zamkniecie pliku
    if (fclose(file) != 0)
    {
        // Obsluga bledów zamykania pliku
        fprintf(stderr, "Blad zamykania pliku %s: %s\n", file_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
