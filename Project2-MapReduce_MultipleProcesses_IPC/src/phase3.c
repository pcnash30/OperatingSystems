#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include "phase2.h"
// You are free to use your own logic. The following points are just for getting started
/* Reduce Function
	1)	The reducer receives the list from 'm' mappers via pipes
	2)	Ensure proper closing of pipe ends
	3) 	Combine the lists to create a single list
	4) 	Write the list to "ReducerResult.txt" in the current folder
*/

void phase3(int numMappers, char * phase3Dir) {
  pid_t reduce = fork();
  chdir(phase3Dir);
  int letterArray[26];
  for (int letter = 0; letter < 26; letter++) {//Initialize array to 0
    letterArray[letter] = 0;
  }
  if (reduce == 0) {//Child process
    for (int i = 0; i < numMappers; i++) {
      printf("hello\n");
      char buf[150];
      close(mapPipes[i][1]);
      read(mapPipes[i][0], buf, sizeof(buf));
      int ind=0;
      while(buf[ind] != 's'){
        ind++;
      }
      ind++;
      int iterator = 0;
      char * letterString = (char*) malloc(20);
      free(letterString);
      while(buf[ind] != 's'){
        //printf("Debug \n" );
        char * letterString = (char*) malloc(20);
        int h = 0;
        while(buf[ind] != '-') {
          letterString[h++] = buf[ind];
          ind++;
        }
        ind++;
        int d = atoi(letterString);
      //  printf("\tInt is:%d \t String is: %s\n", d, letterString);
        free(letterString);
        letterArray[iterator++] += d;
      }
      for (int letter2 = 0; letter2 < 26; letter2++) {//Initialize array to 0
        printf("%d\n", letterArray[letter2]);
      }
    }
    //Make and print to ReducerResult.txt

  }else{//parent process will wait and return;
    wait(NULL);
  }
}
