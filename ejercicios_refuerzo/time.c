#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    struct timeval current_time; //guarda dos valores de
    gettimeofday(&current_time, NULL);
    int second1 = current_time.tv_sec; //sacar los segundos
    int second2;
    int microsec1 = current_time.tv_usec;//sacar los micro segundos
    int microsec2;

    int rc = fork();
    if (rc < 0){
        //fork failed
        exit(1);
    }else if (rc == 0){
        //process child
        //printf("hello, I am child (pid:%d)\n", (int) getpid());
        system(argv[1]);

    } else {
        wait(NULL);
        //printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
        gettimeofday(&current_time, NULL);
        second2 = current_time.tv_sec;
        microsec2 = current_time.tv_usec;
        int secondFinal = second2 - second1;
        int microsecFinal = microsec2 - microsec1;
        printf("Elapsed time: %i,%i\n", secondFinal, microsecFinal);
    }
    return 0;
}