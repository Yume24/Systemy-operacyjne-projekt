#pragma ONCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define TRUCK_MAX_LOAD 100
#define CONVEYOR_MAX_NUMBER 20
#define CONVEYOR_MAX_LOAD 30
#define NUMBER_OF_TRUCKS 1
#define TRUCK_GONE_TIME 5

extern pid_t workers[3];
extern pid_t trucks[NUMBER_OF_TRUCKS];
extern int message_queue_id;
extern key_t queue_key;

int create_message_queue(key_t key);
void remove_message_queue(int msgid);
void create_workers();
void create_trucks();