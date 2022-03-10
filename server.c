/**
 * @file server.c
 * @author Matěj Konopík (xkonop03)
 * @brief Socket server for system info generating (cpu load, cpu name, hostname)
 * @version 1.0
 * @date 2022-03-10
 * 
 * @copyright Copyright (c) Matěj Konopík 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//socket manipulation libs
#include <arpa/inet.h>
#include <unistd.h>

//default port when not explicitly specified via program arg
#define DEFAULTPORT 10000

//maximum http response size
#define MAX 1024
#define SA struct sockaddr

//request and response definitions
#define HOSTNAME    "GET /hostname"
#define LOAD        "GET /load"
#define CPU         "GET /cpu-name"
#define HTTP_RESP   "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n"
#define HTTP_FAIL   "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\n400 Bad Request"

//debug flag
#define DEBUG 0

/**
*Custom debug printing macro
*/
#define M_PRINT(fmt)                                                        \
    do {                                                                    \
        if (DEBUG){                                                         \
            fprintf(stderr, "%s\n", fmt);                                   \
        }                                                                   \
    } while (0)




int getPort(int argc, const char** argv);

int getLoad();

char* getHostname();

char* getCPU();

void errAndDie(char* message);



int main(int argc, const char** argv){
    //load port
    int port = getPort(argc, argv);

    //init socket
    int socketDescr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socketDescr == -1){
        errAndDie("Socket init failed.");
    }
    M_PRINT("Socket initiated");

    //sets options - for correct debugging
    int opt = 1;
    setsockopt(socketDescr, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    //loads address data
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    //binds the socket to a port
    if(bind(socketDescr, (SA*)&address, sizeof(address)) == -1){
        errAndDie("Bind failed.");
    }
    M_PRINT("Socket bound");

    //starts listening to incoming traffic
    if(listen(socketDescr, 3) < 0){
      errAndDie("Listen failed.");
    }
    M_PRINT("Listening");

    //main loop for accepting requests
    int socketSize = sizeof(SA);
    int clientSocket;
    //buffer for handling messages
    char buff[MAX];
    while(1){
        if((clientSocket = accept(socketDescr,(SA*)&address, (socklen_t*)&socketSize)) < 0){
            errAndDie("Accept failed.");
        }
        M_PRINT("Accepted");

        //clear buffer
        bzero(buff, MAX);
        //read request
        read(clientSocket, buff, sizeof(buff));

        //hostname request
        if(!strncmp(buff, HOSTNAME, strlen(HOSTNAME))){
            char* hostname = getHostname();
            bzero(buff, MAX);
            strcpy(buff, HTTP_RESP);
            strcat(buff, hostname);
            send(clientSocket, buff, MAX, 0);
            bzero(buff, MAX);
            free(hostname);
        }
        //load request
        else if(!strncmp(buff, LOAD, strlen(LOAD))){
            int load = getLoad();
            char charload[3]; //FIXME ?????? driv dva ale co kdyz tam bude 100%?
            bzero(buff, MAX);
            sprintf(charload, "%d", load);
            strncat(charload, "%%", 1);
            strcpy(buff, charload);

            send(clientSocket, buff, MAX, 0);
            bzero(buff, MAX);
        }
        //cpu ID request
        else if(!strncmp(buff, CPU, strlen(CPU))){
            char* cpu = getCPU();
            bzero(buff, MAX);
            strcpy(buff, HTTP_RESP);
            strcat(buff, cpu);
            send(clientSocket, buff, MAX, 0);
            bzero(buff, MAX);
            free(cpu);
        }
        //unvalid request, answer with 400 Bad request
        else{
            send(clientSocket, HTTP_FAIL, sizeof(HTTP_FAIL), 0);
        }
        //closes socket
        close(clientSocket);
    }

    close(socketDescr);
    printf("FREED, CLOSED!\n");
    return 0;
}


/**
*@brief Custom error printing function - always exits with code 1
*@param message Error message to print
*/
void errAndDie(char* message){
    fprintf(stderr, "RUNTIME ERROR: %s\n", message);
    exit(1);
}

