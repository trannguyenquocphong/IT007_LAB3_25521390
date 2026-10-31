#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Chưa nhập số\n");
        return 1;
    }
    char *endptr;
    long n = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || n <= 0)
    {
        printf("Vui lòng nhập số nguyên dương.\n");
        return 1;
    }
    const int SIZE = 4096;
    const char *name = "COLLATZ_SHM";
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    char *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    pid_t pid = fork();
    if (pid == 0)
    {
        int offset = 0;
        while (n != 1)
        {
            offset += sprintf(ptr + offset, "%ld, ", n);
            if (n % 2 == 0)
                n = n / 2;
            else
                n = 3 * n + 1;
        }
        sprintf(ptr + offset, "1");
        munmap(ptr, SIZE);
        close(fd);
        exit(0);
    }
    else if (pid > 0)
    {
        wait(NULL);
        printf("%s\n", ptr);
        munmap(ptr, SIZE);
        close(fd);
        shm_unlink(name);
    }
    return 0;
}