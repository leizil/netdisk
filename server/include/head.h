#define _XOPEN_SOURCE 
#ifndef __FTP_H__
#define __FTP_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/epoll.h>
#include<pthread.h>
#include<mysql/mysql.h>
#include<sys/mman.h>
#include<openssl/md5.h>
#define BUF_SIZE 8*1024
#define THREAD_SIZE 3
#define USRINFOSIZE 20
#define LOGSIZE 4096
#define PWDSIZE 100
#define SALTSIZE 3
#define STR_LEN 3
#define MD5_SIZE 16
#define MD5_STR_LEN (2*MD5_SIZE)


#define ERR_EXIT(msg) do { \
						perror(msg);\
						exit(1);\
						}while(0)
typedef struct tag_session
{
	int  sess_sfd ;
	char sess_buf[BUF_SIZE] ;

}session_t, *psession_t ;

typedef struct que_node
{
    int clientsfd;
    struct que_node *pnext;
    session_t session;
}que_node_t,*pqnode_t;

typedef struct que{
    pqnode_t phead,ptail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int node_size;
}que_t,*pque_t;

typedef struct pool{
    int thread_size;
    pque_t pque;
    pthread_t *pthid;
}pool_t,*ppool_t;

typedef int socket_t ;

typedef struct userInfo{
    int server_sfd;
    int client_sfd;
    char userName[USRINFOSIZE];
    char userPass[USRINFOSIZE];
    char userLog[LOGSIZE];
    char salt[SALTSIZE];
    char pwd[PWDSIZE];
}userInfo_t,*pUserInfo_t;
int threadInit(ppool_t ppool);
int queGet(pque_t pque,pqnode_t *pqnode);
int queAdd(pque_t pque,pqnode_t pqnode);
int epollAddFd(int fd,int epfd);
void ftp_init_from_file(char* path, char* ip, char* port);
socket_t socket_server(char* ip, char* port);
void client_handle(psession_t arg);
int recvn(socket_t fd_recv, char* recv_buf, int len );
int readn(int fd_read, char* read_buf, int len );
int sendn(socket_t fd_send, char* send_buf, int len );
int writen(int fd_write, char* write_buf, int len );
socket_t socket_client(char* ip, char* port);
void trim_space(char* src);
int upload(socket_t fd_up, char* file_name);
int download(socket_t fd_down, char* file_name);

//database function
int insertUserInfo(char *userName,char *salt,char *userPass);
int select_byusername(char *username,char *salt,char *userPass);
int select_byUserNameForVirtualFile(char *userName,char *md5_sre);
//用户登录系统
int login_client(psession_t ps);
int login_server(psession_t ps);
int generateSalt(char *salt);
int logup_server(char *userName,char *salt,char *userPass);
//virtual file function databasefunction
int insertfileinfo(char * username,char *file_name);
int select_byfilename(char *username,char *file_name);
int get_file_md5(const char *file_name,char *md5_str);



int file_size(char *filename);
int send_file_size(socket_t fd_down,char* file_name);
int recv_file_size(socket_t fd_up,char *file_name);
#endif


