//  to use clock_gettime compile with: cc -Wall -O3  counter.c  -lrt -lm -o counter
#include <stdio.h>
#include <stdlib.h>                                               /* exit(3) */
#include <unistd.h>
//#include <time.h>                                      /* clock_gettime(3) */
#include <sys/timeb.h>                                           /* ftime(3) */
#include <signal.h>
int main(int argc,char **argv,char **envp)
{
  unsigned long long i;
  //struct timespec startTime,stopTime;
  struct timeb startTime;
  double startTimeF=0.0;
  //int signo;
  //sigset_t signalMask;
  //struct timespec updatePeriodTS={1,0};
  char* utgid=NULL;

  utgid=getenv("UTGID");
  if(!utgid)utgid="N/A";
  //clock_gettime(CLOCK_REALTIME,&startTime);
  //startTimeF=(double)startTime.tv_sec+(double)startTime.tv_nsec*1e-9;
  ftime(&startTime);
  startTimeF=(double)startTime.time+(double)startTime.millitm/1000.0;
  fprintf(stderr,"%s: startTime=%f\n",utgid,startTimeF);
  //sigemptyset(&signalMask);
  //sigaddset(&signalMask,SIGINT);
  //sigaddset(&signalMask,SIGTERM);
  //sigprocmask(SIG_BLOCK,&signalMask,NULL);
  for(i=0;;i++)
  {
    printf("%llu\n",i);
    fflush(stdout);
    sleep(1);
//    updatePeriodTS.tv_sec=1;
//    updatePeriodTS.tv_nsec=0;
//    signo=sigtimedwait(&signalMask,NULL,&updatePeriodTS);
//    if(signo!=-1)
//    {
//      //clock_gettime(CLOCK_REALTIME,&stopTime);
//      //stopTimeF=(double)stopTime.tv_sec+(double)stopTime.tv_nsec*1e-9;
//      ftime(&stopTime);
//      stopTimeF=(double)stopTime.time+(double)stopTime.millitm/1000.0;
//      fprintf(stderr,"%s: stopTime=%f\n",utgid,stopTimeF);
//      exit(0);
//    }
  }
  return 0;
}
