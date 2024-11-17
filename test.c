#include "mail.h"

int main()
{
    create_mailbox();

    char opt[10];
    int rec_pid;
    struct Mail *mail = malloc(sizeof(struct Mail));
    mail->sender_pid = getpid();

    while(1)
    {
        printf("--------------------------------\n");
        printf("PID:%d\n",(int)getpid());
        printf("发送s 接收r 撤回z 查询q 退出e\n");
        printf("输入操作：");
        scanf("%s",opt);
        if(strcmp(opt,"e")==0)
        {
            free(mail);
            delete_mailbox();
            return 0;
        }
        else if(strcmp(opt,"s")==0)
        {
            printf("发送邮件给：");
            scanf("%d",&rec_pid);
            printf("邮件内容：");
            scanf("%s",mail->message);
            send_mail(rec_pid, mail);
        }
        else if(strcmp(opt,"r")==0)
            receive_mail();
        else if(strcmp(opt,"z")==0)
            quash_mail(rec_pid);
        else if(strcmp(opt,"q")==0)
            printf("当前有%d封未读邮件\n",query_mailbox());
        else
            printf("输入有误 请重新输入\n");

    }
    delete_mailbox();
    return 0;
}