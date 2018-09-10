
/* ls replacement
   security reasons
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>

#include "ls.h"
#include "stupid-ftpd.h"

int list_match(char *,char *);

int list(int soc,int nr,int format,char *directory,char *mask,
	 int callback(int,int,char *)) {

  DIR *dir;
  struct dirent *entr;

  char filename[1024];
  /*  char fullpath[1024];*/
  char line[1024+80];
  char datestr[128];
  int dirlen,i;
  time_t timeval;
  struct stat st;
  struct tm *ntime;

  if (directory==NULL) return 0;

  strcpy(filename,directory);
  strcat(filename,"/");
  dirlen=strlen(filename);

  if ((dir=opendir(directory))==NULL) return 0;

#ifdef DEBUG
  fprintf(stderr,"list(): %s\n",directory);
#endif  

  if (format==FORMAT_HTML) {
    sprintf(line,
	    "Content-type: text/html\r\n\r\n"
	    "<html><head><title>Directory contents: %s"
	    "</title></head><body>\r\n",directory);
    callback(soc,nr,line);
  }

  while ((entr=readdir(dir))!=NULL) {
    if (entr->d_name[0]=='.') {
      if (strcmp(entr->d_name,".")==0 ||
	  strcmp(entr->d_name,"..")==0) {
	continue;
      }
    }
#ifdef DEBUG
    fprintf(stderr,"list_match(%s,%s)\n",entr->d_name,mask);
#endif  
    if (list_match(entr->d_name,mask)) {
      if (format==FORMAT_SHORT) {
	strcpy(line,entr->d_name);
	strcat(line,"\r\n");
	if (!callback(soc,nr,line)) break;
      } else {

	/* stat */

	if (strlen(entr->d_name)+dirlen>=1024) continue;  /* sorry ... */
	strcpy(filename+dirlen,entr->d_name);
	if (stat(filename,&st)<0) continue;

	/* date */

	timeval=time(NULL);
	ntime=localtime(&timeval);
	i=ntime->tm_year;
	
	ntime=localtime(&st.st_mtime);

	if (ntime->tm_year==i)
	  strftime(datestr,sizeof(datestr),"%b %d %H:%M",ntime);
	else strftime(datestr,sizeof(datestr),"%b %d  %Y",ntime);

	/* permissions */

	if (!S_ISDIR(st.st_mode) && !S_ISLNK(st.st_mode) && 
	    !S_ISREG(st.st_mode)) continue;

	if (format==FORMAT_LONG) {
	  sprintf(line,"%c%c%c%c%c%c%c%c%c%c %3d %s %s %13ld %s %s\r\n",
		  S_ISDIR(st.st_mode) ? 'd' : S_ISLNK(st.st_mode) ? 'l' : '-',
		  st.st_mode & S_IRUSR ? 'r' : '-',
		  st.st_mode & S_IWUSR ? 'w' : '-',
		  st.st_mode & S_IXUSR ? 'x' : '-',
		  st.st_mode & S_IRGRP ? 'r' : '-',
		  st.st_mode & S_IWGRP ? 'w' : '-',
		  st.st_mode & S_IXGRP ? 'x' : '-',
		  st.st_mode & S_IROTH ? 'r' : '-',
		  st.st_mode & S_IWOTH ? 'w' : '-',
		  st.st_mode & S_IXOTH ? 'x' : '-',
		  (int)st.st_nlink,
		  pchild[nr]->username,
		  "ftp",
		  (long)st.st_size,
		  datestr,
		  entr->d_name);
	  
	  if (!callback(soc,nr,line)) break;
	} else {
	  /* HTML-format */

	  sprintf(line,"<a href=\"%s%s\">%s%s</a> %ld %s<br>\r\n",directory,
		  entr->d_name,
		  (S_ISDIR(st.st_mode) ? "/" : ""),
		  entr->d_name,(long)st.st_size,datestr);
	  if (!callback(soc,nr,line)) break;
	}
      }
    }
  }

  closedir(dir);

  if (format==FORMAT_HTML) {
    sprintf(line,"</body></html>\r\n");
    callback(soc,nr,line);
  }

#ifdef DEBUG
  /*  fprintf(stderr,"Left list().\n");*/
#endif

  return 1;
}

int guess_star(char *str,char *mask) {
  /* pump from here !!! */
  int i=0;

#ifdef DEBUG
  /*  fprintf(stderr,"Entered guess_star(%s,%s).\n",str,mask);*/
#endif

  if (mask[0]==0) return 1;

  for (;i<strlen(str);i++)
    if (list_match(str+i,mask)) return 1;

#ifdef DEBUG
  /*  fprintf(stderr,"Left guess_star().\n");*/
#endif

  return 0;
}

int list_match(char *str,char *mask) {
  int i=0;

#ifdef DEBUG
  /*  fprintf(stderr,"Entered list_match(%s,%s).\n",str,mask);*/
#endif

  /* character per character matching */
  do {
    if (mask[i]=='*') return guess_star(str,mask+1);
    
    if (mask[i]=='?') {
      if (str[i]!=0) continue;
      else return 0;
    }
    
    if (mask[i]!=str[i]) return 0;

  } while (mask[++i]!=0);

#ifdef DEBUG
  /*  fprintf(stderr,"Left list_match().\n");*/
#endif

  if (str[i]==0) return 1;
  else return 0;
}

#ifdef TEST
int cb(char *str) {
  OUT(str);
  return 1;
}

int main(int argc,char **argv) {
  if (argc==3) {
    list(FORMAT_LONG,argv[1],argv[2],cb);
    return 0;
  } else {
    fprintf(stderr,"Need exactly 2 parameters!\n");
    fprintf(stderr,"Syntax: ls directory mask\n");
    return -1;
  }
}
#endif
