#pragma ONCE
#define _XOPEN_SOURCE 700
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

#define TRUCK_MAX_LOAD 30    // Maksymalna ladownosc ciezarowki
#define TRUCK_GONE_TIME 30   // Czas rozwozenia cegiel przez ciezarowke
#define TRUCK_LOADING_TIME 1 // Czas potrzebny do zaladowania cegly na ciezarowke

// Kolory do kolorowania tesktu w terminalu
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

extern volatile sig_atomic_t running;
extern int queue_id;
extern int semaphore_id;

// Struktura komunikatu dla kolejki komunikatow
struct message
{
    long mtype;       // Typ komunikatu (0 - normalna cegla, 1 - cegla pozostawiona)
    int worker_id;    // ID pracownika
    int brick_weight; // Masa cegly
};

void place_brick(int id, int mass, int type, int queue_id);
void get_bricks(int truck_id, int queue_id, int semaphore_id, int *current_load, int *are_there_bricks, int *is_interrupted);
void safe_sleep(int seconds);
void sem_op(int semid, int val[], int num_sems);
void sem_op_one_sem(int semid, int val, int sem_num);
void write_to_file(char *file_name, char *message);