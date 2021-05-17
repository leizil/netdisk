#include "../include/head.h"
socket_t socket_server(char* ip, char* port)
{
	int fd_server ;
	struct sockaddr_in server_addr ;
	fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_server == -1)
	{
		return -1;
	}
	memset(&server_addr, 0, sizeof(server_addr) );
	server_addr.sin_family = AF_INET ;
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr.s_addr = inet_addr(ip);
	int reuse = 1 ;
	int buf_num = BUF_SIZE;
	if(0 != setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR,  (void*)&reuse, sizeof(int)) )
	{
		close(fd_server);
		return -1;
	}
	if(0 != setsockopt(fd_server, SOL_SOCKET, SO_SNDBUF,  (void*)&buf_num, sizeof(int)) )
	{
		close(fd_server);
		return -1 ;	
	}
	if(0 != setsockopt(fd_server, SOL_SOCKET, SO_RCVBUF,  (void*)&buf_num, sizeof(int)) )
	{
		close(fd_server);
		return -1 ;	
	}

	if(-1 == bind(fd_server, (struct sockaddr*)&server_addr, sizeof(server_addr) ) )
	{
		close(fd_server);
		return -1 ;	
	}
	if(-1 == listen(fd_server, 5))
	{
		close(fd_server);
		return -1 ;
	}
	return fd_server ;

}
socket_t socket_client(char* ip, char* port)
{
	int fd_client ;
	struct sockaddr_in server_addr ;
	fd_client = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_client == -1)
	{
		return -1;
	}
	memset(&server_addr, 0, sizeof(server_addr) );
	server_addr.sin_family = AF_INET ;
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr.s_addr = inet_addr(ip);
	int reuse = 1 ;
	int buf_num = BUF_SIZE;
	if(0 != setsockopt(fd_client, SOL_SOCKET, SO_REUSEADDR,  (void*)&reuse, sizeof(int)) )
	{
		close(fd_client);
		return -1;
	}
	if(0 != setsockopt(fd_client, SOL_SOCKET, SO_SNDBUF,  (void*)&buf_num, sizeof(int)) )
	{
		close(fd_client);
		return -1 ;	
	}
	if(0 != setsockopt(fd_client, SOL_SOCKET, SO_RCVBUF,  (void*)&buf_num, sizeof(int)) )
	{
		close(fd_client);
		return -1 ;	
	}
	if(-1 == connect(fd_client, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)))
	{
		close(fd_client);
		return -1 ;
	}
	return fd_client ;

}
int recvn(socket_t fd_recv,char* recv_buf, int len)
{
	int sum = 0 ;
	int nrecv ;
	while(sum < len)
	{
		nrecv = recv(fd_recv, &recv_buf[ sum], len - sum, 0);
		sum += nrecv ;
	}
	recv_buf[sum] = '\0';
	return sum ;
}
int readn(int fd_read, char* read_buf, int len)
{
	int sum = 0 ;
	int nread ;
	while(sum < len)
	{
		nread = read(fd_read, &read_buf[ sum], len - sum);
		if(nread == 0)
		{
			break ;
		}
		sum += nread ;
	}
	read_buf[sum] = '\0';
	return sum ;
		
}
int sendn(socket_t fd_send, char* send_buf, int len)
{
	int sum = 0 ;
	int nsend ;
	while(sum < len)
	{
		nsend = send(fd_send, send_buf + sum, len - sum, 0);
		sum += nsend ;
	}
	return sum ;
}
int writen(int fd_write, char* write_buf, int len)
{
	int sum = 0 ;
	int nwrite ;
	while(sum < len)
	{
		nwrite = write(fd_write, write_buf + sum, len - sum);
		sum += nwrite ;
	}
	return sum ;

}
void trim_space(char* src)
{
	int index, cur ;
	for(cur = -1, index = 0 ; index < strlen(src); index ++)
	{
		if(src[index] !=' ' && src[index] != '\n')
		{
			src[++cur] = src[index];
			
		}else 
		{
			if(cur != -1 && src[cur] != ' '&& src[cur] !='\n' )
			{
				src[++cur] = ' ';
			}
		}

	}
	for(; cur >= 0 ; cur --)
	{
		if(src[cur] != ' ')
		{
			break ;
		}
	}
	src[++cur] = '\0';
}
int upload(socket_t fd_up, char* file_name)
{
    int filesize=file_size(file_name);
	int fd_file = open(file_name, O_RDONLY);
	if(fd_file == -1)
	{
		return -1 ;
	}
	char *read_buf = (char*)malloc(8 * 1024);
	bzero(read_buf, 8 * 1024);
	int nread ;
    if(filesize<100*1024*1024){
	while(1)
	{
		nread = readn(fd_file, read_buf, 8192);
		if(nread < 8192)
		{
			send(fd_up, &nread, sizeof(int), 0);
			sendn(fd_up, read_buf, nread);
			break ;
		}else
		{
			
			send(fd_up, &nread, sizeof(int), 0);
			sendn(fd_up, read_buf, nread);
		}
	}
	int flag = 0 ;
	send(fd_up, &flag, sizeof(flag), 0);
    }
    else
    {
        char *pMap=(char*)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd_file,0);
        send(fd_up,pMap,filesize,0);
    }
	close(fd_file);
	return 0 ;
}
int download(socket_t fd_down, char* file_name)
{   
    int file_bias=send_file_size(fd_down,file_name);
    if(file_bias!=0){
        printf("had exit!\n");
    }
	int fd_file = open(file_name, O_WRONLY|O_CREAT|O_APPEND,0666 );
	if(fd_file == -1)
	{
		return -1 ;
	}
        char* write_buf = (char*)malloc(8192);
        bzero(write_buf, 8192);
        int nwrite ;
        while(1)
        {
            recv(fd_down, &nwrite, sizeof(int), 0);
            if(nwrite == 0)
            {
                break ;
            }
            recvn(fd_down, write_buf, nwrite);
            writen(fd_file, write_buf, nwrite);
        }
	close(fd_file);
	return 0 ;
}


