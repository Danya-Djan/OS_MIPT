#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char folder_path[] = "./fd/";
char file_name[] = ".\0";

int create_links(char* path_to_file, char* file) {
    int counter = 0;
    sprintf(path_to_file, "%s%d.lnk", folder_path, counter);
    while (!symlink(file, path_to_file) && counter < 100) {
        ++counter;
        for (int i = 0; i < 100; ++i) file[i] = '\0';
        sprintf(file, "%d.lnk", counter - 1);

        int fd = open(path_to_file, O_RDONLY);
        if (fd < 0) break;
        close(fd);
        sprintf(path_to_file, "%s%d.lnk", folder_path, counter);
    }
    return counter;
}

int main() {
    char *path_to_file  = (char *)calloc((100), sizeof(char));
    char *file = (char *)calloc((100), sizeof(char));
    memcpy(file, file_name, strlen(file_name));

    int max_depth = create_links(path_to_file, file);
    printf("Max depth: %d\n", max_depth);

    free(path_to_file);
    free(file);
    return 0;
}
