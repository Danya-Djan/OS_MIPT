#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double EPS = 10e-6;

int main(int argc, char* argv[], char* env[]) {
    int it = 0;
    while (env[it] && strncmp(env[it], "LILDJAN=", 8)) {
        it++;
    }
    if (!env[it]) {
        printf("Нет переменной\n");
        return -1;
    }
    double a = atof(env[it] + 8);
    if (a < 0) {
        printf("Меньше 0\n");
        return -1;
    }
    if (a == 0) {
        printf("sqrt(a) = 0\n");
        return -1;
    } else if (a == 1) {
        printf("sqrt(a) = 1\n");
        return -1;
    }
    double x = 1;
    double temp = x;
    while (fabs(temp - (x = (x + a / x) / 2.)) >= EPS) {
        temp = x;
    }
    printf("sqrt(a) = %f\n", x);
    return 0;
}