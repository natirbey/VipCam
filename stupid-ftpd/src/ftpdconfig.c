
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#ifdef __CYGWIN__
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netdb.h>
#include <netinet/in.h>
#endif
#include "ftpdconfig.h"

#ifdef __CYGWIN__
#define DEFAULT_CONFPATH "C:\\WINDOWS\\Stupid-FTPD.INI"
#else
#define DEFAULT_CONFPATH "/etc/stupid-ftpd/stupid-ftpd.conf"
#endif
#define DEFAULT_BANMSG "You are BANNED from this server.\r\n"

static char inifilename[512];

#define OPTS_COUNT 13
const char *options[OPTS_COUNT]={
  "SERVERROOT","MAXUSERS","PORT","LOGIN-TIMEOUT","USER",
  "TIMEOUT","MODE","LOG","MOTD","BYEMSG","BAN","BANMSG",
  "CHANGEROOTTYPE"
};
#define OPT_ROOT            0
#define OPT_MAXUSERS        1
#define OPT_PORT            2
#define OPT_LOGINTIMEOUT    3
#define OPT_USER            4
#define OPT_TIMEOUT         5
#define OPT_MODE            6
#define OPT_LOG             7
#define OPT_MOTD            8
#define OPT_BYEMSG          9
#define OPT_BAN            10
#define OPT_BANMSG         11
#define OPT_CHANGEROOTTYPE 12
#define OPT_COMMENT        0xFFFF

char serverroot[MAXPATHLEN+4];
char motdfile[MAXPATHLEN+4];
char byemsgfile[MAXPATHLEN+4];
char banmessage[72];
int serverport=21;
int maxusers=10;
int logintimeout=180;
int idletimeout=240;
unsigned int transfertimeout=240;
int daemonmode=0;
FILE *logfile=NULL;
FILE *fpid=NULL;
int realchangeroot=1;

const char *SERVER_PROMPT="*ftpd> ";

logininfo_t *logininfo[LOGININFO_COUNT];
int logininfocount=0;

banlist_t banlist[BANMAX_COUNT];
int bancount=0;

/* prototypes */
void setpermissions(int,char *);
int optindex(char *);
void strtoupper(char *);


void out_prompt(void) {
  printf("%s",SERVER_PROMPT);
  fflush(stdout);
}

void strtoupper(char *s) {
  int n;

  for (n=0;n<strlen(s);n++)
    s[n]=toupper(s[n]);
}

int translatepermissions(char *perm)
{
  int n,p=PERM_NONE;
  if (perm==NULL) return PERM_NONE;

  for (n=0;n<strlen(perm);n++)
    switch (perm[n]) {
    case 'D':
      p|=PERM_DOWNLOAD;
      break;
    case 'U':
      p|=PERM_UPLOAD;
      break;
    case 'O':
      p|=PERM_OVERWRITE;
      break;
    case 'M':
      p|=PERM_MULTIPLE;
      break;
    case 'E':
      p|=PERM_ERASE;
      break;
    case 'A':
      p|=PERM_ALL;
      break;
    default:
      return -1;
    }
  return p;
}

void setpermissions(int nr,char *perm)
{
  int p;

  p=translatepermissions(perm);
  logininfo[nr]->perm= p<0 ? PERM_NONE : p;

}

int optindex(char *opt)
{
  int n;

  strtoupper(opt);

  n=0;while (opt[n]==' ' && n<strlen(opt)) n++;
  if (opt[n]=='#' || opt[n]==0) return OPT_COMMENT;
  
  for (n=0;n<OPTS_COUNT;n++)
    if (!strcmp(opt,options[n])) return n;

  return -1;
}

