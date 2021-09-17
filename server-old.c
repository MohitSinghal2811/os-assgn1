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
#include <errno.h>
#include <sys/resource.h>


#define PORT 8080
#define BUF_SIZE 1024 // block transfer size
#define QUEUE_SIZE 100
#define LISTENER_THREAD_NUM 100 // maximum number of threads that can be created for listening

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
int numThreads = 10;

int my_var = 1;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

sem_t listenerThreadSemaphore;

void executeJob(struct Job job){
    char buffer[BUF_SIZE] = {0};  // buffer for outgoing file
    read(job.socket_fd, buffer, BUF_SIZE);
    int fd = open(buffer, O_RDONLY);
    printf("Executing job %d\n", job.socket_fd);
    char messg[] = "Executing job... Waiting for the job to get finished";
    write(job.socket_fd, messg, sizeof(messg)/sizeof(messg[0]));
    while(1){
        int bytes = read(fd, buffer, BUF_SIZE);
        if(bytes <= 0) break;
        write(job.socket_fd, buffer, bytes);
    }
    close(fd);
    close(job.socket_fd);
    printf("Job %d done\n", job.socket_fd);
    char messg2[] = "Job finished";
    write(job.socket_fd, messg, sizeof(messg2)/sizeof(messg2[0]));
}

void* pushJob(void* arg){
    struct Job job = *(struct Job*)arg;
    free(arg);
    char messg[] = "HIIII\n";
    write(job.socket_fd, messg, sizeof(messg)/sizeof(messg[0]));
    pthread_mutex_lock(&mutexQueue);
    if(countQueue == 100){
        char messg[] = "Failed to push the job in the Dispatcher queue";
        write(job.socket_fd, messg, sizeof(messg)/sizeof(messg[0]));
    }
    else{
        countQueue++;
        jobQueue[endQueue] = job;
        endQueue = (endQueue + 1)%queueSize;
        char messg[] = "Pushed to the Dispatcher queue... Waiting for the job to get executed";
        write(job.socket_fd, messg, sizeof(messg)/sizeof(messg[0]));
    }
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
    pthread_exit(&my_var);
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
    if(pthread_create(&thread, NULL, &pushJob, (void *) a) != 0){
        char messg[] = "Failed to create Listener thread";
        write(job.socket_fd, messg, sizeof(messg)/sizeof(messg[0]));
    }else{
        char messg[] = "Listener thread created....Waiting for the job to get pushed in the dispatcher queue";
        write(job.socket_fd, messg, sizeof(messg)/sizeof(messg[0]));
    }
    
}

int fatal(char *string){
    printf("%s\n", string);
    exit(1);
}

int main(int argc, char* argv[]){
    struct rlimit limit;
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("%ld \n", usage.ru_maxrss);


    if(getrlimit(RLIMIT_STACK, &limit) == 0)
        printf("%ld %ld \n", limit.rlim_cur, limit.rlim_max);
    else
        fprintf(stderr, "%s\n", hstrerror(errno));


    limit.rlim_cur = 0;
    limit.rlim_max = 0;

    if(setrlimit(RLIMIT_STACK, &limit) == -1)
        fprintf(stderr, "%s\n", hstrerror(errno));


    if(getrlimit(RLIMIT_STACK, &limit) == 0)
        printf("%ld %ld \n", limit.rlim_cur, limit.rlim_max);
    else
        fprintf(stderr, "%s\n", hstrerror(errno));


    pthread_t dispatcherThreads[numThreads];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    sem_init(&listenerThreadSemaphore, 0, LISTENER_THREAD_NUM);


    // This creates the Thread Pool which would be used by the dispatcher
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

    getrusage(RUSAGE_SELF, &usage);
    printf("%ld \n", usage.ru_maxrss);


    while(1){
        new_socket = accept(server_fd, 0, 0);
        if(new_socket < 0) fatal("Accept failed");
        struct Job job = {
            .dll_name = "DLL_name", 
            .fun_name = "fun_name", 
            .arg = "ARGS",
            .socket_fd = new_socket
        };
        sem_wait(&listenerThreadSemaphore);
        createListenerThread(job);
        int sem; 
        sem_getvalue(&listenerThreadSemaphore, &sem);
        printf("%d", sem);
        sem_post(&listenerThreadSemaphore);
        getrusage(RUSAGE_SELF, &usage); 
        printf("%ld \n", usage.ru_maxrss);  
    }
}
