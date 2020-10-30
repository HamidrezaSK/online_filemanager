#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/ioctl.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <pthread.h>
#include "client_func.h"
#include "client.h"
#include "peer.h"

void  print_all_serverfiles()
{
    for (int i = 0;i<SERVERMAXFILENUM;i++)
    {
        if(serverfiles[i] != NULL)
            printf("%s # ",serverfiles[i]->name);
        else
        {
            printf("\n");
            break;
        }

    }
}

void  print_all_myfiles()
{
    for (int i = 0;i<MAXFILENUM;i++)
    {
        if(myfiles[i] != NULL)
            printf("index : %d ,%s # ",i,myfiles[i]->name);
        else
        {
            printf("\n");
            break;
        }

    }
}

char* get_my_ip()
{
    int n;
    struct ifreq ifr;
    char array[] = MYINTERFACE;
    n = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , array , IFNAMSIZ - 1);
    ioctl(n, SIOCGIFADDR, &ifr);
    close(n);
    return inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
}

char* create_register_messaage(char* message)
{
    char *str;
    str = (char *) malloc(101);
    memset(str,0,sizeof(str));
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
    return str;
}

char* create_search_messaage(char* message)
{
    char *str;
    str = (char *) malloc(101);
    memset(str,0,sizeof(str));
    strcpy(str, "S");
    strcat(str, message);
    return str;
}

char* create_getall_messaage()
{
   char * str = (char *) malloc(101);
    strcpy(str, "O");
    return str;
}

char* create_ack_messaage()
{
   char * str = (char *) malloc(101);
    strcpy(str, "A");
    return str;
}
char* create_delete_messaage(char* message)
{
    char *str;
    str = (char *) malloc(101);
    memset(str,0,sizeof(str));

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
    return str;
}


void delete_from_myfiles(char* filename)
{
    int item_index = -1;
    for(int i = 0;i<MAXFILENUM;i++)
    {
        if(myfiles[i] == NULL)
            return;
        if(item_index == -1 &&strcmp(myfiles[i]->name,filename)==0)
        {
            printf("index of wanted file: %d\n",i);

            item_index = i;
        }
        if(item_index!=-1)
        {
            if(i < MAXFILENUM-1)
            {
                if(myfiles[i+1] == NULL)
                {
                    memset(myfiles[i]->name,0,strlen(myfiles[i]->name));
                    printf("deletion process\n");
                    memset(myfiles[i],0,sizeof(myfiles[i]));
                    free(myfiles[i]->name);
                    free(myfiles[i]);
                    myfiles[i] = NULL;
                }
                else
                {
                    // myfiles[i]->name = myfiles[i+1]->name;
                    strcpy(myfiles[i]->name,myfiles[i+1]->name);
                }

            }
            else 
            {
                memset(myfiles[i]->name,0,sizeof(char)*MAXNAMESIZE);
                memset(myfiles[i],0,sizeof(myfiles[i]));
                myfiles[i] = NULL;
                free(myfiles[i]->name);
                free(myfiles[i]);
                myfiles[i] = NULL;


            }
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
            np = (file *) malloc(sizeof(*np));
            memset(np,0,sizeof(np));
            np->name = (char *) malloc(sizeof(char)*MAXNAMESIZE);
            strcpy(np->name,filename);
            myfiles[i] = np;
            return;
        }
    }
    printf("file added to myfiles successfully.\n");

}

void add_to_serverfiles(char* filename,int index)
{
    file  *np;
    np = (file *) malloc(sizeof(*np));
    np->name = filename;
    serverfiles[index] = np;
    return;
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
            memset(serverfiles[i],0,sizeof(serverfiles[i]));
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
    sendto(sockfd, (const char *)str, MAXLINE, 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));
    printf("register message sent.\n");
    
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
        MSG_WAITALL, (struct sockaddr *) &servaddr, 
            &len);
    
    status=process_resp(buffer);
    if(status == 0){
        printf("file registered successfully.\n");
        add_to_myfiles(message);
        print_all_myfiles();
    }
    else{
        char* err = (char*)malloc(sizeof(char)* strlen(buffer));
        memset(err,0,sizeof(err));
        strncpy(err, buffer+1,strlen(buffer)-1);
        printf("Server: %s\n",err);
        free(err);
    }
    
}


int send_search_req(char*message,int sockfd,struct sockaddr_in servaddr)
{
    char *str = create_search_messaage(message);
    char buffer[MAXLINE]; 
    int  n;
    unsigned int len;
    int status;
    sendto(sockfd, (const char *)str, MAXLINE, 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));
    printf("search message sent.\n");
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
        MSG_WAITALL, (struct sockaddr *) &servaddr, 
            &len);
    status=process_resp(buffer);
    if(status == 0)
    {
        client_setup(buffer);
    }
    if(status == -1)
    {
        char* err = (char*)malloc(sizeof(char)* strlen(buffer));
        memset(err,0,sizeof(err));
        strncpy(err, buffer+1,strlen(buffer)-1);
        printf("Server: %s\n",err);
        free(err);
    }
    return status;
}

void send_delete_req(char*message,int sockfd,struct sockaddr_in servaddr)
{
    char *str = create_delete_messaage(message);
    char buffer[MAXLINE];
    int  n;
    unsigned int len;
    int status;
    sendto(sockfd, (const char *)str, MAXLINE, 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));
    printf("delete message sent.\n");
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
        MSG_WAITALL, (struct sockaddr *) &servaddr, 
            &len);
    status=process_resp(buffer);
    if(status == 0)
    {
        printf("file deleted.\n");
        delete_from_myfiles(message);
        print_all_myfiles();
    }
    
    else
    {
        char* err = (char*)malloc(sizeof(char)* strlen(buffer));
        memset(err,0,sizeof(err));
        strncpy(err, buffer+1,strlen(buffer)-1);
        printf("Server: %s\n",err);
        free(err);
    }
}

void send_getall_req(int sockfd,struct sockaddr_in servaddr)
{
    clear_serverfiles();
    char *message = create_getall_messaage();
    char buffer[MAXLINE];
    int  n;
    unsigned int len;
    sendto(sockfd, (const char *)message, MAXLINE, 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));
    printf("getall message sent.\n");
    free(message);
    int index = 0;
    while(1)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
            MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len);
        if(buffer[0] == 'E')
        {
            char* err = (char*)malloc(sizeof(char)* strlen(buffer));
            memset(err,0,sizeof(err));
            strncpy(err, buffer+1,strlen(buffer)-1);
            printf("Server: %s\n",err);
            free(err);
            break;
        }
        char* filename = (char*)malloc(sizeof(char)* strlen(buffer));
        memset(filename,0,sizeof(filename));
        strncpy(filename, buffer+1,strlen(buffer)-1);

        add_to_serverfiles(filename,index);
        message=create_ack_messaage();
        sendto(sockfd, (const char *)message, MAXLINE, 
            0, (const struct sockaddr *) &servaddr,  
                sizeof(servaddr));
        index++;
        free(message);
        if(buffer[0] == 'L')
            break;
    }    
}






