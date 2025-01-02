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

#define CONVEYOR_MAX_NUMBER 20
#define CONVEYOR_MAX_LOAD 30
#define NUMBER_OF_TRUCKS 1

extern pid_t workers[3];
extern pid_t trucks[NUMBER_OF_TRUCKS];
extern int message_queue_id;
extern key_t queue_key;
extern char queue_key_string[16];

int create_message_queue(key_t key);
void remove_message_queue(int msgid);
void create_workers();
void create_trucks();