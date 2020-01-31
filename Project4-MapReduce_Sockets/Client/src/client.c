/*test machine: CSEL-KH4250-18
* date: 12/6/19
* name: Peyton Nash, Taylor O'Neill
* x500: nashx282, oneil569
*/

#include "../include/protocol.h"
#include "../include/phase2.h"

void createLogFile(void) {//Creates log file
    pid_t p = fork();
    if (p==0) //Deletes previous log file
        execl("/bin/rm", "rm", "-rf", "log", NULL);

    wait(NULL);
    mkdir("log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);//Makes log file
    logfp = fopen("log/log_client.txt", "w");//Opens log file with global variable
}

int* packetExtraCredit(int code, int MapperID, int data[]) {
  int *packet = (int *) malloc(sizeof(int)*28);
  packet[0] = code;
  packet[1] = MapperID;
  for(int i =2; i<28; i++){
    packet[i] = data[i-2];
  }
  return packet;
}

int main(int argc, char *argv[]) {
    int mappers;
    char folderName[100] = {'\0'};//Testcase folder
    char *server_ip;
    int server_port;

    //Arguement checking
    if (argc == 5) { // 4 arguments
        strcpy(folderName, argv[1]);
        mappers = atoi(argv[2]);
        server_ip = argv[3];
        server_port = atoi(argv[4]);

        if (mappers > MAX_MAPPER_PER_MASTER) {
            printf("Maximum number of mappers is %d.\n", MAX_MAPPER_PER_MASTER);
            printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
            exit(1);
        }
        if (mappers < 1) {
            printf("Minimum number of mappers is 1.\n");
            printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
            exit(1);
        }
    }
    else {
        printf("Invalid or less number of arguments provided\n");
        printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
        exit(1);
    }

    // create log file
    createLogFile();

    // phase1 - File Path Partitioning include <dirent.h>
    traverseFS(mappers, folderName);

    // Phase2 - Mapper Clients's Deterministic Request Handling
    pid_t p[mappers];
    char * origDir = (char *) malloc(1000);//Original directory
    char tempCount[4];
    getcwd(origDir, 1000);//Updates current direcotry to include MapperInput folder
    for(int i = 0; i < mappers; i++) {
      p[i] = fork();
      if(!p[i]) { //Child process
        char * currentDir = (char *) malloc(1000);//Current working directory
        bzero(tempCount, sizeof(tempCount));
        strcpy(currentDir, origDir);
        strcat(currentDir, "/");
        strcat(currentDir, "MapperInput/Mapper_");
        sprintf(tempCount,"%d",i+1);
        strcat(currentDir, tempCount);
        strcat(currentDir,".txt");//Updates path to mapper_i.txt file
        phase2(i+1, server_ip, server_port, currentDir);//Makes TCP Connection
        exit(i);
      }
    }
    int * ss = (int*) malloc(sizeof(int));//Used for debugging purposes
    for (int j = 0; j < mappers; j++) {
      wait(ss);
      if (*ss == 13) {//Communication error
        printf("Socket error (%d) has occurred: Child Process Killed.\n", *ss);
      }
    }

    free(origDir);

    // start extra credit - phase3
    FILE* fpExtraCredit = fopen("commands.txt", "r");
    char commandsBuffer[10];
    int zeroPacket[26] = {0};
    for(int i = 0; i < 26; i++) {//Zero packet used for requests with 0's for data
      zeroPacket[i] = 0;
    }

    while (fgets(commandsBuffer, sizeof(commandsBuffer), fpExtraCredit)) {
      char * token = strtok(commandsBuffer, "\n");
      int requestCode = atoi(token);

      if (requestCode < 1 || requestCode > 6) {
        fprintf(logfp, "[%d] wrong command\n", -1);
      } //wrong command
      else {
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
        fprintf(logfp, "[%d] open connection\n", -1);
        if (requestCode == GET_AZLIST) {
          int rExtraCredit[28];
          write(sockfd, packetExtraCredit(requestCode, -1, zeroPacket), sizeof(int)*28);
          read(sockfd, rExtraCredit, sizeof(rExtraCredit));
          fprintf(logfp, "[%d] GET_AZLIST: %d ", -1, rExtraCredit[1]);
          for (int j = 2; j < 27; j++) { //prints each value of AZ List
            fprintf(logfp, "%d ", rExtraCredit[j]);
          }
          fprintf(logfp, "%d\n", rExtraCredit[27]);
        }
        else {
          int rExtraCredit[3];
          write(sockfd, packetExtraCredit(requestCode, -1, zeroPacket), sizeof(int)*28);
          read(sockfd, rExtraCredit, sizeof(rExtraCredit));
          if (requestCode == CHECKIN) {
            fprintf(logfp, "[%d] CHECKIN: %d %d\n", -1, rExtraCredit[1], rExtraCredit[2]);
          }
          else if (requestCode == GET_MAPPER_UPDATES) {
            fprintf(logfp, "[%d] GET_MAPPER_UPDATES: %d %d\n", -1, rExtraCredit[1], rExtraCredit[2]);
          }
          else if (requestCode == GET_ALL_UPDATES) {
            fprintf(logfp, "[%d] GET_ALL_UPDATES: %d %d\n", -1, rExtraCredit[1], rExtraCredit[2]);
          }
          else {
            fprintf(logfp, "[%d] CHECKOUT: %d %d\n", -1, rExtraCredit[1], rExtraCredit[2]);
          }
        }
        close(sockfd);
        fprintf(logfp, "[%d] close connection\n", -1);
      }
    }

    // Phase3 - Master Client's Dynamic Request Handling (Extra Credit)
    fclose(logfp);//Closes log file
    exit(0);
    return 0;

}
