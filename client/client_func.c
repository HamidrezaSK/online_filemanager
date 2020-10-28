#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <ifaddrs.h>
#include "client_func.h"
#include "client.h"


char* get_my_ip()
{
    struct ifaddrs *id;
    char *str = (char *) malloc(15);
    if(getifaddrs(&id)!=0)
    {
        printf("cant find my ip\n");
    }
    inet_ntop(AF_INET, &(id->ifa_addr), str, INET_ADDRSTRLEN);
    return str;
}

char* create_register_messaage(char* message)
{
    char *str;
    str = (char *) malloc(101);
    strcpy(str, "R");
    strcat(str, message);
    char *sip = get_my_ip();
    char sport[5];
    sprintf(sport,"%d",P2PPORT);
    strcat(str,"#");
    strcat(str,sip);
    strcat(str,"#");
    strcat(str,sport);
    strcat(str,"#");
    free(sip);
    return str;
}

char* create_search_messaage(char* message)
{
    char *str;
    str = (char *) malloc(101);
    strcpy(str, "S");
    strcat(str, message);
    return str;
}

char* create_getall_messaage(char* message)
{
   char * str = (char *) malloc(101);
    strcpy(str, "O");
    return str;
}

char* create_delete_messaage(char* message)
{
    char *str;
    str = (char *) malloc(101);
    strcpy(str, "U");
    strcat(str, message);
    char *sip = get_my_ip();
    char sport[5];
    sprintf(sport,"%d",P2PPORT);
    strcat(str,"#");
    strcat(str,sip);
    strcat(str,"#");
    strcat(str,sport);
    strcat(str,"#");
    free(sip);
    return str;
}


void delete_from_myfiles(char* filename)
{
    int item_index = -1;
    for(int i = 0;i<MAXFILENUM;i++)
    {
        if(myfiles[i] == NULL)
            return;
        if(item_index == -1 &&myfiles[i].name == filename)
        {
            item_index = i;
        }
        if(item_index!=-1)
        {
            if(i < MAXFILENUM-1)
            {
                if(myfiles[i+1] == NULL)
                    free(myfiles[i]);
                else
                    myfiles[i].name = myfiles[i+1].name;

            }
            else 
                free(myfiles[i]);
        }
    }
}

void add_to_myfiles(char* filename)
{
    for(int i = 0; i<MAXFILENUM ;i++)
    {
        if(myfiles[i] == NULL)
        {
            file  *np;
            np = (struct nlist *) malloc(sizeof(*np));
            np->name = filename;
            myfiles[i] = np;
            return;
        }
    }
}
int process_resp(char* message)
{
    if(message[0] == 'E')
        return -1;
    else
        return 0;
}

void clear_serverfiles()
{
    for (int i = 0;i<SERVERMAXFILENUM;i++)
    {
        if(serverfiles[i] != NULL)
        {
            free(serverfiles[i]);
            serverfiles[i] = NULL;
        }
        else
            break;

    }
}

void send_register_req(char*message,int sockfd,struct sockaddr_in servaddr)
{
    char *str = create_register_messaage(message);
    char buffer[MAXLINE]; 
    int  n;
    unsigned int len;
    int status;
    sendto(sockfd, (const char *)message, strlen(message), 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));
    printf("register message sent.\n");
    
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
        MSG_WAITALL, (struct sockaddr *) &servaddr, 
            &len);
    
    status=process_resp(buffer);
    if(status == 0)
    {
        printf("file deleted.\n");
        delete_from_myfiles(message);
    }
    else{
        char* err = (char*)malloc(sizeof(char)* strlen(message));
        strncpy(err, message+1,strlen(message)-1);
        printf("Server: %s\n",err);
        free(err)
    }
    
}


void send_search_req(char*message,int sockfd,struct sockaddr_in servaddr)
{
    char *str = create_search_messaage(message);
    char buffer[MAXLINE]; 
    int  n;
    unsigned int len;
    sendto(sockfd, (const char *)message, strlen(message), 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));
    printf("search message sent.\n");
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
        MSG_WAITALL, (struct sockaddr *) &servaddr, 
            &len);
        status=process_resp(buffer);
   /* if(status == 0)
    {
         Not implemented yet 
        go to peer to peer file sharing request
    }
    */
    if(status == -1)
    {
        char* err = (char*)malloc(sizeof(char)* strlen(message));
        strncpy(err, message+1,strlen(message)-1);
        printf("Server: %s\n",err);
        free(err)
    }

    
    
    
}
void send_delete_req(char*message,int sockfd,struct sockaddr_in servaddr)
{
    char *str = create_delete_messaage(message);
    char buffer[MAXLINE];
    int  n;
    unsigned int len;
    sendto(sockfd, (const char *)message, strlen(message), 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));
    printf("delete message sent.\n");
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
        MSG_WAITALL, (struct sockaddr *) &servaddr, 
            &len);
    status=process_resp(buffer);
    if(status == 0)
    {
         Not implemented yet 
        go to peer to peer file sharing request
    }
    
    else
    {
        char* err = (char*)malloc(sizeof(char)* strlen(message));
        strncpy(err, message+1,strlen(message)-1);
        printf("Server: %s\n",err);
        free(err)
    }
}

void send_getall_req(int sockfd,struct sockaddr_in servaddr)
{
    clear_serverfiles();
    char *str = create_getall_messaage(message);
    char buffer[MAXLINE];
    int  n;
    unsigned int len;
    sendto(sockfd, (const char *)message, strlen(message), 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));
    printf("getall message sent.\n");
    int index = 0;
    while(1)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
            MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len);
        char* filename = (char*)malloc(sizeof(char)* strlen(buffer));
        strncpy(filename, message+1,strlen(message)-1);
        add_to_myfiles(filename);
        if(buffer[0] == 'L')
            break;
    }    
}





