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
#include "server_func.h"


file *data = NULL;
int current_size = 0;

// Driver code 
int main() { 
    int sockfd;
    char buffer[MAXLINE];
    char *message;
    struct sockaddr_in servaddr, cliaddr;
    if (data == NULL)
    {
        data = (file*) malloc(sizeof(char)*(MAXFILENUM+MAXIPSIZE+MAXPORTSIZE));
    }
   
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
        buffer[n] = '\0'; 
        switch (buffer[0])
        {
            case 'R':
                printf("received a register request.\n");
                if(process_register_req(buffer)==0)
                    message= create_ack_response();
                else
                {
                    printf("could not process register request");
                    message = create_error_response(400);
                }
                break;
            case 'S':
                break;
            case 'U':
                if(process_delete_req(buffer)==0)
                    message=create_ack_response();
                else
                    message = create_error_response(404);
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
        printf("current_size: %d\n",current_size);
        printf("message: %s\n",message);
        sendto(sockfd, (const char *)message, strlen(message),  
            0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
        free(message); 
        printf("response message sent.\n");
    }
      
    return 0; 
} 
