#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
   int     fd1[2], fd2[2], result;

   size_t size;
   char  resstring[14];

   if(pipe(fd1) < 0){
     printf("Can\'t open pipe\n");
     exit(-1);
   }
   if(pipe(fd2) < 0){
     printf("Can\'t open pipe\n");
     exit(-1);
   }

   result = fork();

   if(result < 0) {
      printf("Can\'t fork child\n");
      exit(-1);
   } else if (result > 0) {

      if(close(fd1[0]) < 0){
         printf("parent: Can\'t close reading side of pipe_1\n"); exit(-1);
      }

      size = write(fd1[1], "Hello, parent!", 14);

      if(size != 14){
        printf("Can\'t write all string to pipe_1\n");
        exit(-1);
      }

      if(close(fd1[1]) < 0){
         printf("parent: Can\'t close writing side of pipe_1\n"); exit(-1);
      }
      //pipe_2
      if(close(fd2[1]) < 0){
         printf("parent: Can\'t close writing side of pipe_2\n"); exit(-1);
      }

      size = read(fd2[0], resstring, 14);

      if(size < 0){
         printf("Can\'t read string from pipe_2\n");
         exit(-1);
      }

      printf("Parent exit, resstring:%s\n", resstring);

      if(close(fd2[0]) < 0){
         printf("parent: Can\'t close reading side of pipe_2\n"); exit(-1);
      }


   } else {

      if(close(fd2[0]) < 0){
         printf("child: Can\'t close reading side of pipe_2\n"); exit(-1);
      }

      size = write(fd2[1], "Hello, child!", 14);

      if(size != 14){
        printf("Can\'t write all string to pipe_2\n");
        exit(-1);
      }

      if(close(fd2[1]) < 0){
         printf("child: Can\'t close writing side of pipe_2\n"); exit(-1);
      }

      //pipe_1
      if(close(fd1[1]) < 0){
         printf("child: Can\'t close writing side of pipe_1\n"); exit(-1);
      }

      size = read(fd1[0], resstring, 14);

      if(size < 0){
         printf("Can\'t read string from pipe_1\n");
         exit(-1);
      }


      if(close(fd1[0]) < 0){
         printf("child: Can\'t close reading side of pipe\n"); exit(-1);
      }
      

      printf("Child exit, resstring:%s\n", resstring);
   }

   return 0;
}
