/*****************************************************************************/
/*
 * $Log: getCoalescence.c,v $
 * Revision 1.9  2008/10/09 09:53:06  galli
 * minor changes
 *
 * Revision 1.8  2008/02/23 22:45:06  galli
 * minor changes
 *
 * Revision 1.7  2007/08/10 12:51:51  galli
 * added function getCoalSensorVersion()
 * compatible with libFMCutils v 2
 *
 * Revision 1.6  2006/10/21 06:02:23  galli
 * bug fixed
 *
 * Revision 1.5  2006/10/20 22:47:28  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.4  2004/11/17 00:38:36  galli
 * counters reset on resetCoal() call
 * time since started added to coalescenceFactors structure
 *
 * Revision 1.3  2004/11/08 15:07:20  galli
 * publishes sensor version
 *
 * Revision 1.2  2004/11/06 13:00:59  galli
 * coalInfo() function added
 *
 * Revision 1.1  2004/11/06 09:52:49  galli
 * Initial revision
 *
 * */
/*****************************************************************************/
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <errno.h>                                                  /* errno */
#include <string.h>                                  /* strerror(), strcmp() */
#include <ctype.h>                                              /* isspace() */
#include <syslog.h>                                              /* syslog() */
#include <sys/time.h>                                      /* gettimeofday() */
#include <sys/types.h>                                             /* open() */
#include <sys/stat.h>                                              /* open() */
#include <fcntl.h>                                                 /* open() */
#include <unistd.h>                              /* lseek(), read(), close() */
#include <values.h>                                               /* FLT_MIN */
#include <limits.h>                                 /* ULONG_MAX, ULLONG_MAX */
/*****************************************************************************/
#include "getNIFfromProc.h"                               /* struct nifsData */
#include "getIrqFromProc.h"                         /* struct interruptsStat */
#include "getCoalescence.h"                      /* struct coalescenceFactor */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#include "fmcTssUtils.h"                                   /* getTssString() */
/*****************************************************************************/
/* global variables */
int doResetCoalCnts=0;    /* set to 1 by resetCoal() to schedule a cnt reset */
static char rcsid[]="$Id: getCoalescence.c,v 1.9 2008/10/09 09:53:06 galli Exp $";
/*****************************************************************************/
struct coalescenceFactors
getCoal(struct nifsData nifs,struct interruptsStat ints,int deBug,int errU)
{
  static struct coalescenceFactors coals;
  int i,j,k,l;
  static int isFirstCall=1;
  float intRate,frameRate;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  coals.success=0;
  /*-------------------------------------------------------------------------*/
  if(isFirstCall)
  {
    /*.......................................................................*/
    /* clean */
    memset(&coals,0,sizeof(coals));
    coals.version=rcsid;
    /*.......................................................................*/
    /* find active NIF number */
    for(i=0;i<nifs.nifN;i++)
    {
      if(strstr(nifs.data[i].name,"eth")==nifs.data[i].name)
      {
        for(j=0;j<ints.srcN;j++)
        {
          if(strstr(ints.srcName[j],nifs.data[i].name)!=NULL)
          {
            coals.ethN++;
            break;
          }
        }
      }
    }
    if(deBug)mPrintf(errU,DEBUG,__func__,0,"ethN=%d",coals.ethN);
    /*.......................................................................*/
    /* allocate space for array */
    coals.data=(struct coalescenceFactor*)malloc((size_t)(coals.ethN*
                                            sizeof(struct coalescenceFactor)));
    memset(coals.data,0,coals.ethN*sizeof(struct coalescenceFactor));
    /*.......................................................................*/
    /* find active NIF names */
    for(i=0,k=0;k<coals.ethN;i++)
    {
      if(strstr(nifs.data[i].name,"eth")==nifs.data[i].name)
      {
        for(j=0;j<ints.srcN;j++)
        {
          if(strstr(ints.srcName[j],nifs.data[i].name)!=NULL)
          {
            coals.data[k].ifName=strdup(nifs.data[i].name);
            k++;
            break;
          }
        }
      }
    }
    if(deBug)
    {
      for(k=0;k<coals.ethN;k++)
      {
        mPrintf(errU,DEBUG,__func__,0,"ifName %d = %s",k,coals.data[k].ifName);
      }
    }
    /*.......................................................................*/
    isFirstCall=0;
    /*.......................................................................*/
  }                                                       /* if(isFirstCall) */
  /*-------------------------------------------------------------------------*/
  for(i=0;i<coals.ethN;i++)
  {
    coals.data[i].coalFactor=0;
    coals.data[i].aCoalFactor=0;
  }
  /*-------------------------------------------------------------------------*/
  if(doResetCoalCnts)
  {
    for(k=0;k<coals.ethN;k++)coals.data[k].mCoalFactor=0;
    mPrintf(errU,INFO,__func__,0,"Counters reset requested by user!");
    doResetCoalCnts=0;
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate coalescence factors */
  for(i=0,k=0;k<coals.ethN;i++)
  {
    if(!strcmp(nifs.data[i].name,coals.data[k].ifName))
    {
      for(j=0;j<ints.srcN;j++)
      {
        if(strstr(ints.srcName[j],coals.data[k].ifName)!=NULL)
        {
          /* present values */
          for(l=0,intRate=0.0;l<ints.cpuN;l++)intRate+=ints.irqRate[j][l];
          frameRate=nifs.data[i].rate.rxFrameRate+
                     nifs.data[i].rate.txFrameRate;
          if(intRate<FLT_MIN)coals.data[k].coalFactor=0;
          else coals.data[k].coalFactor=frameRate/intRate;
          /* average values */
          for(l=0,intRate=0.0;l<ints.cpuN;l++)intRate+=ints.aIrqRate[j][l];
          frameRate=nifs.data[i].arate.rxFrameRate+
                     nifs.data[i].arate.txFrameRate;
          if(intRate<FLT_MIN)coals.data[k].aCoalFactor=0;
          else coals.data[k].aCoalFactor=frameRate/intRate;
          /* maximum values */
          if(coals.data[k].mCoalFactor<coals.data[k].coalFactor)
             coals.data[k].mCoalFactor=coals.data[k].coalFactor;
          k++;
          break;
        }
      }
    }
  }
  coals.stss=nifs.stss;
  /*-------------------------------------------------------------------------*/
  if(k==coals.ethN)coals.success=1;
  /*-------------------------------------------------------------------------*/
  return coals;
}
/*****************************************************************************/
void resetCoal()
{
  doResetCoalCnts=1;
  return;
}
/*****************************************************************************/
char *coalInfo(char *what)
{
  static char info[1024];
  memset(info,0,sizeof(info));
  if(!strcmp(what,"ls"))
  {
    snprintf(info,1024,"coalescence_ratio ");
  }
  else if(!strcmp(what,"coalescence_ratio"))
  {
    snprintf(info,1024,"The ratio between the number of frames received by "
             "a network interface and the number of interrupts raised by the "
             "network interface card. Usually the Gigabit Ethernet NICs "
             "don't raise an interrupt for every frame received, but store "
             "the received frames in a buffer and raise an interrupt after "
             "an appropriate delay to deliver more than one frame for each "
             "interrupt handler execution. This procedure can reduce "
             "CPU utilization up to 30%% in frame receiving and up to 11%% "
             "in frame sending. This mechanism can be tuned by setting "
             "appropriate parameters on NIC (e.g., for Intel e1000, "
             "InterruptThrottleRate, RxIntDelay, RxAbsIntDelay, TxIntDelay, "
             "and TxAbsIntDelay).");
  }
  else
  {
    sprintf(info,"No informations about item: \"%s\".",what);
  }
  return info;
}
/*****************************************************************************/
/* getCoalSensorVersion() - Returns the RCS identifier of this file.         */
/*****************************************************************************/
char *getCoalSensorVersion()
{
  return rcsid;
}
/*****************************************************************************/
