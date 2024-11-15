#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>


key_t get_shmkey(pid_t pid);

int get_shmid(key_t shmkey,size_t size);

void* get_mailbox(int shmid);

