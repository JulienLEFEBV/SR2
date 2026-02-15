#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

int cpt=0,tube[2], NB, NBS;
time_t t;

void envoi(int s){
    char str1[]="Je suis ton père ...";
    while(write(tube[1],str1,22*sizeof(char))==-1);
    cpt++;
    if (cpt<NB) alarm(NBS);
    else close(tube[1]);
}

void erreur(void)
{
    perror("Erreur ");
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc!=3){
        fprintf(stderr,"Usage: %s [nb secondes] [nb message à envoyé]\n",argv[0]);
        exit(1);
    }
    NBS=atoi(argv[1]);
    NB=atoi(argv[2]);
    if(pipe(tube)==-1) erreur();
    switch(fork())
    {
        case -1: erreur();
        case 0: close(tube[1]);
                char str2[22];
                int i=0;
                while(read(tube[0],str2,22*sizeof(char))!=0){
                    printf("Fils - message reçu de mon père: %s  %d reçu à %s",str2,i,(time(&t), ctime(&t)));
                    i++;
                }
                close(tube[0]);
                printf("Fils - Je me termine à %s",(time(&t), ctime(&t)));
                exit(0);
        default: close(tube[0]);
                 sigset_t mask;
                 sigemptyset(&mask);
                 if(sigprocmask(SIG_SETMASK,&mask,NULL)==-1) erreur();
                 struct sigaction action;
                 action.sa_handler=envoi;
                 action.sa_mask=mask;
                 action.sa_flags=0;
                 if(sigaction(SIGALRM,&action,NULL)==-1) erreur();
                 alarm(NBS);           
    }
    while(cpt!=NB) {
        for(int i=0;i<1000000000;i++);
        printf("Père - Je travaille entre les envois\n");
    }
    while(wait(NULL)==-1) if(errno!=EINTR) erreur();
    printf("Père - Je me termine à %s",(time(&t), ctime(&t)));
    return 0;
}