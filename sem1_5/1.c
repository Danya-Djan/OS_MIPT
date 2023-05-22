#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>



double EPS = 10e-6;

int main(int argc, char *argv[], char *envp[]) {
    if (argc > 2 || argc < 2) {
        printf("Неверное число аргументов\n");
        return -2;
    }s
    double a = atof(argv[1]);

    if (atof(argv[1]) == 0) {
        printf("Не число\n");
        return -1;
    }

    if (a < 0) {
        printf("Меньше 0\n");
        return -1;
    }

    double x = 1;
    while (fabs(x - (x = (x + a / x) / 2.)) >= EPS) {
    }
    printf("sqrt(a) = %f\n", x);
    return 0;
}