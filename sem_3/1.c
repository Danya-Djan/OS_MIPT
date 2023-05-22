#include <unistd.h>
#include <stdio.h>

int main() {

    int pid = getpid();

    int ppid = getppid();

    printf("%d\n%d\n", pid, ppid);
}

// 1. Если возвращается целое значение, то это значение больше нуля. Если меньше нуля - ошибка
// 2. Если указатель == NULL - error
// #include <errno.h>
// int errno;
// void perror(char *str)
// Выдаётся str: инфо об ошибке
// 