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
  unsigned long long sentMsg;
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
  char *fortName=NULL;
  FILE *fortFP=NULL;
  char buf[MSG_SZ]="";
  int sev=0;
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
  if(argc-optind!=0 && argc-optind!=1)usage();
  if(argc-optind==0)fortName="/opt/FMC/tests/oneliners";
  else fortName=argv[optind];
  if(!addHeader && tryN==0)tryN=1;
  /*-------------------------------------------------------------------------*/
  fortFP=fopen(fortName,"r");
  if(!fortFP)
  {
    fprintf(stderr,"fopen(\"%s\"): %s!\n",fortName,strerror(errno));
    exit(1);
  }
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
  startTime=time(NULL);
  for(sentMsg=0,sentBytes=0;;)
  {
    /*.......................................................................*/
    if(max>0 && sentMsg>=max)break;
    if(sentMsg<100 || !(sentMsg%50))
      printf("msg #: %llu - drop#: %llu\n",sentMsg,droppedN);
    /*.......................................................................*/
    for(p=buf;;)
    {
      if(!fgets(p,MSG_SZ-(p-buf),fortFP))
      {
        fseek(fortFP,0L,SEEK_SET);
        break;
      }
      if(*p=='%')
      {
        *p=0;
        /*...................................................................*/
        sev=1+sentMsg%5;
        sentBytes+=snprintf(NULL,0,"%llu %s",sentMsg,buf);
        if(outFIFO && addHeader)
          rv=mfPrintf(outFD,sev,__func__,tryN,"%llu %s",sentMsg++,buf);
        else if(!outFIFO && addHeader)
          rv=mPrintf(L_DIM,sev,__func__,tryN,"%llu %s",sentMsg++,buf);
        else if(outFIFO && !addHeader)
          rv=rfPrintf(outFD,tryN,"%llu %s",sentMsg++,buf);
        else if(!outFIFO && !addHeader)
          rv=rPrintf(L_DIM,tryN,"%llu %s",sentMsg++,buf);
        if(rv)droppedN+=rv;
        /*...................................................................*/
        break;
      }
      p=strchr(p,'\n');
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
  msgRate=(double)sentMsg/(double)elapsedTime;
  byteRate=(double)sentBytes/(double)elapsedTime;
  bitRate=byteRate*8.0;
  droppedP=(double)droppedN*100.0/sentMsg;
  printf("Message rate: %f Msg/s\t= %f kMsg/s\n",msgRate,msgRate/1000.);
  printf("Byte rate     %f KiB/s\t= %f MiB/s\n",byteRate/1024.,
         byteRate/1048576.);
  printf("Bit rate:     %f kb/s\t= %f Mb/s\n",bitRate/1000.,bitRate/1000000.);
  printf("Dropped/sent: %llu/%llu = %f %%\n",droppedN,sentMsg,droppedP);
  printf("----------------------------------------------------------------\n");
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
void usage(void)
{
  fprintf(stderr,"Usage: %s [-p fifo_path][-s seconds][-n nanoseconds]\n"
          "                      [-M number_of_messages][-A(no-drop)]\n"
          "                      [-H][fortune_file][-r tryN]\n",pName);
  exit(1);
}
/*****************************************************************************/
