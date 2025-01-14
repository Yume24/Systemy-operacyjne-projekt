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

#define TRUCK_MAX_LOAD 30
#define TRUCK_GONE_TIME 30
#define TRUCK_LOADING_TIME 2

extern volatile sig_atomic_t running;
extern int queue_id;
extern int semaphore_id;
extern int semaphore_id_trucks;

struct message
{
    long mtype;
    int worker_id;
    int brick_weight;
};

void place_brick(int id, int mass, int type, int queue_id);
void get_bricks(int truck_id, int queue_id, int semaphore_id,int *current_load, int *are_there_bricks, int *is_interrupted);
void safe_sleep(int seconds);
void sem_op(int semid, int val[], int num_sems);