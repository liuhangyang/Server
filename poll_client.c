/*************************************************************************
	> File Name: poll_client.c
	> Author:yang 
	> Mail:yanglongfei@xiyoulinux.org
	> Created Time: 2016年03月05日 星期六 06时06分45秒
 ************************************************************************/

#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<poll.h>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>

#define MAXINE 1024
#define IPADDRESS "127.0.0.1"
#define SERV_PORT 1996
#define max(a,b) (a>b)?a:b
static void handle_connection(int sockfd);
int main(int argc,char *argv[])
{
    int sockfd;
    struct sockaddr_in serveraddr;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,IPADDRESS,&serveraddr.sin_addr);
    connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    //处理连接描述符;
    handle_connection(sockfd);
    return 0;
}
static void handle_connection(int sockfd)
{
    printf("connect\n");
    char sendline[MAXINE],recvline[MAXINE];
    int maxfd,stdlineof;
    struct pollfd pfds[2];
    int n;
    //添加连接描述符
    pfds[0].fd=sockfd;
    pfds[0].events=POLLIN;
    //添加标准输出描述符
    pfds[1].fd=STDIN_FILENO;
    pfds[1].events=POLLIN;
    for(;;){
        poll(pfds,2,-1);
        if(pfds[0].revents & POLLIN){
            n= read(sockfd,recvline,MAXINE);
            if(n ==0){
                fprintf(stderr,"client:server id close\n");
                close(sockfd);
            }
            write(STDIN_FILENO,recvline,n);
        }
        //测试标准输入是否准备好
        if(pfds[1].revents & POLLIN){
            n=read(STDIN_FILENO,sendline,MAXINE);
            if(n==0){
                shutdown(sockfd,SHUT_WR);
                continue;
            }
            write(sockfd,sendline,n);
        }
    }
}
