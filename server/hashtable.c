#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hashtable.h"


/* hash: form hash value for string s */
unsigned hash(char *s)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
      hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

/* lookup: look for s in hashtab */
struct nlist *lookup(char *s)
{
    struct nlist *np;
    for (np = hashtab[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
          return np; /* found */
    return NULL; /* not found */
}

// char *strdup(char *);
/* install: put (name, defn) in hashtab */
int install(char *name, char *ip , char *port)
{
    struct nlist *np;
    unsigned hashval;
    if ((np = lookup(name)) == NULL) { /* not found */
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
          return 500;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
        np->owner_count = 0;
        np->owners = (owner*) malloc(sizeof(owner)*(20));//MAXOWNERSIZE = 20;
    }
     else{ /* already there */
        for(int i = 0;i<np->owner_count;i++)
        {
            if(strcmp( np->owners[i].port , port )==0&&strcmp( np->owners[i].ip , ip)==0)
                return 406;
        }
    }
    if(np->owner_count >= 20)
        return 512;
    owner temp = {port,ip};
    np->owners[np->owner_count] =temp;
    np->owner_count++;
    return 0;
}

int del(char *name, char *ip , char *port)
{
    struct nlist *np;
    int item_index = -1;
    if ((np = lookup(name)) != NULL) /* found */
    {
        for(int i = 0;i<np->owner_count;i++)
        {
            if(item_index == -1&&strcmp( np->owners[i].port , port )==0&&strcmp( np->owners[i].ip , ip)==0)
            {
                printf("found the owner\n");
                item_index = i;
                continue;
            }
            if(item_index!=-1&&i<np->owner_count-1)
            {
                np->owners[i].ip= np->owners[i+1].ip;
                np->owners[i].port= np->owners[i+1].port;
            }
        }
        if(np->owner_count>0)
            np->owner_count --;
    }
    else{ /* not found */
        return 404;
    }
    if(np->owner_count==0)
    {
        free(np->owners);
        unsigned hashval = hash(name);
        hashtab[hashval] = NULL;
    }
    if(item_index == -1)
     return 401;

    return 0;
}
