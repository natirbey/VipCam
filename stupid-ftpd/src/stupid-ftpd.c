
/*

  stupid-ftpd.c
  ------------

  
  Main program. Bind/Listen/Accept routines.

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <signal.h>
#ifdef __CYGWIN__
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define INVALID_SOCKET -1
#endif
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

#include "stupid-ftpd.h"
#include "ftpdconfig.h"
#include "ftpcommand.h"
#include "servercommand.h"
#include "ftperror.h"
#include "ls.h"

#define REACTION_TIME 1L

child_t *pchild[HARD_USERLIMIT];

int server_socket=0;
int serverstop=0;
int reinit=1;


/* prototypes */

void init_login(void);
void check_timeouts(void);
void cleanchild(int);
void listener_loop(void);
void startserver(void);
int process_banned(int,char *);
void do_changeroot(void);
void do_writepidfile(void);


int process_banned(int sock,char *host) {
  int n;
  char buffer[128];

  for (n=0;n<bancount;n++) 
    if (list_match(host,banlist[n])) {
#ifdef DEBUG
      fprintf(stderr,"Ban hit '%s'\n",host);
#endif
      if (banmessage[0]!=0) {
	sprintf(buffer,"540 %s\r\n",banmessage);
	write(sock,buffer,strlen(buffer));
      }
      close(sock);
      return 1;
    }

  return 0;
}

void init_login(void) {
  int nr,sock,sock_size;
  char buffer[128];
  struct sockaddr_in sa;
  struct hostent *hp;
  unsigned char *i;

  sock_size=sizeof(sa);
  if ((sock=accept(server_socket,(struct sockaddr *)&sa,&sock_size))<0) {
    if (!daemonmode)
      printf("Error: accept failed.\n");
    return;
  }
  
  for (nr=0;nr<maxusers;nr++) 
    if (!pchild[nr]->sock) break;
  if (nr>=maxusers) {
    if (!daemonmode) {
      printf("Info: Server is full.\n");
      out_prompt();
    }
    sprintf(buffer,"530 Server is busy, sorry.\r\n");
    write(sock,buffer,strlen(buffer));
    close(sock);
    return;
  }
  i=(unsigned char *)&sa.sin_addr;

  pchild[nr]->userip[0]=*i++;
  pchild[nr]->userip[1]=*i++;
  pchild[nr]->userip[2]=*i++;
  pchild[nr]->userip[3]=*i++;

  sprintf(buffer,"%d.%d.%d.%d",
	  pchild[nr]->userip[0],pchild[nr]->userip[1],
	  pchild[nr]->userip[2],pchild[nr]->userip[3]);

  if (process_banned(sock,buffer)) return;

  hp=gethostbyaddr((char *)&sa.sin_addr,sizeof(long int),AF_INET);
  if (hp!=NULL) {
    strncpy(pchild[nr]->url,hp->h_name,sizeof(pchild[nr]->url)-1);
    *(pchild[nr]->url+strlen(hp->h_name))=0;
  } else strcpy(pchild[nr]->url,"unknown");

  if (process_banned(sock,pchild[nr]->url)) return;

  pchild[nr]->sock=sock;
  pchild[nr]->idletime=time(NULL);
  strcpy(pchild[nr]->reldir,"/");
  strcpy(pchild[nr]->rootdir,"???*???"); /* better */
  pchild[nr]->state=USR_USER;
  pchild[nr]->resume=0;
  pchild[nr]->pasvsock=0;
  pchild[nr]->pid=0;
  pchild[nr]->dataport=0;
  pchild[nr]->perm=PERM_NONE;
  if (!daemonmode) {
    printf("User %d, connected (IP: %d.%d.%d.%d).\n",nr+1,
	   pchild[nr]->userip[0],
	   pchild[nr]->userip[1],
	   pchild[nr]->userip[2],
	   pchild[nr]->userip[3]);
    out_prompt();
  }

  user_return(nr,RET_220);
}

void check_timeouts(void)
{
  int n;
  time_t t;

  /* check the timeouts of all 3 phases */
  t=time(NULL);
  for (n=0;n<maxusers;n++) {
    if (pchild[n]->sock) {
      switch (pchild[n]->state) {
      case USR_ONLINE:
	if (pchild[n]->pid==0 || waitpid(pchild[n]->pid,NULL,WNOHANG)<0) {
	  pchild[n]->pid=0;
	  if (t-pchild[n]->idletime>=idletimeout) {
	    handle_timeout(n);
	  }
	} else pchild[n]->idletime=t;
	break;
      case USR_USER:
      case USR_PASS:
	if (t-pchild[n]->idletime>=logintimeout) handle_timeout(n);
	break;
      default: /* better */
	handle_suspects(n);
	break;
      }
    }
  }
}

void cleanchild(int nr) {
#ifdef WAIT_DEBUG
	pid_t proc;

	fprintf(stderr,"\nEntered in cleanchild\n");

	while ((proc=wait3(NULL, WNOHANG, NULL)) > 0);

	fprintf(stderr,"\nLeft cleanchild\n");
#else
	while (wait3(NULL, WNOHANG, NULL) > 0);
#endif
#ifdef __SVR4 
	sigset(SIGCHLD,cleanchild);
#else
	signal(SIGCHLD,cleanchild);
#endif
}

