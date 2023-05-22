#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <string.h>

#define PRINT_TYPE 0

void print_file_name(char * file_name, mode_t st_mode)
{
	int is_exec = 0;
	if (st_mode & S_IXUSR || st_mode & S_IXGRP || st_mode & S_IXOTH) is_exec = 1;
	
	switch (st_mode & S_IFMT)
	{
		case S_IFREG:
		{
			if (PRINT_TYPE) printf("[--regular-file--] ");

			if (is_exec) printf("\033[1;32m");

			break;
		}
		case S_IFDIR:
		{
			if (PRINT_TYPE) printf("[---directory----] ");
			
			printf("\033[1;34m");

			break;
		}
		case S_IFLNK:
		{
			if (PRINT_TYPE) printf("[-symbolic-link--] ");
			
			printf("\033[1;36m");
			
			break;
		}
		case S_IFBLK:
		{
			if (PRINT_TYPE) printf("[--block-device--] ");
		       
			printf("\033[1;30;33m");
		
			break;
		}
		case S_IFSOCK:
		{
			if (PRINT_TYPE) printf("[-----socket-----] ");
			
			printf("\033[1;35m");

			break;
		}
		case S_IFCHR:
		{
			if (PRINT_TYPE) printf("[character-device] ");
			
			printf("\033[1;30;33m");

			break;
		}
		case S_IFIFO:
		{
			if (PRINT_TYPE) printf("[------FIFO------] ");
			
			printf("\033[0;30;33m");

			break;
		}
		default:
		{
			printf("\nError: Unexpected file type\n");
			exit(EXIT_FAILURE);
		}
	}

	printf("%s\033[m", file_name);
}

int main(int argc, char * argv[])
{
	char * dir_name;

	if (argc == 1)
	{
		dir_name = ".";
	}
	else if (argc == 2)
	{
		dir_name = argv[1];
	}
	else
	{
		printf("Error: Invalid input\n");
		exit(EXIT_FAILURE);
	}	

	DIR * dir;	
	if ((dir = opendir(dir_name)) == NULL)
	{
		printf("Error: Unable to open directory\n");
		exit(EXIT_FAILURE);
	}

	struct stat st;
	char file_name[512] = {};

	struct stat link_st;
	char * link_name;

	struct passwd * pws;
	struct group * gr;
	struct tm t;

	blkcnt_t blocks = 0;

	struct dirent * de;

	while ((de = readdir(dir)) != NULL)
	{
		sprintf(file_name, "%s/%s", dir_name, de->d_name);

		if (lstat(file_name, &st) < 0)
		{
			printf("Error: Unable to lstat file\n");
			exit(EXIT_FAILURE);
		}
		
		printf((st.st_mode & S_IRUSR) ? "r"  : "-" );
		printf((st.st_mode & S_IWUSR) ? "w"  : "-" );
		printf((st.st_mode & S_IXUSR) ? "x"  : "-" );
		printf((st.st_mode & S_IRGRP) ? "r"  : "-" );
		printf((st.st_mode & S_IWGRP) ? "w"  : "-" );
		printf((st.st_mode & S_IXGRP) ? "x"  : "-" );
		printf((st.st_mode & S_IROTH) ? "r"  : "-" );
		printf((st.st_mode & S_IWOTH) ? "w"  : "-" );
		printf((st.st_mode & S_IXOTH) ? "x " : "- ");

		printf("%4d ",  st.st_nlink);
		
		pws = getpwuid(st.st_uid);
		printf("%10s ",  pws->pw_name);
		
		gr = getgrgid(st.st_gid);
		printf("%10s ",  gr->gr_name);
		
		printf("%6lld ", st.st_size);

		blocks += st.st_blocks;
		
		t = *localtime(&st.st_mtime);
		printf("%d-%02d-%02d %02d:%02d:%02d ", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

		print_file_name(de->d_name, st.st_mode);

		if ((st.st_mode & S_IFMT) == S_IFLNK)
		{
			link_name = (char *)calloc(st.st_size + 1, sizeof(char));

			if (readlink(file_name, link_name, st.st_size + 1) < 0)
			{
				printf("\nError: Unable to get linked file\n");
                printf("file name: %s\n", file_name);
				exit(EXIT_FAILURE);
			}

			if (lstat(link_name, &link_st) < 0)
			{
				printf("\nError: Unable to lstat file\n");
                perror("Error");
                printf("link name: \"%s\"\n", link_name);
				exit(EXIT_FAILURE);
			}

			printf(" -> ");

			print_file_name(link_name, link_st.st_mode);

			free(link_name);
		}
		
		printf("\n");	
	}

	printf("Итого: %lld\n", blocks / 2);
	
	if (closedir(dir) < 0)
	{
		printf("Error: Unable to close directory\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
