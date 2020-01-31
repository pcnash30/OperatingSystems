/*test machine: CSEL-KH4250-18
* date: 12/6/19
* name: Peyton Nash, Taylor O'Neill
* x500: nashx282, oneil569
*/

#include "../include/protocol.h"
#include "../include/phase2.h"

int* packet(int code, int MapperID, int data[]) {//Used to compress data into integer array
  int *packet = (int *) malloc(sizeof(int)*28);
  packet[0] = code;
  packet[1] = MapperID;
  for(int i =2; i<28; i++){
    packet[i] = data[i-2];
  }
  return packet;//Returns data in int array
}

void phase2(int mapper_id, char *server_ip, int server_port, char *samplePath) {
  int zeroPacket[26] = {0};
  for(int i = 0; i < 26; i++) {//Zero packet used for requests with 0's for data
    zeroPacket[i] = 0;
  }
  // Create a TCP socket.
  int sockfd = socket(AF_INET , SOCK_STREAM , 0);
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(server_port);
  address.sin_addr.s_addr = inet_addr(server_ip);

  //Checks if valid connection
  if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == -1) {
    perror("Connection failed!");
    exit(-1);
  }
  fprintf(logfp, "[%d] open connection\n", mapper_id);

  //________CHECKIN___START____________________________________________
  int rCheckIn[3];
  write(sockfd, packet(CHECKIN, mapper_id, zeroPacket), 28*sizeof(int)); //CHECKIN commnad
  read(sockfd, rCheckIn, sizeof(rCheckIn));
  fprintf(logfp, "[%d] CHECKIN: %d %d\n", mapper_id, rCheckIn[1], rCheckIn[2]);
  //________CHECKIN___END______________________________________________

  //________UPDATE_AZLIST___START______________________________________
  FILE* fp = fopen(samplePath, "r");//Mapper_.txt file pointer
  char* tempString = malloc(1000);//Absolute txt to read
  int updateCount = 0;
  while (fgets(tempString, 1000, fp)) {//Line by line of location of .txt files to read
    int allTheLetters[26] = {0};
    updateCount++;
    char* tempPath = strtok(tempString, "\n");
    FILE* fp0 = fopen(tempPath, "r");//Indivual .txt file to word count
    if (fp0 == NULL) {
      printf("Could not open .txt file in Mapper_i.txt\n");
      exit(-1);
    }
    char line[128];
    for (int letter = 0; letter < 26; letter++) {//Initialize array to 0
      allTheLetters[letter] = 0;
    }
    while (fgets(line, sizeof(line), fp0)) {//Goes through each line of .txt file
      char tempLetter = tolower(line[0]);//Saves first letter of word on line
      int index = tempLetter - 'a';//Offset from asci 'a'
      allTheLetters[index] += 1;//Increments specificed index of A-Z array
    }
    int rUpdateAZList[3];
    write(sockfd, packet(UPDATE_AZLIST, mapper_id, allTheLetters), 28*sizeof(int));
    read(sockfd, rUpdateAZList, sizeof(rUpdateAZList));
    fclose(fp0);//Closes .txt file
  }
  fprintf(logfp, "[%d] UPDATE_AZLIST: %d\n", mapper_id, updateCount);
  fclose(fp);//Closes mapper_i.txt file
  //________UPDATE_AZLIST___END________________________________________

  //________GET_AZLIST___START__________________________________________
  int rGetAZList[28];
  write(sockfd, packet(GET_AZLIST, mapper_id, zeroPacket), 28*sizeof(int));
  read(sockfd, rGetAZList, sizeof(rGetAZList));
  fprintf(logfp, "[%d] GET_AZLIST: %d ", mapper_id, rGetAZList[1]);
  for (int j = 2; j < 27; j++) { //prints each value of AZ List
    fprintf(logfp, "%d ", rGetAZList[j]);
  }
  fprintf(logfp, "%d\n", rGetAZList[27]);
  //________GET_AZLIST___END__________________________________________

  //________GET_MAPPER_UPDATES___START________________________________
  int rGetMapperUpdates[3];
  write(sockfd, packet(GET_MAPPER_UPDATES, mapper_id, zeroPacket), 28*sizeof(int));
  read(sockfd, rGetMapperUpdates, sizeof(rGetMapperUpdates));
  fprintf(logfp, "[%d] GET_MAPPER_UPDATES: %d %d\n", mapper_id, rGetMapperUpdates[1], rGetMapperUpdates[2]);
  //________GET_MAPPER_UPDATES___END__________________________________

  //________GET_ALL_UPDATES___START___________________________________
  int rGetAllUpdates[3];
  write(sockfd, packet(GET_ALL_UPDATES, mapper_id, zeroPacket), 28*sizeof(int));
  read(sockfd, rGetAllUpdates, sizeof(rGetAllUpdates));
  fprintf(logfp, "[%d] GET_ALL_UPDATES: %d %d\n", mapper_id, rGetAllUpdates[1], rGetAllUpdates[2]);
  //________GET_ALL_UPDATES___END_____________________________________

  //________CHECKOUT___START__________________________________________
  int rCheckOut[3];
  write(sockfd, packet(CHECKOUT, mapper_id, zeroPacket), 28*sizeof(int)); //CHECKOUT commnad
  read(sockfd, rCheckOut, sizeof(rCheckOut));
  fprintf(logfp, "[%d] CHECKOUT: %d %d\n", mapper_id, rCheckOut[1], rCheckOut[2]);
  //________CHECKOUT___END____________________________________________

  close(sockfd);//Closes socket for each process
  fprintf(logfp, "[%d] close connection\n", mapper_id);
}
