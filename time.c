#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 1;
    }
    const char *name = "TIME_SHM";
    const int SIZE = sizeof(struct timeval);
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    struct timeval *start_time = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    struct timeval end_time;
    pid_t pid = fork();
    if (pid == 0)
    {
        gettimeofday(start_time, NULL);
        execl("/bin/sh", "sh", "-c", argv[1], NULL);
        exit(1);
    }
    else if (pid > 0)
    {
        wait(NULL);
        gettimeofday(&end_time, NULL);
        double elapsed = (end_time.tv_sec - start_time->tv_sec) + (end_time.tv_usec - start_time->tv_usec) / 1000000.0;
        printf("Thoi gian thuc thi: %.5f\n", elapsed);
        munmap(start_time, SIZE);
        close(fd);
        shm_unlink(name);
    }
    return 0;
}