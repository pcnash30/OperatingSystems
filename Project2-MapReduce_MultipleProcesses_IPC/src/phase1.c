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
#include "phase1.h"
// You are free to use your own logic. The following points are just for getting started
/* 	Data Partitioning Phase - Only Master process involved
	1) 	Create 'MapperInput' folder
	2) 	Traverse the 'Sample' folder hierarchy and insert the text file paths
		to Mapper_i.txt in a load balanced manner
	3) 	Ensure to keep track of the number of text files for empty folder condition

*/
int filePartition(char* curDir, int mapcount, int currentMap,char* mainDir);//Returns file count and puts files to analyze

/*______________________________________________________________________________
This section is believed to be done. What do we want when there is no files
(empty directory)? I believe it just returns as of now, so that should be good?
Update:Have not tested with relative path, only absolute
______________________________________________________________________________*/

int phase1(char* currentDir, char* sampleDir, int mapcount){
  getcwd(currentDir, 100);//Updates current direcotry to include MapperInput folder
  strcat(currentDir, "/");
  strcat(currentDir, "MapperInput");
  char temp[4];
  char * rmDir = malloc(100);
  for(int i =0; i<32;i++){//Removes all files from MapperInput Folder
      strcpy(rmDir,currentDir);
      strcat(rmDir,"/Mapper_");
      sprintf(temp,"%d",i);//Converts int to string
      strcat(rmDir,temp);
      strcat(rmDir,".txt");
      remove(rmDir);
  }
  free(rmDir);
  DIR *testdir = opendir(sampleDir);
  struct dirent *testnum;
  int number = 0;
  while ((testnum = readdir(testdir)) != NULL) {
    number++;
  }
  if (number <= 2) {
    return 0;
  }
  rmdir(currentDir);
  mkdir(currentDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);//Makes MapperInput folder
  filePartition(sampleDir, mapcount, 0, currentDir);//Actual partitioning
  return 1;
}


int filePartition(char* curDir, int mapcount, int currentMap, char* mainDir) {
  DIR *dir = opendir(curDir);//Current directory
  if (dir == NULL) {
    printf("directory is null:%s\n", curDir);
  }
  struct dirent *info;
  int z = 0;
	int count = 0;
  while (((info = readdir(dir)) != NULL))//While every file has not been accessed
	{
		if (z == 0 || z == 1) {//Skip unwanted parts of direcotry path
			z++;
			continue;
		}

		if(info->d_type == DT_REG){//Regular file
      char newFile[150];//File to be printed in Mapper_i.txt
      char mapFile[100];//Mapper_i.txt location
      char tempCount[20];//Temp variable to convert "i" to string

      strcpy(newFile,curDir);//Creates new file path
		  strcat(newFile, "/");
      strcat(newFile, info->d_name);
      strcat(newFile, "\n");//Return chracter (returns in printf)
     //printf("\nFile name:%s", newFile);
      int i = 150;
      while(newFile[i] != '\n'){//Used to properly decide length of File (for write function)
        i--;
      }
     //printf("%d\n", i);
      char *actualFile = malloc(sizeof(char)*i);//Used to actually write to .txt
      sprintf(actualFile, "%s", newFile);//Print the newFile to the actualFile
    // printf("\tActual file is: o%so\n",actualFile);
      strcpy(mapFile, mainDir);//Creates path to correct Mapper_i.txt
      strcat(mapFile,"/");
      strcat(mapFile,"Mapper_");
      sprintf(tempCount,"%d",currentMap);
      strcat(mapFile, tempCount);
      strcat(mapFile,".txt");
      int openFile= open(mapFile,O_WRONLY|O_APPEND|O_CREAT, S_IRWXU);
      if(openFile==-1){
        printf("Could not open file\n");
      }
      write(openFile,actualFile,sizeof(char)*i+1);//prints newFile equivalent of appropiate length
      close(openFile);
      free(actualFile);
      currentMap++;//increments the mapper
      if(currentMap==mapcount)//if mapper reachs end, loop back from beginning
        currentMap=0;
  	  }
      else{//Is a directory
      //printf("We are in directory: %s\n", info->d_name);
		    char newPath[PATH_MAX];
		    strcpy(newPath,curDir);
		  	strcat(newPath, info->d_name);
        currentMap = filePartition(newPath, mapcount,currentMap, mainDir);//Recalls funciton in new directory
    }
  }
  closedir(dir);
  free(info);
  return currentMap;//Used to correctly partition data amongst mappers
}
