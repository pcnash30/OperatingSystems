/*test machine: CSELAB_machine_name * date: mm/dd/yy
* name: full_name1 , [full_name2]
* x500: id_for_first_name , [id_for_second_name] */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include <ctype.h>

void * parseFile(void * args) {
  if (strcmp(globalString, "-p") == 0 || strcmp(globalString, "-bp") == 0) {
    fprintf(fpLog, "producer\n");
  }
  char buf[1024] = {0};
  int lineNumber = 0;
  while (fgets(buf, sizeof(buf), fpProducer)) {
    for (int i = 0; i < sizeof(buf); i++) {
      if (buf[i] < 65 || buf[i] > 90) {
        if (buf[i] < 97 || buf[i] > 122) {
          if (buf[i] != '\n' && buf[i] != '\r') {
            buf[i] = ' ';
          }
          else {
            break;
          }
        }
      }
    }
    Node * newNode = (Node *) malloc(sizeof(Node)); // create node that will be added to queue
    newNode->letters = (char *) malloc(1000); //allocate space for the string in the node
    newNode->line = lineNumber;
    newNode->nextNode = NULL; // For now, this is the end of the queue
    char * tempString = strtok(buf, " \n\r"); //find words in line
    while (tempString != NULL) {
      char temp = tolower(*(tempString));
      strncat(newNode->letters, &temp, 1); // appends the first letter of tempString to letters string
      tempString = strtok(NULL, " \n\r"); // find next word
    }
    pthread_mutex_lock(&queueLock);
    if (strcmp(globalString, "-b") == 0 || strcmp(globalString, "-bp") == 0) {
      while (numItems == queueSize) {
        pthread_cond_wait(&queueFull, &queueLock);
      }
    }
    if (strcmp(globalString, "-p") == 0 || strcmp(globalString, "-bp") == 0) {
      fprintf(fpLog, "producer: %d\n", lineNumber++);
    }
    tailPointer->nextNode = newNode; // append newNode to end of queue
    tailPointer = tailPointer->nextNode; //reset tailPointer to be the end of the queue
    numItems++;
    pthread_mutex_unlock(&queueLock);
    char buf[1024] = {0};
  }
  producerDone = 1;
  fclose(fpProducer);
}
