#define PORT     8080 
#define MAXLINE 101 
#define MAXFILENUM 50
#define MAXPORTSIZE 4
#define MAXIPSIZE 15
#define MAXNAMESIZE 79
#define ERROR_404 "NOT FOUND"
#define ERROR_400 "BAD REQUEST"




typedef struct {
    char *filename;
    char *fileport;
    char *fileip;
} file;

extern file *data;
extern int current_size;