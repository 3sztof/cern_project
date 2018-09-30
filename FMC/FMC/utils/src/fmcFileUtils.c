/* ######################################################################### */
/*
 * $Log: fmcFileUtils.c,v $
 * Revision 1.5  2009/01/22 10:16:10  galli
 * do not depend on kernel-devel headers
 *
 * Revision 1.4  2008/10/23 07:45:25  galli
 * minor changes
 *
 * Revision 1.3  2008/10/18 22:44:14  galli
 * bug fixed
 *
 * Revision 1.2  2008/10/18 15:47:56  galli
 * position the file offet to the beginning of the file
 *
 * Revision 1.1  2008/10/18 15:31:28  galli
 * Initial revision
 */
/* ######################################################################### */
#include <stdio.h>
#include <sys/stat.h>                                            /* lstat(2) */
#include <sys/types.h>                                            /* open(2) */
#include <sys/stat.h>                                             /* open(2) */
#include <fcntl.h>                                                /* open(2) */
#include <unistd.h>                                               /* read(2) */
#include <stdlib.h>                                             /* malloc(3) */
#include <alloca.h>  /* alloca(3) */
#include <string.h>                                           /* strerror(3) */
#include <errno.h>
#include "fmcMacro.h"     /* eExit(), likely(), unlikely(), printOutFuncOK() */
#include "fmcMsgUtils.h"                             /* mPrintf(), rPrintf() */
/* ######################################################################### */
static char rcsid[]="$Id: fmcFileUtils.c,v 1.5 2009/01/22 10:16:10 galli Exp galli $";
/* ######################################################################### */
void *realloc1(void *oldp,size_t size,int errU);
/* ######################################################################### */
/* Read the file at path filePath in the buffer buf of size bufSz filling    */
/* the buffer till bufFil (returned value).                                  */
/* If buf!=NULL and bufSz!=0 the supplied buffer is used if enough roomy.    */
/* Otherwise buf is reallocated.                                             */
/* If buf==NULL the buffer is allocated by the fmcFile2str.                  */
/* RETURN the filled buf size                                                */
/* ######################################################################### */
/* Example:                                                                  */
/* char *buf=NULL;                                                           */
/* size_t bufSz=0;                                                           */
/* size_t bufFil=0;                                                          */
/* bufFil=fmcFile2str(&buf,&bufSz,4096,"/proc/cpuinfo",L_STD,4);             */
/* bufFil=fmcFile2str(&buf,&bufSz,4096,"/proc/bus/pci/devices",L_STD,4);     */
/* ######################################################################### */
size_t fmcFile2str(char **buf, size_t *bufSz, size_t chunkSz, char *filePath,
                   int errU, int deBug)
{
  /*-------------------------------------------------------------------------*/
  /* buf:    buffer pointer                                                  */
  /* bufSz:  size of allocated buffer                                        */
  /* bufFil: size of filled part of the buffer                               */
  /*-------------------------------------------------------------------------*/
  /* Read maximum PROC_MAX_READ_CHARS bytes at a time */
  /* kernel 2.6.18 writes max 1024*3=3072 Bytes at a time */
  /*-------------------------------------------------------------------------*/
  char *chunk;                                      /* buffer for one read() */
  size_t bufFil=0;                      /* size of filled part of the buffer */
  int fd;
  size_t n;
  /*-------------------------------------------------------------------------*/
  /* make temporary buffer */
  chunk=alloca(chunkSz);
  /*-------------------------------------------------------------------------*/
  /* clean buffer */
  if(*buf)memset(*buf,0,*bufSz);
  /*-------------------------------------------------------------------------*/
  /* read the whole file into a memory buffer, allocating memory as we go    */
  /*-------------------------------------------------------------------------*/
  /* open file */
  fd=open(filePath,O_RDONLY,0);
  if(unlikely(fd==-1))
  {
    if(unlikely(deBug>0))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",
              filePath,strerror(errno));
    }
    return bufFil;
  }
  /*-------------------------------------------------------------------------*/
  /* position the file offet to the beginning of the file */
  if(lseek(fd,0L,SEEK_SET)<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot lseek file \"%s\": %s",
            filePath,strerror(errno));
    return bufFil;
  }
  /*-------------------------------------------------------------------------*/
  for(;;)
  {
    /*.......................................................................*/
    memset(chunk,0,chunkSz);                           /* clear chunk buffer */
    n=read(fd,chunk,chunkSz);                            /* I expect n<=3072 */
    if(unlikely(deBug>2))
    {
       mPrintf(errU,VERB,__func__,0,"File: \"%s\": read(): %zu Bytes chunk "
               "read.",filePath,n);
    }
    /*.......................................................................*/
    if(unlikely(n<0))                                          /* read error */
    {
      if(errno==EINTR)                            /* Interrupted system call */
      {
        if(unlikely(deBug>1))
        {
          mPrintf(errU,WARN,__func__,0,"read(%s): %s! Retrying...",filePath,
                  strerror(errno));
        }
        continue;                                                   /* retry */
      }
      else                                              /* severe read error */
      {
        if(unlikely(deBug>0))
        {
          mPrintf(errU,ERROR,__func__,0,"Error reading file \"%s\": %s!",
                  filePath,strerror(errno));
        }
        close(fd);
        bufFil=0;
        return bufFil;
      }                                              /* if severe read error */
    }                                                       /* if read error */
    /*.......................................................................*/
    if(!n)                                     /* 0 Bytes read the last time */
    {
      if(unlikely(unlikely(!*buf)||unlikely(!bufFil))) /* 0 Total Bytes read */
      {
        if(unlikely(deBug>0))
        {
           mPrintf(errU,WARN,__func__,0,"Zero Bytes read in file: \"%s\"!",
                   filePath);
        }
        close(fd);
        bufFil=0;
        return bufFil;
      }
      else                                                            /* EOF */
      {
        if(unlikely(deBug>2))
        {
          mPrintf(errU,WARN,__func__,0,"Last read character: (ASCII "
                  "%u).",(*buf)[bufFil-1]);
        }
        if((*buf)[bufFil-1]!=0)                          /* truncated string */
        {
          while(*bufSz<bufFil+1)
          {
            (*bufSz)+=chunkSz;
            *buf=realloc1(*buf,*bufSz,errU);  /* allocate more memory for \0 */
            if(unlikely(deBug>3))
            {
              mPrintf(errU,DEBUG,__func__,0,"realloc(%u)",*bufSz);
            }
          }
          (*buf)[bufFil]='\0';
          bufFil++;
          if(unlikely(deBug>1))
          {
            mPrintf(errU,INFO,__func__,0,"File \"%s\": Added missing NULL "
                    "terminator.",filePath);
          }
          if(unlikely(deBug>2))
          {
            mPrintf(errU,DEBUG,__func__,0,"File: \"%s\": Total %zu Bytes "
                    "read.",filePath,bufFil);
          }
        }                                                /* truncated string */
        break;
      }                                                               /* EOF */
    }                                    /* if zero Bytes read the last time */
    /*.......................................................................*/
    while(*bufSz<bufFil+n)
    {
      (*bufSz)+=chunkSz;
      *buf=realloc1(*buf,*bufSz,errU);               /* allocate more memory */
      if(unlikely(deBug>3))
      {
        mPrintf(errU,DEBUG,__func__,0,"realloc(%u)",*bufSz);
      }
    }
    memcpy(*buf+bufFil,chunk,n);                      /* copy buffer into it */
    bufFil+=n;                               /* increment total byte counter */
    if(unlikely(deBug>2))
    {
      mPrintf(errU,DEBUG,__func__,0,"File: \"%s\": Total %zu Bytes read.",
              filePath,bufFil);
    }
  }                                                               /* for(;;) */
  /*-------------------------------------------------------------------------*/
  close(fd);
  /*-------------------------------------------------------------------------*/
  return bufFil;
}
/*****************************************************************************/
void *realloc1(void *oldp,size_t size,int errU)
{
  void *p;
  /*-------------------------------------------------------------------------*/
  if(size==0)size++;
  p=realloc(oldp,size);
  if(!p)
  {
    mPrintf(errU,FATAL,__func__,0,"realloc1: realloc(%zu) failed: %s!",size,
            strerror(errno));
    exit(1);
  }
  return(p);
}
/*****************************************************************************/
char *getFmcFileUtilsVersion()
{
  return rcsid;
}
/* ######################################################################### */
