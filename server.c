#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <semaphore.h>
#include <string.h>
#include "DLL_invoker.c"




#define PORT 8080
#define BUF_SIZE 1024 // block transfer size
#define QUEUE_SIZE 10


int printfunction(char *string){
    printf("%s\n", string);
    exit(1);
} 

int fatal(char *string){
    printf("%s\n", string);
    exit(1);
} 

struct Job{
    char* dll_name;
    char* fun_name;
    char* arg;
    int socket_fd;
};

int queueSize = 100;
struct Job jobQueue[100];
int countQueue = 0;
int startQueue = 0;
int endQueue = 0;


pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;
sem_t listenerThreadSemaphore;


void executeJob(struct Job job){

    char buffer[BUF_SIZE] = {0};  // buffer for outgoing file
    read(job.socket_fd, buffer, BUF_SIZE);
    int fd = open(buffer, O_RDONLY);
    while(1){
        int bytes = read(fd, buffer, BUF_SIZE);
        if(bytes <= 0) break;
        write(job.socket_fd, buffer, bytes);
    }
    close(fd);
    close(job.socket_fd);
    printf("Job %ld done\n", job.socket_fd);
}

void pushJob(struct Job job){
    // pthread_mutex_lock(&mutexQueue);
    // struct Job job = *(struct Job*)arg;
    if(countQueue == 100){
        printf("Internal Server Error\n");
    }
    else{
        countQueue++;
        jobQueue[endQueue] = job;
        endQueue = (endQueue + 1)%queueSize;
    }
    // free(arg);
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void* runThread(void* args){
    while(1){
        pthread_mutex_lock(&mutexQueue);
        int flag = 0;
        struct Job job;
        while(countQueue == 0){
            pthread_cond_wait(&condQueue, &mutexQueue);
        }
        countQueue--;
        job = jobQueue[startQueue];
        startQueue = (startQueue + 1)%queueSize;
        flag = 1;
        pthread_mutex_unlock(&mutexQueue);
        if(flag == 1){
            executeJob(job);
        }
    }
}



int main(int argc, char* argv[]){
    struct rlimit limit;
    struct rusage usage;

    if(argc != 4){
        printfunction("Usage: ./server [Thread Limit] [Memory Limit (in MB)] [File Limit] ");
    }

    limit.rlim_cur = 1024 * 1024 * atoi(argv[2]);
    limit.rlim_max = 1024 * 1024 * atoi(argv[2]);

    if(setrlimit(RLIMIT_AS, &limit) == -1)
        fprintf(stderr, "%s\n", hstrerror(errno));

    int numThreads = atoi(argv[1]);

    pthread_t threads[numThreads];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);

    for(int i = 0;i<numThreads;i++){
        if( pthread_create(&threads[i], NULL, &runThread, NULL) != 0){
            printf("Can't create more than %d thread(s) \n", i);
        }   
    }

    // sem_init(&listenerThreadSemaphore, 0, LISTENER_THREAD_NUM);

    int server_fd, new_socket;
    struct sockaddr_in channel; // holds IP address
    int opt = 1;
    int addrlen = sizeof(channel);

    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY);
    channel.sin_port = htons( PORT );

    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create socket file descriptor
    if(server_fd < 0) fatal("socket failed");

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt));

    int b = bind(server_fd, (struct sockaddr *) &channel, sizeof(channel));
    if(b < 0) fatal("bind failed");

    int l = listen(server_fd, QUEUE_SIZE); // specify queue size
    if(l < 0) fatal("listen failed");

    while(1){
        
        new_socket = accept(server_fd, 0, 0);
        if(new_socket < 0) fatal("accept failed");

        char input[BUF_SIZE];
    
        int bytes;
        bytes = read(new_socket, input, BUF_SIZE);

        char delim[] = " ";
	    char *ptr = strtok(input, delim);
        // printf("%s\n", ptr);
        // printf("%s\n", strtok(NULL, delim));
        // printf("%s\n", strtok(NULL, delim));
        char* dll_name = ptr;
        char* fun_name = strtok(NULL, delim);
        char* arg = strtok(NULL, delim);

        struct Job job = {
            .dll_name = dll_name, 
            .fun_name = fun_name, 
            .arg = arg,
            .socket_fd = new_socket
        };

        printf("%s %s %s\n", job.dll_name, job.fun_name, job.arg);
        fflush(stdout);

        // close(new_socket);

        pushJob(job);
    }

}

