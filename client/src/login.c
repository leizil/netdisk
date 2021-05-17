#include "../include/head.h"


int login_client(pUserInfo_t puserinfo){
    printf("######## WELCOME  CLOUDPAN ##########\n");
    printf("please login \n");
    printf("userName : ");
    scanf("%s",puserinfo->userName);
    send(puserinfo->client_sfd,puserinfo,sizeof(userInfo_t),0);
    printf("login_client: sent userName\n");
    //recv(puserinfo->client_sfd,puserinfo,sizeof(userInfo_t),0);
    printf("login_client: recv salt\n");
    printf("\nuserPass: ");
    scanf("%s",puserinfo->userPass);

    return 0;
}

int judgelog(int fd_client){
    int signsuccess=0;
    recv(fd_client,&signsuccess,sizeof(signsuccess),0);
    if(signsuccess==1){
        printf("WELCOME !\n");
        return 1;
    }else{
        printf("wrong passwd!\n");
        return 0;
    }
}
