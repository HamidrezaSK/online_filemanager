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

int process_register_req(char* message)
{
    int level=0;
    int start=1;
    for (int i=0;i<strlen(message);i++)
    {
        if(message[i] == '#')
        {
            if(level == 0){
                data[current_size].filename = (char*)malloc(sizeof(char)*MAXNAMESIZE);
                strncpy(data[current_size].filename, message+start,i -start);
                level ++;
            }
            else if(level == 1)
            {
                data[current_size].fileip = (char*)malloc(sizeof(char)*MAXIPSIZE);
                strncpy(data[current_size].fileip, message+start,i -start);
                level ++;
            }
            else{
                data[current_size].fileport = (char*)malloc(sizeof(char)*MAXPORTSIZE);
                strncpy(data[current_size].fileport, message+start,i -start);
                break;
            }

            start = i + 1;
        }
    }
    current_size++;
    if(level == 2)
        return 0;
    return -1;
}

int process_delete_req(char* message)
{
    char* filename= (char*)malloc(sizeof(char)*MAXNAMESIZE);
    char* fileip= (char*)malloc(sizeof(char)*MAXIPSIZE);
    char* fileport= (char*)malloc(sizeof(char)*MAXPORTSIZE);
    int level=0;
    int start=1;
    int item_index = -1;
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
    for(int i = 0;i<current_size-1;i++)
    {
        if(data[i].filename == filename && data[i].fileip == fileip && data[i].fileport == fileport)
        {
            item_index = i;
            continue;
        }
        if(item_index!=-1)
        {
            data[i].filename = data[i+1].filename;
            data[i].fileip = data[i+1].fileip;
            data[i].fileport = data[i+1].fileport;
        }
    }
    if(item_index == -1)
        return -1;
    current_size --;
    free(data[current_size].filename);
    free(data[current_size].fileip);
    free(data[current_size].fileport);
    free(filename);
    free(fileip);
    free(fileport);
    return 0;   
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
        case 400:
            strcat(str, ERROR_400);
    }
    return str;
}
