// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "server.h"

  
// Driver code 
int main() { 
    int sockfd;
    char buffer[MAXLINE]; 
    char *hello = "Hello from server"; 
    struct sockaddr_in servaddr, cliaddr;
    file *data = (file*)malloc((sizeof(char))*(MAXFILENUM+MAXIPSIZE+MAXPORTSIZE);
    printf("size of data elements :%lu\n",sizeof(file));
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
    int level=0;
    int start=1;
    int current_size = 0;

    len = sizeof(cliaddr);  //len is value/resuslt 
    while(sockfd > 0){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len);
        // inet_ntop(AF_INET, &(cliaddr.sin_addr), str, INET_ADDRSTRLEN);
        buffer[n] = '\0'; 
        switch (buffer[0])
        {
            case 'R':
                printf("received a register request.\n");
                level=0;
                start=1;
                for (int i=0;i<strlen(buffer);i++)
                {
                    if(buffer[i] == '#')
                    {
                        if(level == 0){
                            data[current_size].filename = (char*)malloc(sizeof(char)*MAXNAMESIZE);
                            strncpy(data[current_size].filename, buffer+start,i -start);
                            level ++;
                        }
                        else if(level == 1)
                        {
                            data[current_size].fileip = (char*)malloc(sizeof(char)*MAXIPSIZE);
                            strncpy(data[current_size].fileip, buffer+start,i -start);
                            level ++;
                        }
                        else{
                            data[current_size].fileport = (char*)malloc(sizeof(char)*MAXPORTSIZE);
                            strncpy(data[current_size].fileport, buffer+start,i -start);
                            break;
                        }

                        start = i + 1;
                    }
                }
                current_size++;
                break;
            case 'S':
                break;
            case 'U':
                break;
            case 'O':
                break;
            case 'A':
                break;
            case 'E':
                break;
        }
            
        printf("Client : %s\n", buffer);
        printf("data: %s, %s ,%s\n",data[0].filename,data[0].fileip,data[0].fileport);
        sendto(sockfd, (const char *)hello, strlen(hello),  
            0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
        printf("Hello message sent.\n");
    }
      
    return 0; 
} 
