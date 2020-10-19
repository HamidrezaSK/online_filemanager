
// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <pthread.h>
  
#define PORT     8080 
#define MAXLINE 1024
#define MAX_THREADS 32767

int thread_no = 0;
typedef struct {
    char buffer[MAXLINE];
    int n;
    int sockfd;
    struct sockaddr_in cliaddr; 
} myarg_t;

void *request_handler(void *arg) {
    myarg_t *args = (myarg_t *) arg;
    int n = args->n;
    struct sockaddr_in cliaddr = args->cliaddr;
    int sockfd = args->sockfd;
    printf("%d , %d \n",sockfd,args->sockfd);
    args->buffer[n] = '\0';
    char *hello = "Hello from server";
    printf("Client : %s\n", args->buffer); 
    sendto(sockfd, (const char *)hello, strlen(hello),  
        0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
    printf("Hello message sent.\n");  
    return NULL;
}
  
// Driver code 
int main() { 
    int sockfd;
    struct sockaddr_in servaddr, cliaddr; 
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
    int flag = 1;  
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {  
        perror("setsockopt fail");  
    } 
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    int  n=1; 
    unsigned int len;
    pthread_t threads[MAX_THREADS];
    int rc = 0;
  
    len = sizeof(cliaddr);  //len is value/resuslt 
    
    while(n){
        myarg_t *args = (myarg_t*)malloc(sizeof(myarg_t));
        args->n = recvfrom(sockfd, (char *)args->buffer, MAXLINE,  
                    MSG_WAITALL, (struct sockaddr *)&args->cliaddr, 
                    &len);
        args->sockfd = sockfd;
        rc =pthread_create(&threads[thread_no], NULL, request_handler, (void*)args);
		if(rc){
			printf("can not create another thread\n");
		}
		else{
			thread_no++;
		}
    }
      
    return 0; 
} 
