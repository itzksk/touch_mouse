#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <linux/fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define KEY 12345

int main() {

    int shmid,i=0,j=0,fd=0;
    int cnt=0,ret;
    char *shm_addr = NULL;
    char ch;
    shmid = shmget(KEY,getpagesize(),IPC_CREAT|0666);
    shm_addr = shmat(shmid,NULL,IPC_CREAT|0666);
    printf("shm_addr=0x%x\tshmid=%d\n",shm_addr,shmid);
    fd = open("coordinates.txt",O_RDONLY);
    if (fd < 0) {
        printf("Error in opening coordinates.txt\n");
	exit(1);
    }
    while(1) {
        ret = read(fd,&ch,sizeof(ch));
	if(ret==0) {printf("read file error\n");exit(1);}
    //semlock
	shm_addr[i]=ch;
	printf("%c",shm_addr[i++]);
    //semunlock	
	if(ch == '\n') {
	    cnt++;
	    if(cnt==20) {cnt=0;i=0;}
        }
    }

    
    while(1);
}


