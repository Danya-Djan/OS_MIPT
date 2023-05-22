#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[]) {
	int fd[2];
	size_t size;
	int result;
	char string[] = "Its work!";
	char sfd[1000];
   	char resstring[14];

	if(pipe(fd) < 0) {
    	printf("Can\'t open pipe\n");
    	exit(-1);
	}

	result = fork();

	if(result < 0) {
      	printf("Can\'t fork child\n");
    	exit(-1);
    } else if (result > 0) {
    	if(close(fd[1]) < 0){
			exit(-1);
    	}

		size = read(fd[0], resstring, 14);

		if (size < 0) {
			exit(-1);
	  	}

		if(close(fd[0]) < 0){exit(-1);
      	}

		printf("Parent exit, resstring:%s\n", resstring);
	} else {
		if(close(fd[0]) < 0){ 
			exit(-1);
    	}
		sprintf(sfd, "%d", fd[1]);
		(void) execle("./second", "./second", sfd, 0, envp);

		if(close(fd[1]) < 0){exit(-1);
      	}
	}

	return 0;
}
