#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#define MAIL_MAX_NUM 3

typedef struct Mail
{
    pid_t sender_pid;//发送者pid
    char message[1000];//最大MAIL_SIZE
}Mail;

typedef struct MailBox
{
    int mail_num;//当前邮箱未读邮件数量
    int rec_index;//第一封未读邮件偏移量,无邮件时为-1
    int snd_index;//下一封邮件写入的偏移量
    struct Mail mails[MAIL_MAX_NUM];
}MailBox;

//创建邮箱成功返回1,失败返回-1
int create_mailbox();

//发送成功返回1,失败返回-1
int send_mail(pid_t pid, struct Mail* mail);

//接收成功返回1,失败返回-1
int receive_mail();

//撤销成功返回1,失败返回-1
int quash_mail();

//销毁成功返回1,失败返回-1
int delete_mailbox();

//返回当前未读邮件
int query_mailbox();
