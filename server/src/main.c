#include "../include/head.h"
void sig_handle(int signum)
{
	while(waitpid(-1, NULL, WNOHANG) > 0) ;
}
int main(int argc, char* argv[])// conf_file path
{
    
//	int pid ;
	int conn_num = 0 ;
	char log_buf[128];
//	char ip[16] = "" ;
//	char port[10]= "";
	pqnode_t pqnode=NULL;

//	signal(17, sig_handle);
//	ftp_init_from_file(argv[1], ip, port );
   // printf("before socket\n");
	socket_t fd_server = socket_server(argv[1], argv[2]);
   // printf("after socket\n");
	socket_t fd_client ; 
	if(fd_server == -1)
	{
		ERR_EXIT("socket");
	}


    //pUserInfo_t puserinfo=(pUserInfo_t) calloc(1,sizeof(userInfo_t));
   // login_server(puserinfo);


    ppool_t ppool=(ppool_t)calloc(1,sizeof(pool_t));
    threadInit(ppool);
   // printf("线程池初始化完成\n");
    int epfd=epoll_create(1);
    epollAddFd(fd_server,epfd);
    struct epoll_event events[2];
    int readyNum=0;
	while(1)
	{
        readyNum=epoll_wait(epfd,events,2,-1);
        for(int i=0;i<readyNum;++i){
            if(events[i].data.fd==fd_server){
                fd_client = accept(fd_server, NULL, NULL);
                if(fd_client == -1)
                {
                    ERR_EXIT("accept");
                }

                
                bzero(log_buf, 128);
                sprintf(log_buf, "%d connect\n",++conn_num );
                write(1, log_buf, strlen(log_buf));
                
                pqnode=(pqnode_t)calloc(1,sizeof(que_node_t));
                pqnode->session.sess_sfd=fd_client;
                pqnode->clientsfd=fd_client;
               // printf("main:before cond\n");
                pthread_mutex_lock(&ppool->pque->mutex);
                queAdd(ppool->pque,pqnode);
                pthread_cond_signal(&ppool->pque->cond);
                pthread_mutex_unlock(&ppool->pque->mutex);
               // printf("main:after cond\n");
            }
        }
	}

}
