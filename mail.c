#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

#define MAIL_MAX_NUM 10
#define MAX_MAIL_SIZE 1024

typedef struct {
    int pid;
    char content[MAX_MAIL_SIZE];
} Mail;

typedef struct {
    sem_t mutex; // 互斥锁
    sem_t empty; // 空槽位计数器
    sem_t full;  // 满槽位计数器
    Mail mails[MAIL_MAX_NUM];
    int mail_num;
    int rec_index;
    int snd_index;
} MailBox;

// 假设的函数声明
key_t get_shmkey(pid_t pid); // utils
int get_shmid(key_t key, size_t size); // utils
void *get_mailbox(int shmid); // utils

// 创建邮箱
int create_mailbox() {
    key_t shmkey = get_shmkey(getpid());
    int shmid = get_shmid(shmkey, sizeof(struct MailBox));
    printf("进程shmid = %d\n", shmid);
    void *mailbox_addr = get_mailbox(shmid);

    // 初始化mailbox信息
    struct MailBox mailbox = {{0}, MAIL_MAX_NUM, 0, {0}, -1, 0};
    sem_init(&mailbox.mutex, 1, 1); // 初始化互斥锁
    sem_init(&mailbox.empty, 1, MAIL_MAX_NUM); // 初始化空槽位计数器
    sem_init(&mailbox.full, 1, 0); // 初始化满槽位计数器

    // 将mailbox写入共享内存首地址
    memcpy(mailbox_addr, &mailbox, sizeof(mailbox));
    return 1;
}

// 发送邮件
int send_mail(pid_t pid, struct Mail* mail) {
    if (sizeof(*mail) > MAX_MAIL_SIZE) {
        printf("邮件过大:%zu\n", sizeof(*mail));
        return -1;
    }

    key_t shmkey = get_shmkey(pid);
    int shmid = get_shmid(shmkey, sizeof(struct MailBox));
    if (shmid == -1) {
        printf("目标邮箱不存在");
        return -1;
    }
    struct MailBox* rec_mailbox = (struct MailBox*)get_mailbox(shmid);

    sem_wait(&rec_mailbox->empty); // 等待一个空槽位
    sem_wait(&rec_mailbox->mutex); // 加锁

    memcpy(&rec_mailbox->mails[rec_mailbox->snd_index], mail, sizeof(*mail));
    rec_mailbox->mail_num++;
    if (rec_mailbox->rec_index == -1)
        rec_mailbox->rec_index = rec_mailbox->snd_index;

    rec_mailbox->snd_index = (rec_mailbox->snd_index + 1) % MAIL_MAX_NUM;

    sem_post(&rec_mailbox->mutex); // 解锁
    sem_post(&rec_mailbox->full); // 增加一个满槽位

    printf("发送成功\n");
    return 0;
}

// 接收邮件
//接收成功返回1,失败返回-1
int receive_mail() {
    key_t shmkey = get_shmkey(getpid());
    int shmid = get_shmid(shmkey, sizeof(struct MailBox));
    if (shmid == -1) {
        printf("邮箱不存在");
        return -1;
    }
    struct MailBox* my_mailbox = (struct MailBox*)get_mailbox(shmid);

    sem_wait(&my_mailbox->full); // 等待一个满槽位
    sem_wait(&my_mailbox->mutex); // 加锁

    if (my_mailbox->mail_num > 0) {
        struct Mail received_mail = my_mailbox->mails[my_mailbox->rec_index];
        printf("收到邮件，发件人PID：%d，内容：%s\n", received_mail.pid, received_mail.content);
        my_mailbox->mail_num--;
        my_mailbox->rec_index = (my_mailbox->rec_index + 1) % MAIL_MAX_NUM;
    } else {
        printf("邮箱为空\n");
        sem_post(&my_mailbox->mutex); // 解锁
        return -1;
    }

    sem_post(&my_mailbox->mutex); // 解锁
    sem_post(&my_mailbox->empty); // 增加一个空槽位
    return 1;
}

// 撤销邮件
int quash_mail(pid_t pid) {
    key_t shmkey = get_shmkey(pid);
    int shmid = get_shmkey(pid);
    if (shmid == -1) {
        printf("目标邮箱不存在");
        return -1;
    }
    struct MailBox* rec_mailbox = (struct MailBox*)get_mailbox(shmid);

    sem_wait(&rec_mailbox->mutex); // 加锁

    if (rec_mailbox->mail_num > 0) {
        memset(&rec_mailbox->mails[rec_mailbox->snd_index], 0, sizeof(struct Mail));
        rec_mailbox->mail_num--;
        if (rec_mailbox->mail_num == 0)
            rec_mailbox->rec_index = -1;

        rec_mailbox->snd_index = (rec_mailbox->snd_index - 1 + MAIL_MAX_NUM) % MAIL_MAX_NUM;
        sem_post(&rec_mailbox->mutex); // 解锁
        sem_post(&rec_mailbox->empty); // 增加一个空槽位
        printf("撤销成功\n");
        return 0;
    } else {
        printf("对方邮箱为空\n");
        sem_post(&rec_mailbox->mutex); // 解锁
        return -1;
    }
}

// 销毁邮箱
//销毁成功返回1,失败返回-1
int delete_mail_box() {
    key_t shmkey = get_shmkey(getpid());
    int shmid = get_shmid(shmkey, sizeof(struct MailBox));
    if (shmid == -1) {
        printf("邮箱不存在");
        return -1;
    }
    struct MailBox* mailbox = (struct MailBox*)get_mailbox(shmid);

    sem_destroy(&mailbox->mutex); // 销毁互斥锁
    sem_destroy(&mailbox->empty); // 销毁空槽位计数器
    sem_destroy(&mailbox->full);  // 销毁满槽位计数器

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        return -1;
    }
    return 1;
}
