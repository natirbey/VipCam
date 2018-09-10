/*

  ftpcommand.c
  ------------


  FTP-protocol request handlers.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef __CYGWIN__
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <signal.h>
#include <errno.h>

#include "ftpcommand.h"
#include "stupid-ftpd.h"
#include "ftpdconfig.h"
#include "ftperror.h"
#include "ls.h"

#define TRFMSG_INTERVAL 1000000

#define REQ_COUNT 23
const char *request[REQ_COUNT]={
  "SYST","QUIT","TYPE","PORT","LIST","CWD","PWD","RETR",
  "USER","PASS","REST","SIZE","MDTM","STOR","CDUP","NOOP","GET",
  "NLST","PASV","ABOR","MKD","RMD","DELE"
};

const char *ftp_returnstr[]={
  "150 OK\r\n",
  "501 Error in parameters.\r\n",
  "226 Transfer complete.\r\n",
  "221 Bye.\r\n",
  "425 Can't open passive connection.\r\n",
  "428 Server shutdown.\r\n",
  "429 Server restart.\r\n",
  "530 Server is busy, sorry.\r\n",
  "220 Welcome to Stupid-FTPd server.\r\n",
  "331 Guest login ok, send your e-mail address as password.\r\n",
  "531 Anonymous access denied.\r\n",
  "532 Login incorrect.\r\n",
  "533 No multiple logins allowed.\r\n",
  "534 Home directory not found.\r\n",
  "422 Too busy to play with you.\r\n",
  "423 Connection timed out.\r\n",
  "215 UNIX Type: L8 (Linux).\r\n",
  "500 Unknown type.\r\n",
  "502 Illegal PORT request.\r\n",
  "200 PORT command successful.\r\n",
  "250 CWD command successful.\r\n",
  "553 Permission denied.\r\n",
  "350 Resume request successful.\r\n",
  "451 Transmition error occured.\r\n",
  "209 Received break, bye.\r\n",
  "503 Command not understood.\r\n",
  "227 Abort successful.\r\n",
  "591 Data transmition stopped by admin.\r\n",
  "491 Download in progress.\r\n",
  "257 MKD command successful.\r\n",
  "258 RMD command successful.\r\n",
  "259 File successfully deleted.\r\n"
  "540 User banned.\r\n",
  "540 User banned.\r\n",
  "550 not a file.\r\n",
  NULL
};

/* prototypes */
void user_spool(int,int,char *);
int checkpath(int,char *,char *);
void childtimeout(int);
int waitaccept(int);
void do_list(int,char *,int);
int do_chdir(int,char *);
int do_retr(int,char *);
int do_stor(int,char *);
void do_user(int,char *);
void do_pass(int,char *);
void do_size(int,char *);
void do_mdtm(int,char *);
void do_pasv(int);
int do_mkdir(int,char *);
int do_rmdir(int,char *);
int do_dele(int,char *);


void user_return(int nr,int id)
{
  /*  printf("Sending '%s'\n",ftp_returnstr[id]); */
  write(pchild[nr]->sock,ftp_returnstr[id],strlen(ftp_returnstr[id]));
}

void user_spool(int nr,int id,char *file)
{
  char buffer[1024],marker[32];
  FILE *fp;
  fd_set fds;
  struct timeval tv;
  int n;

  if (file[0]!=0) {
    if ((fp=fopen(file,"r"))!=NULL) {
      n=0;
      while (fgets(buffer,sizeof(buffer)-1,fp)!=NULL) {
	if (n==0) sprintf(marker,"%d- ",id);
	write(pchild[nr]->sock,marker,strlen(marker));

	if ((n=strlen(buffer))>0) {
	  if (buffer[n-1]=='\n') buffer[n-1]=0;
	  strcat(buffer,"\r\n");
	}
	do {
	  FD_ZERO(&fds);
	  FD_SET(pchild[nr]->sock,&fds);
	  tv.tv_sec=transfertimeout;tv.tv_usec=0;
	  
	  if (select(pchild[nr]->sock+1,NULL,&fds,NULL,&tv)<=0) {
#ifdef DEBUG
	    fprintf(stderr,"Spool (%d) timeout to client %d.\n",id,nr+1);
#endif
	    kill_connection(nr);
	    return;
	  }
	} while (!FD_ISSET(pchild[nr]->sock,&fds));
	
	write(pchild[nr]->sock,buffer,strlen(buffer));
      }
    }
  }
}

