#define MAXLINE 101
#define MAXNAMESIZE 79

typedef struct {
    char *name;
    char *ip; 
    char *port; 
} request;

typedef struct {
    int sockfd; 
} myarg_t;

void client_setup(char* message);
void * server_setup(void);
