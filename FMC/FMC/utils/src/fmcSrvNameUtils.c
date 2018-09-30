/* ######################################################################### */
/*
 * $Log: fmcSrvNameUtils.c,v $
 * Revision 1.6  2008/10/23 07:50:06  galli
 * minor changes
 *
 * Revision 1.4  2008/08/29 11:07:40  galli
 * use SVC_HEAD from fmcVersion.h
 */
/* ######################################################################### */
#include <stdio.h>                                              /* sprintf() */
#include <unistd.h>                                         /* gethostname() */
#include <ctype.h>                                              /* toupper() */
#include <errno.h>                                                  /* errno */
#include <string.h>                                            /* strerror() */
#include <syslog.h>                                              /* syslog() */
#define MSG_SIZE 256
/*****************************************************************************/
#include "fmcSrvNameUtils.h"
#include "fmcVersion.h"                                       /* FMC version */
/* ######################################################################### */
static char rcsid[]="$Id: fmcSrvNameUtils.c,v 1.6 2008/10/23 07:50:06 galli Exp $";
/* ######################################################################### */
char *getSrvPrefix(void)
{
  char hostName[80];
  char *p;
  char msg[MSG_SIZE];
  const char *fName="getSrvPrefix()";
  char srvPrefix[80];
  /*-------------------------------------------------------------------------*/
  /* get hostname */
  if(!gethostname(hostName,80)<0)
  {
    snprintf(msg,MSG_SIZE,"FATAL> %s: gethostname(): %s.\n",fName,
             strerror(errno));
    syslog(LOG_ERR|LOG_DAEMON,msg);
    return "/";
  }
  /*-------------------------------------------------------------------------*/
  /* strip domain */
  p=strchr(hostName,'.');
  if(p)*p='\0';
  /* convert to upper case */
  for(p=hostName;*p;p++)*p=toupper(*p);
  /*-------------------------------------------------------------------------*/
  snprintf(srvPrefix,80,"%s/%s",SVC_HEAD,hostName);
  return strdup(srvPrefix);
}
/*****************************************************************************/
char *oldGetSrvPrefix(void)
{
  char hostName[80];
  char *p;
  char msg[MSG_SIZE];
  const char *fName="getSrvPrefix()";
  char srvPrefix[80];
  /*-------------------------------------------------------------------------*/
  /* get hostname */
  if(!gethostname(hostName,80)<0)
  {
    snprintf(msg,MSG_SIZE,"FATAL> %s: gethostname(): %s.\n",fName,
             strerror(errno));
    syslog(LOG_ERR|LOG_DAEMON,msg);
    return "/";
  }
  /*-------------------------------------------------------------------------*/
  /* strip domain */
  p=strchr(hostName,'.');
  if(p)*p='\0';
  /* convert to upper case */
  for(p=hostName;*p;p++)*p=toupper(*p);
  /*-------------------------------------------------------------------------*/
  snprintf(srvPrefix,80,"/%s",hostName);
  return strdup(srvPrefix);
}
/*****************************************************************************/
char *getFmcSrvNameUtilsVersion()
{
  return rcsid;
}
/* ######################################################################### */
