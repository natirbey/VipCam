
/*#define TEST*/

#ifdef TEST
#define OUT(x) fprintf(stdout,"%s\n",(x))
#endif

#define FORMAT_LONG  0
#define FORMAT_SHORT 1
#define FORMAT_HTML 2

extern int list(int,int,int,char *,char *,int callback(int,int,char *));
extern int list_match(char *,char *);
