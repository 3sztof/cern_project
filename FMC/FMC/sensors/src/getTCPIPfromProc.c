/*****************************************************************************/
/* 
 * $Log: getTCPIPfromProc.c,v $
 * Revision 1.21  2009/01/29 10:52:45  galli
 * cleanup
 *
 * Revision 1.19  2007/08/10 14:07:38  galli
 * added function getTCPIPsensorVersion()
 * compatible with libFMCutils v 2
 *
 * Revision 1.18  2006/10/23 21:01:35  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.17  2005/08/28 09:55:27  galli
 * bugs fixed
 *
 * Revision 1.16  2005/08/28 09:07:02  galli
 * function tcpipInfo() suppressed
 *
 * Revision 1.15  2004/11/17 12:37:39  galli
 * counters reset on resetTCPIP() call
 * time since started added to tcpipData structure
 *
 * Revision 1.14  2004/10/22 09:45:53  galli
 * Print errors to logger
 *
 * Revision 1.13  2004/09/13 14:14:17  galli
 * added maximum rates and maximum ratios
 *
 * Revision 1.12  2004/09/05 16:04:44  galli
 * tcpipInfo() function added
 *
 * Revision 1.10  2004/09/05 09:46:40  galli
 * added 64-bit counters
 * modified rates, added ratios, average rates and average ratios
 *
 * Revision 1.4  2004/09/03 21:40:17  galli
 * low level file access: open instead of fopen
 *
 * Revision 1.1  2004/08/06 07:31:23  galli
 * Initial revision
 * */
