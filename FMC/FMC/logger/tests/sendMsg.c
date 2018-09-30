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
/*****************************************************************************/
int fifoInit(char *fifoPath,int noDrop);
void usage(void);
char *pName=NULL;
/*****************************************************************************/
int main(int argc,char **argv)
{
  char *fName="main()";
  char *p=NULL;
  struct timespec delay={0,1000000};                                /* 0.1 s */
  unsigned long long sentMsg;
  char msg[4096];
  int flag=0;
  char *outFIFO=NULL;
  int outFD=-1;
  int noDrop=0;
  char hostName[80]="";
  int max=0;
  time_t now;
  struct tm lNow;
  char sNow[80];
  int msgSize=79;
  sigset_t signalMask;
  int signo=-1;
  struct timespec nullTS={0,0};
  char debugStr[4096]="";
  char infoStr[4096]="";
  char warnStr[4096]="";
  char errorStr[4096]="";
  char fatalStr[4096]="";
  time_t startTime=0,stopTime=0;
  unsigned long long elapsedTime=0;
  double msgRate=0.0,byteRate=0.0,bitRate=0.0;
  int addHeader=1;
  /*-------------------------------------------------------------------------*/
  pName=strdup(basename(argv[0]));
  gethostname(hostName,80);
  p=strchr(hostName,'.');
  if(p)*p='\0';
  /*-------------------------------------------------------------------------*/
  while((flag=getopt(argc,argv,"+hp:s:n:M:AS:N"))!=EOF)
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
      case 'h':
      default:
        usage();
    }
  }
  if(argc-optind!=0)usage();
  if(!outFIFO)outFIFO="/tmp/logSrv.fifo";
  if(msgSize>4094)
  {
    fprintf(stderr,"Illegal message size %d>4094!\n",msgSize);
    usage();
  }
  /*-------------------------------------------------------------------------*/
  outFD=fifoInit(outFIFO,noDrop);
  if(outFD==-1)
  {
    fprintf(stderr,"Can't open FIFO: \"%s\"!",outFIFO);
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  for(;;)
  {
    if(strlen(debugStr)>4094-8)break;
    strcat(debugStr,"debug ");
  }
  for(;;)
  {
    if(strlen(infoStr)>4094-8)break;
    strcat(infoStr,"info ");
  }
  for(;;)
  {
    if(strlen(warnStr)>4094-8)break;
    strcat(warnStr,"warning ");
  }
  for(;;)
  {
    if(strlen(errorStr)>4094-8)break;
    strcat(errorStr,"error ");
  }
  for(;;)
  {
    if(strlen(fatalStr)>4094-8)break;
    strcat(fatalStr,"fatal ");
  }
  /*-------------------------------------------------------------------------*/
  sigemptyset(&signalMask);
  sigaddset(&signalMask,SIGINT);
  sigaddset(&signalMask,SIGTERM);
  sigprocmask(SIG_BLOCK,&signalMask,NULL);
  /*-------------------------------------------------------------------------*/
  startTime=time(NULL);
  for(sentMsg=0;;)
  {
    nullTS.tv_sec=0;
    nullTS.tv_nsec=0;
    signo=sigtimedwait(&signalMask,NULL,&nullTS);
    if(signo==SIGTERM||signo==SIGINT)break;
    /*.......................................................................*/
    if(sentMsg<100 || !(sentMsg%50))printf("msg #: %llu\n",sentMsg);
    /*.......................................................................*/
    if(max>0 && sentMsg>=max)break;
    if(addHeader)
    {
      now=time(NULL);
      localtime_r(&now,&lNow);
      strftime(sNow,80,"%b%d-%H%M%S",&lNow);
      snprintf(msg,4096,"%s[DEBUG]%s: %s: %s: %llu %s",sNow,hostName,pName,
               fName,sentMsg++,debugStr);
    }
    else snprintf(msg,4096,"%llu %s",sentMsg++,debugStr);
    msg[msgSize]='\n';
    msg[msgSize+1]='\0';
    write(outFD,msg,strlen(msg));
    if(delay.tv_nsec||delay.tv_sec)
    {
      signo=sigtimedwait(&signalMask,NULL,&delay);
      if(signo==SIGTERM||signo==SIGINT)break;
    }
    /*.......................................................................*/
    if(max>0 && sentMsg>=max)break;
    if(addHeader)
    {
      now=time(NULL);
      localtime_r(&now,&lNow);
      strftime(sNow,80,"%b%d-%H%M%S",&lNow);
      snprintf(msg,4096,"%s[INFO] %s: %s: %s: %llu %s",sNow,hostName,pName,
               fName,sentMsg++,infoStr);
    }
    else snprintf(msg,4096,"%llu %s",sentMsg++,infoStr);
    msg[msgSize]='\n';
    msg[msgSize+1]='\0';
    write(outFD,msg,strlen(msg));
    if(delay.tv_nsec||delay.tv_sec)
    {
      signo=sigtimedwait(&signalMask,NULL,&delay);
      if(signo==SIGTERM||signo==SIGINT)break;
    }
    /*.......................................................................*/
    if(max>0 && sentMsg>=max)break;
    if(addHeader)
    {
      now=time(NULL);
      localtime_r(&now,&lNow);
      strftime(sNow,80,"%b%d-%H%M%S",&lNow);
      snprintf(msg,4096,"%s[WARN] %s: %s: %s: %llu %s",sNow,hostName,pName,
               fName,sentMsg++,warnStr);
    }
    else snprintf(msg,4096,"%llu %s",sentMsg++,warnStr);
    msg[msgSize]='\n';
    msg[msgSize+1]='\0';
    write(outFD,msg,strlen(msg));
    if(delay.tv_nsec||delay.tv_sec)
    {
      signo=sigtimedwait(&signalMask,NULL,&delay);
      if(signo==SIGTERM||signo==SIGINT)break;
    }
    /*.......................................................................*/
    if(max>0 && sentMsg>=max)break;
    if(addHeader)
    {
      now=time(NULL);
      localtime_r(&now,&lNow);
      strftime(sNow,80,"%b%d-%H%M%S",&lNow);
      snprintf(msg,4096,"%s[ERROR]%s: %s: %s: %llu %s",sNow,hostName,pName,
               fName,sentMsg++,errorStr);
    }
    else snprintf(msg,4096,"%llu %s",sentMsg++,errorStr);
    msg[msgSize]='\n';
    msg[msgSize+1]='\0';
    write(outFD,msg,strlen(msg));
    if(delay.tv_nsec||delay.tv_sec)
    {
      signo=sigtimedwait(&signalMask,NULL,&delay);
      if(signo==SIGTERM||signo==SIGINT)break;
    }
    /*.......................................................................*/
    if(max>0 && sentMsg>=max)break;
    if(addHeader)
    {
      now=time(NULL);
      localtime_r(&now,&lNow);
      strftime(sNow,80,"%b%d-%H%M%S",&lNow);
      snprintf(msg,4096,"%s[FATAL]%s: %s: %s: %llu %s",sNow,hostName,pName,
               fName,sentMsg++,fatalStr);
    }
    else snprintf(msg,4096,"%llu %s",sentMsg++,fatalStr);
    msg[msgSize]='\n';
    msg[msgSize+1]='\0';
    write(outFD,msg,strlen(msg));
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
  byteRate=msgRate*(double)msgSize;
  bitRate=msgRate*(double)msgSize*8.0;
  printf("Message rate: %f Msg/s\t= %f kMsg/s\n",msgRate,msgRate/1000.);
  printf("Byte rate     %f KiB/s\t= %f MiB/s\n",byteRate/1024.,
         byteRate/1048576.);
  printf("Bit rate:     %f kb/s\t= %f Mb/s\n",bitRate/1000.,bitRate/1000000.);
  printf("----------------------------------------------------------------\n");
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
void usage(void)
{
  fprintf(stderr,"Usage: %s [-p fifo_path][-s seconds][-n nanoseconds]\n"
          "                      [-M number_of_messages][-A(no-drop)]\n"
          "                      [-S message_size(<4094)][-N]\n",pName);
  exit(1);
}
/*****************************************************************************/
int fifoInit(char *fifoPath,int noDrop)
{
  struct stat statBuf;
  int fifoFD=-1;
  /*-------------------------------------------------------------------------*/
  /* check if fifoPath is writable */
  if(access(fifoPath,W_OK)==-1)                          /* access denied */
  {
    fprintf(stderr,"access(\"%s\"): %s!",fifoPath,strerror(errno));
    return -1;
  }
  /* check if fifoPath is a pipe */
  if(stat(fifoPath,&statBuf)==-1)
  {
    fprintf(stderr,"stat(\"%s\"): %s!",fifoPath,strerror(errno));
    return -1;
  }
  if(!S_ISFIFO(statBuf.st_mode))
  {
    fprintf(stderr,"I-node: \"%s\" is not a FIFO!",fifoPath);
    return -1;
  }
  /*-------------------------------------------------------------------------*/
  /* open error log */
  if(noDrop)fifoFD=open(fifoPath,O_WRONLY|O_APPEND);   /* no-drop */
  else fifoFD=open(fifoPath,O_RDWR|O_NONBLOCK|O_APPEND); /* congestion-proof */
  if(fifoFD==-1)
  {
    if(errno==ENXIO)
    {
      fprintf(stderr,"open(\"%s\"): No process has the FIFO \"%s\" "
             "open for reading!",fifoPath,fifoPath);
    }
    else
    {
      fprintf(stderr,"open(\"%s\"): %s!",fifoPath,strerror(errno));
    }
    return -1;
  }
  /*-------------------------------------------------------------------------*/
  return fifoFD;
}
/*****************************************************************************/
