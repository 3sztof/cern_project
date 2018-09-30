#include <stdio.h>
#include <time.h>                                             /* nanosleep() */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>                                              /* getopt() */
#include <string.h>                                              /* strtol() */
#include <stdlib.h>                                                /* exit() */
#include <errno.h>
#include <libgen.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>                                              /* isprint() */
#include <locale.h>
#include "fmcMsgUtils.h"               /* MSG_SZ, mPrintf(), rPrintf(), etc. */
/*****************************************************************************/
int fifoInit(char *fifoPath,int noDrop);
void usage(void);
char *pName=NULL;
/*****************************************************************************/
int main(int argc,char **argv)
{
  char *p=NULL;
  struct timespec delay={0,1000000};                                /* 0.1 s */
  unsigned long long sntMsg;
  unsigned long long sentBytes;
  int flag=0;
  char *outFIFO=NULL;
  int outFD=-1;
  int noDrop=0;
  char hostName[80]="";
  int max=0;
  int msgSize=80;
  sigset_t signalMask;
  int signo=-1;
  struct timespec nullTS={0,0};
  char outStr[MSG_SZ+20]="";
  int outStrLen=0;
  time_t startTime=0,stopTime=0;
  unsigned long long elapsedTime=0;
  double msgRate=0.0,byteRate=0.0,bitRate=0.0;
  int addHeader=1;
  int i=0;
  char c=0;
  int tryN=0;
  int rv;
  unsigned long long droppedN=0;
  double droppedP=0.0;
  /*-------------------------------------------------------------------------*/
  pName=strdup(basename(argv[0]));
  gethostname(hostName,80);
  p=strchr(hostName,'.');
  if(p)*p='\0';
  /*-------------------------------------------------------------------------*/
  while((flag=getopt(argc,argv,"+hp:s:n:M:AS:Nr:"))!=EOF)
  {
    switch(flag)
    {
      case 'p':
        outFIFO=optarg;
        break;
      case 'A':
        noDrop=1;
        break;
      case 'N':
        addHeader=0;
        break;
      case 's':
        delay.tv_sec=(time_t)strtol(optarg,(char**)NULL,0);
        break;
      case 'n':
        delay.tv_nsec=strtol(optarg,(char**)NULL,0);
        break;
      case 'M':
        max=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'S':
        msgSize=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'r':
        tryN=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'h':
      default:
        usage();
    }
  }
  if(argc-optind!=0)usage();
  if(msgSize>=MSG_SZ-1)
  {
    fprintf(stderr,"Illegal message size %d>=%d!\n",msgSize,MSG_SZ-1);
    usage();
  }
  if(!addHeader && tryN==0)tryN=1;
  /*-------------------------------------------------------------------------*/
  if(outFIFO)
  {
    outFD=loggerOpen(outFIFO,noDrop,L_STD,ERROR);
    if(outFD==-1)
    {
      fprintf(stderr,"Error status returned by loggerOpen()!\n");
      return 1;
    }
  }
  else
  {
    if(dfltLoggerOpen(10,noDrop,ERROR,DEBUG,0)==-1)
    {
      fprintf(stderr,"Error status returned by dfltLoggerOpen()!\n");
      return 1;
    }
  }
  /*-------------------------------------------------------------------------*/
  memset(outStr,0,sizeof(outStr));
  /*-------------------------------------------------------------------------*/
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigprocmask(SIG_BLOCK,&signalMask,NULL);
  /*-------------------------------------------------------------------------*/
  setlocale(LC_CTYPE,"C");
  startTime=time(NULL);
  for(sntMsg=0,sentBytes=0;;)
  {
    int sev=0;
    nullTS.tv_sec=0;
    nullTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&nullTS);
    if(signo==SIGTERM||signo==SIGINT)break;
    /*.......................................................................*/
    if(sntMsg<100 || !(sntMsg%50))
      printf("msg #: %llu - drop#: %llu\n",sntMsg,droppedN);
    /*.......................................................................*/
    sev=1+rand()%6;
    /*.......................................................................*/
    if(max>0 && sntMsg>=max)break;
    outStrLen=2+rand()%msgSize;                    /* \n\0 to (msgSize)+\n\0 */
    for(i=0;i<outStrLen-2;)
    {
      c=(char)(rand()%128);
      if(c=='\0')continue;
      if(i>0 && strchr(".,:!?",outStr[i-1]))outStr[i++]=' ';
      else if(isalnum(c))outStr[i++]=c;
      else if(i>2 && strchr(".,:!?",c) && isalnum(outStr[i-1]) && 
              isalnum(outStr[i-2]) && isalnum(outStr[i-3]))outStr[i++]=c;
      else if(i>2 && c==' ' && outStr[i-1]!=' ' && outStr[i-2]!=' ' &&
              outStr[i-3]!=' ')outStr[i++]=c;
    }
    outStr[outStrLen-2]='\n';
    outStr[outStrLen-1]='\0';
    /*.......................................................................*/
    sentBytes+=snprintf(NULL,0,"%llu %d %s",sntMsg,outStrLen,outStr);
    if(addHeader)
    {
      if(outFIFO)
        rv=mfPrintf(outFD,sev,__func__,tryN,"%llu %d %s",sntMsg++,outStrLen,
                    outStr);
      else
        rv=mPrintf(L_DIM,sev,__func__,tryN,"%llu %d %s",sntMsg++,outStrLen,
                   outStr);
    }
    else
    {
      if(outFIFO)
        rv=rfPrintf(outFD,tryN,"%llu %d %s",sntMsg++,outStrLen,outStr);
      else
        rv=rPrintf(L_DIM,tryN,"%llu %d %s",sntMsg++,outStrLen,outStr);
    }
    if(rv)droppedN+=rv;
    /*.......................................................................*/
    if(delay.tv_nsec||delay.tv_sec)
    {
      signo=sigtimedwait(&signalMask,NULL,&delay);
      if(signo==SIGTERM||signo==SIGINT)break;
    }
    /*.......................................................................*/
  }
  stopTime=time(NULL);
  /*-------------------------------------------------------------------------*/
  printf("----------------------------------------------------------------\n");
  elapsedTime=(unsigned long long)stopTime-startTime;
  printf("Elapsed time: %llu s\n",elapsedTime);
  if(elapsedTime==0)
  {
    printf("Not enough messages to evaluate rates\n");
    exit(0);
  }
  msgRate=(double)sntMsg/(double)elapsedTime;
  byteRate=(double)sentBytes/(double)elapsedTime;
  bitRate=byteRate*8.0;
  droppedP=(double)droppedN*100.0/sntMsg;
  printf("Message rate: %f Msg/s\t= %f kMsg/s\n",msgRate,msgRate/1000.);
  printf("Byte rate     %f KiB/s\t= %f MiB/s\n",byteRate/1024.,
         byteRate/1048576.);
  printf("Bit rate:     %f kb/s\t= %f Mb/s\n",bitRate/1000.,bitRate/1000000.);
  printf("Dropped/sent: %llu/%llu = %f %%\n",droppedN,sntMsg,droppedP);
  printf("----------------------------------------------------------------\n");
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
void usage(void)
{
  fprintf(stderr,"Usage: %s [-p fifo_path][-s seconds][-n nanoseconds]\n"
          "                      [-M number_of_messages][-A(no-drop)]\n"
          "                      [-S message_size(<%d)][-N][-r tryN]\n",
          pName,MSG_SZ-1);
  exit(1);
}
/*****************************************************************************/
