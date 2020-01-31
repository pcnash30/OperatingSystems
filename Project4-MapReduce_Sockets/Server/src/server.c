#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <zconf.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include "../include/protocol.h"

typedef struct threadArg {
	int clientfd;
	char* clientip;
	int clientport;
} threadArg;

typedef struct updateEntry {
  int mapperID;
  int numberOfUpdates;
  int isCheckedIn;
} updateEntry;

void* RequestHandler(void*);
int azList[26];
updateEntry updateStatus[50];
pthread_mutex_t azListLock;
pthread_mutex_t updateStatusLock;

int* ResponseOKPacket(int code, int data) { // a function used to create a Small OK Response Packet
	int* responsePacket = (int*) malloc(sizeof(int)*3);
	responsePacket[0] = code;
	responsePacket[1] = RSP_OK;
	responsePacket[2] = data;
	return responsePacket;
}

int* ResponseNotOKPacket(int code, int data) { // a function used to create a Small NOT OK Response Packet
	int* responsePacket = (int*) malloc(sizeof(int)*3);
	responsePacket[0] = code;
	responsePacket[1] = RSP_NOK;
	responsePacket[2] = data;
	return responsePacket;
}

int* AZListResponseOKPacket() { // a function used to create a Large OK Response Packet (GET_AZLIST case)
	int* responsePacket = (int*) malloc(sizeof(int)*28);
	responsePacket[0] = GET_AZLIST;
	responsePacket[1] = RSP_OK;
	pthread_mutex_lock(&azListLock);
	for (int i = 2; i < 28; i++) {
		responsePacket[i] = azList[i-2];
	}
	pthread_mutex_unlock(&azListLock);
	return responsePacket;
}

int* AZListResponseNotOKPacket() { // a function used to create a Large NOT OK Response Packet (GET_AZLIST case)
	int* responsePacket = (int*) malloc(sizeof(int)*28);
	responsePacket[0] = GET_AZLIST;
	responsePacket[1] = RSP_NOK;
	for (int i = 2; i < 28; i++) {
		responsePacket[i] = 0;
	}
	return responsePacket;
}

void UpdateAZList(int azData[]) { // a function that updates the az list for every valid call to UPDATE_AZLIST
	pthread_mutex_lock(&azListLock);
	for(int i = 0; i < 26; i++) {
		azList[i] += azData[i+2];
	}
	pthread_mutex_unlock(&azListLock);
}

int IsCheckedIn(threadArg* newArg, int statusTableIndex, int code) { // used to check if a client has checked in before handling their resquest
	if (statusTableIndex == -1) { // case where no entry in the table exists for the client
		printf("Error: Not Checked In. Terminating Thread\n");
		write(newArg->clientfd, ResponseNotOKPacket(code, 0), sizeof(int)*3);
		close(newArg->clientfd);
		printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
		return 0;
	}
	else { // case where an entry does exist. We now check if the entry has checked in
		pthread_mutex_lock(&updateStatusLock);
		if (updateStatus[statusTableIndex].isCheckedIn == 0) { // is it checked in?
			printf("Error: Not Checked In. Terminating Thread\n");
			write(newArg->clientfd, ResponseNotOKPacket(code, 0), sizeof(int)*3);
			close(newArg->clientfd);
			printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
			pthread_mutex_unlock(&updateStatusLock);
			return 0;
		}
		pthread_mutex_unlock(&updateStatusLock);
	}
	return 1;
}

int IsCheckedInGETAZList(threadArg* newArg, int statusTableIndex) { // used to check if a client has checked in before handling their resquest (GET_AZLIST case)
	if (statusTableIndex == -1) { // case where no entry in the table exists for the client
		printf("Error: Not Checked In. Terminating Thread\n");
		write(newArg->clientfd, AZListResponseNotOKPacket(), sizeof(int)*28);
		close(newArg->clientfd);
		printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
		return 0;
	}
	else { // case where an entry does exist. We now check if the entry has checked in
		pthread_mutex_lock(&updateStatusLock);
		if (updateStatus[statusTableIndex].isCheckedIn == 0) { // is it checked in?
			printf("Error: Not Checked In. Terminating Thread\n");
			write(newArg->clientfd, AZListResponseNotOKPacket(), sizeof(int)*28);
			close(newArg->clientfd);
			printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
			pthread_mutex_unlock(&updateStatusLock);
			return 0;
		}
		pthread_mutex_unlock(&updateStatusLock);
	}
	return 1;
}