int checkpath(int nr,char *param,char *path)
{
  char cmd[MAXPATHLEN+4];
  char allowed[MAXPATHLEN+4];

  if (!strcmp(pchild[nr]->rootdir,"/")) {
    strcpy(allowed,pchild[nr]->rootdir);
  }
  else {
    /* should not happen */
    if (strlen(pchild[nr]->rootdir)>=MAXPATHLEN) return 1;
    sprintf(allowed,"%s/",pchild[nr]->rootdir);
  }

  if (strlen(allowed)+strlen(pchild[nr]->reldir)>=sizeof(cmd)) return 1;
  sprintf(cmd,"%s%s",allowed,pchild[nr]->reldir);
  if (param) {
    if (param[0]!='/') {
      if (strlen(cmd)+strlen(param)>=sizeof(cmd)) return 1;
      strcat(cmd,param);
      } else {
	if (strlen(allowed)+strlen(&param[1])>=sizeof(cmd)) return 1;
	strcpy(cmd,allowed);
	strcat(cmd,&param[1]);
      }
  }

#ifdef DEBUG
 fprintf(stderr,"Checking path %s\n",cmd);
#endif

  if (!realpath(cmd,path)) return 1;

#ifdef DEBUG
  fprintf(stderr,"Converted to: %s\n",path);
#endif

  if (strlen(path)>=MAXPATHLEN) return 1;
  strcat(path,"/");
  strcpy(cmd,path);
  cmd[strlen(allowed)]=0;

#ifdef DEBUG
  printf("Comparing (%s) '%s'=='%s'\n",
	 realchangeroot ? "real" : "virtual",
	 cmd,allowed);
#endif

  if (strcmp(cmd,allowed)) return 1;

#ifdef DEBUG
  printf("Debug: path has now %d/%d chars.\n",strlen(path),MAXPATHLEN);
#endif

  return 0;
}

void kill_connection(int nr)
{
  char buffer[256];
  time_t tm;

  if (!daemonmode) {
    printf("User %d, killed.\n",nr+1);
    out_prompt();
  }

  /* close passive port */
  if (pchild[nr]->pasvsock) {
    close(pchild[nr]->pasvsock);
    pchild[nr]->pasvsock=0;
  }

  /* checking retr subprocess */
  if (pchild[nr]->pid) kill(pchild[nr]->pid,SIGTERM);
  pchild[nr]->pid=0;

  /* close connection and tidy up */
  close(pchild[nr]->sock);
  pchild[nr]->sock=0;
  pchild[nr]->dataport=0;

  if (logfile!=NULL) {
    tm=time(NULL);
    strcpy(buffer,ctime(&tm));
    buffer[strlen(buffer)-1]=0;
    
    fprintf(logfile,"%s: LOGOUT: user %d ",buffer,nr+1);
    if (pchild[nr]->usertype<logininfocount)
      fprintf(logfile,"%s ",pchild[nr]->username);
    else fprintf(logfile,"nologin ");
    fprintf(logfile,"%s (%d.%d.%d.%d)\n",
	      pchild[nr]->url,
	      pchild[nr]->userip[0],pchild[nr]->userip[1],
	      pchild[nr]->userip[2],pchild[nr]->userip[3]);
    fflush(logfile);
  }
}

void childtimeout(int nr)
{
  exit(0);
}

int waitaccept(int nr)
{
  int socksize,soc;
  struct sockaddr_in sai;

  signal(SIGALRM,childtimeout);
  alarm(transfertimeout);
  socksize=sizeof(sai);
  soc=accept(pchild[nr]->pasvsock,(struct sockaddr *)&sai,&socksize);
  signal(SIGALRM,SIG_IGN);
  return soc;
}

int list_callback(int soc,int nr,char *line) {
  fd_set fds;
  struct timeval tv;

  do {
    FD_ZERO(&fds);
    FD_SET(soc,&fds);
    tv.tv_sec=transfertimeout;tv.tv_usec=0;
    
    if (select(soc+1,NULL,&fds,NULL,&tv)<=0) {
#ifdef DEBUG
      fprintf(stderr,"LIST timeout to client %d.\n",nr+1);
#endif
      close(soc);
      user_return(nr,RET_501);
      return 0;
    }
  } while (!FD_ISSET(soc,&fds));
    
  write(soc,line,strlen(line));

  return 1;
}

