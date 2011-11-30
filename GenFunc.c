#ifndef GENFUNC_H
#include"GenFunc.h"
#define GENFUNC_H
#endif
int hosttype;
int getfromfile=0;//1 means we have downloaded this file before, and we want to continue to download this file
int	currentget=0;
int	sizeget=0;
int	filelength=0;
char	filename[256];
char magiccode[16]="heacklovely";
char localpath[256]="";//
char host[256],path[256];
char user[32],pass[32];
char process[60];
int port=80,portftp=21,portrtsp=554;
int nthread=1;
int downpart[30][2];//describe how much data has been downloaded of each thread
int startthread=1;
int firstthreadsock;//the first sock that get information from server can be used by the first thread

struct arg g[MAXTHREADS];//every thread's data
struct timeval tpstart,tpend;

pthread_mutex_t lock		=	PTHREAD_MUTEX_INITIALIZER;
//static pthread_mutex_t file_Lock	=	PTHREAD_MUTEX_INITIALIZER;
const unsigned char sdd_XorTable[] = {
    0x05, 0x18, 0x74, 0xd0, 0x0d, 0x09, 0x02, 0x53,
    0xc0, 0x01, 0x05, 0x05, 0x67, 0x03, 0x19, 0x70,
    0x08, 0x27, 0x66, 0x10, 0x10, 0x72, 0x08, 0x09,
    0x63, 0x11, 0x03, 0x71, 0x08, 0x08, 0x70, 0x02,
    0x10, 0x57, 0x05, 0x18, 0x54, 0x00, 0x00, 0x00 };
