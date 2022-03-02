#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//socket libs
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h> 
#include <sys/ioctl.h>
#include <netdb.h>

#define DEFAULTPORT 80


//function to load port, if specified
int getPort(int argc, const char** argv);

int getLoad();

void getHostname(char* hostname);

int main(int argc, const char** argv){
    int port = getPort(argc, argv);
    printf("LOAD: %d\n", getLoad());
    char* hostname = (char*)malloc(256);
    if(hostname == NULL){
        fprintf(stderr, "ERROR: Can not allocate memory.");
        exit(1);
    }
    getHostname(hostname);
    printf("HOSTNAME: %s\n", hostname);

    free(hostname);
    
    return 0;
}



void getHostname(char* hostname){
    FILE* fileptr;
    if(!(fileptr = fopen("/proc/sys/kernel/hostname", "r"))){
        fprintf(stderr, "Error - unable to read hostname.\n");
        exit(1);
    }
    fgets(hostname, 256, fileptr);
    fclose(fileptr);
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
