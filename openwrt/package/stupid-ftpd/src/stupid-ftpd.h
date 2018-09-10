
/*

  stupid-ftpd.h
  ------------


  Header for stupid-ftpd.c
  User status informations.

 */

#include <sys/param.h>

extern int serverstop;

typedef struct {
  int sock,idletime,usertype,pid;
  int resume,state,perm;
  int userip[4],dataip[4];
  unsigned short int dataport;
  int pasvsock;
  char rootdir[MAXPATHLEN+4],reldir[MAXPATHLEN+4],username[128];
  char url[256];
} child_t;
#define USR_USER   0
#define USR_PASS   1
#define USR_ONLINE 2
extern child_t *pchild[];
extern int reinit;
