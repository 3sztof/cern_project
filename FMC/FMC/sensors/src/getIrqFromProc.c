/*****************************************************************************/
/*
 * $Log: getIrqFromProc.c,v $
 * Revision 1.12  2008/10/09 09:53:06  galli
 * minor changes
 *
 * Revision 1.11  2007/08/10 13:13:35  galli
 *  added function getIRQsensorVersion()
 * compatible with libFMCutils v 2
 *
 * Revision 1.10  2006/10/23 20:00:26  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.9  2004/11/17 00:02:14  galli
 * counters reset on resetInterrupts() call
 * time since started added to interruptStat structure
 *
 * Revision 1.8  2004/10/21 23:25:27  galli
 * Logs errors to logger
 *
 * Revision 1.7  2004/09/18 22:21:53  galli
 * added driver info
 *
 * Revision 1.6  2004/09/18 21:58:05  galli
 * cope with change in interrupt source list
 * added interruptInfo() function
 *
 * Revision 1.5  2004/09/14 12:04:46  galli
 * added average and maximum values
 *
 * Revision 1.3  2004/09/02 12:27:28  galli
 * first working version
 *
 * Revision 1.1  2004/08/31 15:20:17  galli
 * Initial revision
 * */
/*****************************************************************************/
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <stdlib.h>                                              /* strtol() */
#include <errno.h>                                                  /* errno */
#include <string.h>                                  /* strerror(), strcmp() */
#include <ctype.h>                                              /* isspace() */
#include <sys/time.h>                                      /* gettimeofday() */
#include <sys/types.h>                                             /* open() */
#include <sys/stat.h>                                              /* open() */
#include <fcntl.h>                                                 /* open() */
#include <unistd.h>                                     /* read(), sysconf() */
#include <values.h>                                               /* FLT_MIN */
#include <math.h>                                                  /* fabs() */
/*****************************************************************************/
#include "getIrqFromProc.h"                           /* struct interruptVal */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#include "fmcTssUtils.h"                                   /* getTssString() */
#define PROC_INTERRUPTS_SIZE 2048
#define MSG_SIZE 4000
/*****************************************************************************/
/* globals */
char intFileName[]="/proc/interrupts";
int doResetIrqCnts=0; /* set to 1 by resetInterrupts() to schedule a cnt rst */
static char rcsid[]="$Id: getIrqFromProc.c,v 1.12 2008/10/09 09:53:06 galli Exp $";
/*****************************************************************************/
/* function prototype */
char *getProcInterrupt(int errU);
/*****************************************************************************/
struct interruptsStat readInterrupts(int deBug,int errU)
{
  static int isFirstCall=1;
  char *procInterruptsBuf=NULL;
  int i,j;
  int tokN;
  static char **tok=NULL;
  char *last=NULL;
  char *bp,*ep;
  static struct interruptsStat irqStat;
  static unsigned **irqCount=NULL,**oldIrqCount=NULL;
  static unsigned long long **tIrqCount=NULL;
  int dIrqCount;
  struct timezone tz;
  int sec,usec;
  float et=0.0,tss=0;
  static struct timeval now={0,0},lastTime={0,0},startTime={0,0};
  /*-------------------------------------------------------------------------*/
  if(deBug>1)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  irqStat.success=0;
  irqStat.version=rcsid;
  irqStat.stss=getTssString(0.0);
  /*-------------------------------------------------------------------------*/
  if(isFirstCall)
  {
    /*.......................................................................*/
    /* evaluate processors number: cpuN */
    irqStat.cpuN=sysconf(_SC_NPROCESSORS_ONLN);
    if(irqStat.cpuN<1)irqStat.cpuN=1;
    /*.......................................................................*/
    /* allocate space for token in a file line */
    tok=(char**)malloc((size_t)((4+irqStat.cpuN)*sizeof(char*)));
    /*.......................................................................*/
    /* evaluate interrupt source number: srcN */
    procInterruptsBuf=getProcInterrupt(errU);  /* get /proc/interrupts conts */
    if(!procInterruptsBuf)
    {
      mPrintf(errU,ERROR,__func__,0,"getProcInterrupt() call retuned an "
              "error!");
      return irqStat;
    }
    bp=procInterruptsBuf;
    bp=strchr(bp,'\n')+1;                                  /* eat first line */
    for(irqStat.srcN=0;;)
    {
      ep=strchr(bp,'\n');                                        /* find EOL */
      if(ep==NULL)break;
      *ep='\0';                             /* set string termination to EOL */
      /* tokenize a line */
      strtok_r(bp," \t",&last);
      for(tokN=1;strtok_r(NULL," \t",&last);tokN++);
      /* end line tokenization */
      if(tokN>=3+irqStat.cpuN)irqStat.srcN++;
      bp=ep+1;     /* set string beginning to the beginning of the next line */
    }
    /*.......................................................................*/
    /* allocate space for srcName and irqN */
    irqStat.srcName=(char**)malloc((size_t)(irqStat.srcN*sizeof(char*)));
    irqStat.irqN=(int*)malloc((size_t)(irqStat.srcN*sizeof(int)));
    /* allocate space for irqCount, oldIrqCount ,tIrqCount and irqRate */
    irqCount=(unsigned**)malloc((size_t)(irqStat.srcN*sizeof(unsigned*)));
    oldIrqCount=(unsigned**)malloc((size_t)(irqStat.srcN*sizeof(unsigned*)));
    tIrqCount=(unsigned long long**)malloc((size_t)(irqStat.srcN*
                                                 sizeof(unsigned long long*)));
    irqStat.irqRate=(float**)malloc((size_t)(irqStat.srcN*sizeof(float*)));
    irqStat.aIrqRate=(float**)malloc((size_t)(irqStat.srcN*sizeof(float*)));
    irqStat.mIrqRate=(float**)malloc((size_t)(irqStat.srcN*sizeof(float*)));
    for(i=0;i<irqStat.srcN;i++)
    {
      irqCount[i]=(unsigned*)malloc((size_t)(irqStat.cpuN*sizeof(unsigned)));
      oldIrqCount[i]=(unsigned*)malloc((size_t)(irqStat.cpuN*sizeof(unsigned)));
      tIrqCount[i]=(unsigned long long*)malloc((size_t)(irqStat.cpuN*
                                                  sizeof(unsigned long long)));
      irqStat.irqRate[i]=(float*)malloc((size_t)(irqStat.cpuN*sizeof(float)));
      irqStat.aIrqRate[i]=(float*)malloc((size_t)(irqStat.cpuN*sizeof(float)));
      irqStat.mIrqRate[i]=(float*)malloc((size_t)(irqStat.cpuN*sizeof(float)));
      memset(irqStat.mIrqRate[i],0,irqStat.cpuN*sizeof(float));
    }
    /*.......................................................................*/
    /* evaluate interrupt source names (srcName) and and number (irqN) */
    procInterruptsBuf=getProcInterrupt(errU);  /* get /proc/interrupts conts */
    if(!procInterruptsBuf)
    {
      mPrintf(errU,ERROR,__func__,0,"getProcInterrupt() call retuned an "
              "error!");
      /* free allocated memory */
      for(i=0;i<irqStat.srcN;i++)
      {
        free(irqCount[i]);free(oldIrqCount[i]);free(tIrqCount[i]);
        free(irqStat.irqRate[i]);free(irqStat.aIrqRate[i]);
        free(irqStat.mIrqRate[i]);
      }
      free(irqStat.srcName);free(irqStat.irqN);free(irqCount);
      free(oldIrqCount);free(tIrqCount);free(irqStat.irqRate);
      free(irqStat.aIrqRate);free(irqStat.mIrqRate);
      free(tok);
      return irqStat;
    }
    bp=procInterruptsBuf;
    bp=strchr(bp,'\n')+1;                                  /* eat first line */
    for(i=0;i<irqStat.srcN;)
    {
      ep=strchr(bp,'\n');                                        /* find EOL */
      if(ep==NULL)break;
      *ep='\0';                             /* set string termination to EOL */
      /* tokenize a line */
      tok[0]=strtok_r(bp," \t",&last);
      for(tokN=1;;tokN++)
      {
        /* keep togrther irq sources conaining spaces */
        if(tokN==2+irqStat.cpuN)tok[tokN]=strtok_r(NULL,"",&last);
        /* split */
        else tok[tokN]=strtok_r(NULL," \t",&last);
        if(tok[tokN]==NULL)break;
      }
      /* end line tokenization */
      while(isspace(*tok[tokN-1]))tok[tokN-1]++;    /* delete leading blanks */
      if(tokN>=3+irqStat.cpuN)
      {
        irqStat.irqN[i]=strtol(tok[0],NULL,10);
        irqStat.srcName[i]=(char*)malloc((size_t)
                                              (1+strlen(tok[2+irqStat.cpuN])));
        strcpy(irqStat.srcName[i],tok[2+irqStat.cpuN]);
        i++;
      }
      bp=ep+1;     /* set string beginning to the beginning of the next line */
    }
    /*.......................................................................*/
    /* set counter to 0 */
    for(i=0;i<irqStat.srcN;i++)for(j=0;j<irqStat.cpuN;j++)irqCount[i][j]=0;
    for(i=0;i<irqStat.srcN;i++)for(j=0;j<irqStat.cpuN;j++)tIrqCount[i][j]=0;
    gettimeofday(&now,&tz);
    gettimeofday(&startTime,&tz);
    /*.......................................................................*/
  }                                                       /* if(isFirstCall) */
  /*-------------------------------------------------------------------------*/
  /* move conter read last time from irqCount to oldIrqCount */
  for(i=0;i<irqStat.srcN;i++)
  {
    for(j=0;j<irqStat.cpuN;j++)
    {
      oldIrqCount[i][j]=irqCount[i][j];
      irqCount[i][j]=0;
    }
  }
  /* move time read last time from now to lastTime */
  memcpy((char*)&lastTime,(char*)&now,sizeof(struct timeval));
  memset((char*)&now,0,sizeof(struct timeval));
  /*-------------------------------------------------------------------------*/
  /* get interrupt values */
  procInterruptsBuf=getProcInterrupt(errU); /* get /proc/interrupts contents */
  if(!procInterruptsBuf)
  {
    mPrintf(errU,ERROR,__func__,0,"getProcInterrupt() call retuned an error!");
    /* free allocated memory */
    for(i=0;i<irqStat.srcN;i++)
    {
      free(irqCount[i]);free(oldIrqCount[i]);free(tIrqCount[i]);
      free(irqStat.irqRate[i]);free(irqStat.aIrqRate[i]);
      free(irqStat.mIrqRate[i]);free(irqStat.srcName[i]);
    }
    free(irqStat.srcName);free(irqStat.irqN);free(irqCount);
    free(oldIrqCount);free(tIrqCount);free(irqStat.irqRate);
    free(irqStat.aIrqRate);free(irqStat.mIrqRate);
    free(tok);
    /* restart */
    isFirstCall=1;
    return irqStat;
  }
  if(deBug>3)
  {
    mPrintf(errU,DEBUG,__func__,0,"procInterruptsBuf=%s",procInterruptsBuf);
  }
  bp=procInterruptsBuf;
  bp=strchr(bp,'\n')+1;                                    /* eat first line */
  for(i=0;i<irqStat.srcN;)
  {
    ep=strchr(bp,'\n');                                          /* find EOL */
    if(ep==NULL)break;
    *ep='\0';                               /* set string termination to EOL */
    /* tokenize a line */
    tok[0]=strtok_r(bp," \t",&last);
    for(tokN=1;;tokN++)
    {
      /* keep togrther irq sources containing spaces */
      if(tokN==2+irqStat.cpuN)tok[tokN]=strtok_r(NULL,"",&last);
      /* split */
      else tok[tokN]=strtok_r(NULL," \t",&last);
      if(tok[tokN]==NULL)break;
    }
    /* end line tokenization */
    while(isspace(*tok[tokN-1]))tok[tokN-1]++;      /* delete leading blanks */
    if(tokN>=3+irqStat.cpuN)
    {
      if(strtol(tok[0],NULL,10)!=irqStat.irqN[i]||
         strcmp(irqStat.srcName[i],tok[2+irqStat.cpuN]))
      {
        mPrintf(errU,WARN,__func__,0,"Irq order changed! Resetting values...");
        /* free allocated memory */
        for(i=0;i<irqStat.srcN;i++)
        {
          free(irqCount[i]);free(oldIrqCount[i]);free(tIrqCount[i]);
          free(irqStat.irqRate[i]);free(irqStat.aIrqRate[i]);
          free(irqStat.mIrqRate[i]);free(irqStat.srcName[i]);
        }
        free(irqStat.srcName);free(irqStat.irqN);free(irqCount);
        free(oldIrqCount);free(tIrqCount);free(irqStat.irqRate);
        free(irqStat.aIrqRate);free(irqStat.mIrqRate);
        free(tok);
        /* restart */
        isFirstCall=1;
        return irqStat;
      }
      for(j=0;j<irqStat.cpuN;j++)
      {
        irqCount[i][j]=strtoul(tok[j+1],NULL,10);
        if(irqCount[i][j]==ULONG_MAX && errno==ERANGE)
        {
          mPrintf(errU,FATAL,__func__,0,"strtoul(%s): %s",tok[j+1],
                  strerror(errno));
          exit(1);
        }
      }
      i++;
    }
    bp=ep+1;       /* set string beginning to the beginning of the next line */
  }
  if(deBug>2)
  {
    for(i=0;i<irqStat.srcN;i++)
    {
      for(j=0;j<irqStat.cpuN;j++)
      {
        mPrintf(errU,DEBUG,__func__,0,"irqCount[%d][%d]=%u",i,j,
                irqCount[i][j]);
      }
    }
  }
  /*-------------------------------------------------------------------------*/
  /* get time */
  gettimeofday(&now,&tz);
  /*-------------------------------------------------------------------------*/
  /* evaluate interrupt rates */
  if(isFirstCall)
  {
    /*.......................................................................*/
    /* set rates to 0.0 */
    for(i=0;i<irqStat.srcN;i++)
    {
      memset(irqStat.aIrqRate[i],0,irqStat.cpuN*sizeof(float));
      memset(irqStat.irqRate[i],0,irqStat.cpuN*sizeof(float));
    }
    /*.......................................................................*/
    isFirstCall=0;
    /*.......................................................................*/
  }
  else
  {
    /*.......................................................................*/
    if(doResetIrqCnts)
    {
      memcpy(&startTime,&lastTime,sizeof(struct timeval));
      for(i=0;i<irqStat.srcN;i++)
      {
        for(j=0;j<irqStat.cpuN;j++)
        {
          tIrqCount[i][j]=0;
          irqStat.mIrqRate[i][j]=0.0;
          irqStat.aIrqRate[i][j]=0.0;
        }
      }
      mPrintf(errU,INFO,__func__,0,"Counters reset requested by user!");
      doResetIrqCnts=0;
    }
    /*.......................................................................*/
    /* evaluate elapsed time */
    if(now.tv_usec<lastTime.tv_usec)
    {
      now.tv_usec+=1000000;
      now.tv_sec-=1;
    }
    sec=now.tv_sec-lastTime.tv_sec;                       /* time difference */
    usec=now.tv_usec-lastTime.tv_usec;                    /* time difference */
    et=(float)sec+((float)usec/(float)1000000.0);
    /*.......................................................................*/
    /* evaluate elapsed time since process start */
    if(now.tv_usec<startTime.tv_usec)
    {
      now.tv_usec+=1000000;
      now.tv_sec-=1;
    }
    sec=now.tv_sec-startTime.tv_sec;
    usec=now.tv_usec-startTime.tv_usec;
    tss=(float)sec+((float)usec/(float)1000000.0);       /* time since start */
    irqStat.stss=getTssString(tss);
    if(deBug>1)
    {
      mPrintf(errU,DEBUG,__func__,0,"tss=%f",tss);
    }
    /*.......................................................................*/
    /* evaluate interrupt count differences and rates */
    for(i=0;i<irqStat.srcN;i++)
    {
      for(j=0;j<irqStat.cpuN;j++)
      {
        dIrqCount=irqCount[i][j]-oldIrqCount[i][j];
        tIrqCount[i][j]+=dIrqCount;
        if(fabs(et)<FLT_MIN)irqStat.irqRate[i][j]=0.0;
        else
        {
          irqStat.irqRate[i][j]=(float)dIrqCount/et;
          if(irqStat.mIrqRate[i][j]<irqStat.irqRate[i][j])
            irqStat.mIrqRate[i][j]=irqStat.irqRate[i][j];
        }
        if(fabs(tss)<FLT_MIN)irqStat.aIrqRate[j][j]=0.0;
        else irqStat.aIrqRate[i][j]=(float)tIrqCount[i][j]/tss;
      }
    }
    /*.......................................................................*/
  }
  /*-------------------------------------------------------------------------*/
  irqStat.success=1;
  /*-------------------------------------------------------------------------*/
  return irqStat;
}
/*****************************************************************************/
void resetInterrupts()
{
  doResetIrqCnts=1;
  return;
}
/*****************************************************************************/
char *getProcInterrupt(int errU)
{
  int intFileFD;
  static char procInterruptsBuf[PROC_INTERRUPTS_SIZE];
  int readBytes;
  /*-------------------------------------------------------------------------*/
  /* read /proc/interrupts in procInterruptsBuf */
  intFileFD=open(intFileName,O_RDONLY);
  if(intFileFD==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",intFileName,
            strerror(errno));
    return NULL;
  }
  if(lseek(intFileFD,0L,SEEK_SET)<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot lseek file \"%s\": %s!",intFileName,
            strerror(errno));
    return NULL;
  }
  if((readBytes=read(intFileFD,procInterruptsBuf,sizeof(procInterruptsBuf)-1))
     <0) 
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot read file \"%s\": %s!",intFileName,
            strerror(errno));
    return NULL;
  }
  close(intFileFD);
  procInterruptsBuf[readBytes]='\0';
  return procInterruptsBuf;
}
/*****************************************************************************/
char *interruptInfo(char *what,int errU)
{
  static char info[2048];
  int cpuN;
  char *procInterruptsBuf=NULL;
  static char **tok=NULL;
  char *last=NULL;
  char *bp,*ep,*p1,*p2;
  int tokN;
  int i;
  /*-------------------------------------------------------------------------*/
  memset(info,0,sizeof(info));
  /*-------------------------------------------------------------------------*/
  if(!strcmp(what,"ls"))
  {
    /* evaluate processors number: cpuN */
    cpuN=sysconf(_SC_NPROCESSORS_ONLN);
    if(cpuN<1)cpuN=1;
    /* allocate space for token in a file line */
    tok=(char**)malloc((size_t)((4+cpuN)*sizeof(char*)));
    /* get /proc/interrupts contents */
    procInterruptsBuf=getProcInterrupt(errU);
    if(!procInterruptsBuf)
    {
      mPrintf(errU,ERROR,__func__,0,"getProcInterrupt() call retuned an "
              "error!");
      free(tok);
      return "";
    }
    strcpy(info,"");
    bp=procInterruptsBuf;
    bp=strchr(bp,'\n')+1;                                  /* eat first line */
    for(i=0;;)
    {
      ep=strchr(bp,'\n');                                        /* find EOL */
      if(ep==NULL)break;
      *ep='\0';                             /* set string termination to EOL */
      /* tokenize a line */
      tok[0]=strtok_r(bp," \t",&last);
      for(tokN=1;;tokN++)
      {
        /* keep togrther irq sources conaining spaces */
        if(tokN==2+cpuN)tok[tokN]=strtok_r(NULL,"",&last);
        /* split */
        else tok[tokN]=strtok_r(NULL," \t",&last);
        if(tok[tokN]==NULL)break;
      }
      /* end line tokenization */
      while(isspace(*tok[tokN-1]))tok[tokN-1]++;    /* delete leading blanks */
      if(tokN>=3+cpuN)
      {
        strcat(info,tok[2+cpuN]);
        strcat(info,",");
        i++;
      }
      bp=ep+1;     /* set string beginning to the beginning of the next line */
    }
    free(tok);
    /* delete leading blanks */
    for(p1=info,p2=info;*p1;p1++,p2++)
    {
      *p2=*p1;
      if(*p1==',' && *(p1+1)==' ')p1++;
    }
    *p2='\0';
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"acpi"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "Advanced Configuration & Power Interface subsystem.");
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"timer"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the 16-bit "
             "Programmable Interval Timer (PIT) on single processor PC or "
             "by 32-bit local Advanced Programmable Interrupt Counter (local "
             "APIC) on SMP system.");
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"keyboard")||!strcmp(what,"i8042"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "keyboard controller.");
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"cascade"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "second interrupt controller. Interrupt 2 is used to cascade "
             "the second interrupt controller to the first, allowing the "
             "use of extra IRQs 8 to 15. The interrupt controller has 8 "
             "input lines that take requests from one of 8 different "
             "devices. The controller then passes the request on to the "
             "processor, telling it which device issued the request (which "
             "interrupt number triggered the request, from 0 to 7). The "
             "original PC and XT had one of these controllers, and hence "
             "supported interrupts 0 to 7 only. Starting with the IBM AT, a "
             "second interrupt controller was added to the system to expand "
             "it. In order to ensure compatibility the designers of the AT "
             "didn't want to change the single interrupt line going to the "
             "processor. So what they did instead was to cascade the two "
             "interrupt controllers together. The first interrupt "
             "controller still has 8 inputs and a single output going to "
             "the processor. The second one has the same design, but it "
             "takes 8 new inputs (doubling the number of interrupts) and "
             "its output feeds into input line 2 of the first controller. "
             "If any of the inputs on the second controller become active, "
             "the output from that controller triggers interrupt 2 on the "
             "first controller, which then signals the processor.");
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"ide0"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "primary IDE channel.");
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"ide1"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "secondary IDE channel.");
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"PS/2 Mouse"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the PS/2 "
             "Mouse.");
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"rtc"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the Real "
             "Time Clock (RTC).");
  }
  /*-------------------------------------------------------------------------*/
  else if(strstr(what,"Serial")==what)
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the serial "
             "device: \"%s\".",what);
  }
  /*-------------------------------------------------------------------------*/
  else if(strstr(what,"eth")==what)
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "Ethernet network interface \"%s\".",what);
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"aic7xxx"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "Adaptec SCSI interface \"%s\".",what);
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"sym53c8xx"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "NCR SCSI interface 53C8XX ");
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"usb-uhci"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "USB Universal Host Controller Interface \"%s\".",what);
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"usb-ohci"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "USB Open Host Controller Interface \"%s\".",what);
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"usb-ehci"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "USB Enhanced Host Controller Interface \"%s\".",what);
  }
  /*-------------------------------------------------------------------------*/
  else if(!strcmp(what,"Intel ICH2"))
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "Intel I/O Controller Hub 2 (ICH2).");
  }
  /*-------------------------------------------------------------------------*/
  else
  {
    snprintf(info,2048,"Number of interrupts issued in a second by the "
             "device: \"%s\".",what);
  }
  /*-------------------------------------------------------------------------*/
  return info;
}
/*****************************************************************************/
/* getIRQsensorVersion() - Returns the RCS identifier of this file.          */
/*****************************************************************************/
char *getIRQsensorVersion()
{
  return rcsid;
}
/*****************************************************************************/

