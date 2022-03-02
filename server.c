#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//socket libs
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h> 
#include <sys/ioctl.h>
#include <netdb.h>

#define DEFAULTPORT 1400
#define MAX 1024
#define SA struct sockaddr
#define HOSTNAME    "GET /hostname"
#define LOAD        "GET /load"
#define CPU         "GET /cpu-name"
#define HTTP_RESP   "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n"
#define HTTP_FAIL   "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\n400 Bad Request"

#define DEBUG 0 

#define M_PRINT(fmt) \
    do { if (DEBUG){ fprintf(stderr, "%s\n", fmt); }} while (0)


//function to load port, if specified
int getPort(int argc, const char** argv);

int getLoad();

char* getHostname();

char* getCPU();

void errAndDie(char* message);

int main(int argc, const char** argv){
    //load port
    int port = getPort(argc, argv);
    char* cpuName = getCPU();
    //printf("CPU MODEL:\t%s\n", cpuName);
    free(cpuName);

    int socketDescr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socketDescr == -1){
        errAndDie("Socket init failed.");
    }
    M_PRINT("Socket initiated");
    int opt = 1;
    setsockopt(socketDescr, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    

    if(bind(socketDescr, (SA*)&address, sizeof(address)) == -1){
        errAndDie("Bind failed.");
    }
    printf("Socket bound on port %d\n", port);

    if(listen(socketDescr, 3) < 0){
      errAndDie("Listen failed.");
    }
    M_PRINT("Listening");

    int socketSize = sizeof(SA); 
    int clientSocket;
    char buff[MAX];
    while(1){
        if((clientSocket = accept(socketDescr,(SA*)&address, (socklen_t*)&socketSize)) < 0){
            errAndDie("Accept failed.");
        }
        M_PRINT("Accepted");
        bzero(buff, MAX);
        read(clientSocket, buff, sizeof(buff));

        if(!strncmp(buff, HOSTNAME, strlen(HOSTNAME))){
            char* hostname = getHostname();
            printf("%s", hostname);
            bzero(buff, MAX);
            strcpy(buff, HTTP_RESP);
            strcat(buff, hostname);
            send(clientSocket, buff, MAX, 0);
            bzero(buff, MAX);
            free(hostname);
        }
        else if(!strncmp(buff, LOAD, strlen(LOAD))){
            int load = getLoad();
            char charload[2];
            bzero(buff, MAX);
            sprintf(charload, "%d", load);
            strcpy(buff, charload); //SIGSEGV
            send(clientSocket, buff, MAX, 0);
            bzero(buff, MAX);
        }
        else if(!strncmp(buff, CPU, strlen(CPU))){
            char* cpu = getCPU();
            bzero(buff, MAX);
            strcpy(buff, HTTP_RESP);
            strcat(buff, cpu);
            send(clientSocket, buff, MAX, 0);
            bzero(buff, MAX);
            free(cpu);
        }
        else{
            send(clientSocket, HTTP_FAIL, sizeof(HTTP_FAIL), 0);
        }
        

        close(clientSocket);
    }
    
    close(socketDescr);
    printf("FREED, CLOSED!\n");
    return 0;
}

void errAndDie(char* message){
    fprintf(stderr, "RUNTIME ERROR: %s\n", message);
    exit(1);
}

char* getHostname(){
    char* hostname = (char*)malloc(256);
    if(hostname == NULL){
        fprintf(stderr, "ERROR: Can not allocate memory.");
        exit(1);
    }
    FILE* fileptr;
    if(!(fileptr = fopen("/proc/sys/kernel/hostname", "r"))){
        fprintf(stderr, "Error - unable to read hostname.\n");
        exit(1);
    }
    fgets(hostname, 256, fileptr);
    fclose(fileptr);
    return hostname;
}


int getPort(int argc, const char** argv){
    if(argc == 2){
        return atoi(argv[1]);
    }
    return DEFAULTPORT;
}


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

char* getCPU(){
    char* cpuModel = (char*)malloc(256);
    if(cpuModel == NULL){
        fprintf(stderr, "ERROR: Can not allocate memory.");
        exit(1);
    }
    FILE* fileptr;
    if(!(fileptr = fopen("/proc/cpuinfo", "r"))){
        fprintf(stderr, "Error - unable to read hostname.\n");
        exit(1);
    }
    char buffer[256];

    while(fgets(buffer, 256, fileptr)!=NULL){
        if(strstr(buffer, "model name") != NULL){
        strcpy(cpuModel, buffer);
        break;        
        }
    }
    strtok(cpuModel, ":");
    char* tmp = strtok(NULL, ":");
    strcpy(cpuModel, tmp); //FIXME OVERLAP!!!
    fclose(fileptr);
    return cpuModel;
}
