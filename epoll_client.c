/*************************************************************************
	> File Name: epoll_client.c
	> Author:yang 
	> Mail:yanglongfei@xiyoulinux.org
	> Created Time: 2016年03月05日 星期六 17时28分31秒
 ************************************************************************/

#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<sys/epoll.h>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>

#define MAXSIZE 1024
#define IPADDRESS "127.0.0.1"
#define SERV_PORT 1999
#define FDSIZE 1024
#define EPOLIEVENTS 20

static void handle_connection(int sockfd);
static void handle_events(int epollfd,struct epoll_event *events,int fd,int sockfd,char *buf);
static void do_read(int epollfd,int fd,int sockfd,char *buf);
static void do_write(int epollfd,int fd,int sockfd,char *buf); 
static void add_event(int epollfd,int fd,int state);
static void delete_event(int epollfd,int fd,int state);
static void modify_event(int epollfd,int fd,int state);

int main(int argc,char *argv[])
{
    int sockfd;
    struct sockaddr_in serveraddr;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,IPADDRESS,&serveraddr.sin_addr);
    connect(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    handle_connection(sockfd);
    close(sockfd);
    return 0;
}
static void handle_connection(int sockfd)
{
    int epollfd;
    struct epoll_event events[EPOLIEVENTS];
    char buf[MAXSIZE];
    int ret;
    epollfd=epoll_create(FDSIZE);
    add_event(epollfd,STDIN_FILENO,EPOLLIN);
    for(; ;){
        ret = epoll_wait(epollfd,events,EPOLIEVENTS,-1);
        handle_events(epollfd,events,ret,sockfd,buf);
    }
    close(epollfd);
}
static void handle_events(int epollfd,struct epoll_event *events,int num,int sockfd,char *buf)
{
    int fd;
    int i;
    for(i=0;i<num;i++){
        fd=events[i].data.fd;
        if(events[i].events & EPOLLIN){
            do_read(epollfd,fd,sockfd,buf);
        }else if(events[i].events & EPOLLOUT){
            do_write(epollfd,fd,sockfd,buf);
        }
    }
}
static void do_read(int epollfd,int fd,int sockfd,char *buf)
{
    int nread;
    nread=read(fd,buf,MAXSIZE);
    if(nread == -1){
        perror("read error\n");
        close(fd);
    }else if(nread == 0){
        fprintf(stderr,"server close.\n");
        close(fd);
    }else{
        if(fd == STDIN_FILENO){  //输入
            add_event(epollfd,sockfd,EPOLLOUT);
        }else{
            delete_event(epollfd,sockfd,EPOLLIN);
            add_event(epollfd,STDOUT_FILENO,EPOLLOUT);
        }
    }
}
static void do_write(int epollfd,int fd,int sockfd,char *buf)
{
    int  nwrite;
    nwrite=write(fd,buf,strlen(buf));
    if(nwrite == -1){
        perror("write error");
        close(fd);
    }else{
        if(fd == STDOUT_FILENO){
            delete_event(epollfd,fd,EPOLLOUT);
        }else {
            modify_event(epollfd,fd,EPOLLIN);
        }
    }
    memset(buf,0,MAXSIZE);
}
static void add_event(int epollfd,int fd,int state)
{
    struct epoll_event ev;
    ev.events=state;
    ev.data.fd=fd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}
static void delete_event(int epollfd,int fd,int state)
{
    struct epoll_event ev;
    ev.events=state;
    ev.data.fd=fd;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}
static void modify_event(int epollfd,int fd,int state)
{
    struct epoll_event ev;
    ev.events=state;
    ev.data.fd=fd;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}
