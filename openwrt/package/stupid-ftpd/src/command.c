
/*

  command.c
  ---------


  Only for the stupid-ftpd interactive mode.
  You can find commands here, which can be entered
  while stupid-ftpd is running.

  Type "help" to get a short explanation.

 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <signal.h>
#include <string.h>

#include "command.h"
#include "ftpdconfig.h"
#include "stupid-ftpd.h"
#include "ftpcommand.h"
#include "ftperror.h"

/* define config-file options */
#define CMD_COUNT 16
const char *commands[CMD_COUNT]={
  "HELP","QUIT","LIST","WHO","KILL","STOP","ROOT","ADD",
  "DEL","REG","PERM","INIT","DOWN","FDOWN","BAN","UNBAN"
};
#define CMD_HELP   0
#define CMD_QUIT   1
#define CMD_LIST   2
#define CMD_WHO    3
#define CMD_KILL   4
#define CMD_STOP   5
#define CMD_ROOT   6
#define CMD_ADD    7
#define CMD_DEL    8
#define CMD_REG    9
#define CMD_PERM  10
#define CMD_INIT  11
#define CMD_DOWN  12
#define CMD_FDOWN 13
#define CMD_BAN   14
#define CMD_UNBAN 15


const char *commandhelp[CMD_COUNT]={
  "help","exit server","list connections","info about user",
  "kill user's connection","stop user's down-/upload",
  "change a user's root directory","add a new user","deletes a user",
  "list registered users","sets a user's permissions",
  "restart server","sets restricted mode",
  "forces down mode (no more operations)",
  "bans users from server",
  "removes bans from server"
};
const char *spacer="        ";


/* prototypes */
void docmd_help(char *);
void docmd_list(void);
void docmd_who(char *);
void docmd_kill(char *);
void docmd_stop(char *);
void docmd_quit(void);
void docmd_init(void);
void docmd_root(char *);
void docmd_del(char *);
void docmd_reg(void);
void docmd_perm(char *);
void docmd_down(int);

const char *commandlonghelp[CMD_COUNT]={
  "Syntax: help [command]\n\n'help' alone will display all available \
commands.\n'help command' will give you some info about a command.\n\n\
Example: help list\n",
  "Syntax: quit\n\n'quit' will shut down the server.\n",
  "Syntax: list\n\n'list' will display open connections and their status.\n",
  "Syntax: who [nr]\n\n'who' alone will give you information about users.\n\
'who <nr>' will give you information about user with the specified number.\n",
  "Syntax: kill <nr>\n\n'kill' closes ALL connections to a user.\n",
  "Syntax: stop <nr>\n\n'stop' stops a data transfer to/from a user.\n\
it doesnt close his login shell.\n",
  "Syntax: root <nr> <path>\n\n'root' changes a users root directory \
while he is logged in.\nIt doesn't affect the login database.\n\
(It might be confusing for the user, be careful!)\n",
  "Syntax: add <login> <passwd> <subdir> <maxlogins> <flags>\n\n\
'add' will add a new login to the current login database.\n\n\
Example: add jimmy0 hyper /tmp 5 DU\n\
This command adds a new user with the login 'jimmy0' which has to \n\
identify with password 'hyper'. His root directory will be '/tmp'.\n\
He can use up to 5 logins at once. He will be allowed to download \n\
and upload files.\n\n\
Example: add john12 * /home/john12 0 M\n\
This command adds a new user with the login 'john12' which doesn't need to \n\
identify with a password. His root directory will be '/home/john12'.\n\
He can use unlimited logins at once and has got ALL permissions.\n",
  "Syntax: del <login>\n\n'del' will delete the user <login> from the \
current login database.\nIt doesn't modify the rc-file.\n",
  "Syntax: reg\n\n'reg' will display all available login from the currently \
used login database.\n",
  "Syntax: perm <nr> <permissions>\n\n'perm' will modify the permissions of \
a login account in the current login database.\nIt doesn't modify the \
rc-file.\n\nPermission types:\n\
D = download\n\
U = upload\n\
O = overwrite\n\
M = multiple logins\n\
E = erase\n\
A = MASTER MODE (Be careful! It enables EVERYTHING!)\n",
  "Syntax: init\n\n'init' closes all connections, rereads the rc-file \
and restarts \nthe server. (Be careful! It has the same effect as \
restarting the process.)\n",
  "Syntax: down\n\n'down' will set the server to restricted mode.\n\
That means that no further logins from non-MASTER users will be accepted.\n",
  "Syntax: fdown\n\n'fdown' will set the server to down mode.\n\
That means that no further logins from ANY even MASTER users will be \
accepted.\n",
  "Syntax: ban [mask]\n\n'ban' will show the current server ban-list.\n\
'ban <mask>' will ban servers by name or IP. \"*\" and \"?\" are allowed.\n",
  "Syntax: unban [<nr>]\n\n'unban' will show the current server ban-list.\n\
'unban <nr>' will unban the user number <nr> from the ban-list.\n"
};

