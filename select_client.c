/*************************************************************************
	> File Name: select_client.c
	> Author:yang 
	> Mail:yanglongfei@xiyoulinux.org
	> Created Time: 2016年03月05日 星期六 03时45分15秒
 ************************************************************************/

#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<sys/select.h>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>
#include<errno.h>

#define MAXLINE 1024
#define IPADDRESS "127.0.0.1"
#define SERV_PORT  1995
#define max(a,b) (a>b)?a:b
static void handle_recv_msg(int sockfd, char *buf)
{
    printf("client recv msg\n");
    printf(":%s\n",buf);
    sleep(5);
    write(sockfd,buf,strlen(buf)+1);
}
static void handle_connection(int sockfd)
{
    char sendline[MAXLINE],recvline[MAXLINE];
    int maxfdp,stdlineof;
    fd_set readfds;
    int n;
    struct timeval tv;
    int reval = 0;
    while(1){
        FD_ZERO(&readfds);
        FD_SET(sockfd,&readfds);
        maxfdp=sockfd;
        tv.tv_sec=5;
        tv.tv_usec=0;
        reval=select(maxfdp+1,&readfds,NULL,NULL,&tv);
        if(reval ==0){
            printf("client timeout\n");
            continue;
        }
        if(FD_ISSET(sockfd,&readfds)){
            n=read(sockfd,recvline,MAXLINE);
            if(n<=0){
                fprintf(stderr,"client:server is closed\n");
                close(sockfd);
                FD_CLR(sockfd,&readfds);
                return;
            }
            handle_recv_msg(sockfd,recvline);
        }
    }
}
int main(int argc,char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,IPADDRESS,&servaddr.sin_addr);

    int reval=0;
    reval =connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    if(reval <0){
        fprintf(stderr,"connect fail,error:%s\n",strerror(errno));
        return -1;
    }

    printf("client send to server.\n");
    write(sockfd,"hello server",32);
    handle_connection(sockfd);
    return 0;
}
