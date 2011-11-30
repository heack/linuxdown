#ifndef DOWNFROMHTTP_H
#include"DownFromHTTP.h"
#define DOWNFROMHTTP_H
#endif
#ifndef GENFUNC_H
#include"GenFunc.h"
#define GENFUNC_H
#endif
int DownLoadHttp(struct arg *s_info)
{
    char get[256];
    char buf[1024]="";
    char headhost[256];
    char range[32];
#define FALSE 0
#define TRUE 1
    //this function used to download assigned file from a1 bytes to a2 bytes from host, try 5 times 
    int connected =FALSE;
    int i,j,starttext,boolprint=0;
    int sock,nextthread=0,boolbyte=0;
    unsigned long timeuse;
    int readen,bytesget=0;
    int inflen,index=0,offset=0;
    int bytesleft=(s_info->a2)-(s_info->a1)+1;
    struct timeval tv;
    if(s_info->booldown==1&&s_info->allowotherdown==1)return 0;
    if(s_info->threadnumber!=0){
    	while(g[(s_info->threadnumber)-1].booldown==0){
    		sleep(1);
		
	}
    }
    while(1){
        if((sock=ConnectHttp(host,port))<=0)sleep(1);
        else{
   		connected=TRUE;
   		break;
        }
    }
    if(connected){

        sprintf(get,"GET %s HTTP/1.1\r\nRange: bytes=%d-\r\nHost: %s:%d\r\n\r\n",path,s_info->a1,host,port);

        if(send(sock,get,strlen(get),0)!=strlen(get)){
		close(sock);
		sleep(5);
		
   		DownLoadHttp(s_info);
		return 0;
        }


        s_info->fd=open(filename,O_WRONLY|O_CREAT,0644);
        lseek(s_info->fd,s_info->a1,0);
	pthread_mutex_lock(&lock);
	s_info->booldown=1;
  	pthread_mutex_unlock(&lock);
        
       

        ClearBuf(buf);
        while(bytesleft>0){
		readen =ReadEn(sock,30);
        	if(readen<1){
			close(sock);
			close(s_info->fd);
			sleep(2);
			pthread_mutex_lock(&lock);
			s_info->booldown=0;
			s_info->allowotherdown=0;
            		pthread_mutex_unlock(&lock);
			DownLoadHttp(s_info);
			return 0;
		}
		
		boolprint++;
		if(boolprint==10)boolprint=0;
   		if(bytesleft>=1024&&offset==0){

			
			bytesget=recv(sock,buf,1024,MSG_WAITALL);

			
  			starttext=GetStartText(buf,bytesget);
  			write(s_info->fd,buf+starttext,1024-starttext);
  			bytesleft+=starttext-bytesget;
			sizeget+=bytesget-starttext;
			s_info->a1+=bytesget-starttext;
			offset+=bytesget-starttext;
			ClearBuf(buf);
			continue;
		}
   		else if(bytesleft>=1024){
   			bytesget=recv(sock,buf,1024,0);
        	if(bytesget==0)boolbyte++;
       			if(bytesget<0||(bytesget==0&&boolbyte>=10)){
				close(sock);
				close(s_info->fd);
				pthread_mutex_lock(&lock);
				s_info->booldown=0;
				s_info->allowotherdown=0;
            			pthread_mutex_unlock(&lock);
				DownLoadHttp(s_info);
				return 0;	
			}

			write(s_info->fd,buf,bytesget);
	      		offset+=bytesget;
	      		sizeget+=bytesget;
			s_info->a1+=bytesget;
			bytesleft-=bytesget;
			ClearBuf(buf);
			
	  	}
	  	else{
	      		if(offset==0){//used only if the thread size <1024
				bytesget=recv(sock,buf,2048,MSG_WAITALL);
				starttext=GetStartText(buf,bytesget);
		
				if(bytesleft!=bytesget-starttext){
					close(sock);
					close(s_info->fd);
					sleep(2);
					pthread_mutex_lock(&lock);
					s_info->booldown=0;
					s_info->allowotherdown=0;
            				pthread_mutex_unlock(&lock);
		    			DownLoadHttp(s_info);
					return 0;
				}
				write(s_info->fd,buf+starttext,strlen(buf)-starttext);
			offset+=bytesget-starttext;
			sizeget+=bytesget-starttext;
			s_info->a1+=bytesget;
			bytesleft-=bytesget-starttext;
			
	      		}

	      		else{
				bytesget=recv(sock,buf,bytesleft,0);
        if(bytesget==0)boolbyte++;
	      			if(bytesget<0||(bytesget==0&&boolbyte>=10)){
					close(sock);
					close(s_info->fd);
					pthread_mutex_lock(&lock);
					s_info->booldown=0;
					s_info->allowotherdown=0;
            				pthread_mutex_unlock(&lock);
					DownLoadHttp(s_info);
					return 0;	
				}
	      			write(s_info->fd,buf,bytesget);
	      			offset+=bytesget;
	      			sizeget+=bytesget;
				s_info->a1+=bytesget;
	      			bytesleft-=bytesget;
		
			if(bytesleft<=1024){
				nextthread++;
				/*if(g[s_info->threadnumber+nextthread].booldown==0&&g[s_info->threadnumber+nextthread].allowotherdown==1){
        				pthread_mutex_lock(&lock);
					g[s_info->threadnumber+nextthread].booldown=1;
        				pthread_mutex_unlock(&lock);
					bytesleft+=g[s_info->threadnumber+nextthread].a2-g[s_info->threadnumber+nextthread].a1+1;
					s_info->a2=g[s_info->threadnumber+nextthread].a2;
				}*/
			
			}
			}
			ClearBuf(buf);
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
	}
	close(sock);
        return 0;
    }
    else return -1;
}
int GetHttpData()
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
		if(i==(nthread-1))g[i].a2=filelength-1;
		g[i].bytesleft=g[i].a2-g[i].a1+1;
	}
        else{
		if(g[i].boolend==1)g[i].booldown=1;
		else g[i].booldown=0;
		//g[i].allowotherdown=0;
	
	}
        pthread_create(&thrd[i],NULL,(void*)DownLoadHttp,(void*)&g[i]);
    }
    for(i=0;i<=(nthread)-1;i++)
        pthread_join(thrd[i],NULL);
    return 0;
    
}

