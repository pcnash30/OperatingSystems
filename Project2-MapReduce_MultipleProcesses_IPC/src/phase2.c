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
#include <ctype.h>
#include "phase2.h"
// You are free to use your own logic. The following points are just for getting started
/* 	Map Function
	1)	Each mapper selects a Mapper_i.txt to work with
	2)	Creates a list of letter, wordcount from the text files found in the Mapper_i.txt
	3)	Send the list to Reducer via pipes with proper closing of ends
*/

/*void closePipes(int j, int numMappers) {
  for (int i = 0; i < numMappers; i++) {
    if (i == j) {
      close(mapPipes[i][0]); // we want to keep writing pipe open
      continue;
    }
    close(mapPipes[i][0]);
    close(mapPipes[i][1]);
  }
}*/

/*void ParentClosePipes(int numMappers) {
  for (int i = 0; i < numMappers; i++) {
    close(mapPipes[i][1]); // close all write end of pipes
  }
}*/

void readFile(char *currentDir, int j, char *samplePath) {
  char * tempFile = malloc(100);
  chdir(currentDir);
  strcat(tempFile, "Mapper_");
  char temp[4];
  sprintf(temp,"%d", j);
  strcat(tempFile, temp);
  strcat(tempFile, ".txt");
  printf("%s\n",tempFile);
  FILE *fp = fopen(tempFile, "r");
  if (fp == NULL) {
    printf("NULL\n");

  }
  free(tempFile);
  char tempString[1000];
  int allTheLetters[26];
  for (int letter = 0; letter < 26; letter++) {//Initialize array to 0
    allTheLetters[letter] = 0;
  }
  while (fgets(tempString, sizeof(tempString)+1, fp)) {
    char * temppath = strtok(tempString, "\n");
    char * curpath = strtok(temppath, "/");
    char * finalPath = (char*) malloc(100);
    strcat(finalPath, "/");
    while (curpath != NULL) {//
      strcat(finalPath, curpath);
      strcat(finalPath, "/");
      curpath = strtok(NULL, "/");
      if (strstr(curpath, ".txt") != NULL)
        break;
    }
    chdir(finalPath);
    //printf("final is 0%s0\n", finalPath);
    printf("cur is 0%s0\n", curpath);
    FILE *fp0 = fopen(curpath, "r");
    if (fp0 == NULL) {
      printf("is null\n");
    }
    char line[128];
    while (fgets(line, 128, fp0)) {
      char tempLetter = tolower(line[0]);
      int index = tempLetter - 'a';//Offset from asci 'a'
      allTheLetters[index] += 1;
    }
    fclose(fp0);
    free(finalPath);
  }
  fclose(fp);
  char *bufToPipe = (char*)malloc(sizeof(char)*150);
  strcat(bufToPipe, "s");
  for (int letterIndex = 0; letterIndex < 26; letterIndex++) {
    printf("%d is %d\n",letterIndex, allTheLetters[letterIndex]);
    char buf[4];
    sprintf(buf, "%d", allTheLetters[letterIndex]);
    strcat(bufToPipe, buf);
    strcat(bufToPipe, "-");
  }
  strcat(bufToPipe, "s");

  for (int lol = 0; lol < 100; lol++) {
    //printf("%d: to write is %c\n",lol, bufToPipe[lol]);
  }
  close(mapPipes[j][0]);
  write(mapPipes[j][1], bufToPipe, 150);
  close(mapPipes[j][1]);
}

pid_t phase2(char* currentDir, int numMappers, char* samplePath){
  for (int m = 0; m < numMappers; m++) {
    pipe(mapPipes[m]);
  }
  //ParentClosePipes(numMappers); // parent should close write end of pipes
  pid_t pid;
  int childnum=0;
  for (int j = 0; j < numMappers; j++) {
    pid = fork();
    if (pid > 0) {//Parent continues
      childnum++;
      continue;
    }
    else {//Child reads Mapper_i.txt
      readFile(currentDir, j, samplePath);
      break;
    }
  }
  if(pid>0){
    for(int waitTime = 0; waitTime < numMappers; waitTime++){
      wait(NULL);//Parent waits for all children to finish pahse 2
    }
  }
  return pid;//All children will return 0, parent will return >0
}
