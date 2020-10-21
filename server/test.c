#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <ifaddrs.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 


int main()
{

    char * a = "Rfuckme#127.0.0.1#4334#";
    char *filename = (char*)malloc(sizeof(char)*50);
    char *fileport = (char*)malloc(sizeof(char)*50);
    char *fileip = (char*)malloc(sizeof(char)*50);
    // char *data = (char*)malloc(sizeof(char)*50);

    int level=0;
    int start=1;
    for (int i=0;i<strlen(a);i++)
        {
            if(a[i] == '#')
            {
                if(level == 0){
                    strncpy(filename, a+start,i -start);
                    level ++;
                }
                else if(level == 1)
                {
                    strncpy(fileip, a+start,i -start);
                    level ++;
                }
                else{
                    strncpy(fileport, a+start,i -start);
                    break;
                }

                start = i + 1;
            }
        }
    printf("filename: %s\n",filename);
    printf("fileip: %s\n",fileip);
    printf("fileport: %s\n",fileport);
    struct ifaddrs *id;
    int val;
    char str[55];
    val = getifaddrs(&id);
    inet_ntop(AF_INET, &(id->ifa_addr), str, INET_ADDRSTRLEN);
    printf("Network Address is %s with code %d\n",str,val);

    return 0;
}