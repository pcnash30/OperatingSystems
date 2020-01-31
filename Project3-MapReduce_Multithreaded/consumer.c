/*test machine: CSELAB_machine_name * date: mm/dd/yy
* name: full_name1 , [full_name2]
* x500: id_for_first_name , [id_for_second_name] */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void * readFromQueue(void * args) {
  int tempLettersEmpty;
  int * threadNum = (int*) args;
  if (strcmp(globalString, "-p") == 0 || strcmp(globalString, "-bp") == 0) {
    fprintf(fpLog, "consumer %d\n", *threadNum);
  }
  while (!producerDone || numItems != 0) {
    pthread_mutex_lock(&queueLock);
    char * tempLetters = malloc(1000);
    if (numItems != 0) {
      tempLettersEmpty=0;
      Node * tempPointer = startPointer;
      startPointer = startPointer->nextNode;
      if (strcmp(globalString, "-p") == 0 || strcmp(globalString, "-bp") == 0) {
        fprintf(fpLog, "consumer %d: %d\n", *threadNum, startPointer->line);
      }
      free(tempPointer->letters);
      free(tempPointer);
      strcpy(tempLetters,startPointer->letters);
      numItems--;
      if (strcmp(globalString, "-b") == 0 || strcmp(globalString, "-bp") == 0) {
        pthread_cond_signal(&queueFull);
      }
    }
    else {
      tempLettersEmpty=1;
    }
    pthread_mutex_unlock(&queueLock);
    if(!tempLettersEmpty) {
      pthread_mutex_lock(&histogramLock);
      for (int z = 0; z < strlen(tempLetters); z++) {
        finalHistogram[tempLetters[z] - 'a'] += 1;
      }
      pthread_mutex_unlock(&histogramLock);
    }
  }
  if (strcmp(globalString, "-p") == 0 || strcmp(globalString, "-bp") == 0) {
    fprintf(fpLog, "Consumer %d: %d\n",*threadNum ,-1);
  }
}
