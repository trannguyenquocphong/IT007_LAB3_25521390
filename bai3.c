#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int sig_count = 0;

void on_sigint(int sig)
{
    sig_count++;
    if (sig_count >= 2)
    {
        printf("\ncount.sh has stoppped\n");
        exit(0);
    }
}

int main()
{
    printf("Welcome to IT007, I am 25521390!\n");
    signal(SIGINT, on_sigint);
    pid_t pid = fork();
    if (pid == 0)
    {
        execl("./count.sh", "./count.sh", "120", NULL);
        exit(0);
    }
    else if (pid > 0)
    {
        wait(NULL);
        while (1)
        {
            pause();
        }
    }
    return 0;
}