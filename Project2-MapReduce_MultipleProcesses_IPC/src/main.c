#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "main.h"

int main(int argc, char *argv[]){
	// argument count check
	if(argc !=3){
		printf("Invalid number of arguements: Recieved %d \t Expected 2\n", argc-1);
		return -1;
	} else if(atoi(argv[2])<1){
		printf("Invalid mapper count. Count needs to be at least 1");
		return -1;
	}
	int numMappers = atoi(argv[2]);//Number of mappers
	char * path = (char *) malloc(1000);//Path of sample folder
	char * currentDir = (char *) malloc(1000);//Current working directoryv
	char * phase3Dir = (char*) malloc(1000);
	getcwd(phase3Dir,1000);
	strcpy(path, argv[1]);//Gets path from program call

	//just make a function call to code in phase1.c
	//phase1 - Data Partition Phase
	int n = phase1(currentDir, path, numMappers);//Calls phase1
	if (!n) {
		printf("Sample folder is empty\n");
		return 0;
	}
	//create pipes

	//How do we dynamically alocate pipes?
	//Create max amount of pipes and close based on actual mapper count?

	//just make a function call to code in phase2.c
	//phase2 - Map Function
	pid_t parentID = phase2(currentDir, numMappers, path);
	//parentID is greater iff parent process is returner, child
	//proccesses have a parentID of 0.

	//phase3 - Reduce Function
	if (parentID != 0) {//Parent proces
		printf("%s\n", currentDir);
		phase3(numMappers, phase3Dir);
	}else{//child proces
		return 0;
	}
	//phase4
	//wait for all processes to reach this point

	//just make a function call to code in phase4.c
	//master process reports the final output
	free(path);
	free(currentDir);
	free(phase3Dir);
	return 0;

}