void docmd_help(char *arg)
{
  int n;

  if (arg==NULL) {
    printf("\nAvailable commands:\n");
    for (n=1;n<CMD_COUNT;n++)
      printf("%s%s - %s\n",commands[n],&spacer[strlen(commands[n])],
	     commandhelp[n]);
    
    printf("\n");
  } else {
    for (n=0;n<strlen(arg);n++)
      arg[n]=toupper(arg[n]);
    
    if (strlen(arg)<1) {
      printf("Use: help [command]\n");
      return;
    }
    for (n=0;n<strlen(arg);n++)
      if (arg[n]==' ') break;
    
    if (arg[n]==' ')
      arg[n]=0;


    for (n=0;n<CMD_COUNT;n++)
      if (!strcmp(commands[n],arg)) break;

    if (n<CMD_COUNT)
      printf("\n%s\n",commandlonghelp[n]);
    else printf("\nSorry, I don't know anything about this command.\n\n");
  }
}

void docmd_list(void)
{
  int n,i,nr;

  nr=0;
  for (n=0;n<maxusers;n++) {
    if (pchild[n]->sock) {
      printf("User %d",n+1);nr++;
      switch (pchild[n]->state) {
      case USR_USER:
	printf(", connected.\n");
	break;
      case USR_PASS:
	printf(", authorizing.\n");
	break;
      default:
	printf(" %s, dir: %s, ",pchild[n]->username,
	       pchild[n]->reldir);
	if (pchild[n]->pid==0 || waitpid(pchild[n]->pid,NULL,WNOHANG)<0) {
	  i=time(NULL)-pchild[n]->idletime;
	  printf("idle: %d secs.\n",(int)i);
	} else {
	  printf("uses pid: %d.\n",pchild[n]->pid);
	}
	break;
      }
    }
  }
  if (nr==0) printf("No users connected.\n");
  
  return;
}

void docmd_who(char *arg)
{
  int n,nr=0;

  if (arg==NULL || strlen(arg)==0) {
    nr=0;
    for (n=0;n<maxusers;n++) {
      if (pchild[n]->sock) {
	printf("User %d, %s (%d.%d.%d.%d)\n",n+1,pchild[n]->url,
	       pchild[n]->userip[0],pchild[n]->userip[1],
	       pchild[n]->userip[2],pchild[n]->userip[3]);
	nr++;
      }
    }
    if (nr==0) printf("No users connected.\n");
  } else {
    n=atoi(arg)-1;
    if (n>=0 && n<maxusers && pchild[n]->sock) {
      printf("User %d, %s (%d.%d.%d.%d)\n",n+1,pchild[n]->url,
	     pchild[n]->userip[0],pchild[n]->userip[1],
	     pchild[n]->userip[2],pchild[n]->userip[3]);
    } else printf("No such user.\n");
  }
}

void docmd_kill(char *arg)
{
  int nr;

  if (arg==NULL || strlen(arg)==0) {
    printf("Kill <usernr>\n");
    return;
  } else {
    nr=atoi(arg);
    if (nr==0 || nr>=maxusers) return;
    kill_connection(nr-1);
  }
}

void docmd_stop(char *arg)
{
  int nr;

  if (arg==NULL || strlen(arg)==0) {
    printf("Stop <usernr>\n");
    return;
  } else {
    nr=atoi(arg)-1;
    if (nr<0) return;
    /* checking retr subprocess */
    if (pchild[nr]->pid) kill(pchild[nr]->pid,SIGTERM);
    pchild[nr]->pid=0;
    if (pchild[nr]->pasvsock) {
      close(pchild[nr]->pasvsock);
      pchild[nr]->pasvsock=0;
    }
    user_return(nr,RET_591);
  }
}

void docmd_quit(void)
{
  int nr;

  for (nr=0;nr<maxusers;nr++) {
    if (pchild[nr]->sock) {
      user_return(nr,RET_428);
      kill_connection(nr);
    }
  }

  serverstop=1;
}

void docmd_init(void)
{
  int nr;

  for (nr=0;nr<maxusers;nr++) {
    if (pchild[nr]->sock) {
      user_return(nr,RET_429);
      kill_connection(nr);
    }
  }

  serverstop=1;reinit=1;
}

void docmd_root(char *param)
{
  int nr;
  char newroot[520],newroot2[MAXPATHLEN];
  /*  int i;*/

  if (param==NULL || sscanf(param,"%d %512s",&nr,newroot)<2) {
    printf("Root <usernr> <newrootdir>\n");
    return;
  } else {
    if (!realpath(newroot,newroot2)) {
      printf("Illegal root directory.\n");
      return;
    } else {
      if (nr<0 || nr>=maxusers || pchild[nr-1]->sock==0) {
	printf("Illegal user id.\n");
	return;
      }
      strcpy(pchild[nr-1]->rootdir,newroot2);
      strcpy(pchild[nr-1]->reldir,"/");
      printf("Root changed for user %d",nr);
    }
  }
}

