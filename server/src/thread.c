#include "../include/head.h"

void * threadFunc(void *p){
    pque_t pque=(pque_t) p;
        pqnode_t pqnode=NULL;
    int getSucccess=1;
    while(1){
        pthread_mutex_lock(&pque->mutex);
        printf("waiting job\n");
        if(pque->node_size==0){
            pthread_cond_wait(&pque->cond,&pque->mutex);
        }
        printf("threadFunc:I will queGet job node\n");
        getSucccess=queGet(pque,&pqnode);
     //   printf("threadFunc:got job\n");
      //  printf("threadFunc: session sfd is %d buff is %s\n",pqnode->session.sess_sfd,pqnode->session.sess_buf);
        pthread_mutex_unlock(&pque->mutex);
        if(getSucccess==0){
         //   printf("before client_handle\n");
            client_handle(&pqnode->session);
            printf("after client_handle\n");
        }
        
    }
    pthread_exit(0);
}
int threadInit(ppool_t ppool){
    ppool->pque=(pque_t)calloc(1,sizeof(que_t));
    ppool->pque->node_size=0;
    ppool->pque->phead=NULL;
    ppool->pque->ptail=NULL;
    pthread_cond_init(&ppool->pque->cond,NULL);
    pthread_mutex_init(&ppool->pque->mutex,NULL);

    ppool->pthid=(pthread_t*)calloc(THREAD_SIZE,sizeof(pthread_t));

    for(int i=0;i<THREAD_SIZE;++i){
        pthread_create(ppool->pthid+i,NULL,threadFunc,ppool->pque);
     //   printf("thread create successfully!\n");
    }
    return 0;

}

int queGet(pque_t pque,pqnode_t *pqnode){
   // printf("queGet: sizeof(que_node_t)= %ld\n",sizeof(que_node_t));
   // printf("queGet: In queGet\n");
    if(pque->node_size==0){
        printf("queGet:no que node\n");
        return -1;
    }
    //printf("queGet: sizeof(head) is %ld\n",sizeof(*pque->phead));
   // printf("queGet: sizeof(**pqnode) is %ld\n",sizeof(**pqnode));
   // printf("queGet: sizeof(head) is %ld\n",sizeof(pque->phead));
   // printf("queGet: sizeof(*pqnode) is %ld\n",sizeof(*pqnode));
    //printf("queGet: que head clientsfd =%d\n",pque->phead->clientsfd);
    *pqnode=pque->phead ;
   // printf("queGet: got head,**pqnode clientsfd=%d\n",(*pqnode)->clientsfd);
    pque->node_size--;
   // printf("node_size =%d \n",pque->node_size);
    if(pque->node_size==0){
        pque->phead=NULL;
       // printf("head is NULL\n");
        pque->ptail=NULL;
       // printf("tail is NULL\n");
    }else{

        pque->phead=pque->phead->pnext;
       // printf("pque.head =head.pnext\n");
    }
    
   // printf("queGet: pqnode newsfd= %d session buf %s\n",(*pqnode)->session.sess_sfd,(*pqnode)->session.sess_buf);
   // printf("queGet:node_size =%d\n",pque->node_size);
    return 0;
}

int queAdd(pque_t pque,pqnode_t pqnode){
    if(pque->node_size==0){
        pque->phead=pqnode;
        pque->ptail=pqnode;
    }else{
        pque->ptail->pnext=pqnode;
        pque->ptail=pqnode;
    }
    pque->node_size++;
  //  printf("queAdd:pqnode newsfd= %d session buf %s\n",pque->ptail->clientsfd,pque->ptail->session.sess_buf);
   // printf("queAdd:node_size =%d\n",pque->node_size);
    return 0;
}
