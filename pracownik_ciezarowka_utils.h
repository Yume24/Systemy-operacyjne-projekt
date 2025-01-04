#pragma ONCE
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
#include <time.h>

#define TRUCK_MAX_LOAD 100
#define TRUCK_GONE_TIME 8
#define TRUCK_LOADING_TIME 2

extern volatile sig_atomic_t running;
extern int queue_id;

struct message
{
    long mtype;
    int worker_id;
    int brick_weight;
};

int link_to_queue(key_t key);
void place_brick(int id, int mass, int type, int queue_id);
void get_bricks(int truck_id, int queue_id, int *current_load);
void safe_sleep(int seconds);