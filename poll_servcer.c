/*************************************************************************
	> File Name: poll_servcer.c
	> Author:yang 
	> Mail:yanglongfei@xiyoulinux.orgmZ
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<poll.h>
#include<unistd.h>
#include<sys/types.h>

#define IPADDESS "127.0.0.1"
#define  PORT 1996
#define MAXLINE 1024
#define  LISTENQ 5
#define OPEN_MAX 1000 
#define INFTIM -1
static int socket_bind(const char *ip,int port);
static void do_poll(int listenfd);
static void handle_connection(struct pollfd *connfds,int num);
int main(int argc,char *argv[])
{
    int listenfd;
    //struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    listenfd=socket_bind(IPADDESS,PORT);
    listen(listenfd,LISTENQ);
    do_poll(listenfd);
    return 0;
}
static int socket_bind(const char *ip,int port)
{
    int listenfd;
    struct sockaddr_in serveraddr;
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd == -1){
        perror("socket error\n");
        exit(1);
    }
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    inet_pton(AF_INET,ip,&serveraddr.sin_addr);
    serveraddr.sin_port=htons(port);
    if(bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))==-1){
        perror("bind error\n");
        exit(1);
    }
    return listenfd;
}
static void do_poll(int listenfd)
{
    printf("do_poll\n");
    int connfd,sockfd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    struct pollfd clientfds[OPEN_MAX];
    int max;
    int i;
    int neardy;
    //添加监听描述符
    clientfds[0].fd=listenfd;
    clientfds[0].events=POLLIN;
    //初始化客户连接描述符
    for(i=1;i<OPEN_MAX;i++){
        clientfds[i].fd=-1;
    }
    max=0;
    for(;;){
        neardy=poll(clientfds,max+1,INFTIM);
        if(neardy == -1){
            perror("poll error\n");
            exit(1);
        }
        //测试监听描述符是否准备好
        if(clientfds[0].revents & POLLIN){
            cliaddrlen=sizeof(cliaddr);
            //接受新的连接
            if((connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen))==-1){
                if(errno == EINTR){
                    continue;
                }else{
                    perror("accept error\n");
                    exit(1);
                }
            
            }
            fprintf(stdout,"accept a new client:%s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
            //将新的连接描述符添加到数组中 
            for(i=1;i<OPEN_MAX;i++){
                if(clientfds[i].fd<0){
                    printf("for\n");
                    clientfds[i].fd=connfd;
                    break;
                }
            }
            printf("connfd\n");
            if(i==OPEN_MAX){
                fprintf(stderr,"too many client\n");
                exit(1);
            }
            //将新的描述符添加到描述符集合中
            clientfds[i].events = POLLIN;
            //记录客户连接套接字的个数
            max=(i>max?i:max);
            if(--neardy <=0){
                continue;
            }
        }
        //处理客户端;
        handle_connection(clientfds,max);
    }
}
static void handle_connection(struct pollfd *connfds,int num)
{
    printf("connection\n");
    int i,n;
    char buf[MAXLINE];
    //buf[MAXLINE]="yang";
    memset(buf,0,MAXLINE);
    for(i=1;i<=num;i++){
        if(connfds[i].fd <0){
            continue;
        }
        //测试客户描述符是否准备好
        if(connfds[i].revents & POLLIN){
            //接受客户端发送的信息
            n=read(connfds[i].fd,buf,MAXLINE);
            if(n==0){
                close(connfds[i].fd);
                connfds[i].fd=-1;
                continue;
            }
            write(STDOUT_FILENO,buf,n);            //向客户端发送buf;
            write(connfds[i].fd,buf,n);
        }
    }
}
