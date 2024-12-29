#include "pracownik_ciezarowka_utils.h"

int link_to_queue(key_t key)
{
    int msgid;

    msgid = msgget(key, IPC_CREAT | 0600);
    if (msgid == -1)
    {
        perror("Blad przy dolaczaniu do kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    printf("Dolaczono do kolejki komunikatow o ID: %d\n", msgid);
    return msgid;
}