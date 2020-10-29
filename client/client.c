#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "client_func.h"
#include "client.h"

  
file *myfiles[MAXFILENUM];
file *serverfiles[SERVERMAXFILENUM];

  
// Driver code 
int main() {
    int sockfd; 
    struct sockaddr_in     servaddr; 
    
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY;
    int c;
    char filename[MAXNAMESIZE];
    int status;
    int exitt = 0;

    while(1)
    {
        printf("enter your request:\n \t S: search for file and request to download it\n\t O: get all registered files in server\n \t R: register an existing file\n \t U: delete one of your registered files\n \t E: exit\n");
        c = getchar( );

        switch (c)
        {
            case 'R':
                printf("Register request\nenter the filename(your name shall not exceed %d)\n",MAXNAMESIZE);
                scanf("%s", filename);
                send_register_req(filename,sockfd,servaddr);
                break;
            case 'S':
                printf("Search request\nenter the filename(your name shall not exceed %d)\n",MAXNAMESIZE);
                scanf("%s", filename);
                status = send_search_req(filename,sockfd,servaddr);
                if(status == 0)
                {
                    send_register_req(filename,sockfd,servaddr);
                }
                break;
            case 'U':
                printf("Delete request\nenter the filename\n");
                scanf("%s", filename);
                send_delete_req(filename,sockfd,servaddr);
                break;
            case 'O':
                printf("Get all registered files request\n");
                send_getall_req(sockfd,servaddr);
                print_all_serverfiles();
                break;
            case 'E':
                printf("Get all registered files request\n");
                send_getall_req(sockfd,servaddr);
                print_all_serverfiles();
                exitt = 1;
                break;
        }
        if(exitt == 1)
        {
            break;
        }

    }
    close(sockfd); 
    return 0; 
} 