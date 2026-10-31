#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

int main()
{
    const int SIZE = 10;
    const char *name = "PC_BUFFER";
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    char *buffer = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memset(buffer, 0, SIZE);
    pid_t pid = fork();
    if (pid > 0)
    {
        srand(time(NULL));
        int in = 0;
        while (1)
        {
            while (buffer[in] != 0 && buffer[in] != -1)
            {
                usleep(10000);
            }
            if (buffer[in] == -1)
            {
                break;
            }
            int num = rand() % 11 + 10;
            buffer[in] = (char)num;
            printf("Producer wrote %d at index %d\n", num, in);
            fflush(stdout);
            in = (in + 1) % SIZE;
            usleep(100000);
        }
        wait(NULL);
        munmap(buffer, SIZE);
        close(fd);
        shm_unlink(name);
    }
    else if (pid == 0)
    {
        int out = 0;
        int sum = 0;
        while (1)
        {
            while (buffer[out] == 0)
            {
                usleep(10000);
            }
            int val = buffer[out];
            buffer[out] = 0;
            sum += val;
            printf("Consumer read %d at index %d | Current sum = %d\n", val, out, sum);
            fflush(stdout);
            out = (out + 1) % SIZE;
            if (sum > 100)
            {
                memset(buffer, -1, SIZE);
                break;
            }
            usleep(150000);
        }
        munmap(buffer, SIZE);
        close(fd);
        exit(0);
    }
    return 0;
}