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

#define FILE_NAME_WORKERS "logs_workers.txt" // Nazwa pliku dla logow pracownikow
#define FILE_NAME_TRUCKS "logs_trucks.txt"   // Nazwa pliku dla logow ciezarowek

int create_message_queue(key_t key);
int create_semaphore(key_t key, int nsems, int init_val[]);