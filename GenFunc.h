//#include "DandiBase.h"
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/errno.h>
#include <stdarg.h>
//#include <tidy.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/time.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAXTHREADS 30
#define SIZEBETWEENTHREADS 16384
#define HTTP 0
#define FTP 1
#define RTSP 2
#define BUFLEN 1024 
#define sdd_ThreadBlockMinSize	262144 //the least size of each thread
#define be2me_32(x) (x)
#define le2me_32(x) (x)
#define BE_32C(x,y) (*((uint32_t*)(x))=be2me_32(y))
#define sdd_ThreadLock		pthread_mutex_lock(&sdd_Lock)
#define sdd_ThreadUnLock	pthread_mutex_unlock(&sdd_Lock)
//#define file_Lock		pthread_mutex_lock(&file_Lock)//file lock while save status
//#define file_UnLock		pthread_mutex_unlock(&file_Lock)
#define sdd_ErrReturn(info,ret)	\
							{ \
							printf(info); \
							return ret; \
							}\

#define sdd_ErrExit(info,n)	\
							{ \
							printf(info); \
							exit(n); \
							}\

//typedef unsigned long uint32_t;
extern int hosttype;
extern int getfromfile;//1 means we have downloaded this file before, and we want to continue to download this file
extern int currentget;
extern int	 sizeget;
extern int filelength;
extern char	filename[256];
extern char magiccode[16];
extern char localpath[256];//
extern char host[256],path[256];
extern char user[32],pass[32];
extern int port,portftp,portrtsp;
extern int nthread;
extern char process[60];
extern int downpart[30][2];//describe how much data has been downloaded of each thread
extern int startthread;
extern int firstthreadsock;//the first sock that get information from server can be used by the first thread
extern struct timeval tpstart,tpend;
struct arg{
    int threadnumber;//0-30
    int a1; //range fromn a1 to a2 also lseed set to a1
    int a2; 
    int fd; //local file
    int booldown;//indict whether a thread has begun
    int allowotherdown;//0 other thread cannot download this thread, 1 not
	int bytesleft;//how many bytes left for this thread to download?
	int boolend;//whether this thread has returned.first will be set to 0;
};
extern struct arg g[MAXTHREADS];//every thread's data
extern pthread_mutex_t lock;
//static pthread_mutex_t file_Lock	=	PTHREAD_MUTEX_INITIALIZER;
extern const unsigned char sdd_XorTable[] ;
//this file contains url refer nthread
extern int SaveThreadStat();
extern int GetThreadStat(char *zxwfile);

extern void hash(char *field, char *param);
extern void call_hash (char *key, char *challenge, int len);
extern void calc_response (char *ret, char *field);
extern int GetStartText(char *buf,int sizebuf);
extern void printprocess(int sizeget, int filelength,unsigned long timeuse);
#define m_sock 10

extern int ReadEn(int sock,int n);
extern int ConnectHttp(char* host,int iport);
extern int GetThreadSize(void);
