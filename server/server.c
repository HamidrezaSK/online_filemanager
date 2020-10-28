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
#include "hashtable.h"




struct nlist *hashtab[HASHSIZE]; /* pointer table */


// Driver code 
int main() { 
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    int status;
   
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
                status = process_register_req(buffer);
                if(status==0)
                    send_ack_response(sockfd,cliaddr);
                else
                {
                    printf("could not process register request.\n");
                    send_error_response(status,sockfd,cliaddr);
                }
                break;
            case 'S':
                send_search_response(buffer,sockfd,cliaddr);
                break;
            case 'U':
                status = process_delete_req(buffer);
                printf("this is the status : %d\n",status);
                if(status==0)
                    send_ack_response(sockfd,cliaddr);
                else
                    send_error_response(status,sockfd,cliaddr);
                break;
            case 'O':
                send_allfiles_response(sockfd,cliaddr);
                break;

        }
        // struct nlist *np=lookup("install.exe");
        // printf("files count : %d , filename: %s ,\n", np->owner_count,np->name);
        // for(int i =0 ; i<np->owner_count;i++)
        // {
        //     printf("owner : %s , %s \n",np->owners[i].ip,np->owners[i].port);
        // }
        // sendto(sockfd, (const char *)message, strlen(message),  
        //     0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
        for(int i = 0;i<HASHSIZE;i++)
        {
            if(hashtab[i]!=NULL)
            {
                printf("filename : %s \n",hashtab[i]->name);
            }
        }
    }
      
    return 0; 
} 