void do_list(int nr,char *param,int listtype) 
{
  struct hostent *hp;
  char cmd[1024],path[1024],mask[1024]/*,cwd[1024]*/;
  unsigned long addr;
  int soc,n;
  unsigned int socksize;
  struct sockaddr_in sai;
  char nullch[8];
  char *cmask;

  strcpy(nullch,".");
  if (pchild[nr]->pasvsock<=0 && pchild[nr]->dataport==0) {
    user_return(nr,RET_501);
    exit(0);
  }

  mask[0]=0;
  if (param) {
    while (param[0]=='-') {
      n=1;
      while (param[n]!=' ' && param[n]!=0) n++;
      if (param[n]==' ') param=&param[n+1];
      else param=&param[n];
    }
    strcpy(cmd,param);
    if (strrchr(cmd,'*') || strrchr(cmd,'?')) {
      if (strrchr(cmd,'/')) {
	strcpy(mask,strrchr(cmd,'/')+1);
	*strrchr(cmd,'/')=0;
      }
      else {
	strcpy(mask,cmd);
	cmd[0]=0;
      }
    }
    if (strrchr(cmd,'*') || strrchr(cmd,'?')) {
      user_return(nr,RET_501);
      exit(0);
    }
  } else {
    cmd[0]=0;
    param=nullch;
  }

  if (param[0]=='/') param++;
  if (param[0]=='/') {
    user_return(nr,RET_501);
    exit(0);
  }

  cmask=strrchr(mask,'/');
  if (cmask!=NULL) {
    *cmask=0;
    cmask++;

    if (strlen(cmd)+strlen(mask)+2>sizeof(cmd)) {
      user_return(nr,RET_501);
      exit(0);
    }
    strcat(cmd,"/");
    strcat(cmd,mask);
    strcpy(mask,cmask);
  }

#ifdef DEBUG
  printf("path before: %s\n",cmd);
#endif

  if (checkpath(nr,cmd,path) || !strncmp(mask,"..",2)) {
    user_return(nr,RET_501);
    exit(0);
  }

#ifdef DEBUG
  printf("path: %s\n",path);
#endif

  if (pchild[nr]->pasvsock<=0) {
    /* IP-CHECK needed !!! */
    sprintf(cmd,"%d.%d.%d.%d",
	    pchild[nr]->dataip[0],pchild[nr]->dataip[1],
	    pchild[nr]->dataip[2],pchild[nr]->dataip[3]);
    addr=(unsigned long)inet_addr(cmd);
    if ((int)addr==-1) {
      user_return(nr,RET_501);
      exit(0);
    }
    
    hp=gethostbyaddr((char *)&addr,sizeof(addr),AF_INET);
    
#ifdef DEBUG
    fprintf(stderr,"Making connection to: %s:%d\n",cmd,pchild[nr]->dataport);
#endif
    
    if ((soc=socket(AF_INET,SOCK_STREAM,0))<0) {
      user_return(nr,RET_501);
      exit(0);
    }
    socksize=sizeof(struct sockaddr_in);
    memset(&sai,0,socksize);
    sai.sin_family=AF_INET;
    sai.sin_port=htons(pchild[nr]->dataport);
    memcpy(&sai.sin_addr,&addr,sizeof(addr));
    if (connect(soc,(struct sockaddr *)&sai,socksize)<0) {
      user_return(nr,RET_501);
      exit(0);
    }
  } else {
    if ((soc=waitaccept(nr))<=0) {
      user_return(nr,RET_501);
      exit(0);
    }
  }

  /*  if (checkpath(nr,nullch,cwd)) {
    user_return(nr,RET_501);
    exit(0);
    }*/

  user_return(nr,RET_150);

  if (strlen(mask)==0) strcpy(mask,"*");

#ifdef DEBUG
  fprintf(stderr,"Calling list: %s, %s\n",path,mask);
#endif

  if (list(soc,nr,listtype,path,mask,list_callback))
    user_return(nr,RET_226);
  else user_return(nr,RET_501);

  close(soc);
  exit(0);

}

int do_chdir(int nr,char *param)
{
  char allowed[MAXPATHLEN+4],path[MAXPATHLEN+4];
  struct stat buf;

  if (checkpath(nr,param,path)) return 1;
  if (strlen(pchild[nr]->rootdir)>=MAXPATHLEN) return 1;
  sprintf(allowed,"%s/",pchild[nr]->rootdir);
  
  if (!stat(path,&buf)) {
    if (buf.st_mode & S_IFDIR) {
      char *np;

      np=path+strlen(allowed)-1;
      if (strlen(np)>MAXPATHLEN) return 1;
      strcpy(pchild[nr]->reldir,np);
    }
    else return 1;
  } else return 1;

  return 0;
}

int do_retr(int nr,char *param) 
{
  struct hostent *hp;
  FILE *fp;
  char cmd[2048],path[2048];
  unsigned long addr;
  int soc;
  unsigned int socksize,n;
  struct sockaddr_in sai;
  unsigned long bytestot,bytesnow,byteslast;
  time_t tm_start,tm;
  fd_set fds;
  struct timeval tv;
  struct stat statb;

  if (pchild[nr]->pasvsock<=0 && pchild[nr]->dataport==0) return RET_501;
  if (checkpath(nr,param,path)) return RET_501;

  if (path[strlen(path)-1]=='/')
    path[strlen(path)-1]=0;

  if (stat(path,&statb)) {
    return RET_501;
  }

  if (S_ISDIR(statb.st_mode)) {
    /*if (list(soc,nr,FORMAT_LONG,path,"*",list_callback))
      user_return(nr,RET_226);
      else user_return(nr,RET_501); */
    /*    close(soc);*/
    return RET_550;
  } else {
      if ((pchild[nr]->perm & PERM_DOWNLOAD)==0) {
	return RET_553;
      }
  }
  
  if (pchild[nr]->pasvsock<=0) {
    /* IP-CHECK needed !!! */
    sprintf(cmd,"%d.%d.%d.%d",
	    pchild[nr]->dataip[0],pchild[nr]->dataip[1],
	    pchild[nr]->dataip[2],pchild[nr]->dataip[3]);
    addr=(unsigned long)inet_addr(cmd);
    if ((int)addr==-1) return RET_501;
    
    hp=gethostbyaddr((char *)&addr,sizeof(addr),AF_INET);
    
    if ((soc=socket(AF_INET,SOCK_STREAM,0))<0) return RET_501;
    socksize=sizeof(struct sockaddr_in);
    memset(&sai,0,socksize);
    sai.sin_family=AF_INET;
    sai.sin_port=htons(pchild[nr]->dataport);
    memcpy(&sai.sin_addr,&addr,sizeof(addr));
    if (connect(soc,(struct sockaddr *)&sai,socksize)<0) return RET_501;
  } else {
    socksize=sizeof(sai);
    if ((soc=accept(pchild[nr]->pasvsock,(struct sockaddr *)&sai,
		    &socksize))<0) {
      return RET_501;
    }
  }
  
  if ((fp=fopen(path,"r"))==NULL) {
    close(soc);
    return RET_501;
  }
  fseek(fp,0,SEEK_END);
  
  bytestot=ftell(fp);bytesnow=byteslast=pchild[nr]->resume;
  sprintf(cmd,
	  "150 Opening BINARY mode data connection for '%s' (%ld bytes).\r\n",
	  param,bytestot);
  fseek(fp,pchild[nr]->resume,SEEK_SET);

  write(pchild[nr]->sock,cmd,strlen(cmd));

  if (!daemonmode) {
    printf("User %d, downloading: %s\n",nr+1,path);
    out_prompt();
  }

  tm_start=time(NULL)-1;
  while ((n=fread(cmd,1,sizeof(cmd),fp))>0) {
    do {
      FD_ZERO(&fds);
      FD_SET(soc,&fds);
      tv.tv_sec=transfertimeout;tv.tv_usec=0;
      if (select(soc+1,NULL,&fds,NULL,&tv)<=0) {
#ifdef DEBUG
	fprintf(stderr,"Send timeout to client %d.\n",nr+1);
#endif
	fclose(fp);
	close(soc);
	return RET_501;
      }
    } while (!FD_ISSET(soc,&fds));

    write(soc,cmd,n);
    bytesnow+=n;
    if (bytesnow-byteslast > TRFMSG_INTERVAL) {
      byteslast+=TRFMSG_INTERVAL;
      tm=time(NULL);

      if (!daemonmode) {
	printf("User %d, %ld/%ldkB (%ld%%) at %ldkB/s\n",nr+1,
	       bytesnow/1024,bytestot/1024,
	       (((bytesnow>>7)*100)/(bytestot>>7)),
	       (bytesnow/(tm-tm_start))/1024);
	out_prompt();
      }
    }
  }

  fclose(fp);
  
  close(soc);

  return RET_226;
}

