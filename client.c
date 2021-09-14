// This program is used for unit testing


#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define PORT 8080
#define BUF_SIZE 1024

int fatal(char *string);

int numThreads = 10;

char *file_name;


void* runThread(void* args ){
    
    int server_fd;
    char buffer[BUF_SIZE];
    struct hostent *h;
    struct sockaddr_in channel;

    server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_fd < 0) fatal("socket");

    channel.sin_family = AF_INET;
    channel.sin_port = htons(PORT);

    int i = inet_pton(AF_INET, "127.0.0.1", &channel.sin_addr);
    if(i <= 0){
        fatal("Address not supported");
    }
    int c = connect(server_fd, (struct sockaddr *) &channel, sizeof(channel));
    if(c < 0) fatal("connection failed");

    write(server_fd, file_name, strlen(file_name) + 1);

    while(1){
        int bytes = read(server_fd, buffer, BUF_SIZE);
        if(bytes<=0) break;
        write(1, buffer, bytes);
    }
    
}


int main(int argc, char **argv){
    if(argc != 2) fatal("Usage: client file-name");

    file_name = argv[1];
    pthread_t threads[numThreads];

    for(int i = 0;i<numThreads;i++){
        if( pthread_create(&threads[i], NULL, &runThread, NULL) != 0){
            printf("Can't create more than %d thread(s) \n", i);
        }
    }

    for(int i = 0;i<numThreads;i++){
        if(pthread_join(threads[i], NULL) != 0){
            printf("failed to join the thread \n");
        }
    }

}


int fatal(char *string){
    printf("%s\n", string);
    exit(1);
}