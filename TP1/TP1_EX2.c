#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char*argv[])
{
    pid_t pid;
    time_t t;
    pid=getpid();
    if (argc!=3){
        fprintf(stderr,"Nombre d'argument invalide \n");
        exit(1);
    }
    int NBS=atoi(argv[1]),NBF=atoi(argv[2]);
    for(int i=0;i<NBF;i++) 
    {
        printf("Mon numero est %d, il est %s",pid,(time(&t), ctime(&t)));
        sleep(NBS);
    }
}