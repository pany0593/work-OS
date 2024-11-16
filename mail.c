#include "mail.h"

//返回邮箱地址
int create_mailbox()
{
    key_t shmkey = get_shmkey(getpid());//utils
    int shmid = get_shmid(shmkey,(size_t)sizeof(struct MailBox));//utils
//    printf("进程shmid = %d\n", shmid);
    void *mailbox_addr = get_mailbox(shmid);//utils
    //初始化mailbox信息
    struct MailBox mailbox = {0, -1, 1};
    //mailbox写入共享内存首地址
    memcpy(mailbox_addr, &mailbox, sizeof(mailbox));
    return 1;
}

//发送成功返回1,失败返回-1
int send_mail(pid_t pid, struct Mail* mail)
{
    if(sizeof(*mail) > 1024)
    {
        printf("邮件过大:%llu\n",sizeof(*mail));
        return -1;
    }

    //通过接收进程pid 获取接收进程的邮箱地址
    key_t shmkey = get_shmkey(pid);//utils
    int shmid = get_shmid(shmkey,(size_t)sizeof(struct MailBox));//utils
    if(shmid == -1)
    {
        printf("目标邮箱不存在");
        return -1;
    }
    struct MailBox* rec_mailbox = (struct MailBox*)get_mailbox(shmid);//utils
    if(rec_mailbox->mail_num >= MAIL_MAX_NUM)
    {
        printf("对方邮箱已满\n");
        return -1;
    }

    memcpy(&rec_mailbox->mails[rec_mailbox->snd_index], mail, sizeof(*mail));//将邮件写入对应位置

    rec_mailbox->mail_num++;//未读邮件+1


    if(rec_mailbox->rec_index == -1)//初始情况
        rec_mailbox->rec_index = rec_mailbox->snd_index;

    rec_mailbox->snd_index++;//写入位置+1
    if(rec_mailbox->snd_index == MAIL_MAX_NUM)
        rec_mailbox->snd_index = 0;

    printf("发送成功\n");
    return 0;
}

//接收成功返回1,失败返回-1
int receive_mail()
{
    key_t shmkey = get_shmkey(getpid()); // utils
    int shmid = get_shmid(shmkey, (size_t)sizeof(struct MailBox)); // utils
    if(shmid == -1)
    {
        printf("邮箱不存在");
        return -1; // 如果共享内存ID无效，说明邮箱不存在
    }
    // 获取当前进程的邮箱地址（共享内存中的邮箱结构体）
    struct MailBox* mailbox = (struct MailBox*)get_mailbox(shmid); // utils

    // 如果邮箱为空，没有邮件可以接收
    if(mailbox->mail_num == 0)
    {
        printf("邮箱为空，没有邮件可以接收\n");
        return -1;
    }

    // 获取当前接收到的邮件，按接收索引读取邮件
    struct Mail *mail = malloc(sizeof(struct Mail));
    memcpy(mail, &mailbox->mails[mailbox->rec_index], sizeof(struct Mail));

    printf("message:%s\n",mail->message);

    mailbox->rec_index++;

    if(mailbox->rec_index == MAIL_MAX_NUM)
        mailbox->rec_index = 0;

    // 减少邮箱中的邮件数量
    mailbox->mail_num--;

    if(mailbox->mail_num == 0)
        mailbox->rec_index = -1;

    printf("接收邮件成功\n");
    return 1;
}


//撤销最近发送的一封邮件
//撤销成功返回1,失败返回-1
int quash_mail(pid_t pid)
{
    //通过接收进程pid 获取接收进程的邮箱地址
    key_t shmkey = get_shmkey(pid);//utils
    int shmid = get_shmid(shmkey,(size_t)sizeof(struct MailBox));//utils
    if(shmid == -1)
    {
        printf("目标邮箱不存在");
        return -1;
    }
    struct MailBox* rec_mailbox = (struct MailBox*)get_mailbox(shmid);//utils
    if(rec_mailbox->mail_num == 0)
    {
        printf("对方邮箱为空\n");
        return -1;
    }

    memset(&rec_mailbox->mails[rec_mailbox->snd_index], 0, sizeof(struct Mail));//将邮件对应位置写空

    rec_mailbox->mail_num --;//未读邮件-1
    if(rec_mailbox->mail_num == 0)
        rec_mailbox->rec_index = -1;

    rec_mailbox->snd_index --;//写入位置-1
    if(rec_mailbox->snd_index == -1)
        rec_mailbox->snd_index = MAIL_MAX_NUM-1;

    printf("撤销成功\n");
    return 0;
}

//销毁成功返回1,失败返回-1
int delete_mailbox()
{
    key_t shmkey = get_shmkey(getpid());//utils
    int shmid = get_shmid(shmkey,(size_t)sizeof(struct MailBox));//utils
//    printf("进程shmid = %d\n", shmid);
    void *mailbox_addr = get_mailbox(shmid);//utils
    shmdt(mailbox_addr);
    if(shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        return -1;
    }
    printf("销毁成功\n");
    return 1;
}