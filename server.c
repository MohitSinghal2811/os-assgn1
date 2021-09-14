#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <semaphore.h>



#define PORT 8080
#define BUF_SIZE 1024 // block transfer size
#define QUEUE_SIZE 100
#define LISTENER_THREAD_NUM = 100 // maximum number of threads that can be created for listening

int fatal(char *string);

struct Job{
    char* dll_name;
    char* fun_name;
    int arg;
    int socket_fd;
};

int queueSize = 100;
struct Job jobQueue[100];
int countQueue = 0;
int startQueue = 0;
int endQueue = 0;
int numThreads = 10;


pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

sem_t listenerSemahphore;

void executeJob(struct Job job){
    
    char buffer[BUF_SIZE] = {0};  // buffer for outgoing file
    read(job.socket_fd, buffer, BUF_SIZE);

    int fd = open(buffer, O_RDONLY);
    printf("executing job %d \n", job.arg);
    while(1){
        int bytes = read(fd, buffer, BUF_SIZE);
        if(bytes <= 0) break;
        write(job.socket_fd, buffer, bytes);
    }
    close(fd);
    close(job.socket_fd);
    printf("Job %d done\n", job.arg);
}

void pushJob(void* arg){
    struct Job job = *(struct Job*)arg;
    free(arg);
    pthread_mutex_lock(&mutexQueue);
    if(countQueue == 100){
        printf("Internal Server Error\n");
    }
    else{
        countQueue++;
        jobQueue[endQueue] = job;
        endQueue = (endQueue + 1)%queueSize;
    }
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void* createDispatcherThread(void* args){
    while(1){
        pthread_mutex_lock(&mutexQueue);
        struct Job job;
        while(countQueue == 0){
            pthread_cond_wait(&condQueue, &mutexQueue);
        }
        countQueue--;
        job = jobQueue[startQueue];
        startQueue = (startQueue + 1)%queueSize;
        pthread_mutex_unlock(&mutexQueue);
        executeJob(job);
    }
}

void* createListenerThread(struct Job job){
    char messg[] = "Waiting for the connection...";
    write(job.socket_fd, messg, sizeof(messg)/sizeof(messg[0]));
    pthread_t thread;
    struct Job* a = (struct Job*) malloc(sizeof(struct Job));
    *a = job;
    if(pthread_create(&thread, NULL, &pushJob, a));
}


int main(int argc, char* argv[]){
    pthread_t dispatcherThreads[numThreads];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    
    for(int i = 0;i<numThreads;i++){
        if( pthread_create(&dispatcherThreads[i], NULL, &createDispatcherThread, NULL) != 0){
            printf("Unable to create a Dispatcher Thread \n", i);
        }   
    }

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
    printf("Waiting...");

    while(1){
        new_socket = accept(server_fd, 0, 0);
        if(new_socket < 0) fatal("accept failed");
        struct Job job = {
            .dll_name = "DLL_name", 
            .fun_name = "fun_name", 
            .arg = new_socket,
            .socket_fd = new_socket
        };
        
    }

}



int fatal(char *string){
    printf("%s\n", string);
    exit(1);
}