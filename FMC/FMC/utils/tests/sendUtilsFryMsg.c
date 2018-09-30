#define _GNU_SOURCE                                              /* strfry() */
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
  char verbStr[MSG_SZ+20]="";
  char debugStr[MSG_SZ+20]="";
  char infoStr[MSG_SZ+20]="";
  char warnStr[MSG_SZ+20]="";
  char errorStr[MSG_SZ+20]="";
  char fatalStr[MSG_SZ+20]="";
  char *outStr[7];
  time_t startTime=0,stopTime=0;
  unsigned long long elapsedTime=0;
  double msgRate=0.0,byteRate=0.0,bitRate=0.0;
  int addHeader=1;
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
  for(;;)
  {
    if(strlen(verbStr)>MSG_SZ-1)break;
    strcat(verbStr,"verbose ");
  }
  for(;;)
  {
    if(strlen(debugStr)>MSG_SZ-1)break;
    strcat(debugStr,"debug ");
  }
  for(;;)
  {
    if(strlen(infoStr)>MSG_SZ-1)break;
    strcat(infoStr,"info ");
  }
  for(;;)
  {
    if(strlen(warnStr)>MSG_SZ-1)break;
    strcat(warnStr,"warning ");
  }
  for(;;)
  {
    if(strlen(errorStr)>MSG_SZ-1)break;
    strcat(errorStr,"error ");
  }
  for(;;)
  {
    if(strlen(fatalStr)>MSG_SZ-1)break;
    strcat(fatalStr,"fatal ");
  }
  /*-------------------------------------------------------------------------*/
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigprocmask(SIG_BLOCK,&signalMask,NULL);
  /*-------------------------------------------------------------------------*/
  verbStr[msgSize+1]='\0';
  debugStr[msgSize+1]='\0';
  infoStr[msgSize+1]='\0';
  warnStr[msgSize+1]='\0';
  errorStr[msgSize+1]='\0';
  fatalStr[msgSize+1]='\0';
  /*-------------------------------------------------------------------------*/
  outStr[1]=verbStr;
  outStr[2]=debugStr;
  outStr[3]=infoStr;
  outStr[4]=warnStr;
  outStr[5]=errorStr;
  outStr[6]=fatalStr;
  /*-------------------------------------------------------------------------*/
  startTime=time(NULL);
  for(sntMsg=0;;)
  {
    int sev;
    nullTS.tv_sec=0;
    nullTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&nullTS);
    if(signo==SIGTERM||signo==SIGINT)break;
    /*.......................................................................*/
    if(sntMsg<100 || !(sntMsg%50))
      printf("msg #: %llu - drop #: %llu\n",sntMsg,droppedN);
    /*.......................................................................*/
    sev=1+sntMsg%6;
    /*.......................................................................*/
    if(max>0 && sntMsg>=max)break;
    strfry(outStr[sev]);
    /*.......................................................................*/
    if(addHeader)
    {
      if(outFIFO)
        rv=mfPrintf(outFD,sev,__func__,tryN,"%llu %s",sntMsg++,outStr[sev]);
      else
        rv=mPrintf(L_DIM,sev,__func__,tryN,"%llu %s",sntMsg++,outStr[sev]);
    }
    else
    {
      if(outFIFO)
        rv=rfPrintf(outFD,tryN,"%llu %s",sntMsg++,outStr[sev]);
      else
        rv=rPrintf(L_DIM,tryN,"%llu %s",sntMsg++,outStr[sev]);
    }
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
  byteRate=msgRate*(double)msgSize;
  bitRate=msgRate*(double)msgSize*8.0;
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
