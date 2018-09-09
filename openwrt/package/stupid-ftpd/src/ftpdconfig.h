
/*

  ftpdconfig.h
  ------------


  Header for ftpdconfig.c
  

 */


#include <sys/param.h>

#define HARD_USERLIMIT 100
#define LOGININFO_COUNT 100


extern const char *options[];
extern char serverroot[];
extern int serverport;
extern int maxusers;
extern int logintimeout;
extern int idletimeout;
extern unsigned int transfertimeout;
extern int daemonmode;
extern FILE *logfile;
extern FILE *fpid;
extern char motdfile[];
extern char byemsgfile[];
extern char banmessage[];
extern int realchangeroot;

int readini(void);
void parsecmdline(int,char **);
void preinit(void);
int checkconf(void);
void add_user(char *);
int getuserbyname(char *);
int translatepermissions(char *);
unsigned char *getmyip(int);
void out_prompt(void);
void add_ban(char *);
void remove_ban(char *);

#define LI_USERNAME_LEN 16
#define LI_PASSWD_LEN 32
#define LI_ROOT_LEN MAXPATHLEN

typedef struct {
  char username[LI_USERNAME_LEN+4];
  char passwd[LI_PASSWD_LEN+4];
  char root[LI_ROOT_LEN+4];
  int maxlogins,perm;
} logininfo_t;

#define banlist_t char *

/* user account permissions */
#define PERM_NONE       0
#define PERM_DOWNLOAD   1
#define PERM_UPLOAD     2
#define PERM_OVERWRITE  4
#define PERM_MULTIPLE   8
#define PERM_ERASE     16
#define PERM_ALL       0xFFFF

extern const char *SERVER_PROMPT;

extern logininfo_t *logininfo[];
extern int logininfocount;

#define BANMAX_COUNT 5
extern banlist_t banlist[];
extern int bancount;
