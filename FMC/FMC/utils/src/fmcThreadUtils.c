/* ######################################################################### */
/*
 * $Log: fmcThreadUtils.c,v $
 * Revision 1.5  2008/10/23 07:51:43  galli
 * minor changes
 *
 * Revision 1.2  2007/10/03 13:22:27  galli
 * clean-up
 *
 * Revision 1.1  2007/10/02 07:28:33  galli
 * Initial revision
 */
/* ######################################################################### */
#include <stdio.h>
#include <unistd.h>                                             /* access(2) */
#include <sys/types.h>                                  /* open(2), lseek(2) */
#include <sys/stat.h>                                             /* open(2) */
#include <fcntl.h>                                                /* open(2) */
#include <unistd.h>                                     /* read(2), lseek(2) */
#include <stdlib.h>                                             /* strtol(3) */
#include <string.h>                                           /* strerror(3) */
#include <errno.h>
#include "fmcMsgUtils.h"                             /* mPrintf(), rPrintf() */
#define STATUS_FILE_NAME "/proc/self/status"
#define STATUS_FILE_SIZE (128*64)
/* ######################################################################### */
static char rcsid[]="$Id: fmcThreadUtils.c,v 1.5 2008/10/23 07:51:43 galli Exp $";
/* ######################################################################### */
/* Returns the thread number of the current process */
int getThreadN(int errU)
{
  int statusFileFD;
  char statusBuf[STATUS_FILE_SIZE];
  int readBytes;
  char *lsp,*lep,*p;
  int threadN=0;
  /*-------------------------------------------------------------------------*/
  /* check status file accessibility */
  if(access(STATUS_FILE_NAME,R_OK)==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Status file: \"%s\" not accessible. "
            "access(2) error: %s!",STATUS_FILE_NAME,strerror(errno));
    return -1;
  }
  /* read STATUS_FILE_NAME in statusBuf */
  statusFileFD=open(STATUS_FILE_NAME,O_RDONLY);
  if(statusFileFD==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",
            STATUS_FILE_NAME,strerror(errno));
    return -1;
  }
  if(lseek(statusFileFD,0L,SEEK_SET)<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot lseek file \"%s\": %s!",
            STATUS_FILE_NAME,strerror(errno));
    return -1;
  }
  if((readBytes=read(statusFileFD,statusBuf,sizeof(statusBuf)-1))<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot read file \"%s\": %s!",
            STATUS_FILE_NAME,strerror(errno));
    return -1;
  }
  close(statusFileFD);
  statusBuf[readBytes]='\0';
  /* loop over lines */
  for(lsp=statusBuf;;)
  {
    lep=strchr(lsp,'\n');
    if(lep)*lep='\0';
    else break;
    if(strstr(lsp,"Threads"))
    {
      p=1+strchr(lsp,':');
      threadN=(int)strtol(p,(char**)NULL,0);
      break;
    }
    lsp=lep+1;
  }
  return threadN;
}
/*****************************************************************************/
char *getFmcThreadUtilsVersion()
{
  return rcsid;
}
/* ######################################################################### */
