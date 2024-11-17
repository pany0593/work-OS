#include "utils.h"

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}sem_union;

int get_semaphore(key_t key)
{
    int semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        return -1;
    }
    return semid;
}

int init_semaphore(key_t key)
{
    int semid = get_semaphore(key);
    if (semid == -1)
    {
        printf("get_semaphore erorr\n");
        return -1;
    }
    // 设置信号量初始值为 1（表示互斥锁）
    sem_union.val = 1;
    if (semctl(semid, 0, SETVAL, sem_union) == -1) {
        perror("操作失败");
        return -1;
    }
    return 0;
}

// P操作 (等待信号量)
int P_operation(key_t shmkey)
{
    int semid = get_semaphore(shmkey);
    if (semid == -1)
    {
        printf("get_semaphore erorr\n");
        return -1;
    }

    struct sembuf sop;
    sop.sem_num = 0; // 操作第0号信号量
    sop.sem_op = -1; // P操作，信号量减1
    sop.sem_flg = 0;

    if (semop(semid, &sop, 1) == -1) {
        perror("操作P失败");
        return -1;
    }
    return 1;
}

// V操作 (释放信号量)
int V_operation(key_t shmkey)
{
    int semid = get_semaphore(shmkey);
    if (semid == -1)
    {
        printf("get_semaphore erorr\n");
        return -1;
    }

    struct sembuf sop;
    sop.sem_num = 0; // 操作第0号信号量
    sop.sem_op = 1;  // V操作，信号量加1
    sop.sem_flg = 0;

    if (semop(semid, &sop, 1) == -1) {
        perror("操作V失败");
        return -1;
    }
    return 0;
}


key_t get_shmkey(pid_t pid)
{
    key_t shmkey;
    //创建shmkey,使用pid传入ftok()生成
    //知道对方进程的pid即可通过相同方法生成shmkey
    if((shmkey = ftok(".", pid)) == -1){
        perror("ftok error!\n");
        exit(1);
    }
    return shmkey;
}

int get_shmid(key_t shmkey,size_t size)
{
    int shmid;
    //创建共享内存shm,返回shm标识符sid
    if((shmid = shmget(shmkey, size, IPC_CREAT|0666)) == -1){
        perror("shm call failed!\n");
        return -1;
    }
    return shmid;
}

void* get_mailbox(int shmid)
{
    void *mailbox_addr = shmat(shmid, (char*)0, 0);
    if(mailbox_addr == (void *)-1)
    {
        perror("attch shared memory error!\n");
        exit(1);
    }
    return mailbox_addr;
}