//this file contains url refer nthread
int SaveThreadStat()
{
	int statfd,i;
	char filestat[256];
	sprintf(filestat,"%s.ly",filename);//heihei
	statfd=open(filestat,O_RDWR|O_CREAT);
	write(statfd,magiccode,sizeof(magiccode));
	write(statfd,host,sizeof(host));
	write(statfd,user,sizeof(user));
	write(statfd,pass,sizeof(pass));
	write(statfd,&hosttype,sizeof(int));
	if(hosttype==HTTP)write(statfd,&port,sizeof(int));
	else if(hosttype==FTP)write(statfd,&portftp,sizeof(int));
	else if(hosttype==RTSP)write(statfd,&portrtsp,sizeof(int));
	write(statfd,filename,sizeof(filename));
	write(statfd,path,sizeof(path));
	write(statfd,&filelength,sizeof(int));
	write(statfd,&sizeget,sizeof(int));
	write(statfd,&nthread,sizeof(int));
	for(i=0;i<nthread;i++){
		write(statfd,&g[i],sizeof(struct arg));
	}
	close(statfd);
	return 0;
}
void printprocess(int sizeget, int filelength,unsigned long timeuse)
{
	int i=0,j=0;
	static a=0;
	a++;
	if(a==3)
		a=0;
	j=100*((float)sizeget/(float)filelength);
	if(j>=100)
		j=100;
	//for(i=0;i<=200;i++)
	//{
	//	printf("\b");
	//}
	printf("\xd");
	if(a==0)
		printf("-");
	else if(a==1)
		printf("\\");
	else if(a==2)
		printf("/");
	printf("|:");
	for(i=0;i<=50;i++)
	{
		if(i*2<=j)
			printf(">");
		else 
			printf("=");
	}
	printf("|");
	printf("%3.1f%% ",100*((float)sizeget/(float)filelength)>100?100:(100*((float)sizeget/(float)filelength)));
	printf(" %4.1fk/s %ldbytes   ",(float)sizeget/(float)(1000*(float)timeuse),sizeget);
	
}
int GetThreadStat(char *zxwfile)
{
	int zxwfilefd,i;
	if((zxwfilefd=open(zxwfile,O_RDONLY))==-1){
		fprintf(stdout,"%s","ERROR:ly file not exists.\n");
		exit(1);
	}
	read(zxwfilefd,magiccode,sizeof(magiccode));
	if(strcmp(magiccode,"heacklovely")){
		printf("ERROR:wrong file");
		exit(1);
	}
	read(zxwfilefd,host,sizeof(host));
	read(zxwfilefd,user,sizeof(user));
	read(zxwfilefd,pass,sizeof(pass));
	read(zxwfilefd,&hosttype,sizeof(int));
	if(hosttype==HTTP)read(zxwfilefd,&port,sizeof(int));
	else if(hosttype==FTP)read(zxwfilefd,&portftp,sizeof(int));
	else if(hosttype==RTSP)read(zxwfilefd,&portrtsp,sizeof(int));
	read(zxwfilefd,filename,sizeof(filename));
	read(zxwfilefd,path,sizeof(path));
	read(zxwfilefd,&filelength,sizeof(int));
	read(zxwfilefd,&sizeget,sizeof(int));
	read(zxwfilefd,&nthread,sizeof(int));
	printf("hosttype %d port %d filename %s path %s filelength %d sizeget %d nthread %d",hosttype,portftp,filename,path,filelength,sizeget,nthread);
	for(i=0;i<nthread;i++){
		read(zxwfilefd,&g[i],sizeof(struct arg));
		printf("thread number %d a1 %d a2 %d booldown %d allowothre%d bytesleft %d boolend %d\n",g[i].threadnumber,g[i].a1,g[i].a2,g[i].booldown,g[i].allowotherdown,g[i].bytesleft,g[i].boolend);
	}

	close(zxwfilefd);
	return 0;
}
static void hash(char *field, char *param) {

  uint32_t a, b, c, d;
 

  /* fill variables */
  a= le2me_32(*(uint32_t*)(field));
  b= le2me_32(*(uint32_t*)(field+4));
  c= le2me_32(*(uint32_t*)(field+8));
  d= le2me_32(*(uint32_t*)(field+12));

#ifdef LOG
  printf("real: hash input: %x %x %x %x\n", a, b, c, d);
  printf("real: hash parameter:\n");
  hexdump(param, 64);
  printf("real: hash field:\n");
  hexdump(field, 64+24);
#endif
  
  a = ((b & c) | (~b & d)) + le2me_32(*((uint32_t*)(param+0x00))) + a - 0x28955B88;
  a = ((a << 0x07) | (a >> 0x19)) + b;
  d = ((a & b) | (~a & c)) + le2me_32(*((uint32_t*)(param+0x04))) + d - 0x173848AA;
  d = ((d << 0x0c) | (d >> 0x14)) + a;
  c = ((d & a) | (~d & b)) + le2me_32(*((uint32_t*)(param+0x08))) + c + 0x242070DB;
  c = ((c << 0x11) | (c >> 0x0f)) + d;
  b = ((c & d) | (~c & a)) + le2me_32(*((uint32_t*)(param+0x0c))) + b - 0x3E423112;
  b = ((b << 0x16) | (b >> 0x0a)) + c;
  a = ((b & c) | (~b & d)) + le2me_32(*((uint32_t*)(param+0x10))) + a - 0x0A83F051;
  a = ((a << 0x07) | (a >> 0x19)) + b;
  d = ((a & b) | (~a & c)) + le2me_32(*((uint32_t*)(param+0x14))) + d + 0x4787C62A;
  d = ((d << 0x0c) | (d >> 0x14)) + a;
  c = ((d & a) | (~d & b)) + le2me_32(*((uint32_t*)(param+0x18))) + c - 0x57CFB9ED;
  c = ((c << 0x11) | (c >> 0x0f)) + d;
  b = ((c & d) | (~c & a)) + le2me_32(*((uint32_t*)(param+0x1c))) + b - 0x02B96AFF;
  b = ((b << 0x16) | (b >> 0x0a)) + c;
  a = ((b & c) | (~b & d)) + le2me_32(*((uint32_t*)(param+0x20))) + a + 0x698098D8;
  a = ((a << 0x07) | (a >> 0x19)) + b;
  d = ((a & b) | (~a & c)) + le2me_32(*((uint32_t*)(param+0x24))) + d - 0x74BB0851;
  d = ((d << 0x0c) | (d >> 0x14)) + a;
  c = ((d & a) | (~d & b)) + le2me_32(*((uint32_t*)(param+0x28))) + c - 0x0000A44F;
  c = ((c << 0x11) | (c >> 0x0f)) + d;
  b = ((c & d) | (~c & a)) + le2me_32(*((uint32_t*)(param+0x2C))) + b - 0x76A32842;
  b = ((b << 0x16) | (b >> 0x0a)) + c;
  a = ((b & c) | (~b & d)) + le2me_32(*((uint32_t*)(param+0x30))) + a + 0x6B901122;
  a = ((a << 0x07) | (a >> 0x19)) + b;
  d = ((a & b) | (~a & c)) + le2me_32(*((uint32_t*)(param+0x34))) + d - 0x02678E6D;
  d = ((d << 0x0c) | (d >> 0x14)) + a;
  c = ((d & a) | (~d & b)) + le2me_32(*((uint32_t*)(param+0x38))) + c - 0x5986BC72;
  c = ((c << 0x11) | (c >> 0x0f)) + d;
  b = ((c & d) | (~c & a)) + le2me_32(*((uint32_t*)(param+0x3c))) + b + 0x49B40821;
  b = ((b << 0x16) | (b >> 0x0a)) + c;
  
  a = ((b & d) | (~d & c)) + le2me_32(*((uint32_t*)(param+0x04))) + a - 0x09E1DA9E;
  a = ((a << 0x05) | (a >> 0x1b)) + b;
  d = ((a & c) | (~c & b)) + le2me_32(*((uint32_t*)(param+0x18))) + d - 0x3FBF4CC0;
  d = ((d << 0x09) | (d >> 0x17)) + a;
  c = ((d & b) | (~b & a)) + le2me_32(*((uint32_t*)(param+0x2c))) + c + 0x265E5A51;
  c = ((c << 0x0e) | (c >> 0x12)) + d;
  b = ((c & a) | (~a & d)) + le2me_32(*((uint32_t*)(param+0x00))) + b - 0x16493856;
  b = ((b << 0x14) | (b >> 0x0c)) + c;
  a = ((b & d) | (~d & c)) + le2me_32(*((uint32_t*)(param+0x14))) + a - 0x29D0EFA3;
  a = ((a << 0x05) | (a >> 0x1b)) + b;
  d = ((a & c) | (~c & b)) + le2me_32(*((uint32_t*)(param+0x28))) + d + 0x02441453;
  d = ((d << 0x09) | (d >> 0x17)) + a;
  c = ((d & b) | (~b & a)) + le2me_32(*((uint32_t*)(param+0x3c))) + c - 0x275E197F;
  c = ((c << 0x0e) | (c >> 0x12)) + d;
  b = ((c & a) | (~a & d)) + le2me_32(*((uint32_t*)(param+0x10))) + b - 0x182C0438;
  b = ((b << 0x14) | (b >> 0x0c)) + c;
  a = ((b & d) | (~d & c)) + le2me_32(*((uint32_t*)(param+0x24))) + a + 0x21E1CDE6;
  a = ((a << 0x05) | (a >> 0x1b)) + b;
  d = ((a & c) | (~c & b)) + le2me_32(*((uint32_t*)(param+0x38))) + d - 0x3CC8F82A;
  d = ((d << 0x09) | (d >> 0x17)) + a;
  c = ((d & b) | (~b & a)) + le2me_32(*((uint32_t*)(param+0x0c))) + c - 0x0B2AF279;
  c = ((c << 0x0e) | (c >> 0x12)) + d;
  b = ((c & a) | (~a & d)) + le2me_32(*((uint32_t*)(param+0x20))) + b + 0x455A14ED;
  b = ((b << 0x14) | (b >> 0x0c)) + c;
  a = ((b & d) | (~d & c)) + le2me_32(*((uint32_t*)(param+0x34))) + a - 0x561C16FB;
  a = ((a << 0x05) | (a >> 0x1b)) + b;
  d = ((a & c) | (~c & b)) + le2me_32(*((uint32_t*)(param+0x08))) + d - 0x03105C08;
  d = ((d << 0x09) | (d >> 0x17)) + a;
  c = ((d & b) | (~b & a)) + le2me_32(*((uint32_t*)(param+0x1c))) + c + 0x676F02D9;
  c = ((c << 0x0e) | (c >> 0x12)) + d;
  b = ((c & a) | (~a & d)) + le2me_32(*((uint32_t*)(param+0x30))) + b - 0x72D5B376;
  b = ((b << 0x14) | (b >> 0x0c)) + c;
  
  a = (b ^ c ^ d) + le2me_32(*((uint32_t*)(param+0x14))) + a - 0x0005C6BE;
  a = ((a << 0x04) | (a >> 0x1c)) + b;
  d = (a ^ b ^ c) + le2me_32(*((uint32_t*)(param+0x20))) + d - 0x788E097F;
  d = ((d << 0x0b) | (d >> 0x15)) + a;
  c = (d ^ a ^ b) + le2me_32(*((uint32_t*)(param+0x2c))) + c + 0x6D9D6122;
  c = ((c << 0x10) | (c >> 0x10)) + d;
  b = (c ^ d ^ a) + le2me_32(*((uint32_t*)(param+0x38))) + b - 0x021AC7F4;
  b = ((b << 0x17) | (b >> 0x09)) + c;
  a = (b ^ c ^ d) + le2me_32(*((uint32_t*)(param+0x04))) + a - 0x5B4115BC;
  a = ((a << 0x04) | (a >> 0x1c)) + b;
  d = (a ^ b ^ c) + le2me_32(*((uint32_t*)(param+0x10))) + d + 0x4BDECFA9;
  d = ((d << 0x0b) | (d >> 0x15)) + a;
  c = (d ^ a ^ b) + le2me_32(*((uint32_t*)(param+0x1c))) + c - 0x0944B4A0;
  c = ((c << 0x10) | (c >> 0x10)) + d;
  b = (c ^ d ^ a) + le2me_32(*((uint32_t*)(param+0x28))) + b - 0x41404390;
  b = ((b << 0x17) | (b >> 0x09)) + c;
  a = (b ^ c ^ d) + le2me_32(*((uint32_t*)(param+0x34))) + a + 0x289B7EC6;
  a = ((a << 0x04) | (a >> 0x1c)) + b;
  d = (a ^ b ^ c) + le2me_32(*((uint32_t*)(param+0x00))) + d - 0x155ED806;
  d = ((d << 0x0b) | (d >> 0x15)) + a;
  c = (d ^ a ^ b) + le2me_32(*((uint32_t*)(param+0x0c))) + c - 0x2B10CF7B;
  c = ((c << 0x10) | (c >> 0x10)) + d;
  b = (c ^ d ^ a) + le2me_32(*((uint32_t*)(param+0x18))) + b + 0x04881D05;
  b = ((b << 0x17) | (b >> 0x09)) + c;
  a = (b ^ c ^ d) + le2me_32(*((uint32_t*)(param+0x24))) + a - 0x262B2FC7;
  a = ((a << 0x04) | (a >> 0x1c)) + b;
  d = (a ^ b ^ c) + le2me_32(*((uint32_t*)(param+0x30))) + d - 0x1924661B;
  d = ((d << 0x0b) | (d >> 0x15)) + a;
  c = (d ^ a ^ b) + le2me_32(*((uint32_t*)(param+0x3c))) + c + 0x1fa27cf8;
  c = ((c << 0x10) | (c >> 0x10)) + d;
  b = (c ^ d ^ a) + le2me_32(*((uint32_t*)(param+0x08))) + b - 0x3B53A99B;
  b = ((b << 0x17) | (b >> 0x09)) + c;
  
  a = ((~d | b) ^ c)  + le2me_32(*((uint32_t*)(param+0x00))) + a - 0x0BD6DDBC;
  a = ((a << 0x06) | (a >> 0x1a)) + b; 
  d = ((~c | a) ^ b)  + le2me_32(*((uint32_t*)(param+0x1c))) + d + 0x432AFF97;
  d = ((d << 0x0a) | (d >> 0x16)) + a; 
  c = ((~b | d) ^ a)  + le2me_32(*((uint32_t*)(param+0x38))) + c - 0x546BDC59;
  c = ((c << 0x0f) | (c >> 0x11)) + d; 
  b = ((~a | c) ^ d)  + le2me_32(*((uint32_t*)(param+0x14))) + b - 0x036C5FC7;
  b = ((b << 0x15) | (b >> 0x0b)) + c; 
  a = ((~d | b) ^ c)  + le2me_32(*((uint32_t*)(param+0x30))) + a + 0x655B59C3;
  a = ((a << 0x06) | (a >> 0x1a)) + b; 
  d = ((~c | a) ^ b)  + le2me_32(*((uint32_t*)(param+0x0C))) + d - 0x70F3336E;
  d = ((d << 0x0a) | (d >> 0x16)) + a; 
  c = ((~b | d) ^ a)  + le2me_32(*((uint32_t*)(param+0x28))) + c - 0x00100B83;
  c = ((c << 0x0f) | (c >> 0x11)) + d; 
  b = ((~a | c) ^ d)  + le2me_32(*((uint32_t*)(param+0x04))) + b - 0x7A7BA22F;
  b = ((b << 0x15) | (b >> 0x0b)) + c; 
  a = ((~d | b) ^ c)  + le2me_32(*((uint32_t*)(param+0x20))) + a + 0x6FA87E4F;
  a = ((a << 0x06) | (a >> 0x1a)) + b; 
  d = ((~c | a) ^ b)  + le2me_32(*((uint32_t*)(param+0x3c))) + d - 0x01D31920;
  d = ((d << 0x0a) | (d >> 0x16)) + a; 
  c = ((~b | d) ^ a)  + le2me_32(*((uint32_t*)(param+0x18))) + c - 0x5CFEBCEC;
  c = ((c << 0x0f) | (c >> 0x11)) + d; 
  b = ((~a | c) ^ d)  + le2me_32(*((uint32_t*)(param+0x34))) + b + 0x4E0811A1;
  b = ((b << 0x15) | (b >> 0x0b)) + c; 
  a = ((~d | b) ^ c)  + le2me_32(*((uint32_t*)(param+0x10))) + a - 0x08AC817E;
  a = ((a << 0x06) | (a >> 0x1a)) + b; 
  d = ((~c | a) ^ b)  + le2me_32(*((uint32_t*)(param+0x2c))) + d - 0x42C50DCB;
  d = ((d << 0x0a) | (d >> 0x16)) + a; 
  c = ((~b | d) ^ a)  + le2me_32(*((uint32_t*)(param+0x08))) + c + 0x2AD7D2BB;
  c = ((c << 0x0f) | (c >> 0x11)) + d; 
  b = ((~a | c) ^ d)  + le2me_32(*((uint32_t*)(param+0x24))) + b - 0x14792C6F;
  b = ((b << 0x15) | (b >> 0x0b)) + c; 

#ifdef LOG
  printf("real: hash output: %x %x %x %x\n", a, b, c, d);
#endif
  
  a += le2me_32(*((uint32_t *)(field+0)));
  *((uint32_t *)(field+0)) = le2me_32(a);
  b += le2me_32(*((uint32_t *)(field+4)));
  *((uint32_t *)(field+4)) = le2me_32(b);
  c += le2me_32(*((uint32_t *)(field+8)));
  *((uint32_t *)(field+8)) = le2me_32(c);
  d += le2me_32(*((uint32_t *)(field+12)));
  *((uint32_t *)(field+12)) = le2me_32(d);

#ifdef LOG
  printf("real: hash field:\n");
  hexdump(field, 64+24);
#endif
}