/**
*@brief Gets the hostname of local linux machine
*@return String containing the hostname
*/
char* getHostname(){
    char* hostname = (char*)malloc(1000);
    if(hostname == NULL){
        fprintf(stderr, "ERROR: Can not allocate memory.");
        exit(1);
    }
    FILE* fileptr;
    if(!(fileptr = fopen("/proc/sys/kernel/hostname", "r"))){
        fprintf(stderr, "Error - unable to read hostname.\n");
        exit(1);
    }
    fgets(hostname, 1000, fileptr);
    fclose(fileptr);
    return hostname;
}

/**
 * @brief Get the port number from program arguments
 * 
 * @param argc arg count
 * @param argv arg value
 * @return (int)port
 */
int getPort(int argc, const char** argv){
    if(argc == 2){
        return atoi(argv[1]);
    }
    return DEFAULTPORT;
}

/**
*Calculates the current load of the CPU using /proc/stat file
*@return Returns integer 0-99 representing the percentage of load
*/
int getLoad(){
    FILE* fileptr;

    if(!(fileptr = fopen("/proc/stat", "r"))){
        fprintf(stderr, "Error - unable to read CPU usage.\n");
        exit(1);
    }

    char statline[256];
    fgets(statline, 256, fileptr);

    char* token = strtok(statline, " ");
    int preUser, preNice, preSystem, preIdle, preIowait, preIrq, preSoftirq, preSteal;

    token = strtok(NULL, " ");
    preUser = atoi(token);
    token = strtok(NULL, " ");
    preNice = atoi(token);
    token = strtok(NULL, " ");
    preSystem = atoi(token);
    token = strtok(NULL, " ");
    preIdle = atoi (token);
    token = strtok(NULL, " ");
    preIowait = atoi(token);
    token = strtok(NULL, " ");

    preIrq = atoi(token);
    token = strtok(NULL, " ");
    preSoftirq = atoi(token);
    token = strtok(NULL, " ");
    preSteal = atoi(token);

    sleep(1);

    rewind(fileptr);
    fgets(statline, 256, fileptr);

    int postUser, postNice, postSystem, postIdle, postIowait, postIrq, postSoftirq, postSteal;

    token = strtok(statline, " ");

    token = strtok(NULL, " ");
    postUser = atoi(token);
    token = strtok(NULL, " ");
    postNice = atoi(token);
    token = strtok(NULL, " ");
    postSystem = atoi(token);
    token = strtok(NULL, " ");
    postIdle = atoi (token);
    token = strtok(NULL, " ");
    postIowait = atoi(token);
    token = strtok(NULL, " ");
    postIrq = atoi(token);
    token = strtok(NULL, " ");
    postSoftirq = atoi(token);
    token = strtok(NULL, " ");
    postSteal = atoi(token);

    int preIdleF, postIdleF, preTotal, preNonIdle, postNonIdle, postTotal, totalD, idleD;
    preIdleF = preIdle + preIowait;
    postIdleF = postIdle + postIowait;

    preNonIdle = preUser + preNice + preSystem + preIrq + preSoftirq + preSteal;
    postNonIdle = postUser + postNice + postSystem + postIrq + postSoftirq + postSteal;

    preTotal = preIdleF + preNonIdle;
    postTotal = postIdleF + postNonIdle;

    totalD = postTotal - preTotal;
    idleD = postIdleF - preIdleF;

    fclose(fileptr);
    return ((totalD - idleD)*100)/totalD;
}

/**
*Loads CPU name from /proc/cpuinfo
*@return string containing the CPU name (1000 chars)
*/
char* getCPU(){
    char* cpuModel = (char*)malloc(1000);
    if(cpuModel == NULL){
        fprintf(stderr, "ERROR: Can not allocate memory.");
        exit(1);
    }
    //reads from proc/cpuinfo file
    FILE* fileptr;
    if(!(fileptr = fopen("/proc/cpuinfo", "r"))){
        fprintf(stderr, "Error - unable to read hostname.\n");
        exit(1);
    }
    char buffer[1000];

    //find and read cpu name
    while(fgets(buffer, 1000, fileptr)!= NULL){
        if(strstr(buffer, "model name") != NULL){
        strcpy(cpuModel, buffer);
        break;
        }
    }
    //string extraction 
    strtok(cpuModel, ":");
    char* tmp = strtok(NULL, ":");
    strcpy(cpuModel, tmp);
    fclose(fileptr);
    if (cpuModel[0] == ' ') 
        memmove(cpuModel, cpuModel+1, strlen(cpuModel));
    return cpuModel;
}
