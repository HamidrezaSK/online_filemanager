#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#define P2PPORT 7656
#define PORT    8080 
#define MAXLINE 1024

char* create_messaage(char* message,char* sip);