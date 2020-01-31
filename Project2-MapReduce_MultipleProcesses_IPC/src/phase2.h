// Add Guard to header file
// Function prototypes for
//			map function that will go through text files in the Mapper_i.txt to get the (letter, wordcount)
#ifndef PHASE2_H
#define PHASE2_H
int mapPipes[32][2];
pid_t phase2();
void readFile();
void closePipes();

#endif
