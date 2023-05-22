#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

void print_file_type(char *filename) {
    struct stat st;

    if (lstat(filename, &st) != 0) {
        perror("Could not get file status");
        return;
    }

    if (S_ISREG(st.st_mode)) {
        printf("%s: regular file\n", filename);
    } else if (S_ISDIR(st.st_mode)) {
        printf("%s: directory\n", filename);
    } else if (S_ISCHR(st.st_mode)) {
        printf("%s: character device\n", filename);
    } else if (S_ISBLK(st.st_mode)) {
        printf("%s: block device\n", filename);
    } else if (S_ISFIFO(st.st_mode)) {
        printf("%s: named pipe (FIFO)\n", filename);
    } else if (S_ISLNK(st.st_mode)) {
        char buf[1024];
        ssize_t len = readlink(filename, buf, sizeof(buf)-1);
        if (len != -1) {
            buf[len] = '\0';
            printf("%s -> %s: symbolic link\n", filename, buf);
        } else {
            printf("%s: invalid symbolic link\n", filename);
        }
    } else if (S_ISSOCK(st.st_mode)) {
        printf("%s: socket\n", filename);
    } else {
        printf("%s: unknown file type\n", filename);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // no arguments, behave like ls
        DIR *dir;
        struct dirent *ent;

        dir = opendir(".");
        if (dir == NULL) {
            perror("Could not open directory");
            return 1;
        }

        while ((ent = readdir(dir)) != NULL) {
            print_file_type(ent->d_name);
        }

        closedir(dir);
    } else {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        return 1;
    }

    return 0;
}
