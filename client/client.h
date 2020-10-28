#define P2PPORT 7656
#define PORT    8080 
#define MAXLINE 101
#define MAXFILENUM 50
#define SERVERMAXFILENUM 50


typedef struct {
    char *name; 
    int owner_count;
    owner *owners;
} file;

typedef struct {
    char *name; 
} local_file;

typedef struct {
    char *port;
    char *ip;
} owner;

extern  file *myfiles[MAXFILENUM];
extern  file *serverfiles[SERVERMAXFILENUM];


