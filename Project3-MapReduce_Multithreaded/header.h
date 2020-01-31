/*test machine: CSELAB_machine_name * date: mm/dd/yy
* name: full_name1 , [full_name2]
* x500: id_for_first_name , [id_for_second_name] */

/*
header.h, header for all source files
it will:
- structure definition
- global variable, lock declaration (extern)
- function declarations
*/

#include <pthread.h>


#ifndef _HEADER_H_
#define _HEADER_H_

int main(int argc, char ** argv);
void * parseFile(void * args);
void * readFromQueue(void * args);

typedef struct Node {
  int line;
  char * letters;
  struct Node * nextNode;
} Node;

char * globalString;

Node * startPointer;
Node * tailPointer;

FILE * fpLog;
FILE * fpProducer;

pthread_mutex_t queueLock;
pthread_mutex_t histogramLock;
pthread_cond_t queueFull;

int numItems;
int producerDone;
int finalHistogram[26];
int queueSize;
#endif