void listener_loop(void) 
{
  int i,n,addr;
  unsigned int sock_size;
  fd_set fds;
  struct timeval tv;
  char cmd[1024];
  struct sockaddr_in sa;
#ifdef __CYGWIN__
  WORD verreq;
  WSADATA wsadata;
#endif

  /* catch broken pipe ! */

#ifdef __SVR4 
  sigignore(SIGPIPE);
  sigset(SIGCHLD,cleanchild);
#else
  signal(SIGPIPE,SIG_IGN);
  signal(SIGCHLD,cleanchild);
#endif

  /*
  myip=getmyip();
  if (myip==NULL) {
    printf("Determining localhost-IP failed.\n");
    exit(1);
  }

#ifdef DEBUG
  printf("My IP is: %d.%d.%d.%d\n",
	 myip[0],myip[1],myip[2],myip[3]);
#endif
  */

  /* server init */

#ifdef __CYGWIN__
  verreq=MAKEWORD(1,1);
  i=WSAStartup(verreq,&wsadata);
  if (i!=0) {
    printf("Error: winsock-lib couldn't be initialized.");
  }
#endif

  if (!daemonmode)
    printf("Starting server ... ");
  if ((server_socket=socket(PF_INET,SOCK_STREAM,0))==INVALID_SOCKET) {
#ifdef __CYGWIN__
    i=WSAGetLastError();
#else
    i=errno;
#endif
    printf("Error: could not create server socket (%d).\n",i);
    exit(1);
  }

  i=1;
  if (setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,(void *)&i,sizeof(i))<0)
    printf("setsockopt failed.\n");
  
  sock_size=sizeof(struct sockaddr_in);
  memset(&sa,0,sock_size);
  
  sa.sin_family=PF_INET;
  sa.sin_port=htons(serverport);
  addr=INADDR_ANY;
  memcpy(&sa.sin_addr.s_addr,&addr,sizeof(int));
  
  if (bind(server_socket,(struct sockaddr *)&sa,sock_size)<0) {
    printf("Error: bind failed.\n");
    printf("       port %i already in use ?\n",serverport);
    exit(1);
  }

  if (listen(server_socket,5)<0) {
    printf("Error: listen failed.\n");
    exit(1);
  }

  if (!daemonmode) {
    printf("OK\n\n");
    out_prompt();
  }

#ifdef DEBUG
  printf("Listening ...\n");
#endif

  serverstop=0;
  while (!serverstop) {
    FD_ZERO(&fds);
    if (!daemonmode) FD_SET(1,&fds);
    FD_SET(server_socket,&fds);
    for (i=0;i<maxusers;i++)
      if (pchild[i]->sock)
	FD_SET(pchild[i]->sock,&fds);

    /*    memset(&tv,0,sizeof(struct timeval)); */
    tv.tv_sec=REACTION_TIME;tv.tv_usec=0L;
    n=select(32+maxusers*4,&fds,NULL,NULL,&tv);

    if (n<0) {
      if (errno==EINTR) continue;
      if (
#ifdef __CYGWIN__
	  1) {
	i=WSAGetLastError();
#else
	!daemonmode) {
	i=errno;
#endif
	fprintf(stderr,"*** Fehler: select (ret: %d,errno=%d).\n",n,i);
      }
      exit(1);
    }

    if (!daemonmode)
      if (FD_ISSET(1,&fds)) {
	i=read(1,cmd,sizeof(cmd));
	if (i<1) continue;
	cmd[i-1]=0;
	execcmd(cmd);
	if (serverstop) break;
	out_prompt();
      }

    if (FD_ISSET(server_socket,&fds)) init_login();

    for (i=0;i<maxusers;i++) {
      if (pchild[i]->sock) {
	if (FD_ISSET(pchild[i]->sock,&fds))
	  serve_child(i);
      }
    }

    check_timeouts();
  }

  close(server_socket);
#ifdef __CYGWIN__
  WSACleanup();
#endif

  if (!daemonmode)
    printf("Server stopped.\n");
}

void startserver(void)
{
  int n;

  if (logfile) fprintf(logfile,"Server restarted.\n");
  for (n=0;n<maxusers;n++) {
    pchild[n]=(child_t *)malloc(sizeof(child_t));
    pchild[n]->sock=0;
  }
  
  listener_loop();
}

void do_writepidfile() {
  /* PID-file processing  */
  if ( fpid != NULL ) {
    fprintf(fpid,"%d\n",(int)getpid());
    fflush(fpid);
    fclose(fpid);
  }
}

void do_changeroot() {

  if (realchangeroot) {

    if (getuid()!=0) {
      realchangeroot=0;
      if (!daemonmode) printf("Chroot only works with root privileges.\n");
      return;
    }

    chdir(serverroot);
    chroot(serverroot);
  }
}

int main(int argc,char **argv)
{
  int n;

  for (n=0;n<HARD_USERLIMIT;n++)
    pchild[n]=(child_t *)NULL;
  for (n=0;n<LOGININFO_COUNT;n++)
    logininfo[n]=(logininfo_t *)NULL;

  parsecmdline(argc,argv);

  while (reinit) {
    reinit=0;
    preinit();
    
    if (readini()<0) exit(1);
    if (!checkconf()) exit(1);
    
    do_writepidfile();
    do_changeroot();

    if (daemonmode) {
      if (fork()==0) startserver();
      exit(0); /* better be sure */
    } else startserver();
    for (n=0;n<maxusers;n++)
      if (pchild[n]) {
	free(pchild[n]);
	pchild[n]=(child_t *)NULL;
      }
    for (n=0;n<logininfocount;n++)
      if (logininfo[n]) {
	free(logininfo[n]);
	logininfo[n]=(logininfo_t *)NULL;
      }
  }
  exit(0);
}

