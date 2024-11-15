#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#define MAIL_SIZE 1024
#define MAIL_MAX_NUM 3
#define MAIL_BOX_SIZE MAIL_SIZE*(MAIL_MAX_NUM+1)
//4096刚好一页

typedef struct Mail
{
    pid_t sender_pid;//发送者pid
    char* message;//最大MAIL_SIZE
    int size;//邮件大小
};
//mail_box信息存在共享内存首地址开始，不超过1024
//第1封邮件在1024地址开始
typedef struct MailBox
{
    int mail_num;//当前邮箱未读邮件数量
    int index;//第一封未读邮件偏移地址
};

//创建邮箱成功返回1,失败返回-1
int create_mailbox();

//发送成功返回1,失败返回-1
int send_mail(pid_t pid);

//接收成功返回1,失败返回-1
int receive_mail();

//撤销成功返回1,失败返回-1
int quash_mail();

//销毁成功返回1,失败返回-1
int delete_mailbox();


