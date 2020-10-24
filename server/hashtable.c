#include <stddef.h>
#include <string.h>
#include "hashtable.h"



static struct nlist *hashtab[HASHSIZE]; /* pointer table */

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
struct nlist *install(char *name, char *ip , char *port)
{
    struct nlist *np;
    unsigned hashval;
    if ((np = lookup(name)) == NULL) { /* not found */
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
          return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
        np->owner_count = 0;
        np->owners = (owner*) malloc(sizeof(owner)*(20));//MAXOWNERSIZE = 20;
    }
    //  else{ /* already there */
        
    //         np->owners[np->owner_count] = {port, ip};
    //         np->owner_count++;
    //     }
    if(np->owner_count >= 20)
        return NULL;
    owner temp = {port,ip};
    np->owners[np->owner_count] =temp;
    np->owner_count++;
    return np;
}

int del(char *name, char *ip , char *port)
{
    struct nlist *np;
    if ((np = lookup(name)) != NULL) /* found */
    {
        if(np->owner_count>0)
            np->owner_count --;
    }
    else{ /* not found */
        return 404;
    }
    if(np->owner_count==0)
        free(np->owners);
    return 0;
}


// int main()
// {
//     struct nlist *np = install("fuck.exe","127.0.0.1","3443");
//     np = install("df.exe","127.0.0.1","3443");
//     np = install("fuck.exe","127.0.0.1","3543");
//     // np = install("suck","mmd");
//     // np = install("khaak","akbar");
//     np = lookup("fuck.exe");
//     printf("owners of fuck.exe are: \n");
//     for (int i = 0;i<np->owner_count;i++)
//         printf("ip : %s , port : %s\n",np->owners[i].ip,np->owners[i].port);
//     // printf("khaak was : %s , %s\n",np->name,np->defn);
//     int siz = 0;
//     for(int i = 0;i<HASHSIZE;i++)
//     {
//         if(hashtab[i]!=NULL)
//             siz++;
//     }
//     printf("hashtab size is : %d\n",siz);

//     return 0;
// }