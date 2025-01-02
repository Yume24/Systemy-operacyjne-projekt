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

struct message
{
    long mtype;
    int worker_id;
    int brick_weight;
};

int link_to_queue(key_t key);
void disconnect_message_queue(int msgid);