

/*

  ftpcommand.h
  ------------


  Header for ftpcommand.c

 */


#define REQ_SYST 0
#define REQ_QUIT 1
#define REQ_TYPE 2
#define REQ_PORT 3
#define REQ_LIST 4
#define REQ_CWD  5
#define REQ_PWD  6
#define REQ_RETR 7
#define REQ_USER 8
#define REQ_PASS 9
#define REQ_REST 10
#define REQ_SIZE 11
#define REQ_MDTM 12
#define REQ_STOR 13
#define REQ_CDUP 14
#define REQ_NOOP 15
#define REQ_GET  16
#define REQ_NLST 17
#define REQ_PASV 18
#define REQ_ABOR 19
#define REQ_MKD  20
#define REQ_RMD  21
#define REQ_DELE 22


/* additional routines */
extern void   serve_child(int);
extern void   kill_connection(int);
extern void   handle_suspects(int);
extern void   handle_timeout(int);
extern void   user_return(int,int);
