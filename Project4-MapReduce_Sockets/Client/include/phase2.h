#ifndef PHASE2_H
#define PHASE2_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>


void phase2(int mapper_id, char *server_ip, int server_port, char *samplePath);
int *packet(int code, int MapperID, int data[]);

FILE *logfp;

#endif
