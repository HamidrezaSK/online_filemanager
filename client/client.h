#define P2PPORT 7656
#define PORT    8080 
#define MAXLINE 101
#define MAXPORTSIZE 4
#define MAXIPSIZE 15
#define MAXNAMESIZE 79
#define MAX_THREADS 79
#define MAXFILENUM 50
#define SERVERMAXFILENUM 50
#define MYINTERFACE "en0"


typedef struct {
    char *name; 
} file;


extern  file *myfiles[MAXFILENUM];
extern  file *serverfiles[SERVERMAXFILENUM];