void docmd_del(char *param)
{
  int m,n;
  char name[70];

  if (param==NULL || sscanf(param,"%64s",name)<1) {
    printf("Syntax: Del <login>\n");
    printf("        Use REG to get the possible logins\n");
    return;
  } else {
    for (n=0;n<logininfocount;n++) {
      if (!strcmp(logininfo[n]->username,name))
	break;
    }
    if (n>=logininfocount) {
      printf("User %s doesn't exist.\n",name);
      return;
    }

    free(logininfo[n]);
    for (m=n+1;m<logininfocount;m++) logininfo[m-1]=logininfo[m];
    logininfocount--;

    printf("User %s deleted.\n",name);

    if (logininfocount<=0)
      printf("Warning: no more users defined.\n");
  }
}

void docmd_reg(void)
{
  int n,m;
  char perm[16];

  for (n=0;n<logininfocount;n++) {
    printf("%d: %s %s (max: ",n+1,logininfo[n]->username,
	   logininfo[n]->root);

    if (logininfo[n]->maxlogins) printf("%d",logininfo[n]->maxlogins);
    else printf("infinite");
    
    m=logininfo[n]->perm;
    if (m==PERM_ALL) strcpy(perm,"A");
    else {
      perm[0]=0;
      if (m&PERM_DOWNLOAD) strcat(perm,"D");
      if (m&PERM_UPLOAD) strcat(perm,"U");
      if (m&PERM_ERASE) strcat(perm,"E");
      if (m&PERM_OVERWRITE) strcat(perm,"O");
      if (m&PERM_MULTIPLE) strcat(perm,"M");
      if (!perm[0]) strcat(perm,"-");
    }
    printf(") [%s]\n",perm);
  }

  if (logininfocount<=0)
    printf("Warning: no users defined.\n");
}

void docmd_perm(char *param) {
  char username[70],permstr[40];
  int nr,p;

  if (param==NULL || sscanf(param,"%64s %32s",username,permstr)<2) {
    printf("Syntax: PERM <login> <permissions>\n");
    printf("        Use REG to get the possible logins.\n");
    printf("        Infos about permissions are in the .conf-file.\n");
    return;
  } else {
    if ((nr=getuserbyname(username))<0) {
      printf("User %s doesn't exist.\n",username);
      return;
    }

    p=translatepermissions(permstr);
    if (p<0) {
      printf("Invalid permissions '%s'.\n",permstr);
      printf("Permissions NOT changed.\n");
      return;
    } else {
      logininfo[nr]->perm=p;
      printf("Permissions changed for user %s.\n",username);
    }
  }
}

void docmd_down(int flag)
{
  int nr;

  if (!flag) {
    for (nr=0;nr<logininfocount;nr++) free(logininfo[nr]);
    logininfocount=0;
  } else {
      for (nr=0;nr<logininfocount;nr++) {
	if (logininfo[nr]->perm !=PERM_ALL) {
	  docmd_del(logininfo[nr]->username);
	  nr--;
	}
      }
  }
  
  for (nr=0;nr<maxusers;nr++)
    if (pchild[nr]->sock) {
      pchild[nr]->perm=PERM_NONE;
    }

  if (flag)
    printf("Server in restricted mode now.\n");
  else printf("Server is down now.\n");
}

void execcmd(char *cmd)
{
  int n;
  char *param=NULL;
 
  if (strlen(cmd)<2) return;
  for (n=0;n<strlen(cmd);n++)
    if (cmd[n]==' ') break;

  if (cmd[n]==' ') {
    cmd[n]=0;
    param=cmd+n+1;
  }

  for (n=0;n<strlen(cmd);n++)
    cmd[n]=toupper(cmd[n]);
  
  for (n=0;n<CMD_COUNT;n++)
    if (!strcmp(commands[n],cmd)) break;

  switch (n) {
  case CMD_LIST:
    docmd_list();
    break;
  case CMD_HELP:
    docmd_help(param);
    break;
  case CMD_KILL:
    docmd_kill(param);
    break;
  case CMD_STOP:
    docmd_stop(param);
    break;
  case CMD_QUIT:
    docmd_quit();
    break;
  case CMD_WHO:
    docmd_who(param);
    break;
  case CMD_ROOT:
    docmd_root(param);
    break;
  case CMD_ADD:
    add_user(param);
    break;
  case CMD_DEL:
    docmd_del(param);
    break;
  case CMD_REG:
    docmd_reg();
    break;
  case CMD_PERM:
    docmd_perm(param);
    break;
  case CMD_INIT:
    docmd_init();
    break;
  case CMD_DOWN:
    docmd_down(1);
    break;
  case CMD_FDOWN:
    docmd_down(0);
    break;
  case CMD_BAN:
    add_ban(param);
    break;
  case CMD_UNBAN:
    remove_ban(param);
    break;
  default:
    printf("I don't know about the command '%s'.\n",cmd);
    printf("Use HELP to display available commands.\n\n");
    break;
  }
}
