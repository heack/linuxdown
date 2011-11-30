/***************************************************************************
 *   Copyright (C) 2005 by kongyang                                        *
 *   be_heack@hotmail.com                                                  *
 *   Version 1.0                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   This program intends to help you download files using HTTP or FTP     *
 *   protocal, and you can use up to 20 threads to make your downloading   *
 *   faster.                                                               *
 *                                                                         *
 *   If you have better opinions about this program, please contact me.    *
 *   My msn is be_heack@hotmail.com, and mail the same.                    *
 *   If you help me a lot, I will add your name to the developer list.     *
 ***************************************************************************/
#ifndef DANDIBASE_H
#include"DandiBase.h"
#define DANDIBASE_H
#endif
#ifndef GENFUNC_H
#include"GenFunc.h"
#define GENFUNC_H
#endif
#ifndef DOWNFROMFTP_H
#include"DownFromFTP.h"
#define DOWNFROMFTP_H
#endif
#ifndef DOWNFROMHTTP_H
#include"DownFromHTTP.h"
#define DOWNFROMHTTP_H
#endif

int main(int argc, char* argv[])
{
    
    char headhost[256];
    char dataget[512];
    int c;
    int ret,length;
    int i_sock;
    char chost[256],crefer[256];
    
    int i=0;//n_down stands for how many threads user want it to use
    if(argc==1){
    	DandiHelp();
	exit(0);
    	
    }
    else if(argc==4){
    	if(argv[3][0]>'9'||argv[3][0]<='0'){
		printf("you can only input a number between 1 and 30\n");
		exit(0);
	}
    	nthread=atoi(argv[3]);
    	if(nthread>30||nthread<=0){
        	printf("you can only input a number between 1 and 30\n");
        	exit(0);
    	}
	sprintf(chost,"%s\0",argv[1]);
    	sprintf(crefer,"%s\0",argv[2]);
    }
    else if(argc==3){
    	if(argv[2][0]>'9'||argv[2][0]<='0'){
		printf("you can only input a number between 1 and 30\n");
		exit(0);
	}
    	nthread=atoi(argv[2]);
    	if(nthread>MAXTHREADS||nthread<=0){
        	printf("you can only input a number between 1 and 30\n");
        	exit(0);
    	}
	sprintf(chost,"%s\0",argv[1]);
    	sprintf(crefer,"\0");
    }
	else if(argc==2){
		GetThreadStat(argv[1]);
		getfromfile=1;
	}
    else{
    	printf("wrong options\n");
	DandiHelp();
	exit(0);
    }
    if(getfromfile==0){
	for(i=0;i<=MAXTHREADS-1;i++){
    		g[i].booldown=1;
		g[i].allowotherdown=1;
		g[i].boolend=0;
    	}
    	sprintf(user,"%s","anonymous");
    	sprintf(pass,"%s","123456");
	GetInfo(chost,crefer);
	}
	
    if(hosttype==HTTP)GetHttpData();
    else if(hosttype==FTP)GetFtpData();
    //else if(hosttype==RTSP)GetRtspData();
    printf("\nDONE\n");
    system("rm -f *.ly");
    return 0;
}


