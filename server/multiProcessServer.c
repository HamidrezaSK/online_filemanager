
// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
  
#define PORT     8080 
#define MAXLINE 1024 
  
// Driver code 
int main() { 
    int sockfd;
    char buffer[MAXLINE]; 
    char *hello = "Hello from server"; 
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
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    int  n; 
    unsigned int len;
  
    len = sizeof(cliaddr);  //len is value/resuslt 
    while(sockfd > 0){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len); 
        int rc = fork();
        if (rc < 0) {
            perror("fork failed"); 
            exit(EXIT_FAILURE); 
        } else if (rc == 0) {
            buffer[n] = '\0';
            sleep(1); 
            printf("Client : %s\n", buffer); 
            sendto(sockfd, (const char *)hello, strlen(hello),  
                0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
            printf("Hello message sent.\n");  
            return 0;
        }
    }
      
    return 0; 
} 
