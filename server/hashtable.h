#define MAXOWNERSIZE 20
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

extern struct nlist *hashtab[HASHSIZE];

struct nlist *lookup(char *);
int install(char *, char * , char *);
int del(char *, char * , char *);