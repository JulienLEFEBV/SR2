#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t pid;
time_t t;

void affichage(int signal)
{
    printf("Ctrl-C/SIGINT reçu par le processus de n° %d\n",pid);
}

void erreur(void)
{
    perror("Erreur ");
    exit(1);
}

int main(int argc, char*argv[])
{
    if (argc!=3){
        fprintf(stderr,"Nombre d'argument invalide \n");
        exit(1);
    }
    sigset_t mask, mask_attente;
    sigfillset(&mask);
    if(sigprocmask(SIG_SETMASK,&mask,NULL)==-1) erreur();
    pid=getpid();
    sigfillset(&mask_attente);
    sigdelset(&mask_attente,SIGINT);
    struct sigaction action;
    action.sa_handler=affichage;
    action.sa_mask=mask;
    action.sa_flags=0;
    if(sigaction(SIGINT,&action,NULL)==-1) erreur();
    if(sigprocmask(SIG_SETMASK,&mask_attente,NULL)==-1) erreur();
    execl("/home/lfj5208a/Sr2/Systeme/TP1/boucler","boucler",argv[1],argv[2],NULL);
    erreur();
    return 1;
}