void add_user(char *arg) {
  char perm[40];

  if (logininfocount<LOGININFO_COUNT) {
    logininfo[logininfocount]=(logininfo_t *)malloc(sizeof(logininfo_t));
    if (logininfo[logininfocount]==NULL) {
      printf("Warning: out of memory.\n");
      return;
    }
    if (arg==NULL ||
	sscanf(arg,"%16s %32s %512s %d %32s",
	       logininfo[logininfocount]->username,
	       logininfo[logininfocount]->passwd,
	       logininfo[logininfocount]->root,
	       &logininfo[logininfocount]->maxlogins,
	       perm)<5) {
      printf("Invalid user registration format.\n");
      printf("The correct format is: <login> <passwd> <subdir> "
	     "<maxlogins> <flags>\n");
      free(logininfo[logininfocount]);
      
      return;
    }

    if (!strcmp(logininfo[logininfocount]->username,"ftp"))
      strcpy(logininfo[logininfocount]->username,"anonymous");
    setpermissions(logininfocount,perm);
    if (!daemonmode) {
      printf("User %s registered.\n",logininfo[logininfocount]->username);
    }

    logininfocount++;
  } else
    printf("Warning: Too many users (>%d) specified.\n",LOGININFO_COUNT);
}

void add_ban(char *arg) {
  int n;

  if (arg==NULL || strlen(arg)<1) {
    printf("Banlist:\n");
    
    for (n=0;n<bancount;n++) {
      printf("%3d: %s\n",n+1,banlist[n]);
    }

    printf("End of banlist.\n");
  } else {
    /* add a new banned user ip */

    if (bancount>=BANMAX_COUNT) {
      printf("Warning: Banlist is full.\n");
      printf("         Ban will NOT be added.\n");
      return;
    }

    for (n=0;n<bancount;n++)
      if (!strcmp(banlist[n],arg)) break;

    if (n<bancount) {
      printf("This ban already exists (%d).\n",n+1);
      return;
    }

    banlist[bancount]=(char *)malloc(strlen(arg)+1);
    strcpy(banlist[bancount],arg);

    bancount++;

    if (!daemonmode)
      printf("Adding ban %d for the IP '%s'\n",bancount,arg);

  }
}

void remove_ban(char *arg) {

  int nr=0;

  if (arg==NULL || strlen(arg)<1) {
    /* display banlist */

    add_ban(NULL);
  } else {

    nr=atoi(arg);
    if (nr<1 || nr>bancount) {
      printf("No such banned user: %d\n",nr);
    } else {

      printf("Removing ban for user %d with ban-mask '%s'\n",nr,
	     banlist[nr-1]);

      nr--;
      free(banlist[nr]);
      memmove(&banlist[nr],&banlist[nr+1],(BANMAX_COUNT-nr-1)*sizeof(char *));
      bancount--;
    }
  }
}

#define OPT_MAXLEN 32
#define ARG_MAXLEN 1024

