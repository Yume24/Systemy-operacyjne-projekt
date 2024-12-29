#pragma ONCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>


struct message
{
    long mtype;
    int worker_id;
    int brick_weight;
};

int link_to_queue(key_t key);
