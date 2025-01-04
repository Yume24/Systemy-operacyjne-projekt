#pragma ONCE
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

#define TRUCK_MAX_LOAD 50
#define TRUCK_GONE_TIME 10
#define TRUCK_LOADING_TIME 3

extern int running;
extern int queue_id;
extern int semaphore_id;

struct message
{
    long mtype;
    int worker_id;
    int brick_weight;
};

void place_brick(int id, int mass, int type, int queue_id);
void get_bricks(int truck_id, int queue_id, int *current_load);
void safe_sleep(int seconds);
void sem_wait(int semid, int val);
void sem_signal(int semid, int val);