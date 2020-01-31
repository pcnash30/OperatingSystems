/*test machine: CSELAB_machine_name * date: 11/13/19
* name: Taylor O'Neill , Peyton Nash
* x500: oneil569 , nashx282 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


//where can the test files be located? all options
int main (int argc, char ** argv) {
  if (argc < 3 || argc > 5) {
    printf("Incorrect Number of Arguments\n");
    return -1;
  }
  if (atoi(argv[1]) < 0 || atoi(argv[1]) > 32) { // I'm not sure what max consumer amount is, but I'm assuming it's less than or equal to 32
    printf("Invalid Consumer Amount\n");
    return -1;
  }
  if (argc != 3) {
    if (strcmp(argv[3], "-p") != 0 && strcmp(argv[3], "-b") != 0 && strcmp(argv[3], "-bp") != 0) {
      printf("Invalid Flag\n");
      return -1;
    }
    else {
      if (argc == 5) {
        if (strcmp(argv[3], "-b") != 0 && strcmp(argv[3], "-bp") != 0) {
          printf("Did not indicate a bounded buffer\n");
          return -1;
        }
        if (atoi(argv[4]) < 1) {
          printf("Invalid Queue Size\n");
          return -1;
        }
        queueSize = atoi(argv[4]);
      }
      if (argc == 4) {
        if (strcmp(argv[3], "-p") != 0) {
          printf("Need a Queue Size\n");
          return -1;
        }
      }
    }
  }
  numItems = 0;
  producerDone = 0;
  for (int z = 0; z < 26; z++) {
    finalHistogram[z] = 0;
  }
  Node * startingNode = (Node*) malloc(sizeof(Node)); // create a dummy starting node for linked list queue
  startingNode->nextNode = NULL; // For now, next node is null
  startPointer = startingNode; // global pointer to start of queue
  tailPointer = startingNode; // global pointer to end of queue
  fpProducer = fopen(argv[2], "r"); //open file and check if valif
  if (fpProducer == NULL) {
    printf("Invalid File\n");
    return -1;
  }
  pthread_t pThread; //prodcuer thread
  pthread_t cThreads[atoi(argv[1])];
  if (argc == 3) {
    pthread_create(&pThread, NULL, &parseFile, NULL);
    globalString = malloc(sizeof(char));
    globalString = "";
  }
  else {
    globalString = malloc(sizeof(char));
    globalString = argv[3];
    if (strcmp(globalString, "-p") == 0 || strcmp(globalString, "-bp") == 0) {
      fpLog = fopen("log.txt", "w");
    }
    pthread_create(&pThread, NULL, &parseFile, NULL); //create thread and jump to function "parseFile" to read content of file
  }
  int num[atoi(argv[1])];
  for (int i = 0; i < atoi(argv[1]); i++) {
    num[i] = i;
    pthread_create(&cThreads[i], NULL, &readFromQueue, &num[i]);
  }
  pthread_join(pThread, NULL); //wait for producer to finish
  for (int j = 0; j < atoi(argv[1]); j++) {
    pthread_join(cThreads[j], NULL);
  }
  if (strcmp(globalString, "-p") == 0 || strcmp(globalString, "-bp") == 0) {
    fclose(fpLog);
  }
  FILE *fpResult = fopen("result.txt", "w");
  for (int y = 0; y < 26; y++) {
    fprintf(fpResult, "%c: %d\n", 'a'+y, finalHistogram[y]);
  }
  fclose(fpResult);
  return 0;
}