/*****************************************************************************/
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <errno.h>                                                  /* errno */
#include <string.h>                                  /* strerror(), strcmp() */
#include <ctype.h>                                              /* isspace() */
#include <stdlib.h>                                             /* strtoul() */
#include <sys/time.h>                                      /* gettimeofday() */
#include <sys/types.h>                                             /* open() */
#include <sys/stat.h>                                              /* open() */
#include <fcntl.h>                                                 /* open() */
#include <unistd.h>                              /* lseek(), read(), close() */
#include <values.h>                                               /* FLT_MIN */
/*****************************************************************************/
#include "getTCPIPfromProc.h"                            /* struct tcpipData */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#include "fmcTssUtils.h"                                   /* getTssString() */
#define PROC_NET_SNMP_SIZE 2048
#define MAX_COUNT 4294967296           /* max counter value (32 bit counter) */
/*****************************************************************************/
/* global variables */
int doResetTCPIPcnts=0;  /* set to 1 by resetTCPIP() to schedule a cnt reset */
static char rcsid[]="$Id: getTCPIPfromProc.c,v 1.21 2009/01/29 10:52:45 galli Exp $";
/*****************************************************************************/
struct tcpipData readTCPIP(int deBug,int errU)
{
  char ipStatFileName[]="/proc/net/snmp";
  int ipStatFileFD=0;
  char ipStatBuf[PROC_NET_SNMP_SIZE];
  char *headBp,*headEp,*dataBp,*dataEp;
  int readBytes=0;
  char *np,*sp,*p;
  unsigned i=0;
  static struct tcpipData data;
  struct timeval now={0,0};
  struct timezone tz;
  int sec,usec;
  float et=0.0,tss=0.0;                    /* elapsed time, time since start */
  static struct timeval lastTime={0,0},startTime={0,0};
  static struct counts lastCnt,dCnt;
  static int isFirstCall=1;
  /*-------------------------------------------------------------------------*/
  if(deBug>1)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  data.success=0;
  data.stss=getTssString(0.0);
  data.version=rcsid;
  memset((char*)&data.cnt,0,sizeof(data.cnt));
  memset((char*)&data.rate,0,sizeof(data.rate));
  memset((char*)&data.ratio,0,sizeof(data.ratio));
  memset((char*)&data.arate,0,sizeof(data.arate));
  memset((char*)&data.aratio,0,sizeof(data.aratio));
  /*-------------------------------------------------------------------------*/
  /* read /proc/net/dev in ifStatBuf */
  ipStatFileFD=open(ipStatFileName,O_RDONLY);
  if(ipStatFileFD==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",
            ipStatFileName,strerror(errno));
    return data;
  }
  if(lseek(ipStatFileFD,0L,SEEK_SET)<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot lseek file \"%s\": %s!",
            ipStatFileName,strerror(errno));
    return data;
  }
  if((readBytes=read(ipStatFileFD,ipStatBuf,sizeof(ipStatBuf)-1))<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot read file \"%s\": %s!",
            ipStatFileName,strerror(errno));
    return data;
  }
  close(ipStatFileFD);
  ipStatBuf[readBytes]='\0';
  /*-------------------------------------------------------------------------*/
  /* parse ipStatBuf */
  headBp=ipStatBuf;
  for(;;)                                                 /* loop over lines */
  {
    headEp=strchr(headBp,'\n');                              /* find 1st EOL */
    if(headEp==NULL)break;
    *headEp='\0';                           /* set string termination to EOL */
    dataBp=headEp+1;   /* set string beginning to the start of the next line */
    dataEp=strchr(dataBp,'\n');                              /* find 2nd EOL */
    if(dataEp==NULL)break;
    *dataEp='\0';                           /* set string termination to EOL */
    /*.......................................................................*/
    if(strstr(headBp,"Ip:")==headBp && strstr(dataBp,"Ip:")==dataBp)   /* Ip */
    {
      sp=strchr(headBp,':')+1;                         /* header field begin */
      np=strchr(dataBp,':')+1;                           /* data field begin */
      for(i=0;;i++)                              /* loop over 2 lines fields */
      {
        sp+=strspn(sp," \t\n");                  /* shift header field begin */
        p=sp+strcspn(sp," \t\n");                  /* shift header field end */
        np+=strspn(np," \t\n");                    /* shift data field begin */
        if(*np=='\0')break;
        *p='\0';                          /* null-terminate the header field */
        if(!strcmp(sp,"Forwarding"))
          data.cnt.ip.Forwarding=strtoul(np,&np,10);
        else if(!strcmp(sp,"DefaultTTL"))
          data.cnt.ip.DefaultTTL=strtoul(np,&np,10);
        else if(!strcmp(sp,"InReceives"))
          data.cnt.ip.InReceives=strtoul(np,&np,10);
        else if(!strcmp(sp,"InHdrErrors"))
          data.cnt.ip.InHdrErrors=strtoul(np,&np,10);
        else if(!strcmp(sp,"InAddrErrors"))
          data.cnt.ip.InAddrErrors=strtoul(np,&np,10);
        else if(!strcmp(sp,"ForwDatagrams"))
          data.cnt.ip.ForwDatagrams=strtoul(np,&np,10);
        else if(!strcmp(sp,"InUnknownProtos"))
          data.cnt.ip.InUnknownProtos=strtoul(np,&np,10);
        else if(!strcmp(sp,"InDiscards"))
          data.cnt.ip.InDiscards=strtoul(np,&np,10);
        else if(!strcmp(sp,"InDelivers"))
          data.cnt.ip.InDelivers=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutRequests"))
          data.cnt.ip.OutRequests=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutDiscards"))
          data.cnt.ip.OutDiscards=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutNoRoutes"))
          data.cnt.ip.OutNoRoutes=strtoul(np,&np,10);
        else if(!strcmp(sp,"ReasmTimeout"))
          data.cnt.ip.ReasmTimeout=strtoul(np,&np,10);
        else if(!strcmp(sp,"ReasmReqds"))
          data.cnt.ip.ReasmReqds=strtoul(np,&np,10);
        else if(!strcmp(sp,"ReasmOKs"))
          data.cnt.ip.ReasmOKs=strtoul(np,&np,10);
        else if(!strcmp(sp,"ReasmFails"))
          data.cnt.ip.ReasmFails=strtoul(np,&np,10);
        else if(!strcmp(sp,"FragOKs"))
          data.cnt.ip.FragOKs=strtoul(np,&np,10);
        else if(!strcmp(sp,"FragFails"))
          data.cnt.ip.FragFails=strtoul(np,&np,10);
        else if(!strcmp(sp,"FragCreates"))
          data.cnt.ip.FragCreates=strtoul(np,&np,10);
        sp=p+1;
      }
    }
    /*.......................................................................*/
    else if(strstr(headBp,"Icmp:")==headBp && 
            strstr(dataBp,"Icmp:")==dataBp)                          /* Icmp */
    {
      sp=strchr(headBp,':')+1;
      np=strchr(dataBp,':')+1;
      for(i=0;;i++)
      {
        sp+=strspn(sp," \t\n");
        p=sp+strcspn(sp," \t\n");
        np+=strspn(np," \t\n");
        if(*np=='\0')break;
        *p='\0';
        if(!strcmp(sp,"InMsgs"))
          data.cnt.icmp.InMsgs=strtoul(np,&np,10);
        else if(!strcmp(sp,"InErrors"))
          data.cnt.icmp.InErrors=strtoul(np,&np,10);
        else if(!strcmp(sp,"InDestUnreachs"))
          data.cnt.icmp.InDestUnreachs=strtoul(np,&np,10);
        else if(!strcmp(sp,"InTimeExcds"))
          data.cnt.icmp.InTimeExcds=strtoul(np,&np,10);
        else if(!strcmp(sp,"InParmProbs"))
          data.cnt.icmp.InParmProbs=strtoul(np,&np,10);
        else if(!strcmp(sp,"InSrcQuenchs"))
          data.cnt.icmp.InSrcQuenchs=strtoul(np,&np,10);
        else if(!strcmp(sp,"InRedirects"))
          data.cnt.icmp.InRedirects=strtoul(np,&np,10);
        else if(!strcmp(sp,"InEchos"))
          data.cnt.icmp.InEchos=strtoul(np,&np,10);
        else if(!strcmp(sp,"InEchoReps"))
          data.cnt.icmp.InEchoReps=strtoul(np,&np,10);
        else if(!strcmp(sp,"InTimestamps"))
          data.cnt.icmp.InTimestamps=strtoul(np,&np,10);
        else if(!strcmp(sp,"InTimestampReps"))
          data.cnt.icmp.InTimestampReps=strtoul(np,&np,10);
        else if(!strcmp(sp,"InAddrMasks"))
          data.cnt.icmp.InAddrMasks=strtoul(np,&np,10);
        else if(!strcmp(sp,"InAddrMaskReps"))
          data.cnt.icmp.InAddrMaskReps=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutMsgs"))
          data.cnt.icmp.OutMsgs=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutErrors"))
          data.cnt.icmp.OutErrors=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutDestUnreachs"))
          data.cnt.icmp.OutDestUnreachs=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutTimeExcds"))
          data.cnt.icmp.OutTimeExcds=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutParmProbs"))
          data.cnt.icmp.OutParmProbs=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutSrcQuenchs"))
          data.cnt.icmp.OutSrcQuenchs=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutRedirects"))
          data.cnt.icmp.OutRedirects=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutEchos"))
          data.cnt.icmp.OutEchos=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutEchoReps"))
          data.cnt.icmp.OutEchoReps=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutTimestamps"))
          data.cnt.icmp.OutTimestamps=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutTimestampReps"))
          data.cnt.icmp.OutTimestampReps=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutAddrMasks"))
          data.cnt.icmp.OutAddrMasks=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutAddrMaskReps"))
          data.cnt.icmp.OutAddrMaskReps=strtoul(np,&np,10);
        sp=p+1;
      }
    }
    /*.......................................................................*/
    else if(strstr(headBp,"Tcp:")==headBp && 
            strstr(dataBp,"Tcp:")==dataBp)                            /* Tcp */
    {
      sp=strchr(headBp,':')+1;
      np=strchr(dataBp,':')+1;
      for(i=0;;i++)
      {
        sp+=strspn(sp," \t\n");
        p=sp+strcspn(sp," \t\n");
        np+=strspn(np," \t\n");
        if(*np=='\0')break;
        *p='\0';
        if(!strcmp(sp,"RtoAlgorithm"))
          data.cnt.tcp.RtoAlgorithm=strtoul(np,&np,10);
        else if(!strcmp(sp,"RtoMin"))
          data.cnt.tcp.RtoMin=strtoul(np,&np,10);
        else if(!strcmp(sp,"RtoMax"))
          data.cnt.tcp.RtoMax=strtoul(np,&np,10);
        else if(!strcmp(sp,"MaxConn"))
          data.cnt.tcp.MaxConn=strtoul(np,&np,10);
        else if(!strcmp(sp,"ActiveOpens"))
          data.cnt.tcp.ActiveOpens=strtoul(np,&np,10);
        else if(!strcmp(sp,"PassiveOpens"))
          data.cnt.tcp.PassiveOpens=strtoul(np,&np,10);
        else if(!strcmp(sp,"AttemptFails"))
          data.cnt.tcp.AttemptFails=strtoul(np,&np,10);
        else if(!strcmp(sp,"EstabResets"))
          data.cnt.tcp.EstabResets=strtoul(np,&np,10);
        else if(!strcmp(sp,"CurrEstab"))
          data.cnt.tcp.CurrEstab=strtoul(np,&np,10);
        else if(!strcmp(sp,"InSegs"))
          data.cnt.tcp.InSegs=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutSegs"))
          data.cnt.tcp.OutSegs=strtoul(np,&np,10);
        else if(!strcmp(sp,"RetransSegs"))
          data.cnt.tcp.RetransSegs=strtoul(np,&np,10);
        else if(!strcmp(sp,"InErrs"))
          data.cnt.tcp.InErrs=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutRsts"))
          data.cnt.tcp.OutRsts=strtoul(np,&np,10);
        sp=p+1;
      }
    }
    /*.......................................................................*/
    else if(strstr(headBp,"Udp:")==headBp && 
            strstr(dataBp,"Udp:")==dataBp)                            /* Udp */
    {
      sp=strchr(headBp,':')+1;
      np=strchr(dataBp,':')+1;
      for(i=0;;i++)
      {
        sp+=strspn(sp," \t\n");
        p=sp+strcspn(sp," \t\n");
        np+=strspn(np," \t\n");
        if(*np=='\0')break;
        *p='\0';
        if(!strcmp(sp,"InDatagrams"))
          data.cnt.udp.InDatagrams=strtoul(np,&np,10);
        else if(!strcmp(sp,"NoPorts"))
          data.cnt.udp.NoPorts=strtoul(np,&np,10);
        else if(!strcmp(sp,"InErrors"))
          data.cnt.udp.InErrors=strtoul(np,&np,10);
        else if(!strcmp(sp,"OutDatagrams"))
          data.cnt.udp.OutDatagrams=strtoul(np,&np,10);
        sp=p+1;
      }
    }
    /*.......................................................................*/
    headBp=dataEp+1;   /* set string beginning to the start of the next line */
    /*.......................................................................*/
  }
  /*-------------------------------------------------------------------------*/
  /* evaluate derived quantities */
  if(isFirstCall)
  {
    memset(&data.tcnt,0,sizeof(data.tcnt));
    memset((char*)&data.mrate,0,sizeof(data.mrate));
    memset((char*)&data.mratio,0,sizeof(data.mratio));
    gettimeofday(&lastTime,&tz);
    gettimeofday(&startTime,&tz);
    memcpy(&lastCnt,&data.cnt,sizeof(data.cnt));
    memset(&dCnt,0,sizeof(dCnt));
    isFirstCall=0;
  }
  else
  {
    /*.......................................................................*/
    gettimeofday(&now,&tz);                  /* get current time: sec & usec */
    /*.......................................................................*/
    if(doResetTCPIPcnts)
    {
      memcpy(&startTime,&lastTime,sizeof(struct timeval));
      memset(&data.tcnt,0,sizeof(struct tCounts));
      memset(&data.mrate,0,sizeof(struct rates));
      memset(&data.mratio,0,sizeof(struct ratios));
      mPrintf(errU,INFO,__func__,0,"Counters reset requested by user!");
      doResetTCPIPcnts=0;
    }
    /*.......................................................................*/
    /* evaluate elapsed time since last call */
    if(now.tv_usec<lastTime.tv_usec)
    {
      now.tv_usec+=1000000;
      now.tv_sec-=1;
    }
    sec=now.tv_sec-lastTime.tv_sec;
    usec=now.tv_usec-lastTime.tv_usec;
    et=(float)sec+((float)usec/(float)1000000.0);            /* elapsed time */
    if(deBug)mPrintf(errU,DEBUG,__func__,0,"elapsed time=%f\n",et);
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
    data.stss=getTssString(tss);
    if(deBug)
    {
      mPrintf(errU,DEBUG,__func__,0,"time since start = %fs = %s",tss,
              data.stss);
    }
    /*.......................................................................*/
    /* evaluate counter increments */
    dCnt.ip.InReceives=data.cnt.ip.InReceives-lastCnt.ip.InReceives;
    dCnt.ip.InHdrErrors=data.cnt.ip.InHdrErrors-lastCnt.ip.InHdrErrors;
    dCnt.ip.InAddrErrors=data.cnt.ip.InAddrErrors-lastCnt.ip.InAddrErrors;
    dCnt.ip.ForwDatagrams=data.cnt.ip.ForwDatagrams-lastCnt.ip.ForwDatagrams;
    dCnt.ip.InUnknownProtos=data.cnt.ip.InUnknownProtos-
                            lastCnt.ip.InUnknownProtos;
    dCnt.ip.InDiscards=data.cnt.ip.InDiscards-lastCnt.ip.InDiscards;
    dCnt.ip.InDelivers=data.cnt.ip.InDelivers-lastCnt.ip.InDelivers;
    dCnt.ip.OutRequests=data.cnt.ip.OutRequests-lastCnt.ip.OutRequests;
    dCnt.ip.OutDiscards=data.cnt.ip.OutDiscards-lastCnt.ip.OutDiscards;
    dCnt.ip.OutNoRoutes=data.cnt.ip.OutNoRoutes-lastCnt.ip.OutNoRoutes;
    dCnt.ip.ReasmTimeout=data.cnt.ip.ReasmTimeout-lastCnt.ip.ReasmTimeout;
    dCnt.ip.ReasmReqds=data.cnt.ip.ReasmReqds-lastCnt.ip.ReasmReqds;
    dCnt.ip.ReasmOKs=data.cnt.ip.ReasmOKs-lastCnt.ip.ReasmOKs;
    dCnt.ip.ReasmFails=data.cnt.ip.ReasmFails-lastCnt.ip.ReasmFails;
    dCnt.ip.FragOKs=data.cnt.ip.FragOKs-lastCnt.ip.FragOKs;
    dCnt.ip.FragFails=data.cnt.ip.FragFails-lastCnt.ip.FragFails;
    dCnt.ip.FragCreates=data.cnt.ip.FragCreates-lastCnt.ip.FragCreates;
    dCnt.tcp.InSegs=data.cnt.tcp.InSegs-lastCnt.tcp.InSegs;
    dCnt.tcp.OutSegs=data.cnt.tcp.OutSegs-lastCnt.tcp.OutSegs;
    dCnt.tcp.RetransSegs=data.cnt.tcp.RetransSegs-lastCnt.tcp.RetransSegs;
    dCnt.tcp.InErrs=data.cnt.tcp.InErrs-lastCnt.tcp.InErrs;
    dCnt.tcp.OutRsts=data.cnt.tcp.OutRsts-lastCnt.tcp.OutRsts;
    dCnt.udp.InDatagrams=data.cnt.udp.InDatagrams-lastCnt.udp.InDatagrams;
    dCnt.udp.NoPorts=data.cnt.udp.NoPorts-lastCnt.udp.NoPorts;
    dCnt.udp.InErrors=data.cnt.udp.InErrors-lastCnt.udp.InErrors;
    dCnt.udp.OutDatagrams=data.cnt.udp.OutDatagrams-lastCnt.udp.OutDatagrams;
    /*.......................................................................*/
    /* increment total counters */
    data.tcnt.ip.InReceives+=dCnt.ip.InReceives;
    data.tcnt.ip.InHdrErrors+=dCnt.ip.InHdrErrors;
    data.tcnt.ip.InAddrErrors+=dCnt.ip.InAddrErrors;
    data.tcnt.ip.ForwDatagrams+=dCnt.ip.ForwDatagrams;
    data.tcnt.ip.InUnknownProtos+=dCnt.ip.InUnknownProtos;
    data.tcnt.ip.InDiscards+=dCnt.ip.InDiscards;
    data.tcnt.ip.InDelivers+=dCnt.ip.InDelivers;
    data.tcnt.ip.OutRequests+=dCnt.ip.OutRequests;
    data.tcnt.ip.OutDiscards+=dCnt.ip.OutDiscards;
    data.tcnt.ip.OutNoRoutes+=dCnt.ip.OutNoRoutes;
    data.tcnt.ip.ReasmTimeout+=dCnt.ip.ReasmTimeout;
    data.tcnt.ip.ReasmReqds+=dCnt.ip.ReasmReqds;
    data.tcnt.ip.ReasmOKs+=dCnt.ip.ReasmOKs;
    data.tcnt.ip.ReasmFails+=dCnt.ip.ReasmFails;
    data.tcnt.ip.FragOKs+=dCnt.ip.FragOKs;
    data.tcnt.ip.FragFails+=dCnt.ip.FragFails;
    data.tcnt.ip.FragCreates+=dCnt.ip.FragCreates;
    data.tcnt.tcp.InSegs+=dCnt.tcp.InSegs;
    data.tcnt.tcp.OutSegs+=dCnt.tcp.OutSegs;
    data.tcnt.tcp.RetransSegs+=dCnt.tcp.RetransSegs;
    data.tcnt.tcp.InErrs+=dCnt.tcp.InErrs;
    data.tcnt.tcp.OutRsts+=dCnt.tcp.OutRsts;
    data.tcnt.udp.InDatagrams+=dCnt.udp.InDatagrams;
    data.tcnt.udp.NoPorts+=dCnt.udp.NoPorts;
    data.tcnt.udp.InErrors+=dCnt.udp.InErrors;
    data.tcnt.udp.OutDatagrams+=dCnt.udp.OutDatagrams;
    /*.......................................................................*/
    /* evaluate rates and maximum rates */
    if(et>FLT_MIN)                                    /* avoid division by 0 */
    {
      /* evaluate rates */
      /* ipRates */
      data.rate.ip.InReceivesRate=(float)dCnt.ip.InReceives/et;
      data.rate.ip.InDeliversRate=(float)dCnt.ip.InDelivers/et;
      data.rate.ip.ForwDatagramsRate=(float)dCnt.ip.ForwDatagrams/et;
      data.rate.ip.OutRequestsRate=(float)dCnt.ip.OutRequests/et;
      data.rate.ip.ReasmReqdsRate=(float)dCnt.ip.ReasmReqds/et;
      data.rate.ip.FragReqdsRate=(float)(dCnt.ip.ReasmOKs+dCnt.ip.FragFails)/et;
      /* tcpRates */
      data.rate.tcp.InSegsRate=(float)dCnt.tcp.InSegs/et;
      data.rate.tcp.OutSegsRate=(float)dCnt.tcp.OutSegs/et;
      /* udpRates */
      data.rate.udp.InDatagramsRate=(float)dCnt.udp.InDatagrams/et;
      data.rate.udp.OutDatagramsRate=(float)dCnt.udp.OutDatagrams/et;
      /* evaluate maximum rates */
      if(data.mrate.ip.InReceivesRate<data.rate.ip.InReceivesRate)
        data.mrate.ip.InReceivesRate=data.rate.ip.InReceivesRate;
      if(data.mrate.ip.InDeliversRate<data.rate.ip.InDeliversRate)
        data.mrate.ip.InDeliversRate=data.rate.ip.InDeliversRate;
      if(data.mrate.ip.ForwDatagramsRate<data.rate.ip.ForwDatagramsRate)
        data.mrate.ip.ForwDatagramsRate=data.rate.ip.ForwDatagramsRate;
      if(data.mrate.ip.OutRequestsRate<data.rate.ip.OutRequestsRate)
        data.mrate.ip.OutRequestsRate=data.rate.ip.OutRequestsRate;
      if(data.mrate.ip.ReasmReqdsRate<data.rate.ip.ReasmReqdsRate)
        data.mrate.ip.ReasmReqdsRate=data.rate.ip.ReasmReqdsRate;
      if(data.mrate.ip.FragReqdsRate<data.rate.ip.FragReqdsRate)
        data.mrate.ip.FragReqdsRate=data.rate.ip.FragReqdsRate;
      if(data.mrate.tcp.InSegsRate<data.rate.tcp.InSegsRate)
        data.mrate.tcp.InSegsRate=data.rate.tcp.InSegsRate;
      if(data.mrate.tcp.OutSegsRate<data.rate.tcp.OutSegsRate)
        data.mrate.tcp.OutSegsRate=data.rate.tcp.OutSegsRate;
      if(data.mrate.udp.InDatagramsRate<data.rate.udp.InDatagramsRate)
        data.mrate.udp.InDatagramsRate=data.rate.udp.InDatagramsRate;
      if(data.mrate.udp.OutDatagramsRate<data.rate.udp.OutDatagramsRate)
        data.mrate.udp.OutDatagramsRate=data.rate.udp.OutDatagramsRate;
    }
    else                                    /* set to 0 if zero elapsed time */
    {
      memset(&data.rate,0,sizeof(data.rate));
    }
    /*.......................................................................*/
    /* evaluate average rates */
    if(tss>FLT_MIN)                                   /* avoid division by 0 */
    {
      /* ipRates */
      data.arate.ip.InReceivesRate=(float)data.tcnt.ip.InReceives/tss;
      data.arate.ip.InDeliversRate=(float)data.tcnt.ip.InDelivers/tss;
      data.arate.ip.ForwDatagramsRate=(float)data.tcnt.ip.ForwDatagrams/tss;
      data.arate.ip.OutRequestsRate=(float)data.tcnt.ip.OutRequests/tss;
      data.arate.ip.ReasmReqdsRate=(float)data.tcnt.ip.ReasmReqds/tss;
      data.arate.ip.FragReqdsRate=(float)(data.tcnt.ip.ReasmOKs+
                                           data.tcnt.ip.FragFails)/tss;
      /* tcpRates */
      data.arate.tcp.InSegsRate=(float)data.tcnt.tcp.InSegs/tss;
      data.arate.tcp.OutSegsRate=(float)data.tcnt.tcp.OutSegs/tss;
      /* udpRates */
      data.arate.udp.InDatagramsRate=(float)data.tcnt.udp.InDatagrams/tss;
      data.arate.udp.OutDatagramsRate=(float)data.tcnt.udp.OutDatagrams/tss;
    }
    else                                /* set to 0 if zero time since start */
    {
      memset(&data.arate,0,sizeof(data.arate));
    }
    /*.......................................................................*/
    /* evaluate ratios */
    /* ipRatios */
    if(dCnt.ip.InReceives>0)
    {
      data.ratio.ip.InHdrErrorsFrac=(float)dCnt.ip.InHdrErrors/
                                    (float)dCnt.ip.InReceives;
      data.ratio.ip.InAddrErrorsFrac=(float)dCnt.ip.InAddrErrors/
                                     (float)dCnt.ip.InReceives;
      data.ratio.ip.InUnknownProtosFrac=(float)dCnt.ip.InUnknownProtos/
                                        (float)dCnt.ip.InReceives;
      data.ratio.ip.InDiscardsFrac=(float)dCnt.ip.InDiscards/
                                   (float)dCnt.ip.InReceives;
      data.ratio.ip.ForwDatagramsFrac=(float)dCnt.ip.ForwDatagrams/
                                      (float)dCnt.ip.InReceives;
      data.ratio.ip.InDeliversFrac=(float)dCnt.ip.InDelivers/
                                   (float)dCnt.ip.InReceives;
      data.ratio.ip.ReasmReqdsFrac=(float)dCnt.ip.ReasmReqds/
                                   (float)dCnt.ip.InReceives;
      if(data.mratio.ip.InHdrErrorsFrac<data.ratio.ip.InHdrErrorsFrac)
        data.mratio.ip.InHdrErrorsFrac=data.ratio.ip.InHdrErrorsFrac;
      if(data.mratio.ip.InAddrErrorsFrac<data.ratio.ip.InAddrErrorsFrac)
        data.mratio.ip.InAddrErrorsFrac=data.ratio.ip.InAddrErrorsFrac;
      if(data.mratio.ip.InUnknownProtosFrac<data.ratio.ip.InUnknownProtosFrac)
        data.mratio.ip.InUnknownProtosFrac=data.ratio.ip.InUnknownProtosFrac;
      if(data.mratio.ip.InDiscardsFrac<data.ratio.ip.InDiscardsFrac)
        data.mratio.ip.InDiscardsFrac=data.ratio.ip.InDiscardsFrac;
      if(data.mratio.ip.ForwDatagramsFrac<data.ratio.ip.ForwDatagramsFrac)
        data.mratio.ip.ForwDatagramsFrac=data.ratio.ip.ForwDatagramsFrac;
      if(data.mratio.ip.InDeliversFrac<data.ratio.ip.InDeliversFrac)
        data.mratio.ip.InDeliversFrac=data.ratio.ip.InDeliversFrac;
      if(data.mratio.ip.ReasmReqdsFrac<data.ratio.ip.ReasmReqdsFrac)
        data.mratio.ip.ReasmReqdsFrac=data.ratio.ip.ReasmReqdsFrac;
    }
    else
    {
      data.ratio.ip.InHdrErrorsFrac=0.0F;
      data.ratio.ip.InAddrErrorsFrac=0.0F;
      data.ratio.ip.InUnknownProtosFrac=0.0F;
      data.ratio.ip.InDiscardsFrac=0.0F;
      data.ratio.ip.ForwDatagramsFrac=0.0F;
      data.ratio.ip.InDeliversFrac=0.0F;
      data.ratio.ip.ReasmReqdsFrac=0.0F;
    }
    if(dCnt.ip.ReasmOKs+dCnt.ip.ReasmFails>0)
    {
      data.ratio.ip.ReasmOKsFrac=(float)dCnt.ip.ReasmOKs/
                                 (float)(dCnt.ip.ReasmOKs+dCnt.ip.ReasmFails);
      data.ratio.ip.ReasmTimeoutFrac=(float)dCnt.ip.ReasmTimeout/
                                  (float)(dCnt.ip.ReasmOKs+dCnt.ip.ReasmFails);
      data.ratio.ip.ReasmFailsFrac=(float)dCnt.ip.ReasmFails/
                                  (float)(dCnt.ip.ReasmOKs+dCnt.ip.ReasmFails);
      if(data.mratio.ip.ReasmOKsFrac<data.ratio.ip.ReasmOKsFrac)
        data.mratio.ip.ReasmOKsFrac=data.ratio.ip.ReasmOKsFrac;
      if(data.mratio.ip.ReasmTimeoutFrac<data.ratio.ip.ReasmTimeoutFrac)
        data.mratio.ip.ReasmTimeoutFrac=data.ratio.ip.ReasmTimeoutFrac;
      if(data.mratio.ip.ReasmFailsFrac<data.ratio.ip.ReasmFailsFrac)
        data.mratio.ip.ReasmFailsFrac=data.ratio.ip.ReasmFailsFrac;
    }
    else
    {
      data.ratio.ip.ReasmOKsFrac=0.0F;
      data.ratio.ip.ReasmTimeoutFrac=0.0F;
      data.ratio.ip.ReasmFailsFrac=0.0F;
    }
    if(dCnt.ip.OutRequests+dCnt.ip.ForwDatagrams>0)
    {
      data.ratio.ip.OutDiscardsFrac=(float)dCnt.ip.OutDiscards/
                            (float)(dCnt.ip.OutRequests+dCnt.ip.ForwDatagrams);
      data.ratio.ip.OutNoRoutesFrac=(float)dCnt.ip.OutNoRoutes/
                            (float)(dCnt.ip.OutRequests+dCnt.ip.ForwDatagrams);
      data.ratio.ip.FragReqdsFrac=(float)(dCnt.ip.FragOKs+dCnt.ip.FragFails)/
                            (float)(dCnt.ip.OutRequests+dCnt.ip.ForwDatagrams);
      if(data.mratio.ip.OutDiscardsFrac<data.ratio.ip.OutDiscardsFrac)
        data.mratio.ip.OutDiscardsFrac=data.ratio.ip.OutDiscardsFrac;
      if(data.mratio.ip.OutNoRoutesFrac<data.ratio.ip.OutNoRoutesFrac)
        data.mratio.ip.OutNoRoutesFrac=data.ratio.ip.OutNoRoutesFrac;
      if(data.mratio.ip.FragReqdsFrac<data.ratio.ip.FragReqdsFrac)
        data.mratio.ip.FragReqdsFrac=data.ratio.ip.FragReqdsFrac;
    }
    else
    {
      data.ratio.ip.OutDiscardsFrac=0.0F;
      data.ratio.ip.OutNoRoutesFrac=0.0F;
      data.ratio.ip.FragReqdsFrac=0.0F;
    }
    if(dCnt.ip.FragOKs+dCnt.ip.FragFails>0)
    {
      data.ratio.ip.FragFailsFrac=(float)dCnt.ip.FragFails/
                                  (float)(dCnt.ip.FragOKs+dCnt.ip.FragFails);
      if(data.mratio.ip.FragFailsFrac<data.ratio.ip.FragFailsFrac)
        data.mratio.ip.FragFailsFrac=data.ratio.ip.FragFailsFrac;
    }
    else
    {
      data.ratio.ip.FragFailsFrac=0.0F;
    }
    if(dCnt.ip.FragOKs>0)
    {
      data.ratio.ip.FragCreatesFrac=(float)dCnt.ip.FragCreates/
                                    (float)dCnt.ip.FragOKs;
      if(data.mratio.ip.FragCreatesFrac<data.ratio.ip.FragCreatesFrac)
        data.mratio.ip.FragCreatesFrac=data.ratio.ip.FragCreatesFrac;
    }
    else
    {
      data.ratio.ip.FragCreatesFrac=0.0F;
    }
    /* tcpRatios */
    if(dCnt.tcp.OutSegs+dCnt.tcp.RetransSegs>0)
    {
      data.ratio.tcp.RetransSegsFrac=(float)dCnt.tcp.RetransSegs/
                                (float)(dCnt.tcp.OutSegs+dCnt.tcp.RetransSegs);
      if(data.mratio.tcp.RetransSegsFrac<data.ratio.tcp.RetransSegsFrac)
        data.mratio.tcp.RetransSegsFrac=data.ratio.tcp.RetransSegsFrac;
    }
    else
    {
      data.ratio.tcp.RetransSegsFrac=0.0F;
    }
    if(dCnt.tcp.OutSegs>0)
    {
      data.ratio.tcp.OutRstsFrac=(float)dCnt.tcp.OutRsts/
                                 (float)dCnt.tcp.OutSegs;
      if(data.mratio.tcp.OutRstsFrac<data.ratio.tcp.OutRstsFrac)
        data.mratio.tcp.OutRstsFrac=data.ratio.tcp.OutRstsFrac;
    }
    else
    {
      data.ratio.tcp.OutRstsFrac=0.0F;
    }
    if(dCnt.tcp.InSegs>0)
    {
      data.ratio.tcp.InErrsFrac=(float)dCnt.tcp.InErrs/(float)dCnt.tcp.InSegs;
      if(data.mratio.tcp.InErrsFrac<data.ratio.tcp.InErrsFrac)
        data.mratio.tcp.InErrsFrac=data.ratio.tcp.InErrsFrac;
    }
    else
    {
      data.ratio.tcp.InErrsFrac=0.0F;
    }
    /* udpRatios */
    if(dCnt.udp.InDatagrams>0)
    {
      data.ratio.udp.NoPortsFrac=(float)dCnt.udp.NoPorts/
                                 (float)dCnt.udp.InDatagrams;
      data.ratio.udp.InErrorsFrac=(float)dCnt.udp.InErrors/
                                  (float)dCnt.udp.InDatagrams;
      if(data.mratio.udp.NoPortsFrac<data.ratio.udp.NoPortsFrac)
        data.mratio.udp.NoPortsFrac=data.ratio.udp.NoPortsFrac;
      if(data.mratio.udp.InErrorsFrac<data.ratio.udp.InErrorsFrac)
        data.mratio.udp.InErrorsFrac=data.ratio.udp.InErrorsFrac;
    }
    else
    {
      data.ratio.udp.NoPortsFrac=0.0F;
      data.ratio.udp.InErrorsFrac=0.0F;
    }
    /*.......................................................................*/
    /* evaluate average ratios */
    /* ipRatios */
    if(data.tcnt.ip.InReceives>0)
    {
      data.aratio.ip.InHdrErrorsFrac=(float)data.tcnt.ip.InHdrErrors/
                                     (float)data.tcnt.ip.InReceives;
      data.aratio.ip.InAddrErrorsFrac=(float)data.tcnt.ip.InAddrErrors/
                                      (float)data.tcnt.ip.InReceives;
      data.aratio.ip.InUnknownProtosFrac=(float)data.tcnt.ip.InUnknownProtos/
                                         (float)data.tcnt.ip.InReceives;
      data.aratio.ip.InDiscardsFrac=(float)data.tcnt.ip.InDiscards/
                                    (float)data.tcnt.ip.InReceives;
      data.aratio.ip.ForwDatagramsFrac=(float)data.tcnt.ip.ForwDatagrams/
                                       (float)data.tcnt.ip.InReceives;
      data.aratio.ip.InDeliversFrac=(float)data.tcnt.ip.InDelivers/
                                    (float)data.tcnt.ip.InReceives;
      data.aratio.ip.ReasmReqdsFrac=(float)data.tcnt.ip.ReasmReqds/
                                    (float)data.tcnt.ip.InReceives;
    }
    else
    {
      data.aratio.ip.InHdrErrorsFrac=0.0F;
      data.aratio.ip.InAddrErrorsFrac=0.0F;
      data.aratio.ip.InUnknownProtosFrac=0.0F;
      data.aratio.ip.InDiscardsFrac=0.0F;
      data.aratio.ip.ForwDatagramsFrac=0.0F;
      data.aratio.ip.InDeliversFrac=0.0F;
      data.aratio.ip.ReasmReqdsFrac=0.0F;
    }
    if(data.tcnt.ip.ReasmOKs+data.tcnt.ip.ReasmFails>0)
    {
      data.aratio.ip.ReasmOKsFrac=(float)data.tcnt.ip.ReasmOKs/
                        (float)(data.tcnt.ip.ReasmOKs+data.tcnt.ip.ReasmFails);
      data.aratio.ip.ReasmTimeoutFrac=(float)data.tcnt.ip.ReasmTimeout/
                        (float)(data.tcnt.ip.ReasmOKs+data.tcnt.ip.ReasmFails);
      data.aratio.ip.ReasmFailsFrac=(float)data.tcnt.ip.ReasmFails/
                        (float)(data.tcnt.ip.ReasmOKs+data.tcnt.ip.ReasmFails);
    }
    else
    {
      data.aratio.ip.ReasmOKsFrac=0.0F;
      data.aratio.ip.ReasmTimeoutFrac=0.0F;
      data.aratio.ip.ReasmFailsFrac=0.0F;
    }
    if(data.tcnt.ip.OutRequests+data.tcnt.ip.ForwDatagrams>0)
    {
      data.aratio.ip.OutDiscardsFrac=(float)data.tcnt.ip.OutDiscards/
                  (float)(data.tcnt.ip.OutRequests+data.tcnt.ip.ForwDatagrams);
      data.aratio.ip.OutNoRoutesFrac=(float)data.tcnt.ip.OutNoRoutes/
                  (float)(data.tcnt.ip.OutRequests+data.tcnt.ip.ForwDatagrams);
      data.aratio.ip.FragReqdsFrac=
                          (float)(data.tcnt.ip.FragOKs+data.tcnt.ip.FragFails)/
                  (float)(data.tcnt.ip.OutRequests+data.tcnt.ip.ForwDatagrams);
    }
    else
    {
      data.aratio.ip.OutDiscardsFrac=0.0F;
      data.aratio.ip.OutNoRoutesFrac=0.0F;
      data.aratio.ip.FragReqdsFrac=0.0F;
    }
    if(data.tcnt.ip.FragOKs+data.tcnt.ip.FragFails>0)
    {
      data.aratio.ip.FragFailsFrac=(float)data.tcnt.ip.FragFails/
                                   (float)(data.tcnt.ip.FragOKs+
                                           data.tcnt.ip.FragFails);
    }
    else
    {
      data.aratio.ip.FragFailsFrac=0.0F;
    }
    if(data.tcnt.ip.FragOKs>0)
    {
      data.aratio.ip.FragCreatesFrac=(float)data.tcnt.ip.FragCreates/
                                     (float)data.tcnt.ip.FragOKs;
    }
    else
    {
      data.aratio.ip.FragCreatesFrac=0.0F;
    }
    /* tcpRatios */
    if(data.tcnt.tcp.OutSegs+data.tcnt.tcp.RetransSegs>0)
    {
      data.aratio.tcp.RetransSegsFrac=(float)data.tcnt.tcp.RetransSegs/
                      (float)(data.tcnt.tcp.OutSegs+data.tcnt.tcp.RetransSegs);
    }
    else
    {
      data.aratio.tcp.RetransSegsFrac=0.0F;
    }
    if(data.tcnt.tcp.OutSegs>0)
    {
      data.aratio.tcp.OutRstsFrac=(float)data.tcnt.tcp.OutRsts/
                                  (float)data.tcnt.tcp.OutSegs;
    }
    else
    {
      data.aratio.tcp.OutRstsFrac=0.0F;
    }
    if(data.tcnt.tcp.InSegs>0)
    {
      data.aratio.tcp.InErrsFrac=(float)data.tcnt.tcp.InErrs/
                                 (float)data.tcnt.tcp.InSegs;
    }
    else
    {
      data.aratio.tcp.InErrsFrac=0.0F;
    }
    /* udpRatios */
    if(data.tcnt.udp.InDatagrams>0)
    {
      data.aratio.udp.NoPortsFrac=(float)data.tcnt.udp.NoPorts/
                                  (float)data.tcnt.udp.InDatagrams;
      data.aratio.udp.InErrorsFrac=(float)data.tcnt.udp.InErrors/
                                   (float)data.tcnt.udp.InDatagrams;
    }
    else
    {
      data.aratio.udp.NoPortsFrac=0.0F;
      data.aratio.udp.InErrorsFrac=0.0F;
    }
    /*.......................................................................*/
    gettimeofday(&lastTime,&tz);
    memcpy(&lastCnt,&data.cnt,sizeof(data.cnt));
    /*.......................................................................*/
  }
  /*-------------------------------------------------------------------------*/
  data.success=1;
  /*-------------------------------------------------------------------------*/
  return data;
}
/*****************************************************************************/
void resetTCPIP()
{
  doResetTCPIPcnts=1;
  return;
}
/*****************************************************************************/
/* getTCPIPsensorVersion() - Returns the RCS identifier of this file.        */
/*****************************************************************************/
char *getTCPIPsensorVersion()
{
  return rcsid;
}
/*****************************************************************************/
