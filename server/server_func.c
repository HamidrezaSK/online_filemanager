#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "server_func.h"
#include "server.h"
#include "hashtable.h"

int process_register_req(char* message)
{
    int level=0;
    int start=1;
    int status;
    char* filename= (char*)malloc(sizeof(char)*MAXNAMESIZE);
    char* fileip= (char*)malloc(sizeof(char)*MAXIPSIZE);
    char* fileport= (char*)malloc(sizeof(char)*MAXPORTSIZE);
    for (int i=0;i<strlen(message);i++)
    {
        if(message[i] == '#')
        {
            if(level == 0){
                strncpy(filename, message+start,i -start);
                level ++;
            }
            else if(level == 1)
            {
                strncpy(fileip, message+start,i -start);
                level ++;
            }
            else{
                strncpy(fileport, message+start,i -start);
                break;
            }
            start = i + 1;
        }
    }
    status = install(filename,fileip,fileport);
    free(filename);
    free(fileip);
    free(fileport);
    if(level == 2)
        return status;
    return 400;
}

int process_delete_req(char* message)
{
    char* filename= (char*)malloc(sizeof(char)*MAXNAMESIZE);
    char* fileip= (char*)malloc(sizeof(char)*MAXIPSIZE);
    char* fileport= (char*)malloc(sizeof(char)*MAXPORTSIZE);
    int level=0;
    int start=1;
    int status;
    for (int i=0;i<strlen(message);i++)
    {
        if(message[i] == '#')
        {
            if(level == 0){
                strncpy(filename, message+start,i -start);
                level ++;
            }
            else if(level == 1)
            {
                strncpy(fileip, message+start,i -start);
                level ++;
            }
            else{
                strncpy(fileport, message+start,i -start);
                break;
            }

            start = i + 1;
        }
    }
    status = del(filename,fileip,fileport);
    free(filename);
    free(fileip);
    free(fileport);
    if(level == 2)
        return status;
    return 400;  
}

char* create_ack_response(void)
{
    char *str = (char *) malloc(101);
    strcpy(str,"A");
    return str;
}

char* create_error_response(int code)
{
    char *str;
    str = (char *) malloc(101);
    strcpy(str, "E");
    switch (code)
    {
        case 404:
            strcat(str, ERROR_404);
            break;
        case 400:
            strcat(str, ERROR_400);
            break;
        case 406:
            strcat(str, ERROR_406);
            break;
        case 500:
            strcat(str, ERROR_500);
            break;
        case 512:
            strcat(str, ERROR_512);
            break;
    }
    return str;
}

void send_error_response(int status,int sockfd,struct sockaddr_in cliaddr)
{
    char * message = create_error_response(status);
    sendto(sockfd, (const char *)message, strlen(message),  
        0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
    free(message);
    return;
}

void send_ack_response(int sockfd,struct sockaddr_in cliaddr)
{
    char * message = create_ack_response();
    sendto(sockfd, (const char *)message, strlen(message),  
        0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
    free(message);
    return;
}
char* create_allfiles_response(char* message,int islast)
{
    char *str;
    str = (char *) malloc(101);
    if(islast == 0)
        strcpy(str, "O");
    else
        strcpy(str, "L");
    strcat(str, message);
    return str;
}

void send_allfiles_response(int sockfd,struct sockaddr_in cliaddr)
{
    int remaining_files = 0;
    char* message;
    int timeout;
    char buffer[MAXLINE];
    unsigned int len;
    len = sizeof(cliaddr);
    int i;
    for(i = 0;i<HASHSIZE;i++)
        if(hashtab[i]!=NULL)
            remaining_files++;
    i = 0;
    while(remaining_files>0)
    {
        timeout = 0;
        if(hashtab[i]!=NULL)
        {
            if(remaining_files>1)
            {
                message = create_allfiles_response(hashtab[i]->name,0);
            }
            else
                message = create_allfiles_response(hashtab[i]->name,1);
            remaining_files--;
        }
        while(timeout>3)
        {
            sendto(sockfd, (const char *)message, strlen(message),  
                0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
            recvfrom(sockfd, (char *)buffer, MAXLINE,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len);
            if(buffer == "A")
                break;
            else
                timeout++;
        
        }
        free(message);
        i++;

    }
    return;
}