void* RequestHandler(void* arg) { // the main thread function that handles requests
  threadArg* newArg = (threadArg*) arg;
  int requestInfo[28] = {0}; // buffer to store requests that have been read
	int statusTableIndex = -1; // indictaor telling us what index of the status entry table the client is in
	int loopIndicator = 1; // keee reading requests until this is set to 0 (happens when CHECKOUT occurs or an error)
	while (loopIndicator) {
		read(newArg->clientfd, requestInfo, sizeof(requestInfo)); // read the request before making a decision
		if (requestInfo[1] == -1) {
			if (requestInfo[0] == CHECKIN || requestInfo[0] == CHECKOUT || requestInfo[0] == GET_MAPPER_UPDATES) {
				printf("Error: Invalid Command Code. Terminating Thread.\n");
				write(newArg->clientfd, ResponseNotOKPacket(requestInfo[0], 0), sizeof(int)*3);
				close(newArg->clientfd);
				printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
				loopIndicator = 0;
			}
			else if (requestInfo[0] == GET_AZLIST) {
				printf("[%d] GET_AZLIST\n", requestInfo[1]);
				write(newArg->clientfd, AZListResponseOKPacket(), sizeof(int)*28);
				close(newArg->clientfd);
				printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
				loopIndicator = 0;
			}
			else {
				printf("[%d] GET_ALL_UPDATES\n", requestInfo[1]);
				int allSum = 0;
				pthread_mutex_lock(&updateStatusLock);
				for (int i = 0; i < 32; i++) { // iterate through all mapperID update counters and sum them up
					if (updateStatus[i].mapperID != 0) {
						allSum += updateStatus[i].numberOfUpdates;
					}
				}
				pthread_mutex_unlock(&updateStatusLock);
				write(newArg->clientfd, ResponseOKPacket(GET_ALL_UPDATES, allSum), sizeof(int)*3);
				close(newArg->clientfd);
				printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
				loopIndicator = 0;
			}
		}
		else {
			if (requestInfo[1] < -1 || requestInfo[1] > 32 || requestInfo[1] == 0) { // case where the request has an invalid mapper id --> terminates thread and closes socket
				printf("Error: Invalid Mapper ID. Terminating Thread.\n");
				write(newArg->clientfd, ResponseNotOKPacket(requestInfo[0], 0), sizeof(int)*3);
				close(newArg->clientfd);
				printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
				loopIndicator = 0;
			}
			else if (requestInfo[0] == CHECKIN) { // CHECKIN request
				printf("[%d] CHECKIN\n", requestInfo[1]);
				int breakIndicator = 0;
				pthread_mutex_lock(&updateStatusLock);
				for (int i = 0; i < 32; i++) { // find an open entry in the updateStatus table
					if (updateStatus[i].mapperID == requestInfo[1] || updateStatus[i].mapperID == 0) { // find entry in the table
						if (updateStatus[i].mapperID == 0) {
							updateStatus[i].mapperID = requestInfo[1];
						}
						if (updateStatus[i].isCheckedIn == 1) { //check if entry is already checked in
							printf("Error: Already Checked In. Terminating Thread.\n");
							breakIndicator = 1;
							break;
						}
						updateStatus[i].isCheckedIn = 1;
						statusTableIndex = i; //update pointer to index in updateStatus table
						write(newArg->clientfd, ResponseOKPacket(CHECKIN, requestInfo[1]), sizeof(int)*3);
						break;
					}
				}
				pthread_mutex_unlock(&updateStatusLock);
				if (breakIndicator) { // case where the client is already checked in --> return an error and close socket
					write(newArg->clientfd, ResponseNotOKPacket(CHECKIN, 0), sizeof(int)*3);
					close(newArg->clientfd);
					printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
					loopIndicator = 0;
				}
			}
			else if (requestInfo[0] == UPDATE_AZLIST) { // UPDATE_AZLIST request
				if (IsCheckedIn(newArg, statusTableIndex, UPDATE_AZLIST)) { //verify client has checked in
					UpdateAZList(requestInfo);
					pthread_mutex_lock(&updateStatusLock);
					updateStatus[statusTableIndex].numberOfUpdates += 1; // increment update counter for respective client
					pthread_mutex_unlock(&updateStatusLock);
					write(newArg->clientfd, ResponseOKPacket(UPDATE_AZLIST, requestInfo[1]), sizeof(int)*3);
			  }
				else {
					loopIndicator = 0;
				}
			}
			else if (requestInfo[0] == GET_AZLIST) { // GET_AZLIST request
				if (IsCheckedInGETAZList(newArg, statusTableIndex)) { //verufy client has checked in
					printf("[%d] GET_AZLIST\n", requestInfo[1]);
					write(newArg->clientfd, AZListResponseOKPacket(), sizeof(int)*28);
				}
				else {
					loopIndicator = 0;
				}
			}
			else if (requestInfo[0] == GET_MAPPER_UPDATES) { //GET_MAPPER_UPDATES request
				if (IsCheckedIn(newArg, statusTableIndex, GET_MAPPER_UPDATES)) { //verify client has checked in
					printf("[%d] GET_MAPPER_UPDATES\n", requestInfo[1]);
					pthread_mutex_lock(&updateStatusLock);
					write(newArg->clientfd, ResponseOKPacket(GET_MAPPER_UPDATES, updateStatus[statusTableIndex].numberOfUpdates), sizeof(int)*3);
					pthread_mutex_unlock(&updateStatusLock);
				}
				else {
					loopIndicator = 0;
				}
			}
			else if (requestInfo[0] == GET_ALL_UPDATES) { // GET_ALL_UPDATES request
				if (IsCheckedIn(newArg, statusTableIndex, GET_ALL_UPDATES)) { //verify client has checked in
					printf("[%d] GET_ALL_UPDATES\n", requestInfo[1]);
					int allSum = 0;
					pthread_mutex_lock(&updateStatusLock);
					for (int i = 0; i < 32; i++) { // iterate through all mapperID update counters and sum them up
						if (updateStatus[i].mapperID != 0) {
							allSum += updateStatus[i].numberOfUpdates;
						}
					}
					pthread_mutex_unlock(&updateStatusLock);
					write(newArg->clientfd, ResponseOKPacket(GET_ALL_UPDATES, allSum), sizeof(int)*3);
				}
				else {
					loopIndicator = 0;
				}
			}
			else if (requestInfo[0] == CHECKOUT) { // CHECKOUT request
				if (IsCheckedIn(newArg, statusTableIndex, CHECKOUT)) { //verify client has checked in
					updateStatus[statusTableIndex].isCheckedIn = 0; // check out client
					printf("[%d] CHECKOUT\n", requestInfo[1]);
					write(newArg->clientfd, ResponseOKPacket(CHECKOUT, requestInfo[1]), sizeof(int)*3);
					close(newArg->clientfd);
					printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
				}
				loopIndicator = 0;
			}
			else { //INVALID request case. Return error message, close socket, and terminate thread
				printf("Invalid Request Sent. Terminating Thread.\n");
				write(newArg->clientfd, ResponseNotOKPacket(requestInfo[0], 0), sizeof(int)*3);
				close(newArg->clientfd);
				printf("close connection from %s:%d\n", newArg->clientip, newArg->clientport);
				loopIndicator = 0;
			}
			bzero(requestInfo, sizeof(requestInfo));
		}
	}
  return NULL;
}

