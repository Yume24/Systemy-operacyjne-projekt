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

#define CONVEYOR_MAX_NUMBER 20 // Maksymalna ilosc cegiel na tasmie
#define CONVEYOR_MAX_LOAD 30   // Maksymalny udzwig tasmy
#define NUMBER_OF_TRUCKS 3     // Ilosc cizarowek

// Kolory do kolorowania tekstu w terminalu
#define BLUE "\033[94m"
#define RESET "\033[0m"

extern pid_t workers[3];
extern pid_t trucks[NUMBER_OF_TRUCKS];
extern int message_queue_id;
extern int semaphore_id;
extern int worker_semaphore_id;
extern int truck_semaphore_id;
extern key_t queue_key;
extern key_t truck_semaphore_key;
extern key_t worker_semaphore_key;

void remove_message_queue(int msgid);
void remove_semaphore(int semid);
void create_workers(char *queue_key_string, char *semaphore_key_string);
void create_trucks(char *queue_key_string, char *semaphore_key_string, char *truck_semaphore_key_string);
void initialize_log_file(char *file_name);