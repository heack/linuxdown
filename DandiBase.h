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
#include <string.h>
//#include "DownFromFTP.h"
//#include "DownFromHTTP.h"
#ifndef GENFUNC_H
#include"GenFunc.h"
#define GENFUNC_H
#endif
extern void DandiHelp();
extern void ClearBuf(char *buf);
extern int FindLength(char *buf);
extern int GetLengthFtp(char *buf);
extern int GetPortFtp(char *buf);
//the following function parse the url and interact with server for some information like fle length etc.
extern int GetInfo(char *arg,char *refer);