int do_stor(int nr,char *param) 
{
  struct hostent *hp;
  FILE *fp;
  char cmd[2048],path[2048],path2[2048];
  unsigned long addr;
  int soc,ret;
  unsigned int socksize,n;
  long byteslast,bytesnow;
  struct sockaddr_in sai;
  time_t tm_start,tm;
  fd_set fds;
  struct timeval tv;

  if (pchild[nr]->pasvsock<=0 && pchild[nr]->dataport==0) return 1;
  if (param==NULL) return 1;

  if (strrchr(param,'/'))
    param=strrchr(param,'/')+1;
  if (strlen(param)==0) return 1;

  strcpy(cmd,".");
  if (checkpath(nr,cmd,path)) return 1;

  if (strlen(path)+strlen(param)+1>sizeof(path)) return 1;
  strcat(path,param);

  /* BUGFIX */


  if ((ret=readlink(path,path2,sizeof(path2)-1))>=0) {
    path2[ret]=0;

#ifdef DEBUG
    printf("link is: %s %d... checking\n",path2,ret);
#endif

    strcpy(path,path2);
    if (strrchr(path2,'/')) {
      *(param=strrchr(path2,'/'))=0;
      param++;

      if (checkpath(nr,path2,path)) return 1;
      if (path[strlen(path)-1]!='/') {
	if (strlen(path)+strlen(param)+2>sizeof(path)) return 1;
	strcat(path,"/");
	strcat(path,param);
      }

#ifdef DEBUG
      printf("resolved: %s\n",path);
#endif
    }
  }

  /* END OF BUGFIX */

  /* overwrite protection */
  if (!(pchild[nr]->perm & PERM_OVERWRITE)) {
    fp=fopen(path,"r");
    if (fp!=NULL) {
      fclose(fp);
      return 2;
    } 
  }

  if (pchild[nr]->pasvsock<=0) {
    /* IP-CHECK needed !!! */
    sprintf(cmd,"%d.%d.%d.%d",
	    pchild[nr]->dataip[0],pchild[nr]->dataip[1],
	    pchild[nr]->dataip[2],pchild[nr]->dataip[3]);
    addr=(unsigned long)inet_addr(cmd);
    if ((int)addr==-1) return 1;
    
    hp=gethostbyaddr((char *)&addr,sizeof(addr),AF_INET);
    
#ifdef DEBUG
    fprintf(stderr,"Making connection to: %s:%d\n",cmd,pchild[nr]->dataport);
#endif
    
    if ((soc=socket(AF_INET,SOCK_STREAM,0))<0) return 1;
    socksize=sizeof(struct sockaddr_in);
    memset(&sai,0,socksize);
    sai.sin_family=AF_INET;
    sai.sin_port=htons(pchild[nr]->dataport);
    memcpy(&sai.sin_addr,&addr,sizeof(addr));
    if (connect(soc,(struct sockaddr *)&sai,socksize)<0) return 1;
  } else {
    socksize=sizeof(sai);
    if ((soc=accept(pchild[nr]->pasvsock,(struct sockaddr *)&sai,
		    &socksize))<0) {
      user_return(nr,RET_501);
      return 1;
    }
  }
  
#ifdef DEBUG
  fprintf(stderr,"Writing file %s\n",path);
#endif

  if ((fp=fopen(path,"w"))==NULL) {
    close(soc);
    return 1;
  }
  
  bytesnow=byteslast=0;
  sprintf(cmd,"150 FILE: %s\r\n",param);
  write(pchild[nr]->sock,cmd,strlen(cmd));

  if (!daemonmode) {
    printf("User %d, uploading: %s\n",nr+1,param);
    out_prompt();
  }

  tm_start=time(NULL)-1;
  while (1) {
    FD_ZERO(&fds);
    FD_SET(soc,&fds);
    tv.tv_sec=transfertimeout;tv.tv_usec=0;
    if (select(soc+1,&fds,NULL,NULL,&tv)<=0) {
      fclose(fp);
      close(soc);
#ifdef DEBUG
      fprintf(stderr,"Receive timeout from client %d.\n",nr+1);
#endif
      return 3;
    }

    if (FD_ISSET(soc,&fds)) {
      n=read(soc,cmd,sizeof(cmd));
      if (n<=0) break; /* client closed con, file complete ? should be ! */
      fwrite(cmd,1,n,fp);
      bytesnow+=n;
      if (bytesnow-byteslast > TRFMSG_INTERVAL) {
	byteslast+=TRFMSG_INTERVAL;
	tm=time(NULL);
	if (!daemonmode) {
	  printf("User %d, %ldkB at %ldkB/s\n",nr+1,
		 bytesnow/1024,
		 (bytesnow/(tm-tm_start))/1024);
	  out_prompt();
	}
      }
    }
  }

  fclose(fp);
  
  close(soc);

#ifdef DEBUG
  fprintf(stderr,"Client %d finished uploading.\n",nr+1);
#endif

  return 0;
}

