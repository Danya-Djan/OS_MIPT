#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_CYAN "\x1b[36m"

void print_file_type(char *filename) {
    struct stat st;

    if (lstat(filename, &st) != 0) {
        perror("Could not get file status");
        return;
    }

    if (S_ISREG(st.st_mode)) {
        printf(ANSI_COLOR_RESET "-");
    } else if (S_ISDIR(st.st_mode)) {
        printf(ANSI_COLOR_BLUE "d");
    } else if (S_ISCHR(st.st_mode)) {
        printf(ANSI_COLOR_CYAN "c");
    } else if (S_ISBLK(st.st_mode)) {
        printf(ANSI_COLOR_CYAN "b");
    } else if (S_ISFIFO(st.st_mode)) {
        printf(ANSI_COLOR_RESET "p");
    } else if (S_ISLNK(st.st_mode)) {
        printf(ANSI_COLOR_CYAN "l");
    } else if (S_ISSOCK(st.st_mode)) {
        printf(ANSI_COLOR_CYAN "s");
    } else {
        printf(ANSI_COLOR_RESET "?");
    }

    printf(" %s\n", filename);
}

void list_files_in_dir(char *dirname) {
    DIR *dir;
    struct dirent *ent;

    dir = opendir(dirname);
    if (dir == NULL) {
        perror("Could not open directory");
        return;
    }

    printf("Directory %s:\n", dirname);
    while ((ent = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirname, ent->d_name);
        print_file_type(path);
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // no arguments, behave like ls
        list_files_in_dir(".");
    } else if (argc == 2 && strcmp(argv[1], "-l") != 0) {
        // one argument, print file type or target of symlink
        print_file_type(argv[1]);
    } else if (argc == 3 && strcmp(argv[1], "-l") == 0) {
        // two arguments with -l option, print file type and details
        struct stat st;

        if (lstat(argv[2], &st) != 0) {
            perror("Could not get file status");
            return 1;
        }

        printf("File type: ");
        print_file_type(argv[2]);

        printf("File size: %lld bytes\n", (long long) st.st_size);
        printf("Last access time: %s", time(&st.st_atime));
        printf("Last modification time: %s", time(&st.st_mtime));
        printf("Last status change time: %s", time(&st.st_ctime));
    } else if (argc == 2 && strcmp(argv[1], "-l") == 0) {
        // one argument with -l option, print file type and details
        list_files_in_dir(".");
    } else {
        printf("Usage: %s [-l] [file/directory]\n", argv[0]);
        return 1;
    }

    return 0;
}