int readini() 
{
  FILE *fp;
  char line[2048],opt[OPT_MAXLEN+4],arg[ARG_MAXLEN+4];
  /*  int n;*/

  if ((fp=fopen(inifilename,"r"))==NULL) {
    printf("Cannot find %s.\n",inifilename);
    printf("Make sure it's available.\n");
    exit(1);
  }

  while (!feof(fp)) {
    char *p;
    unsigned int n;

    if (fgets(line,sizeof(line),fp)==NULL) break; /* end-of-file */
    if (strlen(line)<3) continue; /* line empty */

    /* check if comment */
    n=0;
    while (line[n]<=' ' && n<strlen(line)) n++;
    if (line[n]=='#') {
      /* comment */
      continue;
    }
    
    if ((p=strchr(line,'='))==NULL) {
      printf("Warning: line ignored: %s\n",line);
      continue;
    }

    *p++=0;
    if (strlen(line)>OPT_MAXLEN) {
      printf("Warning: unknown option: %s\n",line);
      continue;
    }

    if (strlen(p)>ARG_MAXLEN) {
      printf("Warning: option value too long: %s\n",p);
      continue;
    }

    strcpy(opt,line);
    strcpy(arg,p);

    if (strlen(arg)<2) continue;
    if (strchr(arg,'\n')) *strchr(arg,'\n')=0;
    if (strchr(arg,'\r')) *strchr(arg,'\r')=0;
    
    switch (optindex(opt)) {
    case -1:
      printf("Warning unknown option: %s.\n",opt);
      break;
    case OPT_COMMENT:
      break;
    case OPT_ROOT:
      if (strlen(serverroot)>MAXPATHLEN) {
	printf("serverroot option argument is too long.");
	fclose(fp);
	return -1;
      }
      strcpy(serverroot,arg);
      if (serverroot[strlen(serverroot)-1]=='/')
	serverroot[strlen(serverroot)-1]=0;
      break;
    case OPT_PORT:
      serverport=atoi(arg);
      break;
    case OPT_MAXUSERS:
      maxusers=atoi(arg);
      if (maxusers>100) {
	printf("MAXUSERS limit is currently: %d\n",HARD_USERLIMIT);
	maxusers=HARD_USERLIMIT;
      }
      break;
    case OPT_LOGINTIMEOUT:
      logintimeout=atoi(arg);
      break;
    case OPT_USER:
      add_user(arg);
      break;
    case OPT_TIMEOUT:
      idletimeout=atoi(arg);
      break;
    case OPT_MODE:
      if (arg==NULL) break;

      strtoupper(arg);
      if (!strcmp(arg,"DAEMON")) daemonmode=1;
      break;
    case OPT_LOG:
      if (arg==NULL || strlen(arg)==0) break;
      logfile=fopen(arg,"a");
      break;
    case OPT_MOTD:
      if (arg==NULL || strlen(arg)==0) {
	motdfile[0]=0;
	break;
      }
      if (strlen(arg)>MAXPATHLEN) {
	printf("motd file argument is too long.\n");
	fclose(fp);
	return -1;
      }
      strcpy(motdfile,arg);
      break;
    case OPT_BYEMSG:
      if (arg==NULL || strlen(arg)==0) {
	byemsgfile[0]=0;
	break;
      }
      if (strlen(arg)>MAXPATHLEN) {
	printf("bymsg file argument is too long.\n");
	fclose(fp);
	return -1;
      }
      strcpy(byemsgfile,arg);
      break;
    case OPT_BAN:
      add_ban(arg);
      break;
    case OPT_BANMSG:
      if (arg==NULL || strlen(arg)==0) {
	banmessage[0]=0;
	break;
      }
      if (strlen(arg)<=70)
	strcpy(banmessage,arg);
      else {
	printf("Warning: Ban message is longer than 70 characters.\n");
	fclose(fp);
	return -1;
      }
      break;
    case OPT_CHANGEROOTTYPE:

      realchangeroot=1; /* default is REAL, default in config is VIRTUAL */
      if (arg==NULL || strlen(arg)==0) break;

      strtoupper(arg);
      if (!strcmp(arg,"VIRTUAL")) realchangeroot=0;
      break;
    }
  }
  fclose(fp);
  return 0;
}

void preinit(void)
{
  serverroot[0]=0;
  serverport=21;
  maxusers=10;
  logintimeout=20;
  idletimeout=60;
  transfertimeout=60;
  logfile=NULL;
  logininfocount=0;
  strcpy(banmessage,DEFAULT_BANMSG);
}

void parsecmdline(int argc,char **argv)
{
  int done=0;

  strcpy(inifilename,DEFAULT_CONFPATH);

  while (!done) {
    switch (getopt(argc,argv,"p:f:")) {
    case 'p':
      fpid = fopen(optarg,"w");
      break;
    case 'f':
      if (strlen(optarg)>=sizeof(inifilename)) {
	printf("Configuration path in argument is too long.\n");
	exit(-1);
      }
      strcpy(inifilename,optarg);
      break;
    case -1:
      done=1;
      break;
    default:
      printf("Syntax: stupid-ftp [-f conffile] [-p pidfile]\n\n");
      break;
    }
  }
}

int checkconf(void)
{
  if (logininfocount<1) {
    printf("No login accounts defined.\n");
    return 0;
  }

  return 1;
}

/* returns user-index or -1 when doesn't exist */
int getuserbyname(char *name)
{
  int n;

  for (n=0;n<logininfocount;n++) {
    if (!strcmp(logininfo[n]->username,name))
      break;
  }

  return n>=logininfocount ? -1 : n;
}

unsigned char *getmyip(int sock) {

  static unsigned char myip[4];
  struct sockaddr_in sa;
  int size;

  size=sizeof(struct sockaddr_in);
  memset(myip,0,sizeof(myip));
  if (getsockname(sock,(struct sockaddr *)&sa, &size)==0) {
    memcpy(myip,&sa.sin_addr,sizeof(myip));
  }

  return myip;
}
