#include "mail.h"

int main()
{
    printf("PID:%d",(int)getpid());
    create_mailbox();
    int rec_pid;
    printf("发送邮件给：");
    scanf("%d",&rec_pid);
    send_mail(rec_pid);
    return 0;
}