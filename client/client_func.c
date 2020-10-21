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
    printf("in create_message: %s\n",str);
    return str;
}




