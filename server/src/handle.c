#include "../include/head.h"

static char* file_type(mode_t md)
{
	if(S_ISREG(md))
	{
		return "-";	
	}else if(S_ISDIR(md))
	{
		return "d";
	}else if(S_ISFIFO(md))
	{
		return "p";
	}else 
	{
		return "o" ;
	}
}
static void do_ls(psession_t ps) 
{
	DIR* pdir = opendir("./");
	if(pdir == NULL)
	{
		int flag = -1 ;
		send(ps ->sess_sfd, &flag, sizeof(int), 0);
	}else
	{
		struct dirent* mydir ;
		int len ;
		while( (mydir = readdir(pdir)) != NULL)
		{
			if(strncmp(mydir->d_name, ".", 1) == 0 || strncmp(mydir->d_name,"..", 2) == 0)
			{
				continue ;
			}
			struct stat mystat;
			bzero(&mystat, sizeof(mystat));
			stat(mydir->d_name, &mystat);
			bzero(ps ->sess_buf, 1024);
			sprintf(ps ->sess_buf, "%-2s%-20s %10ldB", file_type(mystat.st_mode),mydir->d_name, mystat.st_size );
			len =  strlen(ps ->sess_buf);
			send(ps ->sess_sfd, &len, sizeof(int), 0);
			sendn(ps ->sess_sfd, ps ->sess_buf, len);
		}
		len = 0 ;
		send(ps ->sess_sfd, &len, sizeof(int), 0);
	}

}
static void do_cd(psession_t ps) 
{
	char dir[128]= "";
	sscanf(ps ->sess_buf +3, "%s", dir);
	chdir(dir);
	getcwd(dir, 128);
	int len = strlen(dir);
	send(ps ->sess_sfd, &len, sizeof(int), 0);
	sendn(ps ->sess_sfd, dir, strlen(dir));
}
static void do_pwd(psession_t ps) 
{
	bzero(ps ->sess_buf, BUF_SIZE);
	getcwd(ps ->sess_buf, BUF_SIZE);
	int len = strlen(ps ->sess_buf);
	send(ps ->sess_sfd, &len, sizeof(int), 0);
	sendn(ps ->sess_sfd, ps ->sess_buf, len);

}
static void do_gets(psession_t ps) 
{
	char file_name[256];
	int file_pos = 5 ;
	while(bzero(file_name, 256),sscanf(ps ->sess_buf + file_pos,"%s", file_name ) == 1)
	{
		file_pos += strlen(file_name) + 1; 
		if(upload(ps ->sess_sfd, file_name) == 0)
		{
			printf(" file-> %s upload success\n", file_name);
		}else 
		{
			printf(" file-> %s upload failed\n", file_name);
		}

	}

}
static void do_puts(psession_t ps) //puts a.txt b.txt
{
	char file_name[256];
	int file_pos = 5 ;
	while(bzero(file_name, 256), sscanf(ps ->sess_buf + file_pos,"%s", file_name) == 1)
	{
		file_pos += strlen(file_name) + 1 ;
		if(download(ps -> sess_sfd, file_name) == 0)
		{
			printf("file -> %s download success \n", file_name);
		}else
		{

			printf("file -> %s download failed \n", file_name);
		}
	}
}
static void do_remove(psession_t ps)// remove file 
{
	char cmd[256] ="" ;
	sprintf(cmd, "rm -f %s", ps ->sess_buf + 3);
	system(cmd);
	sprintf(ps ->sess_buf, "%s removed", ps ->sess_buf + 3);
	int len = strlen(ps ->sess_buf);
	send(ps ->sess_sfd, &len, sizeof(int),0);
	sendn(ps ->sess_sfd, ps ->sess_buf, len);
	bzero(ps ->sess_buf, BUF_SIZE);
}
void client_handle(psession_t ps)
{
    printf("%d\n",ps->sess_sfd);
    printf("%s\n",ps->sess_buf);
	socket_t fd_client = ps -> sess_sfd ;
    printf("got sess_sfd\n");
	int cmd_len = 0 ;
	int recv_ret ;
    int flag=1;
    int logfd=open("log.txt",O_RDWR|O_CREAT|O_APPEND,0600);
   // pUserInfo_t puserinfo=(pUserInfo_t)calloc(1,sizeof(userInfo_t));
   // puserinfo->server_sfd=ps->sess_sfd;
   // login_server(puserinfo);
    time_t timenow;
    time(&timenow);
	while(1)
	{   
        char buf[1024]={0};
        if(flag==1){
            login_server(ps);
            memset(buf,0,sizeof(buf));
            flag=0;
            sprintf(buf,"user signin time %s\n",ctime(&timenow));
            write(logfd,buf,strlen(buf));
        } 
		bzero(ps -> sess_buf, BUF_SIZE);
		recv_ret = recv(fd_client, &cmd_len, sizeof(int),0);
		if(cmd_len == 0 || recv_ret == 0)
		{
			printf("client exit !\n");
			close(ps ->sess_sfd);
			//free(ps);
            memset(buf,0,sizeof(buf));
            sprintf(buf,"  user exit %s\n",ctime(&timenow));
            write(logfd,buf,strlen(buf));
			return;
		}
		recvn(fd_client, ps->sess_buf, cmd_len);
		if(strncmp("cd", ps ->sess_buf, 2) == 0)
		{
            memset(buf,0,sizeof(buf));
            sprintf(buf,"command: cd  %s\n",ctime(&timenow));
            write(logfd,buf,strlen(buf));
			do_cd(ps);
		}else if(strncmp("ls", ps ->sess_buf, 2) == 0)
		{
            memset(buf,0,sizeof(buf));
            sprintf(buf,"command: ls  %s\n",ctime(&timenow));
            write(logfd,buf,strlen(buf));
			do_ls(ps);
		}else if( strncmp("puts", ps ->sess_buf, 4)== 0)
		{
            memset(buf,0,sizeof(buf));
            sprintf(buf,"command: puts  %s\n",ctime(&timenow));
            write(logfd,buf,strlen(buf));
			do_puts(ps);
		}else if( strncmp("gets", ps ->sess_buf, 4)== 0)
		{
            memset(buf,0,sizeof(buf));
            sprintf(buf,"command: gets  %s\n",ctime(&timenow));
            write(logfd,buf,strlen(buf));
			do_gets(ps);

		}else if( strncmp("rm", ps ->sess_buf, 2)== 0)
		{
            memset(buf,0,sizeof(buf));
            sprintf(buf,"command: rm  %s\n",ctime(&timenow));
            write(logfd,buf,strlen(buf));
			do_remove(ps);

		}else if(strncmp("pwd", ps ->sess_buf, 3) == 0) 
		{
            memset(buf,0,sizeof(buf));
            sprintf(buf,"command: pwd  %s\n",ctime(&timenow));
            write(logfd,buf,strlen(buf));
			do_pwd(ps);

		}else 
		{
			continue ;
		}


	}
}
