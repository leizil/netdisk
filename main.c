#include "../include/head.h"
int main(int argc, char* argv[])
{   
    int ret=0;
	if(argc <= 2)
	{
		printf("Usage: exe ,ip ,port\n");
		exit(-1);
	}
	char* ip_server = argv[1] ;
	char* port_server = argv[2];
	char recv_buf[BUF_SIZE], send_buf[BUF_SIZE];
	int recv_len ,send_len ,read_len, write_len;;
	socket_t fd_client = socket_client(ip_server, port_server);
	if(fd_client == -1)
	{
		perror("socket");
		exit(-1);
	}
	char cmd[128];
    pUserInfo_t puserinfo =(pUserInfo_t)calloc(1,sizeof(userInfo_t));
    puserinfo->client_sfd=fd_client;
    //login_client(puserinfo);
    int flag=1;
    
    char username[10]={0};
    char salt[10]={0};
   // char userpass[20]={0};
    char *userpass;
    char cryptpass[1024]={0};
    if(flag==1){
        
        printf("####################################\n");
        printf("username: ");
        scanf("%s",username);
        trim_space(username);
        send_len = strlen(username);
        send(fd_client, &send_len, sizeof(int), 0);
        sendn(fd_client, username, send_len);
        printf("sent %s\n",username);
        recv(fd_client, &recv_len, sizeof(int), 0);
        recvn(fd_client, salt, recv_len );
        printf("recv %s\n",salt);
        /* printf("\nuserpass: "); */
        /* scanf("%s",userpass); */
        userpass=getpass("\nuserpass: ");
        //char *temp=(char*)calloc(1024,1);
        crypt(userpass,salt);
        strcpy(cryptpass,crypt(userpass,salt));
        puts(cryptpass);
        trim_space(cryptpass);
        send_len = strlen(cryptpass);
        send(fd_client, &send_len, sizeof(int), 0);
        sendn(fd_client, cryptpass, send_len);
        trim_space(username);
        send_len = strlen(username);
        send(fd_client, &send_len, sizeof(int), 0);
        sendn(fd_client, username, send_len);
        ret=judgelog(fd_client);
        if(ret==0){
            close(fd_client);
            return 0;
        }else{

            InitClient();
        }
        flag=0;
    }
	while(bzero(cmd, 128),(read_len = read(0, cmd, 128)) > 0 )
	{
        InitClient();
		trim_space(cmd);
		send_len = strlen(cmd);
		send(fd_client, &send_len, sizeof(int), 0);
		sendn(fd_client, cmd, send_len);
		if(strncmp("cd", cmd, 2) == 0)
		{
			system("clear");
			recv(fd_client, &recv_len, sizeof(int), 0);
			recvn(fd_client, recv_buf, recv_len );
			printf("%s\n", recv_buf);
		}else if(strncmp("ls", cmd, 3) == 0)
		{
			system("clear");
			while(1)
			{
				recv(fd_client, &recv_len, sizeof(int), 0);
				if(recv_len == 0)
				{
					break ;
				}
				recvn(fd_client, recv_buf, recv_len );
				printf("%s\n", recv_buf);
			}
		}else if(strncmp("puts", cmd, 4) == 0)
		{
			char file_name[256];
			int file_pos = 5 ;
			while(bzero(file_name, 256),sscanf(cmd + file_pos,"%s", file_name ) == 1)
			{
				file_pos += strlen(file_name) + 1; 
				if(upload(fd_client, file_name) == 0)
				{
					printf(" file-> %s upload success\n", file_name);
				}else 
				{
					printf(" file-> %s upload failed\n", file_name);
				}
				
			}
		}else if( strncmp("gets", cmd, 4)== 0)
		{
				char file_name[256];
				int file_pos = 5 ;
				while(bzero(file_name, 256), sscanf(cmd + file_pos,"%s", file_name) == 1)
				{
					file_pos += strlen(file_name) + 1 ;
					if(download(fd_client,file_name )== 0)
					{
						printf("file -> %s download success \n", file_name);
					}else
					{
					
						printf("file -> %s download failed \n", file_name);
					}
				}
			
		}else if( strncmp("rm", cmd, 2) == 0)
		{
			system("clear");
			recv(fd_client, &recv_len, sizeof(int), 0);
			recvn(fd_client, recv_buf, recv_len );
			printf("%s\n", recv_buf);
			
		}else if(strncmp("pwd", cmd, 3) == 0)
		{
			system("clear");
			recv(fd_client, &recv_len, sizeof(int), 0);
			recvn(fd_client, recv_buf, recv_len );
			printf("%s\n", recv_buf);
		}else 
		{
			printf("valid command, pls input again!\n");
			continue ;
		}
	}
	return 0 ;
}
