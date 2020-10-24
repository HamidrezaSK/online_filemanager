#define MAXOWNERSIZE=20;
#define HASHSIZE 101

typedef struct {
    char *port;
    char *ip;
} owner;


struct nlist { /* table entry: */
    struct nlist *next; /* next entry in chain */
    char *name; /* defined name */
    int owner_count; /* number of registered owners */
    owner *owners; /* owner peer's ip and port  */
};

struct nlist *lookup(char *);
struct nlist *install(char *, char * , char *);
int del(char *, char * , char *);