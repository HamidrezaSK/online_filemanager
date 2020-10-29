#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <time.h>
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
    memset(filename,0,sizeof(filename));
    memset(fileip,0,sizeof(fileip));
    memset(fileport,0,sizeof(fileport));

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
    printf("process register status : %d\n",status);
    printf("%s %s %s\n",filename,fileip,fileport);
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
    memset(filename,0,sizeof(filename));
    memset(fileip,0,sizeof(fileip));
    memset(fileport,0,sizeof(fileport));
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
    printf("how the fuck%s %s %s\n",filename,fileip,fileport);

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
    memset(str,0,sizeof(str));


    strcpy(str, "E");
    switch (code)
    {
        case 404:
            strcat(str, ERROR_404);
            break;
        case 400:
            strcat(str, ERROR_400);
            break;
        case 401:
            strcat(str, ERROR_401);
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
    printf("error message %s\n",message);
    sendto(sockfd, (const char *)message, MAXLINE,  
        0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
    free(message);
    return;
}

void send_ack_response(int sockfd,struct sockaddr_in cliaddr)
{
    char * message = create_ack_response();
    sendto(sockfd, (const char *)message,MAXLINE,  
        0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
    free(message);
    return;
}
char* create_allfiles_response(char* message,int islast)
{
    char *str;
    str = (char *) malloc(101);
    memset(str,0,sizeof(str));
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
    if(remaining_files==0)
    {
        message = create_error_response(404);
        sendto(sockfd, (const char *)message, MAXLINE,  
            0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
        free(message);
    }
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
            {
                message = create_allfiles_response(hashtab[i]->name,1);
            }
            remaining_files--;
        
            while(timeout<3)
            {
                sendto(sockfd, (const char *)message, MAXLINE,  
                    0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
                recvfrom(sockfd, (char *)buffer, MAXLINE,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                        &len);

                if(buffer[0] == 'A')
                {
                    break;
                }
                else
                    timeout++;
            
            }
            free(message);
        }
        i++;

    }
    return;
}
int gen_rand(int lower,int upper)
{
    return rand() % (upper-lower +1) + lower;
}
char* create_search_response(struct nlist *np)
{
    char *str;
    str = (char *) malloc(101);
    memset(str,0,sizeof(str));

    int random_index;
    strcpy(str, "S");
    strcat(str, np->name);
    strcat(str,"#");
    srand(time(0));
    random_index = gen_rand(0,np->owner_count);
    strcat(str,np->owners[random_index].ip);
    strcat(str,"#");
    strcat(str,np->owners[random_index].port);
    strcat(str,"#");
    return str;
}


void send_search_response(char* message,int sockfd,struct sockaddr_in cliaddr)
{
    char* filename = (char*)malloc(sizeof(char)* strlen(message));
    memset(filename,0,sizeof(filename));
    char* str;
    strncpy(filename, message+1,strlen(message)-1);
    strcat(filename, "\0");

    printf("wanted file: %s\n",filename);

    if(strlen(filename)<1)
    {
        send_error_response(404,sockfd,cliaddr);
        return;
    }
    struct nlist *np;
    if ((np = lookup(filename)) == NULL) {
        send_error_response(404,sockfd,cliaddr);
        return ;
    }
    printf("found the file\n");
    if(np->owner_count == 0)
        {
            send_error_response(404,sockfd,cliaddr);
            return;
        }
    str = create_search_response(np);
    sendto(sockfd, (const char *)str, MAXLINE,  
        0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
    free(str);
    return;
}