void do_user(int nr,char *param)
{
  char buffer[1024];
  int user;
  int n,count;

  if (strlen(param)<1) {
    handle_suspects(nr);
    return;
  }

  if (!strcmp(param,"ftp")) strcpy(param,"anonymous");
  for (user=0;user<logininfocount;user++)
    if (!strcmp(param,logininfo[user]->username)) break;
  pchild[nr]->usertype=user;

  if (user<logininfocount) {
    if (logininfo[pchild[nr]->usertype]->maxlogins>0) {
      count=0;
      for (n=0;n<maxusers;n++)
	if (pchild[n]->sock!=0 && pchild[nr]->usertype==pchild[n]->usertype)
	  count++;
      
      if (count>logininfo[pchild[nr]->usertype]->maxlogins) {
	if (!daemonmode) {
	  printf("Info: Too many users of type '%s'.\n",
		 logininfo[pchild[nr]->usertype]->username);
	  out_prompt();
	}
	user_return(nr,RET_530);
	kill_connection(nr);
	return;
      }
    } 
  }

  if (!strcmp(param,"anonymous")) {
    if (user<logininfocount) user_return(nr,RET_331);
    else {
      user_return(nr,RET_531);
      kill_connection(nr);
    }
  } else {
    sprintf(buffer,"331 Password required for user %s.\r\n",param);
    write(pchild[nr]->sock,buffer,strlen(buffer));
  }
  pchild[nr]->state=USR_PASS;
  pchild[nr]->idletime=time(NULL);
}

void do_pass(int nr,char *param)
{
  char buffer[64];
  int n,i;
  time_t tm;

  if ((pchild[nr]->usertype>=logininfocount)||
      (strcmp(logininfo[pchild[nr]->usertype]->passwd,"*")&&
      ((strlen(param)<1)||
      (strcmp(param,logininfo[pchild[nr]->usertype]->passwd))))) {

    /* check if anonymous access allowed */
#ifdef DEBUG
    printf("Error on password.\n");
#endif
    user_return(nr,RET_532);
    kill_connection(nr);
    return;
  }

  pchild[nr]->perm=logininfo[pchild[nr]->usertype]->perm;
  if ((pchild[nr]->perm & PERM_MULTIPLE)==0) {
    for (n=0;n<maxusers;n++) {
      if (nr!=n && pchild[n]->sock && !(pchild[n]->perm & PERM_MULTIPLE)) {
	for (i=0;i<4;i++)
	  if (pchild[nr]->userip[i]!=pchild[n]->userip[i])
	    break;
	if (i>=4) break;
      }
    }
    if (n<maxusers) {
      user_return(nr,RET_533);
      kill_connection(nr);
      return;
    }
  }

  if (realchangeroot) {
    strcpy(buffer,logininfo[pchild[nr]->usertype]->root);
    if (!realpath(buffer,pchild[nr]->rootdir)) {
      user_return(nr,RET_534);
      kill_connection(nr);
      return;
    }
  } else {
    sprintf(buffer,"%s%s",serverroot,logininfo[pchild[nr]->usertype]->root);
    if (!realpath(buffer,pchild[nr]->rootdir)) {
      user_return(nr,RET_534);
      kill_connection(nr);
      return;
    }
  }

  pchild[nr]->state=USR_ONLINE;
  pchild[nr]->idletime=tm=time(NULL);
  strcpy(pchild[nr]->username,logininfo[pchild[nr]->usertype]->username);

  if (logfile!=NULL) {
    strcpy(buffer,ctime(&tm));
    buffer[strlen(buffer)-1]=0;
    fprintf(logfile,"%s: LOGIN:  user %d %s %s (%d.%d.%d.%d)\n",buffer,nr+1,
	    pchild[nr]->username,pchild[nr]->url,
	    pchild[nr]->userip[0],pchild[nr]->userip[1],
	    pchild[nr]->userip[2],pchild[nr]->userip[3]);
    fflush(logfile);
  }

  user_spool(nr,230,motdfile);
  sprintf(buffer,"230 User %s logged in.\r\n",pchild[nr]->username);
  write(pchild[nr]->sock,buffer,strlen(buffer));
}

