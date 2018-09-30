/*****************************************************************************/
/*
 * $Log: getNIFfromProc.c,v $
 * Revision 1.28  2011/09/27 11:53:17  galli
 * Added include file for compatibility with SLC5
 *
 * Revision 1.24  2009/01/22 10:16:10  galli
 * do not depend on kernel-devel headers
 *
 * Revision 1.20  2007/12/20 13:18:13  galli
 * send messages on counter reset (VERB)
 *
 * Revision 1.19  2007/12/20 12:42:15  galli
 * bug fixed
 *
 * Revision 1.18  2007/08/10 13:42:25  galli
 * added function getNIFsensorVersion()
 * compatible with libFMCutils v 2
 *
 * Revision 1.17  2006/10/25 22:23:11  galli
 * bug fixed
 *
 * Revision 1.16  2006/10/24 12:19:21  galli
 * does not cut the bus address
 *
 * Revision 1.15  2006/10/23 20:43:43  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.13  2005/09/15 14:31:24  galli
 * read (PCI) bus addresses of the interfaces
 *
 * Revision 1.12  2004/11/30 23:24:21  galli
 * bug fixed in managing counter reset
 *
 * Revision 1.11  2004/11/26 09:20:49  galli
 * added total Ethernet rates
 *
 * Revision 1.10  2004/11/16 15:54:56  galli
 * no more external doResetNifCnts. Use resetNIF() function instead.
 *
 * Revision 1.9  2004/11/16 09:36:08  galli
 * added external global variable doResetNifCnts to schedule a counter reset
 * added function getTssString() to convert to string the time elapsed
 * since last reset
 *
 * Revision 1.8  2004/10/22 08:50:50  galli
 * Print errors to logger
 *
 * Revision 1.7  2004/09/12 23:37:30  galli
 * evaluate also max rates and ratios
 * info function
 *
 * Revision 1.6  2004/09/11 23:38:25  galli
 * added 64-bit counters
 * modified rates, added ratios, average rates and average ratios
 *
 * Revision 1.4  2004/09/03 08:11:04  galli
 * low level file access: open instead of fopen
 *
 * Revision 1.1  2004/08/06 07:59:54  galli
 * Initial revision
 * */
