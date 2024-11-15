#include "mail.h"

int main()
{
    printf("PID:%d\n",(int)getpid());
    create_mailbox();

    int rec_pid;
    char message[1000];
    printf("发送邮件给：");
    scanf("%d",&rec_pid);
    printf("邮件内容：");
    scanf("%s",message);

    struct Mail *mail = malloc(sizeof(struct Mail));
    mail->sender_pid = getpid();
    memcpy(mail->message,message,sizeof(message));

    send_mail(rec_pid, mail);
    return 0;
}