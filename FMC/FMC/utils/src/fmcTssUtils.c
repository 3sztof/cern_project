/* ######################################################################### */
/*
 * $Log: fmcTssUtils.c,v $
 * Revision 1.6  2009/01/29 08:59:05  galli
 * minor changes
 *
 * Revision 1.3  2004/11/16 13:19:27  galli
 * round at 1e-3 s
 *
 * Revision 1.2  2004/11/16 11:32:06  galli
 * bug fixed
 *
 * Revision 1.1  2004/11/16 11:30:30  galli
 * Initial revision
 */
/* ######################################################################### */
#include <stdio.h>                                             /* snprintf() */
#include <math.h>                                                 /* floor() */
#include <string.h>                                              /* memset() */
#include "fmcTssUtils.h"
/* ######################################################################### */
static char rcsid[]="$Id: fmcTssUtils.c,v 1.6 2009/01/29 08:59:05 galli Exp galli $";
/* ######################################################################### */
char *getTssString(float tss)
{
  static char tssString[TSS_LEN+1];
  unsigned dd=0,hh=0,mm=0,ss=0,sss=0;
  float fractions=0.0,fss=0.0;
  /*-------------------------------------------------------------------------*/
  memset(tssString,0,sizeof(tssString));
  tss=rint(1e3*tss)/1e3;                                  /* round at 1e-3 s */
  sss=(int)floor(tss);                           /* seconds since last reset */
  fractions=tss-(float)sss;                           /* fraction of seconds */
  ss=sss%60;
  fss=(float)ss+fractions;
  sss/=60;
  mm=sss%60;
  sss/=60;
  hh=sss%24;
  sss/=24;
  dd=sss;
  snprintf(tssString,TSS_LEN+1,"%03ud, %02uh, %02um, %06.3fs",dd,hh,mm,fss);
  if(tssString[TSS_LEN])tssString[TSS_LEN]='\0';
  return tssString;
}
/*****************************************************************************/
char *getFmcTssUtilsVersion()
{
  return rcsid;
}
/* ######################################################################### */
