#ifndef DOWNFROMFTP_H
#include"DownFromFTP.h"
#define DOWNFROMFTP_H
#endif
#ifndef GENFUNC_H
#include"GenFunc.h"
#define GENFUNC_H
#endif

int DownLoadFtp(struct arg *s_info)
{
    char get[256],dataget[2048];
    char buf[2048];
    char port1[8],port2[8];//227 Entering Passive Mode (192,168,8,30,6,32) 6 is stored in port1, while 32 is stored in port2
    char headhost[256];
    char range[32];
    float f;
    unsigned long timeuse;
#define FALSE 0
#define TRUE 1
    //this function used to download assigned file from a1 bytes to a2 bytes from host, try 5 times 
    int connected =FALSE,bytesget;//bytesget stans for how many bytes get one time
    int i,j,starttext,boolprint=0;
    int sock,sockdata;//sockdata is used to download ftp data through another sock because ftp use 2 pipes
    int readen,portthread=0,nextthread=0;
    int inflen,index=0,offset=0;
    int bytesleft=(s_info->a2)-(s_info->a1)+1;
    struct timeval tv;
    
    if(s_info->threadnumber!=0){
    	while(g[(s_info->threadnumber)-1].booldown==0){

    		sleep(1);
		
	}
    }
    if(s_info->booldown==1&&s_info->allowotherdown==1)return 0;
    while(1){
        if((sock=ConnectHttp(host,portftp))<=0)sleep(1);
        else{

   		connected=TRUE;
   		break;
        }
    }
    if(connected){
        
        if(ReadEn(sock,5)>0){
        recv(sock,dataget,512,0);

	ClearBuf(dataget);
	}
	sprintf(headhost,"USER %s\r\n\r\n",user);
;
	if(send(sock,headhost,strlen(headhost),0)!=strlen(headhost)){
		close(sock);
		sleep(5);
		
   		DownLoadFtp(s_info);
		return 0;
        }
	if(ReadEn(sock,5)>0){
	recv(sock,dataget,512,0);
	ClearBuf(dataget);
	}
	
	sprintf(headhost,"PASS %s\r\n\r\n",pass);
	if(send(sock,headhost,strlen(headhost),0)!=strlen(headhost)){
		close(sock);
		sleep(5);
		
   		DownLoadFtp(s_info);
		return 0;
        }
	
	recv(sock,dataget,512,0);
	if(strstr(dataget,"421 No more connection")!=NULL){
		close(sock);
		sleep(5);
		
   		DownLoadFtp(s_info);
		return 0;
	}

        ClearBuf(dataget);
	
	sprintf(headhost,"TYPE I\r\n\r\n");
        
	if(send(sock,headhost,strlen(headhost),0)!=strlen(headhost)){
		close(sock);
		sleep(5);
   		DownLoadFtp(s_info);
		return 0;
        }
	
	recv(sock,dataget,512,0);
	ClearBuf(dataget);
	
	sprintf(headhost,"PASV\r\n\r\n");

	if(send(sock,headhost,strlen(headhost),0)!=strlen(headhost)){
   		printf("Error in sending HEAD\n");
		close(sock);
		sleep(5);
   		DownLoadFtp(s_info);
		return 0;
        }
	
	recv(sock,dataget,512,0);
        while(strstr(dataget,"227")==NULL){
		ClearBuf(dataget);
		if(ReadEn(sock,5)>0){
			recv(sock,dataget,512,0);
			
		}
	}
        if(strstr(dataget,"227 Entering Passive Mode")!=NULL){
		j=4;
		i=0;
		while(j>=1){
			if(*(strstr(dataget,"227 Entering Passive Mode")+i)==',')j--;
			i++;
		}
		j=i+1;
		while(1){
			if(*(strstr(dataget,"227 Entering Passive Mode")+j)==',')break;
			j++;
		}
		
		snprintf(port1,j-i+1,"%s",strstr(dataget,"227 Entering Passive Mode")+i);
		i=j+1;
		while(1){
			if(*(strstr(dataget,"227 Entering Passive Mode")+i)==')')break;
			i++;
		}
		snprintf(port2,i-j,"%s",strstr(dataget,"227 Entering Passive Mode")+j+1);
		portthread=256*atoi(port1)+atoi(port2);		
	}
	
		
	
	ClearBuf(dataget);
	
	sprintf(headhost,"REST %d\r\n\r\n",s_info->a1);
	if(send(sock,headhost,strlen(headhost),0)!=strlen(headhost)){
		close(sock);
		sleep(5);
   		DownLoadFtp(s_info);
		return 0;
        }
	if(ReadEn(sock,5)>0){
	recv(sock,dataget,512,0);
	ClearBuf(dataget);
       }
	if(portthread==0){
		close(sock);
		sleep(5);
		DownLoadFtp(s_info);
		return 0;
	}      
       sprintf(get,"RETR %s\r\n\r\n",path);

        
        if(send(sock,get,strlen(get),0)!=strlen(get)){
   		printf("Error in sending get\n");
		close(sock);
		sleep(5);
   		DownLoadFtp(s_info);
		return 0;
        }

        s_info->fd=open(filename,O_WRONLY|O_CREAT,0644);
	
        lseek(s_info->fd,s_info->a1,0);
        
       

	connected=FALSE;
	for(i=0;i<=5;i++){
        	if((sockdata=ConnectHttp(host,portthread))<=0)
   			sleep(1);
        	else{
   		connected=TRUE;
   		break;
        	}
    	}
	if(connected=FALSE){
		close(sock);
		sleep(5);
		DownLoadFtp(s_info);
		return 0;
	}

        ClearBuf(buf);
	
	boolprint=0;
	if(s_info->booldown==1&&s_info->allowotherdown==1)return 0;	
  	
	
        while(bytesleft>0){
		
        	readen =ReadEn(sockdata,30);
        	if(readen<1){
			close(sockdata);
			sleep(2);
			pthread_mutex_lock(&lock);
			s_info->booldown=0;
			s_info->allowotherdown=0;
            		pthread_mutex_unlock(&lock);
			DownLoadFtp(s_info);
			return 0;
		}
		
		pthread_mutex_lock(&lock);
		if(s_info->booldown==0){
			s_info->booldown=1;
			s_info->allowotherdown=0;
		}
		else if(s_info->allowotherdown==1)return 0;//this means the other thread is downloading its part
  		pthread_mutex_unlock(&lock);
		boolprint++;
		if(boolprint==10)boolprint=0;
   		if(bytesleft>=1024){
       			bytesget=recv(sockdata,buf,1024,MSG_DONTWAIT);
       			if(bytesget<0){
				close(sockdata);
				close(s_info->fd);
				pthread_mutex_lock(&lock);
				s_info->booldown=0;
				s_info->allowotherdown=0;
            			pthread_mutex_unlock(&lock);
				DownLoadFtp(s_info);
				return 0;	
			}
       			write(s_info->fd,buf,bytesget);
	      		bytesleft-=bytesget;
	      		offset+=bytesget;
			s_info->a1+=bytesget;
	      		sizeget+=bytesget;
	  	}
	  	else{

			bytesget=recv(sockdata,buf,bytesleft,MSG_DONTWAIT);
	      		if(bytesget<0){
				//s_info->a1+=offset;
				close(sockdata);
				close(s_info->fd);
				pthread_mutex_lock(&lock);
				s_info->booldown=0;
				s_info->allowotherdown=0;
            			pthread_mutex_unlock(&lock);
				DownLoadFtp(s_info);
				return 0;	
			}
	      		write(s_info->fd,buf,bytesget);
	      		offset+=bytesget;
			s_info->a1+=bytesget;
	      		sizeget+=bytesget;
	      		bytesleft-=bytesget;
			if(bytesleft<1024){
				nextthread++;
			if(g[s_info->threadnumber+nextthread].booldown==0&&g[s_info->threadnumber+nextthread].allowotherdown==1){
        			pthread_mutex_lock(&lock);
				g[s_info->threadnumber+nextthread].booldown=1;
        			pthread_mutex_unlock(&lock);
				bytesleft+=g[s_info->threadnumber+nextthread].a2-g[s_info->threadnumber+nextthread].a1+1;
				s_info->a2=g[s_info->threadnumber+nextthread].a2;
			}
			}
	  	}
		
		if(boolprint==0){
			pthread_mutex_lock(&lock);
			SaveThreadStat();
			pthread_mutex_unlock(&lock);
			gettimeofday(&tpend,NULL);
    			timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
    			timeuse/=1000000;
			printprocess(sizeget, filelength, timeuse);
		}

	  ClearBuf(buf);
	 }
	close(sock);
	close(sockdata);
        return 0;
    }
    else return -1;
}

int GetFtpData(void)
{
    char head[256];
    char headhost[256];
    char dataget[512];
    int threadsize=0;
    int i_sock,i;
    pthread_t thrd[30];
    float timeuse;
    threadsize = GetThreadSize();
    gettimeofday(&tpstart,NULL);
    for(i=0;i<=(nthread)-1;i++){
	if(getfromfile==0){
		pthread_mutex_lock(&lock);		
    		g[i].booldown=0;
     		pthread_mutex_unlock(&lock);
		g[i].threadnumber=i;
       	g[i].a1=threadsize*i;
        	g[i].a2=g[i].a1+threadsize-1;
		if(i==nthread-1)g[i].a2=filelength-1;
		g[i].bytesleft=g[i].a2-g[i].a1+1;
        }
	else{
		if(g[i].boolend==1)g[i].booldown=1;
		else g[i].booldown=0;
		//g[i].allowotherdown=0;
	
	}
        pthread_create(&thrd[i],NULL,(void*)DownLoadFtp,(void*)&g[i]);
    }
    for(i=0;i<=(nthread)-1;i++)
        pthread_join(thrd[i],NULL);
    return 0;
}