void do_size(int nr,char *param)
{
  char path[2048];
  struct stat s;

  if (!checkpath(nr,param,path)) {
    if (path[strlen(path)-1]=='/')
      path[strlen(path)-1]=0;
    
    if (stat(path,&s)==0) {
      sprintf(path,"213 %ld\r\n",(long int)s.st_size);
      write(pchild[nr]->sock,path,strlen(path));
      return;
    } 
  }

  user_return(nr,RET_501);
}

void do_mdtm(int nr,char *param)
{
  char path[1024],tm[32];
  struct stat s;

  if (!checkpath(nr,param,path)) {
    if (path[strlen(path)-1]=='/')
      path[strlen(path)-1]=0;
    
    if (stat(path,&s)==0) {
      strftime(tm,sizeof(tm),"%Y%m%d%H%M%S",gmtime(&s.st_mtime));
      sprintf(path,"213 %s\r\n",tm);
      write(pchild[nr]->sock,path,strlen(path));
      return;
    } 
  }

  user_return(nr,RET_501);
}

void do_pasv(int nr)
{
  int addr;
  unsigned int size;
  struct sockaddr_in sa;
  char sbuffer[256];
  unsigned int port;
  unsigned char *myip;

  if (pchild[nr]->pasvsock>0) {
    close(pchild[nr]->pasvsock);
    pchild[nr]->pasvsock=0;
  }
  
  if ((pchild[nr]->pasvsock=socket(AF_INET,SOCK_STREAM,0))<0) {
    pchild[nr]->pasvsock=0;
    user_return(nr,RET_425);
    return;
  }

  port=1025;
  while (port<65536) {
    size=sizeof(struct sockaddr_in);
    memset(&sa,0,size);
  
    sa.sin_family=AF_INET;
    sa.sin_port=htons(port);
    addr=INADDR_ANY;
    memcpy(&sa.sin_addr.s_addr,&addr,sizeof(int));
    
    if (bind(pchild[nr]->pasvsock,(struct sockaddr *)&sa,size)==0) break;
    port++;
  }
#ifdef DEBUG
  printf("PASV: Port %d\n",port);
#endif
  if (port>=65536) {
    close(pchild[nr]->pasvsock);
    pchild[nr]->pasvsock=0;
    user_return(nr,RET_425);
    return;
  }


  if (listen(pchild[nr]->pasvsock, 1) < 0) {
    close(pchild[nr]->pasvsock);
    pchild[nr]->pasvsock=0;
    user_return(nr,RET_425);
    return;
  }

  /*  size = sizeof(sa);
  if (getsockname(pchild[nr]->pasvsock, (struct sockaddr *) &sa, &size) < 0) {
    close(pchild[nr]->pasvsock);
    pchild[nr]->pasvsock=0;
    user_return(nr,RET_425);
    return;
    }*/
  /*  a=(unsigned char *) &sa.sin_addr;
      p=(unsigned char *) &sa.sin_port;
  
  sprintf(sbuffer,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", 
	  a[0],a[1],a[2],a[3],p[0],p[1]);
  */
  myip=getmyip(pchild[nr]->sock);
  sprintf(sbuffer,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", 
	  myip[0],myip[1],myip[2],myip[3],(port>>8)&0xFF,port&0xFF);

  
  write(pchild[nr]->sock,sbuffer,strlen(sbuffer));
}

int do_mkdir(int nr,char *param)
{
  char path[MAXPATHLEN+4];

  if (strlen(param)>512) param[512]=0;
  if (checkpath(nr,param,path)) return 1;

	/*
  if (strlen(path)+strlen(param)>MAXPATHLEN) return 1;
  strcat(path,param);
	*/

#ifdef DEBUG
  printf("Making directory '%s'.\n",path);
#endif
  
  return mkdir(path,0755);
}

int do_rmdir(int nr,char *param)
{
  char path[1024];
  char allowed[MAXPATHLEN+4];

  if (strlen(param)>512) param[512]=0;
  if (checkpath(nr,param,path)) return 1;

  if (!strcmp(pchild[nr]->rootdir,"/")) {
    strcpy(allowed,pchild[nr]->rootdir);
  }
  else {
    /* should not happen */
    if (strlen(pchild[nr]->rootdir)>=MAXPATHLEN) return 1;
    sprintf(allowed,"%s/",pchild[nr]->rootdir);
  }

  if (strcmp(allowed, path)==0) return 1;

#ifdef DEBUG
  printf("Removing directory '%s'.\n",path);
#endif
  
  return rmdir(path);
}

int do_dele(int nr,char *param)
{
  char path[2048];

  if (strlen(param)==0 || checkpath(nr,param,path)) return 1;

  if (path[strlen(path)-1]=='/') path[strlen(path)-1]=0;

#ifdef DEBUG
  printf("Removing file '%s'.\n",path);
#endif
  
  return unlink(path);
}

void handle_suspects(int nr)
{
  user_return(nr,RET_422);
  kill_connection(nr);
}

