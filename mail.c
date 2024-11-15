#include "mail.h"

//返回邮箱地址
int create_mailbox()
{
    key_t shmkey = get_shmkey(getpid());//utils
    int shmid = get_shmid(shmkey,MAIL_BOX_SIZE);//utils
    void *mailbox_addr = get_mailbox(shmid);//utils
    //初始化mailbox信息
    struct MailBox mailbox = {0, 1024};
    //mailbox写入共享内存首地址
    memcpy(mailbox_addr, &mailbox, sizeof(mailbox));
    return 1;
}

//发送成功返回1,失败返回-1
int send_mail(pid_t pid)
{
    //通过接收进程pid 获取接收进程的邮箱地址
    key_t shmkey = get_shmkey(pid);//utils
    int shmid = get_shmid(shmkey,MAIL_BOX_SIZE);//utils
    struct MailBox* rec_mailbox = get_mailbox(shmid);//utils

    printf("对方未读邮件数%d\n", rec_mailbox->mail_num);

}

//接收成功返回1,失败返回-1
int receive_mail()
{

}

//撤销成功返回1,失败返回-1
int quash_mail()
{

}

//销毁成功返回1,失败返回-1
int delete_mail_box()
{

}