#include "utils.h"

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

