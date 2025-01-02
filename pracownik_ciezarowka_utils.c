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

void disconnect_message_queue(int msgid)
{
    // Usunięcie kolejki komunikatów
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("Blad podczas usuwania kolejki komunikatow");
        exit(EXIT_FAILURE);
    }

    printf("Odlaczono od kolejki o id %d\n", msgid);
}