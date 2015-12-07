/*************************************************************************
	> File Name: Client.c
	> Author:yang 
	> Mail:yanglongfei@xiyoulinux.org
	> Created Time: 2015年12月06日 星期日 23时03分50秒
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <assert.h>
#include <time.h>
int setnonblocking(int fd)      //非阻塞的连接服务器
{
    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);

    return old_option;

}
int main(int argc,char *argv[])
{
    struct sockaddr_in client_addr,my_addr;
    int PORT,PORT1=4096;
    int conn_fd;
    int number;
    char addr[50]={"123."};
    char ad[20];
    if(argc !=3){
       printf("Usage:<服务器IP><端口>\n");
    }
    PORT=atoi(argv[2]);
    bzero(&client_addr,sizeof(struct sockaddr_in));
    client_addr.sin_port = htons(PORT);
    client_addr.sin_family=AF_INET;
    if(inet_aton(argv[1],&client_addr.sin_addr)==0)
    {
        printf("本地IP转换失败！\n");
    }
  while(1){
        int a=3; 
        
        memset(addr,0,50);
        strcpy(addr,"123.");
        bzero(&my_addr,sizeof(struct sockaddr_in));
        my_addr.sin_family=AF_INET;

        my_addr.sin_port = htons(PORT1);
        if((conn_fd=socket(AF_INET,SOCK_STREAM,0))<0)
        {
            printf("打开失败\n");
        }
        setnonblocking(conn_fd);
        while(a>0){
            number=rand()%255;
            sprintf(ad,"%d",number);
            if(a!=1){
                strcat(ad,".");
            }
            strcat(addr,ad);
            a--;
        }
        printf("%s\n",addr);
        inet_aton(addr,&my_addr.sin_addr);
         bind(conn_fd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr_in));
        if((connect(conn_fd,(struct sockaddr *)&client_addr,sizeof(struct sockaddr)))<0)
        {
            printf("连接失败\n");
        }
        close(conn_fd);
    }
    return 0;
}
