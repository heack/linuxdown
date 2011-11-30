#ifndef DANDIBASE_H
#include"DandiBase.h"
#define DANDIBASE_H
#endif

void DandiHelp()
{
	printf("Version 1.2.0 Build by KongYang, SJTU, China\n");
	printf("usage: lunixdown \"PATH\" [REFERER] threadnumber \n");
	printf("if you want to download a broken file use the following command\n");
	printf("linuxdown filename.ly\n");
	printf(".ly file will be found in your current directory\n");
	printf("For example: linuxdown \"http://be_heack.com/linuxdown.c\" \"http://be_heack.com/index.html\" 10\n");
	printf("PATH can be got just by right-click the link and copy link location.\n");
	printf("RFERER is the site address you get your PATH from.\n");
	printf("Threadnumber must be an integer less than 20 \n");
	printf("You need to use quotation mark to surround PATH in order to avoid problems caused by \"&\".\n");
	printf("File will be stored at \"./\" with the name got from net.\n");	
}

void ClearBuf(char *buf)
{
    int i=0;
    for(i=0;i<=1023;i++){
        *(buf+i)='\0';
    }
}
int FindLength(char *buf)
{
    int i=0,start=0,length=0,end=0;
    
    while(buf[start]!='\0'){
        if(buf[start]=='L'){
   if(!strncmp(buf+start,"Length:",7)){
       start+=8;
       break;
   }
        }
        start++;
    }
    end=start;
    
    while(buf[end++]!='\n'){}
    buf[--end]='\0';
    length=atoi(buf+start);//Content-Length: xxxxxx\r\n

    return length;
    
}
int GetLengthFtp(char *buf)
{
}
int GetPortFtp(char *buf)
{
}
//the following function parse the url and interact with server for some information like fle length etc.
int GetInfo(char *arg,char *refer)
{
    char head[1024],referer[256],length1[32],session[128],challenge1[128],challenge2[128],challenge3[128];
    char headhost[256],response[128],checksum[128];
    char port1[8],port2[8];//227 Entering Passive Mode (192,168,8,30,6,32) 6 is stored in port1, while 32 is stored in port2
    char c_port[8];
    char *ses,*cha;
    int iport=0;//sign the first time to see : like www.xxx.com:8000/name this is the position of :
    char dataget[2048]="";
    int i,i_sock,j,l,sockdata;
    
    int length=strlen(arg);
    if(!strncasecmp(arg,"http://",7))hosttype=HTTP;
    else if(!strncasecmp(arg,"ftp://",6))hosttype=FTP;
    else if(!strncasecmp(arg,"rtsp://",7))hosttype=RTSP;
	else sdd_ErrExit("You must input a url start with http:// or ftp:// or rtsp://",0)
	
    
    if(hosttype==HTTP)
	{
        for(i = 0;i <= strlen(arg)-7; ++i)
		{
			if(*(arg+7+i)==':')	iport=i;
			else if(*(arg+7+i)=='/')
			{
				if(iport!=0)
				{
					snprintf(host,iport+1,"%s",arg+7);
					sprintf(path,"/%s",arg+7+i+1);
					snprintf(c_port,i-iport,"%s",arg+7+iport+1);
					port=atoi(c_port);
				}
				else
				{
					snprintf(host,i+1,"%s",arg+7);
					sprintf(path,"/%s",arg+7+i+1);
				}
				break;
			}
        }
        for(i=length-1;i>=0;i--)
		{
		   if(*(arg+i)=='/')
		   {
			   sprintf(filename,"%s",arg+i+1);
			   break;
		   }
        }
    }
    if(hosttype==RTSP)
	{
        for(i=0;i<=strlen(arg)-7;i++)
		{
    		if(*(arg+7+i)==':') iport=i;
   
			if(*(arg+7+i)=='/')
			{
				if(iport!=0)
				{
					snprintf(host,iport+1,"%s",arg+7);
					sprintf(path,"/%s",arg+7+i+1);
					snprintf(c_port,i-iport,"%s",arg+7+iport+1);
					portrtsp=atoi(c_port);
				}
				else
				{
					snprintf(host,i+1,"%s",arg+7);
					sprintf(path,"/%s",arg+7+i+1);
				}

				break;
			}
        }
        for(i=length-1;i>=0;i--)
		{
		   if(*(arg+i)=='/')
		   {
			   sprintf(filename,"%s",arg+i+1);
			   break;
		   }
        }
    }
    
    if(hosttype==FTP)
	{
    	if(strstr(arg,"@")==NULL)
		{
   			for(i=0;i<=strlen(arg)-6;i++)
			{
       			if(*(arg+6+i)==':')
				{
  					iport=i;
  					continue;
       			}
       
       			if(*(arg+6+i)=='/')
				{
  					if(iport!=0)
					{
      					snprintf(host,iport+1,"%s",arg+6);
      					sprintf(path,"/%s",arg+6+i+1);
      					snprintf(c_port,i-iport,"%s",arg+6+iport+1);
      					portftp=atoi(c_port);
  					}else
					{
      					snprintf(host,i+1,"%s",arg+6);
      					sprintf(path,"/%s",arg+6+i+1);
  					}
  					break;
       			}
   			}
   			for(i=length-1;i>=0;i--)
			{
       			if(*(arg+i)=='/')
				{
  					sprintf(filename,"%s",arg+i+1);
  					break;
       			}
   			}
        }
        else{
			for(i=0;i<=strlen(arg)-6;i++)
			{
       			if(*(arg+6+i)==':')
				{
  					iport=i;
  					continue;
       			}

				if(*(arg+6+i)=='@')
				{
  					if(iport!=0)
					{
      						snprintf(user,iport+1,"%s",arg+6);
      						snprintf(pass,i-iport,"%s",arg+6+iport+1);
  					}
  					else
					{
      						snprintf(user,i+1,"%s",arg+6);
      						sprintf(pass,"%s",arg+6+i+1);
  					}
  					sprintf(arg,"ftp://%s",arg+8+strlen(user)+strlen(pass));
  					GetInfo(arg,refer);
  					return 0;

				}
			}
        }
        
        
    } 

    if(hosttype==HTTP)
	{
        i_sock=ConnectHttp(host,port);
        sprintf(head,"GET %s HTTP/1.1\r\nHost: %s:%d\r\nReferer: %s\r\n\r\n",path,host,port,refer);      
        if(send(i_sock,head,strlen(head),0)!=strlen(head))
		{
   			printf("Error in sending HEAD\n");
   			exit(1);
		}
     
		 recv(i_sock,dataget,512,0);
		 close(i_sock);
		 if(strstr(dataget,"\nLocation")!=NULL)
		 {
			 for(i=0;*(strstr(dataget,"\nLocation")+11+i)!='\n';++i);
			 snprintf(arg,i,"%s\0",strstr(dataget,"\nLocation")+11);
			 GetInfo(arg,refer);

			 return 0;
		 }
		 if(strstr(dataget,"\nlocation")!=NULL)
		 {
			 for(i=0;*(strstr(dataget,"\nlocation")+11+i)!='\n';++i);
			 snprintf(arg,i,"%s\0",strstr(dataget,"\nlocation")+11);
			 GetInfo(arg,refer);

			 return 0;
		 }
		 if(strstr(dataget,"Content-Location")!=NULL)
		 {
			 for(i=0;*(strstr(dataget,"Content-Location")+18+i)!='\n';++i);
			 snprintf(filename,i,"%s\0",strstr(dataget,"Content-Location")+18);
			 if(*(path+strlen(path)-1)=='/')sprintf(path,"%s%s",path,filename);
		 }

		 filelength = FindLength(dataget);
		

    	}
	if(hosttype==RTSP)
	{
        i_sock=ConnectHttp(host,portrtsp);
        sprintf(head,"OPTIONS rtsp://%s:%d RTSP/1.0\r\nCSeq: 1\r\nUser-Agent: RealMedia Player (HelixDNAClient)/10.0.0.0(win32)\r\nClientChallenge: 5b7496e598885563bcbff3a11a5f4212\r\nPlayerStarttime: [28/03/2003:22:50:23 00:00]\r\nCompanyID: mAmgkWISO2oJm7Q3N6ntg==\r\nPragma: initiate-session\r\n\r\n",host,port);      
        if(send(i_sock,head,strlen(head),0)!=strlen(head))
			sdd_ErrExit("Error in sending HEAD\n",1)
     
		 recv(i_sock,dataget,512,0);
     
		 if((ses=strstr(dataget,"Session:"))!=NULL)
		 {
     			cha=strstr(ses,"\r\n");
			*cha='\0';
				 sprintf(session,"%s",ses);
			 *cha='\n';
		 }
		 if((cha=strstr(dataget,"RealChallenge1:"))!=NULL)
		 {
			*strstr(cha,"\n")='\0';
			 sprintf(challenge1,"%s",cha+16);
		 }
		printf("%s %s\n",session,challenge1);
		real_calc_response_and_checksum (challenge2, checksum, challenge1);
		sprintf(head,"DESCRIBE %s RTSP/1.0\r\nCSeq: 2\r\nAccept: application/sdp\r\n%s\r\n\r\n",arg,session);
		if(send(i_sock,head,strlen(head),0)!=strlen(head))
			sdd_ErrExit("Error in sending HEAD\n",1)
		recv(i_sock,dataget,2048,0);
		ClearBuf(dataget);
		sprintf(head,"SETUP %s/streamid=0 RTSP/1.0\r\nCSeq: 3\r\nRealChallenge2: %s, sd=%s\r\nRDTFeatureLeavel: 3\r\nTransport: x-pn-tng/tcp;mode=play\r\nIf-Match: %s\r\n\r\n",arg,challenge2,checksum,session+9);
		if(send(i_sock,head,strlen(head),0)!=strlen(head))
			sdd_ErrExit("Error in sending HEAD\n",1)
		sleep(1);
		recv(i_sock,dataget,2048,0);

    }
    if(hosttype==FTP)
	{
      	 i_sock=ConnectHttp(host,portftp);		
	j=recv(i_sock,dataget,1024,0);
        ClearBuf(dataget);
	
        sprintf(head,"USER %s\r\n\r\n",user);
        
        if(send(i_sock,head,strlen(head),0)!=strlen(head))
			sdd_ErrExit("Error in sending HEAD\n",1)
	if(ReadEn(i_sock,5)>0)
	{
		//sleep(1);
		j=recv(i_sock,dataget,1024,0);

	}
	
	if(strstr(dataget,"230")==NULL)	//230 Any password will work
	{
		ClearBuf(dataget);
		sprintf(head,"PASS %s\r\n\r\n",pass);
		if(send(i_sock,head,strlen(head),0)!=strlen(head))
   			sdd_ErrExit("Error in sending HEAD\n",1)
		sleep(1);
		if(ReadEn(i_sock,5)>0)
		{
			j=recv(i_sock,dataget,1024,0);
		}
	}
	ClearBuf(dataget);
      sprintf(head,"SIZE %s\r\n\r\n",path);
	if(send(i_sock,head,strlen(head),0)!=strlen(head))
		sdd_ErrExit("Error in sending HEAD\n",1)
		if(ReadEn(i_sock,5)>0)
		recv(i_sock,dataget,1024,0);
		if(strstr(dataget,"213")!=NULL)
		{
			i=1;
			while(1)
			{
				if(*(strstr(dataget,"213")+i)=='\n')break;
				i++;
			}
				
			snprintf(length1,i-4,"%s",strstr(dataget,"213")+4);
			filelength=atoi(length1);
			if(filelength==0)
			{
				close(i_sock);
				sleep(1);
				GetInfo(arg,refer);
				return 0;
			}
		}
/*else if(strstr(dataget,"Unknown command")!=NULL){
		ClearBuf(dataget);
		sprintf(head,"LIST %s\r\n\r\n",path);
		if(send(i_sock,head,strlen(head),0)!=strlen(head)){
   			printf("Error in sending LIST\n");
   			exit(1);
        	}
		printf("head is %s\n",head);
		j=recv(i_sock,dataget,512,0);
		write(1,dataget,j);
		if(strstr(dataget,filename)!=NULL){
			j=0;
			for(i=0;i<=50;i++){
				if(*(strstr(dataget,filename)-i)==' ')j++;
				if(j==4)l=i;
				if(j==5){
					snprintf(length1,l-i-1,"%s",strstr(dataget,filename)-i+1);
					filelength=atoi(length1);
					if(filelength==0){
					close(i_sock);
					sleep(1);
					GetInfo(arg,refer);
					return 0;
					}
					break;
				}
			}
		}	
	}*/
		else
		{
			close(i_sock);
			sleep(1);
			GetInfo(arg,refer);
			return 0;
		}
		firstthreadsock=i_sock;
         }
    return 0;
}

