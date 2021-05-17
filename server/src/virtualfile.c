#include "../include/head.h"

int insertfileinfo(char *username,char *file_name){
    int ret=0;
    char md5_str[MD5_STR_LEN+1];
    ret=get_file_md5(file_name,md5_str);
    




    return 0;
}


int get_file_md5(const char *file_name,char *md5_str)
{
    int i;
    int fd;
    int ret=0;
    unsigned char data[BUF_SIZE];
    unsigned char md5_value[MD5_SIZE];
    MD5_CTX md5;

    fd=open(file_name,O_RDONLY);

    if(-1==fd){
        perror("open");
        return -1;
    }

    MD5_Init(&md5);
    while(1){
        ret=read(fd,data,BUF_SIZE);
        if(-1==ret){
            perror("read");
            return -1;
        }
        MD5_Update(&md5,data,ret);
        if(0==ret || ret<BUF_SIZE){
            break;

        }

    }
    close(fd);
    MD5_Final(md5_value,&md5);
    for(i=0;i<MD5_SIZE;++i){
        snprintf(md5_str +i*2,2+1,"%02x",md5_value[i]);
    }
    md5_str[MD5_STR_LEN]='\0';
    return 0;
}
