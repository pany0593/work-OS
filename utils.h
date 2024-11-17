#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

int get_semaphore(key_t key);

int init_semaphore(key_t key);

int P_operation(key_t shmkey);

int V_operation(key_t shmkey);

key_t get_shmkey(pid_t pid);

int get_shmid(key_t shmkey,size_t size);

void* get_mailbox(int shmid);

