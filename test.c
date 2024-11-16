#include "mail.h"

void send()
{
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
}

void quash()
{
    int rec_pid;
    printf("撤销的邮箱：");
    scanf("%d",&rec_pid);
    quash_mail(rec_pid);
}

int main()
{
    printf("PID:%d\n",(int)getpid());
    create_mailbox();

    char opt[10];
    while(1)
    {
        printf("输入操作：");
        scanf("%s",opt);
        if(strcmp(opt,"exit")==0)
        {
            delete_mailbox();
            return 0;
        }
        else if(strcmp(opt,"send")==0)
            send();
        else if(strcmp(opt,"receive")==0)
            receive_mail();
        else if(strcmp(opt,"quash")==0)
            quash();
    }
    delete_mailbox();
    return 0;
}