int main(int argc, char* argv[]) {

    int serverPort;

    if (argc == 2) { // 1 arguments
        serverPort = atoi(argv[1]);
    } else {
        printf("Invalid or less number of arguments provided\n");
        printf("./server <server Port>\n");
        exit(0);
    }

    pthread_mutex_init(&azListLock, NULL); // initialize our global variable locks
    pthread_mutex_init(&updateStatusLock, NULL);

    pthread_t reducerThreads[50]; // max of 50 TCP connections

    for (int i = 0; i < 50; i++) { // set initial values for updateStatus
      updateStatus[i].mapperID = 0;
      updateStatus[i].numberOfUpdates = 0;
      updateStatus[i].isCheckedIn = 0;
    }

		for (int j = 0; j < 26; j++) {
			azList[j] = 0; // initialize az list to be zero
		}

    int sock = socket(AF_INET, SOCK_STREAM, 0); // create socket

    struct sockaddr_in servAddress;

    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(serverPort);
    servAddress.sin_addr.s_addr = htonl(INADDR_ANY); // listen on any address

    if (bind(sock, (struct sockaddr*) &servAddress, (socklen_t) sizeof(servAddress)) == -1) {
      perror("bind");
      exit(-1);
    }

    listen(sock, 50); // start looking for clients
		printf("server is listening\n");

    int threadCounter = 0;

    while(1) {

      struct sockaddr_in clientAddress;
      socklen_t size = sizeof(struct sockaddr_in);
      int clientfd = accept(sock, (struct sockaddr*) &clientAddress, &size);

      threadArg* clientArg = (threadArg *) malloc(sizeof(threadArg));
      clientArg->clientfd = clientfd;
      clientArg->clientip = inet_ntoa(clientAddress.sin_addr);
      clientArg->clientport = clientAddress.sin_port;

			printf("open connection from %s:%d\n", clientArg->clientip, clientArg->clientport);

      pthread_create(&reducerThreads[threadCounter++], NULL, &RequestHandler, clientArg);
    }
    return 0;
}
