/* ************************************************************************* */
/*
 * $Log: getOsInfo.c,v $
 * Revision 1.3  2008/10/09 09:53:06  galli
 * minor changes
 *
 * Revision 1.2  2008/02/23 22:42:35  galli
 * working version
 *
 * Revision 1.1  2008/01/22 18:36:22  galli
 * Initial revision
 * */
/* ************************************************************************* */
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <errno.h>                                                  /* errno */
#include <string.h>                                  /* strerror(), strcmp() */
#include <syslog.h>                                              /* syslog() */
#include <sys/types.h>                                             /* open() */
#include <sys/stat.h>                                              /* open() */
#include <fcntl.h>                                                 /* open() */
#include <unistd.h>                                     /* read(), sysconf() */
#include <sys/time.h>                                      /* gettimeofday() */
#include <math.h>                                                  /* fabs() */
#include <values.h>                                               /* FLT_MIN */
#include <stdlib.h>                                              /* strtod() */
#include <sys/utsname.h>                         /* uname(2), struct utsname */
#include <time.h>
/* ************************************************************************* */
#include "getOsInfo.h"                                      /* struct osInfo */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#define REDHAT_RELEASE_SIZE 512
/* ************************************************************************* */
static char rcsid[]="$Id: getOsInfo.c,v 1.3 2008/10/09 09:53:06 galli Exp $";
/* ************************************************************************* */
struct osInfo readOsInfo(int deBug,int errU)
{
  static struct utsname buf;
  static struct osInfo os;
  /* ----------------------------------------------------------------------- */
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  /* ----------------------------------------------------------------------- */
  /* clean & prepare output structure */
  memset((char*)&os,0,sizeof(struct osInfo));
  os.success=1;
  /* ----------------------------------------------------------------------- */
  /* get distribution */
  os.distribution=getRedhatRelease(errU);
  if(!os.distribution)
  {
    os.success=0;
    os.distribution="N/A";
  }
  /* ----------------------------------------------------------------------- */
  if(uname(&buf))
  {
    mPrintf(errU,ERROR,__func__,0,"uname(): %s",strerror(errno));
    os.success=0;
    os.name="N/A";
    os.kernel="N/A";
    os.release="N/A";
    os.machine="N/A";
  }
  else
  {
    os.name=buf.sysname;
    os.kernel=buf.release;
    os.release=buf.version;
    os.machine=buf.machine;
  }
  /* ----------------------------------------------------------------------- */
  return os;
}
/* ************************************************************************* */
struct timeInfo readTimeInfo(int deBug,int errU)
{
  struct timeInfo timeData;
  int upTimeFD;
  char upTimeBuf[UPTIME_FILE_SIZE]="";
  int readBytes;
  int readItems;
  float upTime,upTimeIdle;
  /* ----------------------------------------------------------------------- */
  memset(&timeData,0,sizeof(struct timeInfo));
  /* ----------------------------------------------------------------------- */
  timeData.now=time(NULL);
  /* ----------------------------------------------------------------------- */
  /* read /proc/uptime */
  upTimeFD=open(UPTIME_FILE_NAME,O_RDONLY);
  if(upTimeFD==-1)
  {
    mPrintf(errU,DEBUG,__func__,0,"Cannot open file \"%s\": %s!",
            UPTIME_FILE_NAME,strerror(errno));
    return timeData;
  }
  if(lseek(upTimeFD,0L,SEEK_SET)<0)
  {
    mPrintf(errU,DEBUG,__func__,0,"Cannot lseek file \"%s\": %s!",
            UPTIME_FILE_NAME,strerror(errno));
    return timeData;
  }
  if((readBytes=read(upTimeFD,upTimeBuf,sizeof(upTimeBuf)-1))<0)
  {
    mPrintf(errU,DEBUG,__func__,0,"Cannot read file \"%s\": %s!",
            UPTIME_FILE_NAME,strerror(errno));
    return timeData;
  }
  close(upTimeFD);
  /* ----------------------------------------------------------------------- */
  readItems=sscanf(upTimeBuf,"%f %f\n",&upTime,&upTimeIdle);
  if(readItems<2)
  {
    mPrintf(errU,DEBUG,__func__,0,"Error reading file \"%s\": only %d item(s) "
            "read (out of 2)!",UPTIME_FILE_NAME,readItems);
    return timeData;
  }
  timeData.upTime=(time_t)upTime;
  timeData.upTimeIdle=(time_t)upTimeIdle;
  /* ----------------------------------------------------------------------- */
  return timeData;
}
/* ************************************************************************* */
/* getRedhatRelease() - returns the content of file "/etc/redhat-release"    */
/* ************************************************************************* */
char *getRedhatRelease(int errU)
{
  int fileFD;
  static char buf[512];
  int readBytes;
  char *nlp=NULL;
  /* ----------------------------------------------------------------------- */
  /* read /etc/redhat-release in buf */
  fileFD=open(RELEASE_FILE_NAME,O_RDONLY);
  if(fileFD==-1)
  {
    mPrintf(errU,DEBUG,__func__,0,"Cannot open file \"%s\": %s!",
            RELEASE_FILE_NAME,strerror(errno));
    return NULL;
  }
  if(lseek(fileFD,0L,SEEK_SET)<0)
  {
    mPrintf(errU,DEBUG,__func__,0,"Cannot lseek file \"%s\": %s!",
            RELEASE_FILE_NAME,strerror(errno));
    return NULL;
  }
  if((readBytes=read(fileFD,buf,sizeof(buf)-1))<0)
  {
    mPrintf(errU,DEBUG,__func__,0,"Cannot read file \"%s\": %s!",
            RELEASE_FILE_NAME,strerror(errno));
    return NULL;
  }
  close(fileFD);
  buf[readBytes]='\0';
  nlp=strchr(buf,'\n');
  if(nlp)*nlp='\0';
  return buf;
}
/* ************************************************************************* */
/* getCpuinfoSensorVersion() - Returns the RCS identifier of this file.      */
/*****************************************************************************/
char *getOsInfoSensorVersion()
{
  return rcsid;
}
/*****************************************************************************/
