#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h> 
#include "peer.h"
#include "client.h"
#include "client_func.h"


int extract_data(char* message,char** str){
    if(message[0] == 'F')
    {
        strncpy(*str, message+1,strlen(message)-1);
    }
    else if(message[0] == 'L')
    {

        memset(*str,0,strlen(*str));
        strncpy(*str, message+1,strlen(message)-1);
    }
    else
    {
      perror("[-]Error in packet semantics.");
      return -1;
    }
    return 0;
}

void send_bytes(char *data, int sockfd){
    if (send(sockfd, data, MAXLINE, 0) == -1) {
      perror("[-]Error in sending file.");
      return;
    }
    return;
}
char* create_download_messaage(char* message)
{
    char *str;
    str = (char *) malloc(MAXLINE);
    strcpy(str, "D");
    strcat(str, message);
    return str;
}

char* create_upload_messaage(char* message,int islast){
    char *str;
    str = (char *) malloc(MAXLINE);
    if(islast == 0)
        strcpy(str, "F");
    else
        strcpy(str, "L");
    strcat(str, message);
    return str;
    
}
void * process_uploading(void *arg)
{
    myarg_t *args = (myarg_t *) arg;
    int sockfd = args->sockfd;
    char* buffer=(char *)malloc(sizeof(char)*MAXLINE);
    char* last_buffer=(char *)malloc(sizeof(char)*MAXLINE);

    int n;
    long bytesRead;
    char* message;
    FILE *file=NULL;
    char* filename = (char*)malloc(sizeof(char)* MAXNAMESIZE);
    n = recv(sockfd, buffer, MAXLINE, 0);
    printf("download request:%s,%d\n",buffer,n);

    if(buffer[0] == 'D')
    {
        strncpy(filename, buffer+1,strlen(buffer)-1);
    }
    else
    {
        perror("[-]Error in packet semantics");
        return NULL;
    }


    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("[-]Error in reading file.");
        return NULL;
    }
    printf("finding size of the file\n");
    fseek(file, 0L, SEEK_END);
    long sz = ftell(file);  
    rewind(file);
    while(sz>0)
    {
        if(sz-MAXLINE-1>0)
        {
            bytesRead = fread(buffer,1, MAXLINE-1, file);
        }
        else
        {
            bytesRead = fread(buffer,1, sz , file);
            strncpy(last_buffer, buffer,sz);
            buffer = last_buffer;
        }
        sz -= bytesRead;
        if(sz>0)
        {
            message = create_upload_messaage(buffer,0);
        }
        else
        {
            message = create_upload_messaage(buffer,1);
        }
        send_bytes(message,sockfd);
        message = NULL;
        free(message);
    }
    close(sockfd);
    return NULL;
}

void download_file(char* filename,char*ip,char*port_str)
{
    int port = atoi(port_str);
    int e;
    int n;
    char buffer[MAXLINE];
    char* message;
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *file=NULL;
    file = fopen(filename, "wb");
    if (file == NULL) {
        perror("[-]Error in reading file.");
        return;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    int flag = 1;  
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {  
        perror("setsockopt fail");  
    }
    e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) {
        perror("[-]Error in socket");
        return;
    }
    printf("[+]Connected to Server.\n");
    message = create_download_messaage(filename);
    send_bytes(message,sockfd);
    while(1)
    {
        char* writable_data = (char *)malloc(sizeof(char)*MAXLINE);
        n = recv(sockfd, buffer, MAXLINE, 0);
        e = extract_data(buffer,&writable_data);
        printf("recived data from server: %s\n",writable_data);
        if(e == -1)
        {
            perror("[-]Error in packet semantics");
            return;
        }
        fprintf(file, "%s", writable_data);
        free(writable_data);

        if(buffer[0]=='L')
        {        
            break;
        }
        
    }
    fclose(file);
    printf("file downloaded successfully\n");
}






void * server_setup()
{
    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    int e;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("[-]Error in socket");
        return NULL;
    }
    printf("[+]Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = P2PPORT;
    server_addr.sin_addr.s_addr = inet_addr(get_my_ip());
    int flag = 1;  
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {  
        perror("setsockopt fail");  
    }
    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e < 0) {
        perror("[-]Error in bind");
        return NULL;
    }
    printf("[+]Binding successfull.\n");

    if(listen(sockfd, 10) == 0){
    printf("[+]Listening....\n");
    }else{
    perror("[-]Error in listening");
        return NULL;
    }
    addr_size = sizeof(new_addr);
    pthread_t threads[MAX_THREADS];
    int rc = 0;
    int thread_no = 0;
    while(1)
    {
    myarg_t *args = (myarg_t*)malloc(sizeof(myarg_t));
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    args->sockfd = new_sock;
    rc =pthread_create(&threads[thread_no], NULL, process_uploading, (void*)args);
    if(rc){
        printf("can not create another thread\n");
    }
    else{
        thread_no++;
    }
    if(thread_no == MAX_THREADS)
        break;
    }
    return NULL;


}

void client_setup(char* message)
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
    
    printf("downloading:%s from : %s %s\n",filename,fileip,fileport);

    if(level == 2){
        if(atoi(fileport) != P2PPORT || strcmp(fileip,get_my_ip())!=0)
        {
            download_file(filename,fileip,fileport);
        }
    }
    free(filename);
    free(fileip);
    free(fileport);

}
