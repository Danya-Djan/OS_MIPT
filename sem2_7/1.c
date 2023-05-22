#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char path[] = "./1/";
char full_name[] = ".\0";

int main() {
    char *path_to_file  = (char *)calloc((100), sizeof(char));
    char *filename = (char *)calloc((100), sizeof(char));
    memcpy(filename, full_name, strlen(full_name));

    int counter = 0;
    sprintf(path_to_file, "%s%d.lnk", path, counter);
    while (!symlink(filename, path_to_file) && counter < 100) {
        ++counter;
        for (int i = 0; i < 100; ++i) filename[i] = '\0';
        sprintf(filename, "%d.lnk", counter - 1);

        int file = open(path_to_file, O_RDONLY);
        if (file < 0) break;
        close(file);
        sprintf(path_to_file, "%s%d.lnk", path, counter);
    }
    printf("Max depth is %d\n", counter);
    free(path_to_file);
    free(filename);
    return 0;
}