void InitClient(){
    printf("#########################################\n");
    printf("########### MY CLOUD PAN#################\n");

    printf("1.cd into the dir \n");
    printf("2.ls list file and dir\n");
    printf("3.puts upload\n");
    printf("4.gets download\n");
    printf("5.remove. delete file\n");
    printf("6.pwd show path \n");

}

int file_size(char* filename){
    struct stat statbuf;
    int ret=0;
    ret=stat(filename,&statbuf);
    if(ret!=0){
        return -1;
    }
    return statbuf.st_size;
}

int send_file_size(socket_t fd_down,char * file_name){
    int file_fd=open(file_name,O_RDONLY,0600);
    int ret=0;
    int sendlen=0;
    struct stat statbuf;
    memset(&statbuf,0,sizeof(statbuf));
    if(file_fd==-1){
         statbuf.st_size=0;
    }else{
        ret=stat(file_name,&statbuf);
        if(ret!=0){
            return -1;
        }
    }
    sendlen=(int)statbuf.st_size;
    send(fd_down,&sendlen,sizeof(int),0);
    //ret=sendn(fd_down,(char*)&statbuf.st_size,sendlen);
    if(ret!=-1){
        printf("sent st.size = %ld\n",statbuf.st_size);
    }
    return statbuf.st_size;
}

int recv_file_size(socket_t fd_up,char *file_name){
    int ret=0;
    long int size=0;
    struct stat statbuf;
    ret=stat(file_name,&statbuf);
    if(ret==-1){
        perror("stat");
        return -1;
    }
    ret=recv(fd_up,&size,sizeof(statbuf.st_size),0);
    
    if(ret!=-1){
        printf("recv statbuf.size is %ld\n",statbuf.st_size);
        return size;
    }else{
        perror("recv");
        return -1;
    }
}