/*****************************************************************************/
/* Warning:                                                                  */
/* To be called more frequent than 1/34s for Gigabit Ethernet on i386!       */
/*****************************************************************************/
/* Kernel counter are unsigned long:                                         */
/* i386   ULONG_MAX  = 4294967295           = 0xffffffff                     */
/* x86_64 ULONG_MAX  = 18446744073709551615 = 0xffffffffffffffff             */
/*****************************************************************************/
/* Counter reset period at maximum rate:                                     */
/*---------------------------------------------------------------------------*/
/* 100 MbE on i386           343.6   s                                       */
/*   1 GbE on i386            34.36  s                                       */
/*  10 GbE on i386             3.436 s                                       */
/*  50 GbE on i386             0.697 s                                       */
/*---------------------------------------------------------------------------*/
/* 100 MbE on x86_64       46795     y                                       */
/*   1 GbE on x86_64        4679     y                                       */
/*  10 GbE on x86_64         468     y                                       */
/*  50 GbE on x86_64          93.6   y                                       */
/* 100 GbE on x86_64          46.8   y                                       */
/*****************************************************************************/
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <stdlib.h>                                             /* realloc() */
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
#include <net/if.h>                                          /* struct ifreq */
#include <netinet/in.h>                   /* inet_ntoa(), struct sockaddr_in */
#include <sys/ioctl.h>                                            /* ioctl() */
#include <sys/types.h>                                           /* socket() */
typedef __uint32_t u32;         /* ditto */
typedef __uint16_t u16;         /* ditto */
typedef __uint8_t u8;           /* ditto */
#include <sys/socket.h>            /* socket(), inet_ntoa(), struct sockaddr */
#include <arpa/inet.h>                                        /* inet_ntoa() */
#include <net/if_arp.h>                                      /* ARPHRD_ETHER */
#include <math.h>                                                 /* floor() */
#include <assert.h>                                              /* assert() */
#include <linux/types.h>  /* needed for compatibility with SLC5 */
#include <linux/sockios.h>                                    /* SIOCETHTOOL */
#include <linux/ethtool.h>       /* ETHTOOL_GDRVINFO, struct ethtool_drvinfo */
/*****************************************************************************/
#include "getNIFfromProc.h"                                 /* struct nifCnt */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#include "fmcTssUtils.h"                                   /* getTssString() */
/*****************************************************************************/
#if 0
/* defined in <linux/sockios.h> */
#define SIOCETHTOOL     0x8946          /* Ethtool interface            */
/* defined in <linux/ethtool.h> */
#define ETHTOOL_GDRVINFO        0x00000003 /* Get driver info. */
#define ETHTOOL_BUSINFO_LEN     32
/* these strings are set to whatever the driver author decides... */
struct ethtool_drvinfo{
  uint32_t cmd;
  char     driver[32];     /* driver short name, "tulip", "eepro100" */
  char     version[32];    /* driver version string */
  char     fw_version[32]; /* firmware version string, if applicable */
  char     bus_info[ETHTOOL_BUSINFO_LEN];  /* Bus info for this IF. */
                           /* For PCI devices, use pci_dev->slot_name. */
  char     reserved1[32];
  char     reserved2[24];
  uint32_t eedump_len;     /* Size of data from ETHTOOL_GEEPROM (bytes) */
  uint32_t regdump_len;    /* Size of data from ETHTOOL_GREGS (bytes) */
};
#endif
/*****************************************************************************/
/* globals */
char ifStatFileName[]="/proc/net/dev";
int doResetNifCnts=0;  /* set to 1 by resetNIF() to schedule a counter reset */
static char rcsid[]="$Id: getNIFfromProc.c,v 1.28 2011/09/27 11:53:17 galli Exp galli $";
/*****************************************************************************/
/* function prototype */
static char *getName(char *name,char *p);
char *getProcNetDev(int errU);
char *getIpAddr(char *nifName,int errU);
char *getHwAddr(char *nifName,int errU);
char *getBusAddr(char *nifName,int errU);
int getNIFnumber(char *ifStatBuf);
/*****************************************************************************/
/* Warning: to be called more frequent than 1/34s for Gigabit Ethernet       */
nifsData_t readNIF(int deBug,int errU)
{
  /* counters */
  int i;
  /* pointers */
  char *bp,*ep;
  char *s;
  /* other */
  char *ifStatBuf;
  char name[IF_NAME_LEN+1];
  char *ipAddress=NULL,*hwAddress=NULL,*busAddress=NULL;
  static nifsData_t nifs={0,NULL,NULL,0,NULL,0.0,0.0,0.0,0.0};
  static struct counts *lastCnt=NULL;
  static struct counts *cntInc=NULL;
  struct timeval now;
  struct timezone tz;
  int sec,usec;
  float et=0.0,tss=0.0;
  static struct timeval lastTime={0,0},startTime={0,0};
  static int isFirstCall=1;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  /* nifs = structure returned by readNIF() function */
  nifs.success=0;
  nifs.stss=getTssString(0.0);             /* time since start or last reset */
  nifs.totalEthRx_bitRate=0.0;
  nifs.totalEthRx_packetsRate=0.0;
  nifs.totalEthTx_bitRate=0.0;
  nifs.totalEthTx_packetsRate=0.0;
  /*-------------------------------------------------------------------------*/
  if(isFirstCall)
  {
    /*.......................................................................*/
    /* clean nifs */
    memset(&nifs,0,sizeof(nifs));
    nifs.stss=getTssString(0.0);
    nifs.version=rcsid;
    /*.......................................................................*/
    /* get the number of interfaces nifs.nifN */
    ifStatBuf=getProcNetDev(errU);        /* read /proc/net/dev in ifStatBuf */
    if(!ifStatBuf)
    {
      mPrintf(errU,ERROR,__func__,0,"getProcNetDev() call retuned an error!");
      return nifs;
    }
    nifs.nifN=getNIFnumber(ifStatBuf);
    if(deBug)mPrintf(errU,DEBUG,__func__,0,"nifN=%d",nifs.nifN);
    /*.......................................................................*/
    /* allocate space for nifData */
    nifs.data=(struct nifData*)realloc(nifs.data,nifs.nifN*
                                       sizeof(struct nifData));
    /* clean nifs.data */
    memset(nifs.data,0,nifs.nifN*sizeof(struct nifData));
    /* allocate space for lastCnt and cntInc */
    lastCnt=(struct counts*)realloc(lastCnt,nifs.nifN*sizeof(struct counts));
    cntInc=(struct counts*)realloc(cntInc,nifs.nifN*sizeof(struct counts));
    /*.......................................................................*/
    /* get interface name, IP, MAC and bus addresses: nifs.data[i].name,     */
    /* nifs.data[i].ipAddress, nifs.data[i].hwAddress and                    */
    /* nifs.data[i].busAddress                                               */
    ifStatBuf=getProcNetDev(errU);        /* read /proc/net/dev in ifStatBuf */
    if(!ifStatBuf)
    {
      mPrintf(errU,ERROR,__func__,0,"getProcNetDev() call retuned an error!");
      return nifs;
    }
    bp=ifStatBuf;
    bp=strchr(bp,'\n')+1;                                  /* eat first line */
    bp=strchr(bp,'\n')+1;                                 /* eat second line */
    for(i=0;i<nifs.nifN;i++)                              /* loop over lines */
    {
      ep=strchr(bp,'\n');                                        /* find EOL */
      if(ep==NULL)break;
      *ep='\0';                             /* set string termination to EOL */
      s=getName(name,bp);             /* bp=line, name=interface, s=contents */
      nifs.data[i].name=strdup(name);
      /* get IP address for interface */
      ipAddress=getIpAddr(name,errU);
      if(ipAddress!=NULL)nifs.data[i].ipAddress=ipAddress;
      else nifs.data[i].ipAddress="N/A";
      /* get MAC address for interface */
      hwAddress=getHwAddr(name,errU);
      if(hwAddress!=NULL)nifs.data[i].hwAddress=hwAddress;
      else nifs.data[i].hwAddress="N/D";
      /* get Bus (PCI) address for interface */
      busAddress=getBusAddr(name,errU);
      if(busAddress)nifs.data[i].busAddress=busAddress;
      else nifs.data[i].busAddress="N/D";
      bp=ep+1;     /* set string beginning to the beginning of the next line */
    }
    /*.......................................................................*/
  }                                                       /* if(isFirstCall) */
  /*-------------------------------------------------------------------------*/
  for(i=0;i<nifs.nifN;i++)
  {
    memset(&nifs.data[i].cnt,0,sizeof(struct counts));
    memset(&nifs.data[i].rate,0,sizeof(struct rates));
    memset(&nifs.data[i].ratio,0,sizeof(struct ratios));
    memset(&nifs.data[i].arate,0,sizeof(struct rates));
    memset(&nifs.data[i].aratio,0,sizeof(struct ratios));
  }
  /*-------------------------------------------------------------------------*/
  /* get system counters */
  ifStatBuf=getProcNetDev(errU);          /* read /proc/net/dev in ifStatBuf */
  if(!ifStatBuf)
  {
    mPrintf(errU,ERROR,__func__,0,"getProcNetDev() call retuned an error!");
    return nifs;
  }
  bp=ifStatBuf;
  bp=strchr(bp,'\n')+1;                                    /* eat first line */
  bp=strchr(bp,'\n')+1;                                   /* eat second line */
  for(i=0;;i++)                                           /* loop over lines */
  {
    ep=strchr(bp,'\n');                                          /* find EOL */
    if(ep==NULL)break;
    *ep='\0';                               /* set string termination to EOL */
    s=getName(name,bp);               /* bp=line, name=interface, s=contents */
    if(strcmp(name,nifs.data[i].name))
    {
      mPrintf(errU,ERROR,__func__,0,"Interface order changed! Resetting "
              "sensor...");
      isFirstCall=1;
      return nifs;
    }
    /* read the 16 unsigned long system counters */
    sscanf(s,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           &nifs.data[i].cnt.rx_bytes,
           &nifs.data[i].cnt.rx_packets,
           &nifs.data[i].cnt.rx_errors,
           &nifs.data[i].cnt.rx_dropped,
           &nifs.data[i].cnt.rx_fifo_errors,
           &nifs.data[i].cnt.rx_frame_errors,
           &nifs.data[i].cnt.rx_compressed,
           &nifs.data[i].cnt.rx_multicast,
           &nifs.data[i].cnt.tx_bytes,
           &nifs.data[i].cnt.tx_packets,
           &nifs.data[i].cnt.tx_errors,
           &nifs.data[i].cnt.tx_dropped,
           &nifs.data[i].cnt.tx_fifo_errors,
           &nifs.data[i].cnt.collisions,
           &nifs.data[i].cnt.tx_carrier_errors,
           &nifs.data[i].cnt.tx_compressed);
    bp=ep+1;     /* set string beginning to the beginning of the next line */
  }
  /*-------------------------------------------------------------------------*/
  if(isFirstCall)
  {
    for(i=0;i<nifs.nifN;i++)
    {
      memset(&nifs.data[i].tcnt,0,sizeof(struct counts64));
      memcpy(&lastCnt[i],&nifs.data[i].cnt,sizeof(struct counts));
    }
    memset(cntInc,0,nifs.nifN*sizeof(struct counts));
    gettimeofday(&lastTime,&tz);
    gettimeofday(&startTime,&tz);
    isFirstCall=0;
  }
  else                                                     /* not first call */
  {
    /*.......................................................................*/
    gettimeofday(&now,&tz);                  /* get current time: sec & usec */
    /*.......................................................................*/
    if(doResetNifCnts)
    {
      memcpy(&startTime,&lastTime,sizeof(struct timeval));
      for(i=0;i<nifs.nifN;i++)
      {
        memset(&nifs.data[i].tcnt,0,sizeof(struct counts64));
        memset(&nifs.data[i].mrate,0,sizeof(struct rates));
        memset(&nifs.data[i].mratio,0,sizeof(struct ratios));
      }
      mPrintf(errU,INFO,__func__,0,"Counters reset requested by user!");
      doResetNifCnts=0;
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
    assert(et>=0);
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
    tss=(float)sec+((float)usec/(float)1000000.0);  /* time since start */
    nifs.stss=getTssString(tss);
    if(deBug)
    {
      mPrintf(errU,DEBUG,__func__,0,"time since start = %fs = %s",tss,
              nifs.stss);
    }
    /*.......................................................................*/
    /* evaluate counter increments */
    for(i=0;i<nifs.nifN;i++)
    {
      struct counts *cnt=&nifs.data[i].cnt;
      if(cnt->rx_bytes<lastCnt[i].rx_bytes)
        mPrintf(errU,VERB,__func__,0,"rx_bytes counter reset!");
      if(cnt->rx_packets<lastCnt[i].rx_packets)
        mPrintf(errU,VERB,__func__,0,"rx_packets counter reset!");
      if(cnt->rx_errors<lastCnt[i].rx_errors)
        mPrintf(errU,VERB,__func__,0,"rx_errors counter reset!");
      if(cnt->rx_dropped<lastCnt[i].rx_dropped)
        mPrintf(errU,VERB,__func__,0,"rx_dropped counter reset!");
      if(cnt->rx_fifo_errors<lastCnt[i].rx_fifo_errors)
        mPrintf(errU,VERB,__func__,0,"rx_fifo_errors counter reset!");
      if(cnt->rx_frame_errors<lastCnt[i].rx_frame_errors)
        mPrintf(errU,VERB,__func__,0,"rx_frame_errors counter reset!");
      if(cnt->rx_compressed<lastCnt[i].rx_compressed)
        mPrintf(errU,VERB,__func__,0,"rx_compressed counter reset!");
      if(cnt->rx_multicast<lastCnt[i].rx_multicast)
        mPrintf(errU,VERB,__func__,0,"rx_multicast counter reset!");
      if(cnt->tx_bytes<lastCnt[i].tx_bytes)
        mPrintf(errU,VERB,__func__,0,"tx_bytes counter reset!");
      if(cnt->tx_packets<lastCnt[i].tx_packets)
        mPrintf(errU,VERB,__func__,0,"tx_packets counter reset!");
      if(cnt->tx_errors<lastCnt[i].tx_errors)
        mPrintf(errU,VERB,__func__,0,"tx_errors counter reset!");
      if(cnt->tx_dropped<lastCnt[i].tx_dropped)
        mPrintf(errU,VERB,__func__,0,"tx_dropped counter reset!");
      if(cnt->tx_fifo_errors<lastCnt[i].tx_fifo_errors)
        mPrintf(errU,VERB,__func__,0,"tx_fifo_errors counter reset!");
      if(cnt->collisions<lastCnt[i].collisions)
        mPrintf(errU,VERB,__func__,0,"collisions counter reset!");
      if(cnt->tx_carrier_errors<lastCnt[i].tx_carrier_errors)
        mPrintf(errU,VERB,__func__,0,"tx_carrier_errors counter reset!");
      if(cnt->tx_compressed<lastCnt[i].tx_compressed)
        mPrintf(errU,VERB,__func__,0,"tx_compressed counter reset!");
      cntInc[i].rx_bytes          = cnt->rx_bytes-
                                    lastCnt[i].rx_bytes;
      cntInc[i].rx_packets        = cnt->rx_packets-
                                    lastCnt[i].rx_packets;
      cntInc[i].rx_errors         = cnt->rx_errors-
                                    lastCnt[i].rx_errors;
      cntInc[i].rx_dropped        = cnt->rx_dropped-
                                    lastCnt[i].rx_dropped;
      cntInc[i].rx_fifo_errors    = cnt->rx_fifo_errors-
                                    lastCnt[i].rx_fifo_errors;
      cntInc[i].rx_frame_errors   = cnt->rx_frame_errors-
                                    lastCnt[i].rx_frame_errors;
      cntInc[i].rx_compressed     = cnt->rx_compressed-
                                    lastCnt[i].rx_compressed;
      cntInc[i].rx_multicast      = cnt->rx_multicast-
                                    lastCnt[i].rx_multicast;
      cntInc[i].tx_bytes          = cnt->tx_bytes-
                                    lastCnt[i].tx_bytes;
      cntInc[i].tx_packets        = cnt->tx_packets-
                                    lastCnt[i].tx_packets;
      cntInc[i].tx_errors         = cnt->tx_errors-
                                    lastCnt[i].tx_errors;
      cntInc[i].tx_dropped        = cnt->tx_dropped-
                                    lastCnt[i].tx_dropped;
      cntInc[i].tx_fifo_errors    = cnt->tx_fifo_errors-
                                    lastCnt[i].tx_fifo_errors;
      cntInc[i].collisions        = cnt->collisions-
                                    lastCnt[i].collisions;
      cntInc[i].tx_carrier_errors = cnt->tx_carrier_errors-
                                    lastCnt[i].tx_carrier_errors;
      cntInc[i].tx_compressed     = cnt->tx_compressed-
                                    lastCnt[i].tx_compressed;
    }                                            /* for(i=0;i<nifs.nifN;i++) */
    /*.......................................................................*/
    /* increment total counters */
    for(i=0;i<nifs.nifN;i++)
    {
      unsigned long long old;
      struct counts64 *tcnt=&nifs.data[i].tcnt;
      old=tcnt->rx_bytes;
      tcnt->rx_bytes+=cntInc[i].rx_bytes;
      if(tcnt->rx_bytes<old)
        mPrintf(errU,WARN,__func__,0,"rx_bytes total counter reset!");
      old=tcnt->rx_packets;
      tcnt->rx_packets+=cntInc[i].rx_packets;
      if(tcnt->rx_packets<old)
        mPrintf(errU,WARN,__func__,0,"rx_packets total counter reset!");
      old=tcnt->rx_errors;
      tcnt->rx_errors+=cntInc[i].rx_errors;
      if(tcnt->rx_errors<old)
        mPrintf(errU,WARN,__func__,0,"rx_errors total counter reset!");
      old=tcnt->rx_dropped;
      tcnt->rx_dropped+=cntInc[i].rx_dropped;
      if(tcnt->rx_dropped<old)
        mPrintf(errU,WARN,__func__,0,"rx_dropped total counter reset!");
      old=tcnt->rx_fifo_errors;
      tcnt->rx_fifo_errors+=cntInc[i].rx_fifo_errors;
      if(tcnt->rx_fifo_errors<old)
        mPrintf(errU,WARN,__func__,0,"rx_fifo_errors total counter reset!");
      old=tcnt->rx_frame_errors;
      tcnt->rx_frame_errors+=cntInc[i].rx_frame_errors;
      if(tcnt->rx_frame_errors<old)
        mPrintf(errU,WARN,__func__,0,"rx_frame_errors total counter reset!");
      old=tcnt->rx_compressed;
      tcnt->rx_compressed+=cntInc[i].rx_compressed;
      if(tcnt->rx_compressed<old)
        mPrintf(errU,WARN,__func__,0,"rx_compressed total counter reset!");
      old=tcnt->rx_multicast;
      tcnt->rx_multicast+=cntInc[i].rx_multicast;
      if(tcnt->rx_multicast<old)
        mPrintf(errU,WARN,__func__,0,"rx_multicast total counter reset!");
      old=tcnt->tx_bytes;
      tcnt->tx_bytes+=cntInc[i].tx_bytes;
      if(tcnt->tx_bytes<old)
        mPrintf(errU,WARN,__func__,0,"tx_bytes total counter reset!");
      old=tcnt->tx_packets;
      tcnt->tx_packets+=cntInc[i].tx_packets;
      if(tcnt->tx_packets<old)
        mPrintf(errU,WARN,__func__,0,"tx_packets total counter reset!");
      old=tcnt->tx_errors;
      tcnt->tx_errors+=cntInc[i].tx_errors;
      if(tcnt->tx_errors<old)
        mPrintf(errU,WARN,__func__,0,"tx_errors total counter reset!");
      old=tcnt->tx_dropped;
      tcnt->tx_dropped+=cntInc[i].tx_dropped;
      if(tcnt->tx_dropped<old)
        mPrintf(errU,WARN,__func__,0,"tx_dropped total counter reset!");
      old=tcnt->tx_fifo_errors;
      tcnt->tx_fifo_errors+=cntInc[i].tx_fifo_errors;
      if(tcnt->tx_fifo_errors<old)
        mPrintf(errU,WARN,__func__,0,"tx_fifo_errors total counter reset!");
      old=tcnt->collisions;
      tcnt->collisions+=cntInc[i].collisions;
      if(tcnt->collisions<old)
        mPrintf(errU,WARN,__func__,0,"collisions total counter reset!");
      old=tcnt->tx_carrier_errors;
      tcnt->tx_carrier_errors+=cntInc[i].tx_carrier_errors;
      if(tcnt->tx_carrier_errors<old)
        mPrintf(errU,WARN,__func__,0,"tx_carrier_errors total counter reset!");
      old=tcnt->tx_compressed;
      tcnt->tx_compressed+=cntInc[i].tx_compressed;
      if(tcnt->tx_compressed<old)
        mPrintf(errU,WARN,__func__,0,"tx_compressed total counter reset!");
    }
    /*.......................................................................*/
    /* evaluate rates and maximum rates */
    if(et>FLT_MIN)                                    /* avoid division by 0 */
    {
      struct rates *rate;
      struct rates *mrate;
      nifs.totalEthRx_bitRate     = 0.0;
      nifs.totalEthRx_packetsRate = 0.0;
      nifs.totalEthTx_bitRate     = 0.0;
      nifs.totalEthTx_packetsRate = 0.0;
      for(i=0;i<nifs.nifN;i++)
      {
        rate=&nifs.data[i].rate;
        mrate=&nifs.data[i].mrate;
        /* evaluate rates */
        rate->rxBitRate        = (float)cntInc[i].rx_bytes*8.0/et;
        rate->rxFrameRate    = (float)cntInc[i].rx_packets/et;
        rate->rxMulticastRate  = (float)cntInc[i].rx_multicast/et;
        rate->rxCompressedRate = (float)cntInc[i].rx_compressed/et;
        rate->txBitRate        = (float)cntInc[i].tx_bytes*8.0/et;
        rate->txFrameRate    = (float)cntInc[i].tx_packets/et;
        rate->txCompressedRate = (float)cntInc[i].tx_compressed/et;
        /* evaluate total rates */
        if(strstr(nifs.data[i].name,"eth"))
        {
          nifs.totalEthRx_bitRate     += rate->rxBitRate;
          nifs.totalEthRx_packetsRate += rate->rxFrameRate;
          nifs.totalEthTx_bitRate     += rate->txBitRate;
          nifs.totalEthTx_packetsRate += rate->txFrameRate;
        }
        /* evaluate maximum rates */
        if(mrate->rxBitRate        < rate->rxBitRate)
          mrate->rxBitRate         = rate->rxBitRate;
        if(mrate->rxFrameRate    < rate->rxFrameRate)
          mrate->rxFrameRate     = rate->rxFrameRate;
        if(mrate->rxMulticastRate  < rate->rxMulticastRate)
          mrate->rxMulticastRate   = rate->rxMulticastRate;
        if(mrate->rxCompressedRate < rate->rxCompressedRate)
          mrate->rxCompressedRate  = rate->rxCompressedRate;
        if(mrate->txBitRate        < rate->txBitRate)
          mrate->txBitRate         = rate->txBitRate;
        if(mrate->txFrameRate    < rate->txFrameRate)
          mrate->txFrameRate     = rate->txFrameRate;
        if(mrate->txCompressedRate < rate->txCompressedRate)
          mrate->txCompressedRate  = rate->txCompressedRate;
      }
    }
    else                                    /* set to 0 if zero elapsed time */
    {
      for(i=0;i<nifs.nifN;i++)
      {
        memset(&nifs.data[i].rate,0,sizeof(struct rates));
      }
    }
    /*.......................................................................*/
    /* evaluate average rates */
    if(tss>FLT_MIN)                                   /* avoid division by 0 */
    {
      struct rates *arate;
      struct counts64 *tcnt;
      for(i=0;i<nifs.nifN;i++)
      {
        arate=&nifs.data[i].arate;
        tcnt=&nifs.data[i].tcnt;
        arate->rxBitRate        = (float)tcnt->rx_bytes*8.0/tss;
        arate->rxFrameRate    = (float)tcnt->rx_packets/tss;
        arate->rxMulticastRate  = (float)tcnt->rx_multicast/tss;
        arate->rxCompressedRate = (float)tcnt->rx_compressed/tss;
        arate->txBitRate        = (float)tcnt->tx_bytes*8.0/tss;
        arate->txFrameRate    = (float)tcnt->tx_packets/tss;
        arate->txCompressedRate = (float)tcnt->tx_compressed/tss;
      }
    }
    else                                    /* set to 0 if zero elapsed time */
    {
      for(i=0;i<nifs.nifN;i++)
      {
        memset(&nifs.data[i].arate,0,sizeof(struct rates));
      }
    }
    /*.......................................................................*/
    /* evaluate ratios and maximum ratios */
    for(i=0;i<nifs.nifN;i++)
    {
      struct ratios *ratio=&nifs.data[i].ratio;
      struct ratios *mratio=&nifs.data[i].mratio;
      if(cntInc[i].rx_packets>0)
      {
        /* evaluate ratios */
        ratio->rxBytePerFrame     = (float)cntInc[i].rx_bytes/
                                     (float)cntInc[i].rx_packets;
        ratio->rxErrorFrac       = (float)cntInc[i].rx_errors/
                                     (float)cntInc[i].rx_packets;
        ratio->rxDroppedFrac      = (float)cntInc[i].rx_dropped/
                                     (float)cntInc[i].rx_packets;
        ratio->rxFifoErrorFrac  = (float)cntInc[i].rx_fifo_errors/
                                     (float)cntInc[i].rx_packets;
        ratio->rxFrameErrorFrac = (float)cntInc[i].rx_frame_errors/
                                     (float)cntInc[i].rx_packets;
        /* evaluate maximum ratios */
        if(mratio->rxBytePerFrame     < ratio->rxBytePerFrame)
          mratio->rxBytePerFrame      = ratio->rxBytePerFrame;
        if(mratio->rxErrorFrac       < ratio->rxErrorFrac)
          mratio->rxErrorFrac        = ratio->rxErrorFrac;
        if(mratio->rxDroppedFrac      < ratio->rxDroppedFrac)
          mratio->rxDroppedFrac       = ratio->rxDroppedFrac;
        if(mratio->rxFifoErrorFrac  < ratio->rxFifoErrorFrac)
          mratio->rxFifoErrorFrac   = ratio->rxFifoErrorFrac;
        if(mratio->rxFrameErrorFrac < ratio->rxFrameErrorFrac)
          mratio->rxFrameErrorFrac  = ratio->rxFrameErrorFrac;
      }
      else
      {
        ratio->rxBytePerFrame     = 0.0F;
        ratio->rxErrorFrac       = 0.0F;
        ratio->rxDroppedFrac      = 0.0F;
        ratio->rxFifoErrorFrac  = 0.0F;
        ratio->rxFrameErrorFrac = 0.0F;
      }
      if(cntInc[i].tx_packets>0)
      {
        /* evaluate ratios */
        ratio->txBytePerFrame       = (float)cntInc[i].tx_bytes/
                                       (float)cntInc[i].tx_packets;
        ratio->txErrorFrac         = (float)cntInc[i].tx_errors/
                                       (float)cntInc[i].tx_packets;
        ratio->txDroppedFrac        = (float)cntInc[i].tx_dropped/
                                       (float)cntInc[i].tx_packets;
        ratio->txFifoErrorFrac    = (float)cntInc[i].tx_fifo_errors/
                                       (float)cntInc[i].tx_packets;
        ratio->collisionsFrac        = (float)cntInc[i].collisions/
                                       (float)cntInc[i].tx_packets;
        ratio->txCarrierErrorFrac = (float)cntInc[i].tx_carrier_errors/
                                       (float)cntInc[i].tx_packets;
        /* evaluate maximum ratios */
        if(mratio->txBytePerFrame       < ratio->txBytePerFrame)
          mratio->txBytePerFrame        = ratio->txBytePerFrame;
        if(mratio->txErrorFrac         < ratio->txErrorFrac)
          mratio->txErrorFrac          = ratio->txErrorFrac;
        if(mratio->txDroppedFrac        < ratio->txDroppedFrac)
          mratio->txDroppedFrac         = ratio->txDroppedFrac;
        if(mratio->txFifoErrorFrac    < ratio->txFifoErrorFrac)
          mratio->txFifoErrorFrac     = ratio->txFifoErrorFrac;
        if(mratio->collisionsFrac        < ratio->collisionsFrac)
          mratio->collisionsFrac         = ratio->collisionsFrac;
        if(mratio->txCarrierErrorFrac < ratio->txCarrierErrorFrac)
          mratio->txCarrierErrorFrac  = ratio->txCarrierErrorFrac;
      }
      else
      {
        ratio->txBytePerFrame       = 0.0F;
        ratio->txErrorFrac         = 0.0F;
        ratio->txDroppedFrac        = 0.0F;
        ratio->txFifoErrorFrac    = 0.0F;
        ratio->collisionsFrac        = 0.0F;
        ratio->txCarrierErrorFrac = 0.0F;
      }
    }
    /*.......................................................................*/
    /* evaluate average ratios */
    for(i=0;i<nifs.nifN;i++)
    {
      struct ratios *aratio=&nifs.data[i].aratio;
      struct counts64 *tcnt=&nifs.data[i].tcnt;
      if(tcnt->rx_packets>0)
      {
        aratio->rxBytePerFrame     = (float)tcnt->rx_bytes/
                                      (float)tcnt->rx_packets;
        aratio->rxErrorFrac       = (float)tcnt->rx_errors/
                                      (float)tcnt->rx_packets;
        aratio->rxDroppedFrac      = (float)tcnt->rx_dropped/
                                      (float)tcnt->rx_packets;
        aratio->rxFifoErrorFrac  = (float)tcnt->rx_fifo_errors/
                                      (float)tcnt->rx_packets;
        aratio->rxFrameErrorFrac = (float)tcnt->rx_frame_errors/
                                      (float)tcnt->rx_packets;
      }
      else
      {
        aratio->rxBytePerFrame     = 0.0F;
        aratio->rxErrorFrac       = 0.0F;
        aratio->rxDroppedFrac      = 0.0F;
        aratio->rxFifoErrorFrac  = 0.0F;
        aratio->rxFrameErrorFrac = 0.0F;
      }
      if(tcnt->tx_packets>0)
      {
        aratio->txBytePerFrame       = (float)tcnt->tx_bytes/
                                        (float)tcnt->tx_packets;
        aratio->txErrorFrac         = (float)tcnt->tx_errors/
                                        (float)tcnt->tx_packets;
        aratio->txDroppedFrac        = (float)tcnt->tx_dropped/
                                        (float)tcnt->tx_packets;
        aratio->txFifoErrorFrac    = (float)tcnt->tx_fifo_errors/
                                        (float)tcnt->tx_packets;
        aratio->collisionsFrac        = (float)tcnt->collisions/
                                        (float)tcnt->tx_packets;
        aratio->txCarrierErrorFrac = (float)tcnt->tx_carrier_errors/
                                        (float)tcnt->tx_packets;
      }
      else
      {
        aratio->txBytePerFrame       = 0.0F;
        aratio->txErrorFrac         = 0.0F;
        aratio->txDroppedFrac        = 0.0F;
        aratio->txFifoErrorFrac    = 0.0F;
        aratio->collisionsFrac        = 0.0F;
        aratio->txCarrierErrorFrac = 0.0F;
      }
    }
    /*.......................................................................*/
    /* save last counters */
    gettimeofday(&lastTime,&tz);
    for(i=0;i<nifs.nifN;i++)
    {
      memcpy(&lastCnt[i],&nifs.data[i].cnt,sizeof(struct counts));
    }
    /*.......................................................................*/
  }                                                        /* not first call */
  /*-------------------------------------------------------------------------*/
  nifs.success=1;
  return nifs;
}
/*****************************************************************************/
void resetNIF()
{
  doResetNifCnts=1;
  return;
}
/*****************************************************************************/
/* Get IF name (lo, eth0, eth1, ...) from a /proc/net/dev line */
static char *getName(char *name,char *p)
{
  while(isspace(*p))p++;                              /* skip initial spaces */
  while(*p)
  {
    if(isspace(*p))break;
    if(*p==':')                                         /* could be an alias */
    {
      char *dot=p,*dotname=name;
      *name++=*p++;
      while(isdigit(*p))*name++=*p++;
      if(*p!=':')                                       /* it wasn't, backup */
      {
        p=dot;
        name=dotname;
      }
      if(*p=='\0')return NULL;
      p++;
      break;
    }
    *name++=*p++;
  }
  *name++='\0';
  return p;
}
/*****************************************************************************/
char *getProcNetDev(int errU)
{
  int ifStatFileFD;
  static char ifStatBuf[PROC_NET_DEV_SIZE];
  int readBytes;
  /*-------------------------------------------------------------------------*/
  /* read /proc/net/dev in ifStatBuf */
  ifStatFileFD=open(ifStatFileName,O_RDONLY);
  if(ifStatFileFD==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",
            ifStatFileName,strerror(errno));
    return NULL;
  }
  if(lseek(ifStatFileFD,0L,SEEK_SET)<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot lseek file \"%s\": %s!",
            ifStatFileName,strerror(errno));
    return NULL;
  }
  if((readBytes=read(ifStatFileFD,ifStatBuf,sizeof(ifStatBuf)-1))<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot read file \"%s\": %s!",
            ifStatFileName,strerror(errno));
    return NULL;
  }
  close(ifStatFileFD);
  ifStatBuf[readBytes]='\0';
  return ifStatBuf;
}
/*****************************************************************************/
char *getIpAddr(char *nifName,int errU)
{
  struct ifreq ifr;
  struct sockaddr addr;
  struct sockaddr_in addr_in;
  int sockFd;
  /*-------------------------------------------------------------------------*/
  memset(&addr,0,sizeof(struct sockaddr));
  memset(&addr_in,0,sizeof(struct sockaddr_in));
  memset(&ifr,0,sizeof(struct ifreq));
  sockFd=socket(AF_INET,SOCK_DGRAM,0);
  if(sockFd<0)
  {
    mPrintf(errU,WARN,__func__,0,"if: %s. socket(): %s!",nifName,
            strerror(errno));
    return NULL;
  }
  ifr.ifr_addr.sa_family=AF_INET;
  strcpy(ifr.ifr_name,nifName);         /* interface name, eg: eth0, lo, etc */
  if(ioctl(sockFd,SIOCGIFADDR,&ifr)!=0)
  {
    mPrintf(errU,WARN,__func__,0,"if: %s. ioctl(): %s!",nifName,
            strerror(errno));
    close(sockFd);
    return NULL;
  }
  addr=ifr.ifr_addr;
  close(sockFd);
  addr_in=*((struct sockaddr_in*)&addr);
  return strdup(inet_ntoa(addr_in.sin_addr));
}
/*****************************************************************************/
char *getHwAddr(char *nifName,int errU)
{
  struct ifreq ifr;
  struct sockaddr addr;
  struct sockaddr_in addr_in;
  int sockFd;
  static char hwAddr[18];
  /*-------------------------------------------------------------------------*/
  memset(&addr,0,sizeof(struct sockaddr));
  memset(&addr_in,0,sizeof(struct sockaddr_in));
  memset(&ifr,0,sizeof(struct ifreq));
  sockFd=socket(AF_INET,SOCK_DGRAM,0);
  if(sockFd<0)
  {
    mPrintf(errU,WARN,__func__,0,"if: %s. socket(): %s!",nifName,
            strerror(errno));
    return NULL;
  }
  ifr.ifr_addr.sa_family=AF_INET;
  strcpy(ifr.ifr_name,nifName);         /* interface name, eg: eth0, lo, etc */
  if(ioctl(sockFd,SIOCGIFHWADDR,&ifr)<0)
  {
    mPrintf(errU,WARN,__func__,0,"if: %s. ioctl(): %s!",nifName,
            strerror(errno));
    close(sockFd);
    return NULL;
  }
  if(ifr.ifr_hwaddr.sa_family==ARPHRD_ETHER)
  {
    snprintf(hwAddr,sizeof(hwAddr),"%02x:%02x:%02x:%02x:%02x:%02x",
             (ifr.ifr_hwaddr.sa_data[0]&0377),
             (ifr.ifr_hwaddr.sa_data[1]&0377),
             (ifr.ifr_hwaddr.sa_data[2]&0377),
             (ifr.ifr_hwaddr.sa_data[3]&0377),
             (ifr.ifr_hwaddr.sa_data[4]&0377),
             (ifr.ifr_hwaddr.sa_data[5]&0377));
  }
  else
  {
    strncpy(hwAddr,"N/D",sizeof(hwAddr));
  }
  close(sockFd);
  return strdup(hwAddr);
}
/*****************************************************************************/
char *getBusAddr(char *nifName,int errU)
{
  struct ifreq ifr;
  struct sockaddr addr;
  struct sockaddr_in addr_in;
  int sockFd;
  struct ethtool_drvinfo drvinfo;
  char *busAddr=NULL;
  /*-------------------------------------------------------------------------*/
  memset(&addr,0,sizeof(struct sockaddr));
  memset(&addr_in,0,sizeof(struct sockaddr_in));
  memset(&ifr,0,sizeof(struct ifreq));
  sockFd=socket(AF_INET,SOCK_DGRAM,0);
  if(sockFd<0)
  {
    mPrintf(errU,WARN,__func__,0,"interface: \"%s\". socket(): Cannot get "
            "control socket to get Bus Address: %s!",nifName,strerror(errno));
    return NULL;
  }
  ifr.ifr_addr.sa_family=AF_INET;
  strcpy(ifr.ifr_name,nifName);         /* interface name, eg: eth0, lo, etc */
  drvinfo.cmd=ETHTOOL_GDRVINFO;
  ifr.ifr_data=(caddr_t)&drvinfo;
  if(ioctl(sockFd,SIOCETHTOOL,&ifr)<0)
  {
    if(errno==1)                        /* Operation not permitted (no root) */
    {
      mPrintf(errU,WARN,__func__,0,"interface: \"%s\". ioctl(): %s. Cannot "
              "get Bus Address for the device \"%s\": need root privileges to "
              "get Bus Address!",nifName,strerror(errno),nifName);
    }
    else if(errno==95)                   /* Operation not supported (eg: lo) */
    {
      mPrintf(errU,INFO,__func__,0,"interface: \"%s\". ioctl(): %s. Cannot "
             "get Bus Address for the device \"%s\": the device \"%s\" has "
             "not a Bus Address!",nifName,strerror(errno),nifName,nifName);
    }
    else if(errno==19)                         /* No such device (e.g. eth7) */
    {
      mPrintf(errU,WARN,__func__,0,"interface: \"%s\". ioctl(): %s. Cannot "
              "get Bus Address for the device \"%s\": the device \"%s\" does "
              "not exist!",nifName,strerror(errno),nifName,nifName);
    }
    else
    {
      mPrintf(errU,WARN,__func__,0,"interface: \"%s\". ioctl(): %s: Cannot "
              "get Bus Address for the device \"%s\"!",nifName,strerror(errno),
              nifName);
    }
    close(sockFd);
    return NULL;
  }
  busAddr=strdup(drvinfo.bus_info);
  close(sockFd);
  return busAddr;
}
/*****************************************************************************/
char *nifInfo(char *what)
{
  static char info[1024];
  memset(info,0,sizeof(info));
  if(!strcmp(what,"ls"))
  {
    snprintf(info,1024,"collisionsFrac rxBitRate rxBytePerFrame "
             "rxCompressedRate rxDroppedFrac rxErrorFrac "
             "rxFifoErrorFrac rxFrameErrorFrac rxMulticastRate "
             "rxFrameRate txBitRate txBytePerFrame "
             "txCarrierErrorFrac txCompressedRate txDroppedFrac "
             "txErrorFrac txFifoErrorFrac txFrameRate ");
  }
  else if(!strcmp(what,"collisionsFrac"))
  {
    snprintf(info,1024,"The ratio collisions/tx_packets. collisions is the "
             "total number of Ethernet collisions. tx_packets is the total "
             "number of transmitted Ethernet frames.");
  }
  else if(!strcmp(what,"rxBitRate"))
  {
    snprintf(info,1024,"The total number of bits received in a second.");
  }
  else if(!strcmp(what,"rxBytePerFrame"))
  {
    snprintf(info,1024,"The ratio rx_bytes/rx_packets. rx_bytes is the total "
             "number of bytes received. rx_packets is the total number of "
             "Ethernet frames received. This ratio is the average number of "
             "bytes contained in a received Ethernet frame.");
  }
  else if(!strcmp(what,"rxCompressedRate"))
  {
    snprintf(info,1024,"The total number of compressed frames received in a "
             "second (used by cslip & hdlc).");
  }
  else if(!strcmp(what,"rxDroppedFrac"))
  {
    snprintf(info,1024,"The ratio rx_dropped/rx_packets. rx_dropped is the "
             "number of received Ethernet frames dropped by operating system "
             "due to buffer overflows or throttling policy. rx_packets is "
             "the total number of Ethernet frames received.");
  }
  else if(!strcmp(what,"rxErrorFrac"))
  {
    snprintf(info,1024,"The ratio rx_errors/rx_packets. rx_errors is the "
             "total number of bad Ethernet frames received. rx_packets is "
             "the total number of Ethernet frames received.");
  }
  else if(!strcmp(what,"rxFifoErrorFrac"))
  {
    snprintf(info,1024,"The ratio rx_fifo_errors/rx_packets. rx_fifo_errors "
             "is the number of receiver fifo overrun. rx_packets is the "
             "total number of Ethernet frames received.");
  }
  else if(!strcmp(what,"rxFrameErrorFrac"))
  {
    snprintf(info,1024,"The ratio rx_frame_errors/rx_packets. "
             "rx_frame_errors is the number of receiver frame alignment "
             "errors. rx_packets is the total number of Ethernet frames "
             "received.");
  }
  else if(!strcmp(what,"rxMulticastRate"))
  {
    snprintf(info,1024,"The total number of multicast Ethernet frames "
             "received in a second.");
  }
  else if(!strcmp(what,"rxFrameRate"))
  {
    snprintf(info,1024,"The total number of Ethernet frames received in a "
             "second.");
  }
  else if(!strcmp(what,"txBitRate"))
  {
    snprintf(info,1024,"The total number of bits transmitted in a second.");
  }
  else if(!strcmp(what,"txBytePerFrame"))
  {
    snprintf(info,1024,"The ratio tx_bytes/tx_packets. tx_bytes is the total "
             "number of bytes transmitted. tx_packets is the total number of "
             "Ethernet frames transmitted. This ratio is the average number "
             "of bytes contained in a transmitted Ethernet frame.");
  }
  else if(!strcmp(what,"txCarrierErrorFrac"))
  {
    snprintf(info,1024,"The ratio tx_carrier_errors/tx_packets. "
             "tx_carrier_errors is the number of transmission errors due to "
             "loss of carrier. tx_packets is the total number of Ethernet "
             "frames transmitted. If this ratio is greater than zero there "
             "is probably a cable/connector problem or a bad duplex setting.");
  }
  else if(!strcmp(what,"txCompressedRate"))
  {
    snprintf(info,1024,"The total number of compressed frames transmitted in "
             "a second (used by cslip & hdlc).");
  }
  else if(!strcmp(what,"txDroppedFrac"))
  {
    snprintf(info,1024,"The ratio tx_dropped/tx_packets. tx_dropped is the "
             "number of transmitted Ethernet frames dropped by operating "
             "system due to buffer overflows or throttling policy. "
             "tx_packets is the total number of transmitted Ethernet frames.");
  }
  else if(!strcmp(what,"txErrorFrac"))
  {
    snprintf(info,1024,"The ratio tx_errors/tx_packets. tx_errors is the "
             "total number of errors due to packet transmit problems. "
             "tx_packets is the total number of transmitted Ethernet frames.");
  }
  else if(!strcmp(what,"txFifoErrorFrac"))
  {
    snprintf(info,1024,"The ratio tx_fifo_errors/tx_packets. tx_fifo_errors "
             "is the number of transmitter fifo overrun. tx_packets is the "
             "total number of transmitted Ethernet frames.");
  }
  else if(!strcmp(what,"txFrameRate"))
  {
    snprintf(info,1024,"The total number of Ethernet frames transmitted in a "
             "second.");
  }
  else
  {
    sprintf(info,"No informations about item: \"%s\".",what);
  }
  return info;
}
/*****************************************************************************/
/* getNIFsensorVersion() - Returns the RCS identifier of this file.          */
/*****************************************************************************/
char *getNIFsensorVersion()
{
  return rcsid;
}
/*****************************************************************************/
/* Warning: this function modifies the buffer pointed by ifStatBuf */
int getNIFnumber(char *ifStatBuf)
{
  int nifN;
  char *bp,*ep;
  /*-------------------------------------------------------------------------*/
  bp=ifStatBuf;
  bp=strchr(bp,'\n')+1;                                    /* eat first line */
  bp=strchr(bp,'\n')+1;                                   /* eat second line */
  for(nifN=0;;)                                           /* loop over lines */
  {
    ep=strchr(bp,'\n');                                          /* find EOL */
    if(ep==NULL)break;
    *ep='\0';                               /* set string termination to EOL */
    nifN++;
    bp=ep+1;       /* set string beginning to the beginning of the next line */
  }
  return nifN;
}
/*****************************************************************************/
