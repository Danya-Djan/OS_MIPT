#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

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
        char link_target[1024];
        ssize_t len = readlink(filename, link_target, sizeof(link_target)-1);
        if (len != -1) {
            link_target[len] = '\0';
            printf(" %s -> %s\n", filename, link_target);
        } else {
            printf(" %s -> [unreadable link target]\n", filename);
        }
        return;
    } else if (S_ISSOCK(st.st_mode)) {
        printf(ANSI_COLOR_CYAN "s");
    } else {
        printf(ANSI_COLOR_RESET "?");
    }

    printf((st.st_mode & S_IRUSR) ? "r" : "-");
    printf((st.st_mode & S_IWUSR) ? "w" : "-");
    printf((st.st_mode & S_IXUSR) ? "x" : "-");
    printf((st.st_mode & S_IRGRP) ? "r" : "-");
    printf((st.st_mode & S_IWGRP) ? "w" : "-");
    printf((st.st_mode & S_IXGRP) ? "x" : "-");
    printf((st.st_mode & S_IROTH) ? "r" : "-");
    printf((st.st_mode & S_IWOTH) ? "w" : "-");
    printf((st.st_mode & S_IXOTH) ? "x" : "-");
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
        // two arguments with -l option, print file
        list_files_in_dir(argv[2]);
    } else {
        fprintf(stderr, "Usage: %s [-l dir] [filename]\n", argv[0]);
        return 1;
    }

    return 0;
}
