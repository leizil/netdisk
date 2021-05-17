#include "../include/head.h"


/* int login_client(pUserInfo_t puserinfo){ */
/*     printf("######## WELCOME  CLOUDPAN ##########\n"); */
/*     printf("please login \n"); */
/*     printf("userName : "); */
/*     scanf("%s",puserinfo->userName); */
/*     send(puserinfo->client_sfd,puserinfo,sizeof(userInfo_t),0); */
/*     printf("login_client: sent userName\n"); */
/*     recv(puserinfo->client_sfd,puserinfo,sizeof(userInfo_t),0); */
/*     printf("login_client: recv salt\n"); */
/*     printf("\nuserPass: "); */
/*     scanf("%s",puserinfo->userPass); */

/*     return 0; */
/* } */

int login_server(psession_t ps){
   // puts(puserinfo->salt);
    int recv_len=0;
    int send_len=0;
    int ret=0;
    char userName[10]={0};
    char userPass_client[40]={0};
    char userPass_real[40]={0};
    char salt[10]={0};
    recv(ps->sess_sfd,&recv_len,sizeof(int),0);
    recvn(ps->sess_sfd,userName,recv_len);
    printf("login_server: recv userName %s\n",userName);
    //findusername();
    ret=select_byusername(userName,salt,userPass_real);
    if(ret==0){
        printf("no userName\n please signup!\n");
        generateSalt(salt);
        puts(salt);
        trim_space(salt);
        send_len=strlen(salt);
        send(ps->sess_sfd,&send_len,sizeof(int),0);
        ret=sendn(ps->sess_sfd,salt,send_len);
        if(ret!=-1){printf("login_server: sent salt %s\n",salt);}
        recv(ps->sess_sfd,&recv_len,sizeof(int),0);
        recvn(ps->sess_sfd,userPass_real,recv_len);
        printf("login_server: recv passwd\n");
        puts(userPass_real);
        logup_server(userName,salt,userPass_real);
    }else{
        trim_space(salt);
        send_len=strlen(salt);
        send(ps->sess_sfd,&send_len,sizeof(int),0);
        ret=sendn(ps->sess_sfd,salt,send_len);
        if(ret!=-1){printf("login_server: sent salt %s\n",salt);}
        recv(ps->sess_sfd,&recv_len,sizeof(int),0);
        recvn(ps->sess_sfd,userPass_client,recv_len);
        printf("login_server: recv passwd\n");
        puts(userPass_client);
        int signsuccess=0;
        if(strcmp(userPass_client,userPass_real)==0){
            printf("sign in success!\n");
            signsuccess=1;
            send(ps->sess_sfd,&signsuccess,sizeof(signsuccess),0);

        }else{
            printf("wrong password!\n");

            signsuccess=0;
            send(ps->sess_sfd,&signsuccess,sizeof(signsuccess),0);
            
            return -1;
        }
    }
    return 0;
}
int logup_server (char *userName,char *salt,char*userPass){
    int ret=insertUserInfo(userName,salt,userPass);
    if(ret!=-1){
        printf("sigup success!\n");
    }else{
        return -1;
    }


    return 0;
}



int generateSalt(char *salt){
    int i,flag;
    srand(time(NULL));
    for(i=0;i<STR_LEN;++i){
        flag=rand()%3;
        switch(flag){
            case 0:salt[i]=rand()%26 +'a';break;
            case 1: salt[i]=rand()%26 +'A';break;
            case 2:salt[i]=rand()%10 +'0';break;
        }
    }
   // printf("generateSalt: salt is %s\n",puserinfo->salt);
    return 0;

}