static void call_hash (char *key, char *challenge, int len) 
{
  uint32_t *ptr1, *ptr2;
  uint32_t a, b, c, d;
  uint32_t tmp;

  ptr1=(uint32_t*)(key+16);
  ptr2=(uint32_t*)(key+20);
  
  a = le2me_32(*ptr1);
  b = (a >> 3) & 0x3f;
  a += len * 8;
  *ptr1 = le2me_32(a);
  
  if (a < (len << 3))
  {
#ifdef LOG
    printf("not verified: (len << 3) > a true\n");
#endif
    ptr2 += 4;
  }

  tmp = le2me_32(*ptr2);
  tmp += (len >> 0x1d);
  *ptr2 = le2me_32(tmp);
  a = 64 - b;
  c = 0;  
  if (a <= len)
  {

    memcpy(key+b+24, challenge, a);
    hash(key, key+24);
    c = a;
    d = c + 0x3f;
    
    while ( d < len ) {

#ifdef LOG
      printf("not verified:  while ( d < len )\n");
#endif
      hash(key, challenge+d-0x3f);
      d += 64;
      c += 64;
    }
    b = 0;
  }
  
  memcpy(key+b+24, challenge+c, len-c);
}
static void calc_response (char *ret, char *field) {

  char buf1[128];
  char buf2[128];
  int i;

  memset (buf1, 0, 64);
  *buf1 = 128;
  
  memcpy (buf2, field+16, 8);
  
  i = ( le2me_32(*((uint32_t*)(buf2))) >> 3 ) & 0x3f;
 
  if (i < 56) {
    i = 56 - i;
  } else {
#ifdef LOG
    printf("not verified: ! (i < 56)\n");
#endif
    i = 120 - i;
  }

  call_hash (field, buf1, i);
  call_hash (field, buf2, 8);

  memcpy (ret, field, 16);

}

