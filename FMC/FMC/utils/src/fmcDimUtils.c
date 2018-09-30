/* ######################################################################### */
/*
 * $Log: fmcDimUtils.c,v $
 * Revision 1.6  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.5  2008/10/23 07:44:28  galli
 * minor changes
 *
 * Revision 1.1  2007/09/20 07:25:01  galli
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
#include <string.h>                                           /* strerror(3) */
#include <errno.h>
#include "fmcMsgUtils.h"                             /* mPrintf(), rPrintf() */
/* ######################################################################### */
static char rcsid[]="$Id: fmcDimUtils.c,v 1.6 2012/11/29 15:52:27 galli Exp galli $";
/* ######################################################################### */
char *getDimDnsNode(const char *dimCfgFileName,int errU,int deBug)
{
  struct stat dimCfgFileStat;
  size_t dimCfgBufLen=0;
  ssize_t dimCfgStrLen=0;
  char *dimCfgStr=NULL;
  int dimCfgFD=-1;
  char *dimDnsName=NULL;
  ssize_t dimDnsNameLen=0;
  char *p,*rp,*erp;
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,DEBUG,__func__,0,"Reading DIM configuration file: \"%s\"...",
          dimCfgFileName);
  /* check configuration file accessibility */
  if(access(dimCfgFileName,R_OK)==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Can't access file: \"%s\". access(2) "
            "error: %s!",dimCfgFileName,strerror(errno));
    return NULL;
  }
  /* get configuration file size */
  if(lstat(dimCfgFileName,&dimCfgFileStat)<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Can't get info about file: \"%s\". "
            "lstat(2) error: %s!",dimCfgFileName,strerror(errno));
    return NULL;
  }
  if(deBug)mPrintf(errU,VERB,__func__,0,"Size of file \"%s\": %d B.",
                   dimCfgFileName,(int)dimCfgFileStat.st_size);
  if(!dimCfgFileStat.st_size)
  {
    mPrintf(errU,ERROR,__func__,0,"Empty DIM configuration file: \"%s\"!",
            dimCfgFileName);
    return NULL;
  }
  /* allocate buffer to store confFile */
  dimCfgBufLen=dimCfgFileStat.st_size*1.1;
  dimCfgStr=(char*)malloc(dimCfgBufLen*sizeof(char));
  /* read confFile */
  dimCfgFD=open(dimCfgFileName,O_RDONLY);
  dimCfgStrLen=read(dimCfgFD,dimCfgStr,dimCfgBufLen);
  close(dimCfgFD);
  if(dimCfgStrLen<dimCfgFileStat.st_size)
  {
    mPrintf(errU,ERROR,__func__,0,"Problem reading configuration file \"%s\": "
            "%d B read out of %d B!",dimCfgFileName,dimCfgStrLen,
            dimCfgFileStat.st_size);
  }
  /* check if file is empty */
  if(!dimCfgStr||!dimCfgStr[0]||!dimCfgStrLen)
  {
    mPrintf(errU,ERROR,__func__,0,"Empty DIM configuration retrieved from file "
            "\"%s\"",dimCfgFileName);
    return NULL;
  }
  /* convert line-feed and carriage-return */
  for(p=dimCfgStr;*p;p++)if(*p=='\r')*p=' ';
  for(p=dimCfgStr;*p;p++)if(*p=='\n')*p=';';
  if(deBug)mPrintf(errU,VERB,__func__,0,"DIM configuration retrieved from "
                   "file \"%s\": \"%s\".",dimCfgFileName,dimCfgStr);
  /* loop over configuration file records */
  for(rp=dimCfgStr;;)
  {
    erp=strchr(rp,';');
    if(erp)*erp='\0';
    p=rp;
    p+=strspn(p," \t");                                       /* skip blanks */
    if(!*p)                                            /* skip empty records */
    {
      if(deBug)mPrintf(errU,VERB,__func__,0,"Empty record in file \"%s\" "
                       "skipped.",dimCfgFileName);
      goto nextRecord;
    }
    if(*p=='#')                                      /* skip comment records */
    {
      if(deBug)mPrintf(errU,VERB,__func__,0,"Comment record \"%s\" in file "
                       "\"%s\" skipped.",rp,dimCfgFileName);
      goto nextRecord;
    }
    p=strstr(p,"DIM_DNS_NODE");
    if(!p)                                  /* string DIM_DNS_NODE not found */
    {
      if(deBug)mPrintf(errU,VERB,__func__,0,"String \"DIM_DNS_NODE\" not "
                       "found in record \"%s\".",rp);
      goto nextRecord;
    }
    p+=strlen("DIM_DNS_NODE");
    if(!p)                              /* nothing after string DIM_DNS_NODE */
    {
      mPrintf(errU,ERROR,__func__,0,"Format error in record \"%s\"!",rp);
      goto nextRecord;
    }
    p+=strspn(p," \t");                                       /* skip blanks */
    if(!p||*p!='=')                             /* no '=' after DIM_DNS_NODE */
    {
      mPrintf(errU,ERROR,__func__,0,"Format error in record \"%s\"!",rp);
      goto nextRecord;
    }
    p++;
    if(!p)                                              /* nothing after '=' */
    {
      mPrintf(errU,ERROR,__func__,0,"Format error in record \"%s\"!",rp);
      goto nextRecord;
    }
    p+=strspn(p," \t");                                       /* skip blanks */
    dimDnsNameLen=strcspn(p," \t\n\r;,:");
    if(!dimDnsNameLen)                          /* nothing after '=<spaces>' */ 
    {
      mPrintf(errU,ERROR,__func__,0,"Format error in record \"%s\"!",rp);
      goto nextRecord;
    }
    dimDnsName=(char*)malloc(dimDnsNameLen+1);
    snprintf(dimDnsName,dimDnsNameLen+1,"%s",p);
    break;
    nextRecord:
    if(erp)rp=erp+1;
    else break;
  }
  if(!dimDnsName)
  {
    mPrintf(errU,ERROR,__func__,0,"No DIM_DNS_NODE defined in file \"%s\"!",
    dimCfgFileName);
  }
  else
  {
    mPrintf(errU,DEBUG,__func__,0,"Got DIM_DNS_NODE \"%s\" defined in DIM "
            "configuration file \"%s\"!",dimDnsName,dimCfgFileName);
  }
  if(dimCfgStr)free(dimCfgStr);
  return dimDnsName;
}
/*****************************************************************************/
char *getFmcDimUtilsVersion()
{
  return rcsid;
}
/* ######################################################################### */
