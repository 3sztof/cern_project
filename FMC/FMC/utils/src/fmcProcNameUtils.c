/* ######################################################################### */
/*
 * $Log: fmcProcNameUtils.c,v $
 * Revision 1.5  2009/01/29 11:11:33  galli
 * minor changes
 *
 * Revision 1.2  2008/02/22 08:51:39  galli
 * bug fixed
 *
 * Revision 1.1  2008/02/22 08:26:12  galli
 * Initial revision
 */
/* ######################################################################### */
#include <stdio.h>                                    /* snprintf(), fopen() */
#include <unistd.h>                                           /* readlink(2) */
#include <errno.h>
#include <string.h>                                           /* strerror(3) */
#include <libgen.h>                                           /* basename(3) */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#include "fmcProcNameUtils.h"
/* ######################################################################### */
static char rcsid[]="$Id: fmcProcNameUtils.c,v 1.5 2009/01/29 11:11:33 galli Exp galli $";
/* ######################################################################### */
/* get the name of the current executable file */
char *getProcName(int errU)
{
  static char pathName[PATH_BUF_LEN+1]="";
  int len=0;
  /*-------------------------------------------------------------------------*/
  len=readlink("/proc/self/exe",pathName,sizeof(pathName));
  if(len==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"readlink(): %s.",strerror(errno));
    return pathName;
  }
  if(len<PATH_BUF_LEN)pathName[len]='\0';
  else pathName[PATH_BUF_LEN]='\0';
  return basename(pathName);
}
/*****************************************************************************/
char *getFmcProcNameUtilsVersion()
{
  return rcsid;
}
/* ######################################################################### */