static void calc_response_string (char *result, char *challenge) {
 
  char field[128];
  char zres[20];
  int  i;
      
  /* initialize our field */
  BE_32C (field,      0x01234567);
  BE_32C ((field+4),  0x89ABCDEF);
  BE_32C ((field+8),  0xFEDCBA98);
  BE_32C ((field+12), 0x76543210);
  BE_32C ((field+16), 0x00000000);
  BE_32C ((field+20), 0x00000000);

  /* calculate response */
  call_hash(field, challenge, 64);
  calc_response(zres,field);
 
  /* convert zres to ascii string */
  for (i=0; i<16; i++ ) {
    char a, b;
    
    a = (zres[i] >> 4) & 15;
    b = zres[i] & 15;

    result[i*2]   = ((a<10) ? (a+48) : (a+87)) & 255;
    result[i*2+1] = ((b<10) ? (b+48) : (b+87)) & 255;
  }
}

void real_calc_response_and_checksum (char *response, char *chksum, char *challenge) 
{
  int   ch_len, table_len, resp_len;
  int   i;
  char *ptr;
  char  buf[128];

  /* initialize return values */
  memset(response, 0, 64);
  memset(chksum, 0, 34);

  /* initialize buffer */
  memset(buf, 0, 128);
  ptr=buf;
  BE_32C(ptr, 0xa1e9149d);
  ptr+=4;
  BE_32C(ptr, 0x0e6b3b59);
  ptr+=4;

  /* some (length) checks */
  if (challenge != NULL)
  {
    ch_len = strlen (challenge);

    if (ch_len == 40) /* what a hack... */
    {
      challenge[32]=0;
      ch_len=32;
    }
    if ( ch_len > 56 ) ch_len=56;
    
    /* copy challenge to buf */
    memcpy(ptr, challenge, ch_len);
  }
  
  if (sdd_XorTable != NULL)
  {
    table_len = strlen(sdd_XorTable);

    if (table_len > 56) table_len=56;

    /* xor challenge bytewise with sdd_XorTable */
    for (i=0; i<table_len; i++)
      ptr[i] = ptr[i] ^ sdd_XorTable[i];
  }

  calc_response_string (response, buf);

  /* add tail */
  resp_len = strlen (response);
  strcpy (&response[resp_len], "01d0a8e3");

  /* calculate checksum */
  for (i=0; i<resp_len/4; i++)
    chksum[i] = response[i*4];
}
int GetStartText(char *buf,int sizebuf)
{
    int i;
	//?buf[i + 1] will overflow when i eq (sizebuf - 1)
	//for(i = 0;i <= sizebuf-1;++i)
    for(i = 0;i <= sizebuf-2;++i)
	{
        if(buf[i]=='\n'&&
		  !(buf[i+1] >= 'a'&&buf[i+1] <= 'z'||
			buf[i+1] >= 'A'&&buf[i+1] <= 'Z'))break;
  
    }
    return i+3;
}


