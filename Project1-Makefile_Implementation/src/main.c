#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "dfs_stack.h"

target * stringToTarget(char * depend);
void DFS(target *newTarget);
target *tempTarget[MAX_LINES];
//Parse the input makefile to determine targets, dependencies, and recipes


target * stringToTarget(char * depend) {
	for (int j = 0; j < sizeof(tempTarget); j++) {
		if (tempTarget[j] == NULL) {
			break;
		}
		if (strcmp(tempTarget[j]->name, depend) == 0) {
			return tempTarget[j];
		}
	}
	return NULL;
}

void DFS(target *newTarget) {
		if (newTarget->dep_count != 0) {
			for (int i = 0; i < newTarget->dep_count; i++) {
				const char *tempChar = newTarget->depend[i];
				target *newerTarget = stringToTarget(newTarget->depend[i]);
				if (newerTarget == NULL)
					continue;
				DFS(newerTarget);
			}
		}
		for (int z = 0; z < newTarget->recipe_count; z++) {
			printf("%s\n", newTarget->recipe[z]);
		}
}


void execDFS(target *newTarget) {
		if (newTarget->dep_count != 0) {
			for (int i = 0; i < newTarget->dep_count; i++) {
				const char *tempChar = newTarget->depend[i];
				target *newerTarget = stringToTarget(newTarget->depend[i]);
				if (newerTarget == NULL)
					continue;
				execDFS(newerTarget);
			}
		}
		for (int z = 0; z < newTarget->recipe_count; z++) {
			pid_t pid = fork();
			if (pid > 0) {
				wait(NULL);
			}
			else {
				char * token = strtok(newTarget->recipe[z], " ");
				char * parmList[128];
				int j = 0;
				while (token != NULL) {
					parmList[j] = token;
					token = strtok(NULL, " ");
					j++;
				}
				char * parmList2[j+1];
				int jj = 0;
				for (jj = 0; jj < j; jj++) {
					parmList2[jj] = parmList[jj];
					printf(" %s ", parmList2[jj]);
				}
				parmList2[jj] = NULL;
				printf("\n");
				execvp(parmList2[0],parmList2);
				printf("fghailhalighl;egho;ehgo;erhg;owe");
			}
		}
}







int process_file(char *fname)
{
	FILE* fp = fopen(fname, "r");
	char line[LINE_SIZE];
	int i = 0;

	if (!fp) {
		printf("Failed to open the file: %s \n", fname);
		return -1;
	}

	//Read the contents and store in lines
	while (fgets(line, LINE_SIZE, fp)) {
		strncpy(lines[i++], line, strlen(line));
	}
  int idx = 0;
	for (int i = 0; i < MAX_LINES; i++) {
			char *bufPtr = strstr(lines[i],":");
			if (bufPtr == NULL) {
				continue;
			}
			tempTarget[idx] = malloc(sizeof(target));//Creates target_block
			char *token = strtok(lines[i]," :");
			tempTarget[idx]->name = token;
			//printf("%d\n",i);
			//printf("%s\n", token);
			token = strtok(NULL,": \n");
			int j = 0;
			while (token != NULL) {
				tempTarget[idx]->depend[j++] = token;
			//	printf("1");
			//	printf("%s", tempTarget[idx]->depend[j-1]);
			//	printf("2\n");
				token = strtok(NULL,": \n");
			}
			tempTarget[idx]->dep_count = j;
			i++;
			char *recPtr = lines[i];
			j = 0;
			int indicator = 1;
			while (indicator) {//Continues while recipes are still to be visited
				recPtr = lines[i];
				indicator = 0;//Indicates presence of recipe
				for (int h = 0; h < LINE_SIZE; h++) {
					if (*(recPtr+h) != ' ' && *(recPtr+h) != '\t' && *(recPtr+h) != '\n' && *(recPtr+h) != NULL) {
						//printf("1%c2\n",*(recPtr+h));
						recPtr += h;
						tempTarget[idx]->recipe[j] = strtok(recPtr,"\n");
						//printf("3%s4\n",tempTarget[idx]->recipe[j]);
						indicator = 1;
						j++;
						i++;
						break;
					}
				}
				if (indicator == 0) {
				//	printf("No Recipes after %d\n",j);
				}
			}
			tempTarget[idx]->recipe_count = j;
			idx++;
	}

	fclose(fp);

	return 0;
}

//Validate the input arguments, bullet proof the program
int main(int argc, char *argv[])
{
	printf("\n");
	//$./mymake Makefile
	//Similarly account for -r flag
	if (argc == 2 && strncmp(argv[1], "-p", 2)) {
		process_file(argv[1]);
		//TODO
	}
	//if argc==3 -> flag or targe is present
	if ((argc == 3)){
		//$./mymake -p Makefile
		if ((strncmp(argv[1], "-p", 2)==0) && !process_file(argv[2])) {
			int iterator1 = 0;//used for target count
			while (tempTarget[iterator1] != NULL) {
				printf("target '%s' has %d dependencies and %d recipes\n", tempTarget[iterator1]->name, tempTarget[iterator1]->dep_count, tempTarget[iterator1]->recipe_count);
				int iterator2 = 0;//used for dep_count
				while (tempTarget[iterator1]->depend[iterator2] != NULL) {
					printf("Dependency %d is %s\n", iterator2, tempTarget[iterator1]->depend[iterator2]);
					iterator2++;
				}
				int iterator3 = 0;//used for recipe_count
				while (tempTarget[iterator1]->recipe[iterator3] != NULL) {
					printf("Recipe %d is %s\n", iterator3, tempTarget[iterator1]->recipe[iterator3]);
					iterator3++;
				}
				iterator1++;
				printf("\n");
			}
		} else if (!strncmp(argv[1], "-r", 2) && !process_file(argv[2])) {
				//DFS(tempTarget[0]);
				execDFS(tempTarget[0]);
		} else{
			// ./mymake Makefile target
			printf("We are in Makefile [target]\n");
		}

	}

	exit(EXIT_SUCCESS);
}