void handle_timeout(int nr)
{
  user_return(nr,RET_423);
  kill_connection(nr);
}

void serve_child(int nr) 
{
  char buffer[1024],sbuffer[1024];
  int size,r,i,j;
  char *param;
  time_t tm;

  size=recv(pchild[nr]->sock,buffer,sizeof(buffer),0);

  if (size<=2) {
    handle_suspects(nr);
    return;
  }
  buffer[size-2]=0;

#ifdef DEBUG
  if (buffer)
    printf("Client %d: '%s'\n",nr+1,buffer);
#endif

  param=strchr(buffer,' ');
  if (param) {
    *param=0;
    param++;
  }

  for (r=0;r<strlen(buffer);r++)
    buffer[r]=toupper(buffer[r]);
  for (r=0;r<REQ_COUNT;r++)
    if (!strcmp(buffer,request[r])) break;

  if (pchild[nr]->state==USR_ONLINE) {
    switch (r) {
    case REQ_SYST:
      pchild[nr]->resume=0;
      user_return(nr,RET_215);
      break;
    case REQ_QUIT:
      user_spool(nr,221,byemsgfile);
      user_return(nr,RET_221);
      kill_connection(nr);
      break;
    case REQ_TYPE:
      pchild[nr]->resume=0;
      if (param) param[0]=toupper(param[0]);
      if (param==NULL || (strcmp(param,"I") && strcmp(param,"A"))) {
	user_return(nr,RET_500);
	break;
      }
      sprintf(sbuffer,"200 Type set to %s.\r\n",param);
      write(pchild[nr]->sock,sbuffer,strlen(sbuffer));
      break;
    case REQ_PORT:
      if (pchild[nr]->pasvsock) {
	close(pchild[nr]->pasvsock);
	pchild[nr]->pasvsock=0;
      }
      pchild[nr]->resume=0;
      if ((sscanf(param,"%d,%d,%d,%d,%d,%d",
		  &pchild[nr]->dataip[0],
		  &pchild[nr]->dataip[1],
		  &pchild[nr]->dataip[2],
		  &pchild[nr]->dataip[3],&i,&j))<6) {
	pchild[nr]->dataport=0;
	user_return(nr,RET_502);
	break;
      }
      
      pchild[nr]->dataport=((i&0xFF)<<8)|(j&0xFF);
      user_return(nr,RET_200);
      break;
    case REQ_LIST:
      pchild[nr]->resume=0;
      if (pchild[nr]->pid) {
	user_return(nr,RET_491);
	printf("Warning: User %d trying to start more than 1 download.\n",nr);
	out_prompt();
	break;
      }

      if (param) {
	param[512]=0;
	strcpy(sbuffer,param);
      } else strcpy(sbuffer,"*");

      if ((pchild[nr]->pid=fork())==0) do_list(nr,sbuffer,FORMAT_LONG);
      pchild[nr]->idletime=time(NULL); /* keep alive */
      break;
    case REQ_NLST:
      pchild[nr]->resume=0;
      if (pchild[nr]->pid) {
	user_return(nr,RET_491);
	break;
      }

      if (param) {
	param[512]=0;
	strcpy(sbuffer,param);
      } else strcpy(sbuffer,"*");

      if ((pchild[nr]->pid=fork())==0) do_list(nr,sbuffer,FORMAT_SHORT);
      pchild[nr]->idletime=time(NULL); /* keep alive */
      break;
    case REQ_CDUP:
      strcpy(buffer,"..");
      param=buffer;            /* break through !!! */
    case REQ_CWD:
      pchild[nr]->resume=0;
      if (param) param[512]=0;
      if (do_chdir(nr,param)) {
	sprintf(sbuffer,"550 %s: No such directory.\r\n",param);
	write(pchild[nr]->sock,sbuffer,strlen(sbuffer));
      } else user_return(nr,RET_250);
      break;
    case REQ_PWD:
      pchild[nr]->resume=0;
      sprintf(sbuffer,"257 \"%s\" is current directory.\r\n",pchild[nr]->reldir);
      write(pchild[nr]->sock,sbuffer,strlen(sbuffer));
      break;
    case REQ_RETR:
      if (pchild[nr]->pid) {
	user_return(nr,RET_491);
	break;
      }

      if (logfile!=NULL && param!=NULL) {
	param[512]=0;
	tm=time(NULL);
	strcpy(sbuffer,ctime(&tm));
	sbuffer[strlen(sbuffer)-1]=0;
	fprintf(logfile,"%s: RETR: user %d, %s by %s (%d.%d.%d.%d)\n",
		sbuffer,nr+1,param,pchild[nr]->url,
		pchild[nr]->userip[0],pchild[nr]->userip[1],
	        pchild[nr]->userip[2],pchild[nr]->userip[3]);
	fflush(logfile);
      }
  
      pchild[nr]->pid=fork();
      
      if (pchild[nr]->pid==0) {
	user_return(nr,do_retr(nr,param));
	/*user_return(nr,RET_501);
	  else user_return(nr,RET_226);*/
	
	exit(0);
      }

      pchild[nr]->resume=0;
      break;
    case REQ_REST:
      param[32]=0; /* this should be enough */
      j=0;
      i=sscanf(param,"%d",&j);
      if (i>0 && j>=0) {
	user_return(nr,RET_350);
	pchild[nr]->resume=j;
      } else user_return(nr,RET_501);
      break;
    case REQ_SIZE:
      pchild[nr]->resume=0;
      if (param) param[512]=0;
      do_size(nr,param);
      break;
    case REQ_MDTM:
      pchild[nr]->resume=0;
      if (param) param[512]=0;
      do_mdtm(nr,param);
      break;
    case REQ_STOR:
      if (pchild[nr]->pid) {
	user_return(nr,RET_491);
	break;
      }

      if (logfile!=NULL && param!=NULL) {
	param[512]=0;
	tm=time(NULL);
	strcpy(sbuffer,ctime(&tm));
	sbuffer[strlen(sbuffer)-1]=0;
	fprintf(logfile,"%s: STOR: user %d, %s by %s (%d.%d.%d.%d)\n",
		sbuffer,nr+1,param,pchild[nr]->url,
		pchild[nr]->userip[0],pchild[nr]->userip[1],
	        pchild[nr]->userip[2],pchild[nr]->userip[3]);
	fflush(logfile);
      }
  
      if (pchild[nr]->perm & PERM_UPLOAD) {
	pchild[nr]->pid=fork();
	
	if (pchild[nr]->pid==0) {
	  if (param) param[512]=0;
	  switch (do_stor(nr,param)) {
	  case 1:
	    user_return(nr,RET_501);
	    break;
	  case 2:
	    user_return(nr,RET_553);
	    break;
	  case 3:
	    user_return(nr,RET_451);
	    if (logfile!=NULL) {
	      fprintf(logfile,"ERROR: Sending %s from user %d.",param,nr+1);
	      fflush(logfile);
	    }
	    break;
	  default:
	    user_return(nr,RET_226);
	    if (logfile!=NULL) {
	      fprintf(logfile,"COMPLETE: Sending %s from user %d.",param,nr+1);
	      fflush(logfile);
	    }
	    break;
	  }
	
	  exit(0);
	}
      } else user_return(nr,RET_553);
      pchild[nr]->resume=0;
      break;
    case REQ_NOOP:
      sprintf(sbuffer,"200 OK.\r\n");
      write(pchild[nr]->sock,sbuffer,strlen(sbuffer));
      break;
    case REQ_PASV:
      do_pasv(nr);
      break;
    case REQ_ABOR:
      if (pchild[nr]->pid) kill(pchild[nr]->pid,SIGTERM);
      pchild[nr]->pid=0;
      if (pchild[nr]->pasvsock) {
	close(pchild[nr]->pasvsock);
	pchild[nr]->pasvsock=0;
      }
      user_return(nr,RET_227);
      break;
    case REQ_MKD:
      if (pchild[nr]->perm & PERM_UPLOAD) {
	if (param) {
	  /*	  if (strchr(param,'/')==NULL) {*/
	    if (!do_mkdir(nr,param))
	      user_return(nr,RET_257);
	    else user_return(nr,RET_553);
	    /*} else user_return(nr,RET_501);*/
	} else user_return(nr,RET_501);
      } else user_return(nr,RET_553);
      break;
    case REQ_RMD:
      if (pchild[nr]->perm & PERM_ERASE) {
	if (param) {
	  /*if (strchr(param,'/')==NULL) {*/
	    if (!do_rmdir(nr,param))
	      user_return(nr,RET_258);
	    else user_return(nr,RET_553);
	    /*} else user_return(nr,RET_501);*/
	} else user_return(nr,RET_501);
      } else user_return(nr,RET_553);
      break;
    case REQ_DELE:
      if (pchild[nr]->perm & PERM_ERASE) {
	if (param) {
	  param[512]=0;
	  if (strchr(param,' ')) *strchr(param,' ')=0;
	  if (!do_dele(nr,param)) 
	    user_return(nr,RET_259);
	  else user_return(nr,RET_501);
	} else handle_suspects(nr);
      } else user_return(nr,RET_553);
      break;
    default:
      if ((unsigned char)buffer[0]>0x7F) {
	user_return(nr,RET_209);
	kill_connection(nr);
	break;
      }
      pchild[nr]->resume=0;

#ifdef DEBUG
      printf("Unknown command: '%s'\n",buffer);
      if (param)
	printf("     parameters: '%s'\n",param);
#endif

      user_return(nr,RET_503);
      break;
    }
  } else {
    switch (r) {
    case REQ_USER:
      if (param) {
	param[512]=0;
	if (strchr(param,' ')) *strchr(param,' ')=0;
	do_user(nr,param);
      } else handle_suspects(nr);
      break;
    case REQ_PASS:
      if (param && pchild[nr]->state==USR_PASS) {
	param[512]=0;
	do_pass(nr,param);
      } else handle_suspects(nr);
      break;
    case REQ_GET:
      sprintf(sbuffer,"<HTML><HEAD><TITLE>HTTP-access denied</TITLE></HEAD><BODY><H1>HTTP-access denied.</H1><HR>This is <B>NOT</B> a http-server. Try using \"ftp://'user':'passwd'@'url'/\".</BODY></HTML>\r\n");
      write(pchild[nr]->sock,sbuffer,strlen(sbuffer));
      kill_connection(nr);
      break;
    default:
      handle_suspects(nr);
      break;
    }
  }
}