int ReadEn(int sock,int n)
{
    fd_set rfdset;
    struct timeval tv;
    tv.tv_sec=n;
    tv.tv_usec=0;
    int readen=0;
    FD_ZERO(&rfdset);
    FD_SET(sock,&rfdset);
    
    readen=select(sock+1,&rfdset,NULL,NULL,&tv);

    return readen;
}

int ConnectHttp(char* host,int iport)
{
    int sd;
    struct sockaddr_in pin;
    struct hostent *nlp_host;

    if((nlp_host = gethostbyname(host))==0)
		sdd_ErrReturn("Error resolving local host\n",0);

    bzero(&pin,sizeof(pin));
    pin.sin_family = AF_INET;
    pin.sin_addr.s_addr = ((struct in_addr*)(nlp_host->h_addr))->s_addr;
    pin.sin_port = htons(iport);
    
    if((sd=socket(PF_INET,SOCK_STREAM,0))==-1)
		sdd_ErrReturn("Error opening socket\n",0);
    if(connect(sd,(struct sockaddr*)&pin,sizeof(pin))==-1)
		sdd_ErrReturn("Error connecting to socket\n",0);

    return sd;
}
//obsolete name
//int GetThreadSize(void)
int GetThreadSize(void)
{
    int threadsize;
    int ntemp;
    int i;
    if((threadsize=(filelength/nthread))>=2048){
        ntemp=threadsize/2048;
        threadsize=ntemp*2048;
    }
    else{
        threadsize=2048;//the least size of each thread
        for(i=1;i<=nthread;i++){
	  if(i*threadsize>=filelength){
	      nthread=i;
	      break;
	  }
        }
    }
    return threadsize;
}


