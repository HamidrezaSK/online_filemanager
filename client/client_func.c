#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include "client_func.h"


char* create_messaage(char* message,char* ip)
{
    char *str;
    str = (char *) malloc(101);
    strcpy(str, message);
    char sport[5];
    sprintf(sport,"%d",P2PPORT);
    strcat(str,"#");
    strcat(str,ip);
    strcat(str,"#");
    strcat(str,sport);
    printf("in create_message: %s\n",str);
    return str;
}



