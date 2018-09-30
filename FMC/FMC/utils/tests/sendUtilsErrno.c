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
#include "fmcMsgUtils.h"                       /* mPrintf(), rPrintf(), etc. */
/*****************************************************************************/
int fifoInit(char *fifoPath,int noDrop);
void usage(void);
char *pName=NULL;
/*****************************************************************************/
int main(int argc,char **argv)
{
  register char *p=NULL;
  struct timespec delay={0,1000000};                                /* 0.1 s */
  unsigned long long sntMsg;
  unsigned long long sentBytes;
  int flag=0;
  char *outFIFO=NULL;
  int outFD=-1;
  int noDrop=0;
  char hostName[80]="";
  int max=0;
  sigset_t signalMask;
  int signo=-1;
  time_t startTime=0,stopTime=0;
  unsigned long long elapsedTime=0;
  double msgRate=0.0,byteRate=0.0,bitRate=0.0;
  int addHeader=0;
  char *buf=NULL;
  int sev=0;
  int msgN;
  int tryN=0;
  int rv=0;
  unsigned long long droppedN=0;
  double droppedP=0.0;
  /*-------------------------------------------------------------------------*/
  pName=strdup(basename(argv[0]));
  gethostname(hostName,80);
  p=strchr(hostName,'.');
  if(p)*p='\0';
  /*-------------------------------------------------------------------------*/
  while((flag=getopt(argc,argv,"+hp:s:n:M:AHr:"))!=EOF)
  {
    switch(flag)
    {
      case 'p':
        outFIFO=optarg;
        break;
      case 'A':
        noDrop=1;
        break;
      case 'H':
        addHeader=1;
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
      case 'r':
        tryN=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'h':
      default:
        usage();
    }
  }
  if(argc-optind!=0)usage();
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
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigprocmask(SIG_BLOCK,&signalMask,NULL);
  /*-------------------------------------------------------------------------*/
  printf("sys_nerr=%d\n",sys_nerr);
  startTime=time(NULL);
  for(sntMsg=0,sentBytes=0;;)
  {
    /*.......................................................................*/
    if(max>0 && sntMsg>=max)break;
    if(sntMsg<100 || !(sntMsg%50))
      printf("msg #: %llu - drop#: %llu\n",sntMsg,droppedN);
    /*.......................................................................*/
    sev=1+sntMsg%5;
    msgN=sntMsg%(sys_nerr-1);
    buf=(char*)sys_errlist[msgN];
    sentBytes+=snprintf(NULL,0,"%llu (%d) %s!",sntMsg,msgN,buf);
    if(outFIFO && addHeader)
      rv=mfPrintf(outFD,sev,__func__,tryN,"%llu (%d) %s!",sntMsg++,msgN,
                  buf);
    else if(!outFIFO && addHeader)
      rv=mPrintf(L_DIM,sev,__func__,tryN,"%llu (%d) %s!",sntMsg++,msgN,
                 buf);
    else if(outFIFO && !addHeader)
      rv=rfPrintf(outFD,tryN,"%llu (%d) %s!",sntMsg++,msgN,buf);
    else if(!outFIFO && !addHeader)
      rv=rPrintf(L_DIM,tryN,"%llu (%d) %s!",sntMsg++,msgN,buf);
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
          "                      [-H][-r tryN]\n",pName);
  exit(1);
}
/*****************************************************************************/
