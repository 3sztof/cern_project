/* ######################################################################### */
/*
 * $Log: getCpuinfoFromProc.c,v $
 * Revision 1.11  2008/10/21 11:24:37  galli
 * get also parameters core_id & cpu_cores
 *
 * Revision 1.10  2008/10/20 10:15:55  galli
 * bug fixed
 *
 * Revision 1.7  2008/10/18 15:57:18  galli
 * uses fmcFile2str() from fmcFileUtils.h
 *
 * Revision 1.6  2008/10/09 09:53:06  galli
 * minor changes
 *
 * Revision 1.5  2008/01/31 23:37:18  galli
 * all cpuInfo struct elements are char* (to accomodate "N/A")
 *
 * Revision 1.4  2007/08/10 12:54:24  galli
 * added function getCpuinfoSensorVersion()
 * compatible with libFMCutils v 2
 *
 * Revision 1.2  2006/10/22 22:14:57  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.1  2004/10/17 10:01:32  galli
 * Initial revision
 */
/* ######################################################################### */
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
/*---------------------------------------------------------------------------*/
#include "getCpuinfoFromProc.h"                            /* struct cpuInfo */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#include "fmcFileUtils.h"                                   /* fmcFile2str() */
/* ######################################################################### */
static char rcsid[]="$Id: getCpuinfoFromProc.c,v 1.11 2008/10/21 11:24:37 galli Exp $";
/* ######################################################################### */
struct cpusInfo readCpuinfo(int deBug,int errU)
{
  static struct cpusInfo cpus;
  char *procCpuinfoBuf=NULL;
  size_t procCpuinfoBufSz=0;
  char *bp,*ep;
  int readBytes;
  unsigned pN=0;                                         /* processor number */
  unsigned rc=0;                                           /* record counter */
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  /* evaluate processors number */
  cpus.cpuN=sysconf(_SC_NPROCESSORS_ONLN);
  if(cpus.cpuN<1)cpus.cpuN=1;
  /*-------------------------------------------------------------------------*/
  /* allocate memory for array of structures */
  cpus.info=(struct cpuInfo*)malloc((size_t)cpus.cpuN*sizeof(struct cpuInfo));
  /*-------------------------------------------------------------------------*/
  /* clean & prepare output structures */
  cpus.success=0;
  cpus.version=rcsid;
  memset((char*)cpus.info,0,(size_t)cpus.cpuN*sizeof(struct cpuInfo));
  /*-------------------------------------------------------------------------*/
  /* read /proc/stat in procCpuinfoBuf */
  readBytes=fmcFile2str(&procCpuinfoBuf, &procCpuinfoBufSz, 4096,
                        CPU_INFO_FILE_NAME, L_DIM, deBug+1);
  /*-------------------------------------------------------------------------*/
  /* parse /proc/stat in buffer procCpuinfoBuf */
  for(bp=procCpuinfoBuf,rc=0;;rc++)       /* loop over /proc/cpuinfo records */
  {
    /* bp: record begin pointer */
    /* ep: record end pointer */
    /* sp: record separator pointer */
    char *sp;
    ep=strchr(bp,'\n');                                /* record end pointer */
    if(ep==NULL)break;
    *ep='\0';
    sp=strchr(bp,':');                               /* pointer to separator */
    /*-----------------------------------------------------------------------*/
    if(sp!=NULL)                                   /* if record contains ':' */
    {
      /*.....................................................................*/
      char *nbp,*nep,*vbp,*vep;
      nbp=bp;                                     /* item name begin pointer */
      nbp+=strspn(nbp," \t");                         /* skip heading blanks */
      nep=sp-1;                                     /* item name end pointer */
      while(*nep==' ' || *nep=='\t')nep--;          /* strip trailing blanks */
      nep++;
      if(!(nep>nbp))
      {
        mPrintf(errU,ERROR,__func__,0,"File: \"%s\": Item name error in "
                "record %d: \"%s\"!",CPU_INFO_FILE_NAME, rc+1, bp);
        if(procCpuinfoBuf)free(procCpuinfoBuf);
        return cpus;
      }
      *nep='\0';
      if(*(sp+1)=='\0')                                /* no chars after ':' */
      {
        mPrintf(errU,WARN,__func__,0,"File: \"%s\": Missing item value in "
                "record %d: \"%s\"!",CPU_INFO_FILE_NAME, rc+1, bp);
        vbp="N/A";
      }
      else                                    /* at least one char after ':' */
      {
        vbp=sp+1+strspn(sp+1," \t");             /* item value begin pointer */
        vep=ep;                                    /* item value end pointer */
        if(!(vep>vbp))
        {
          mPrintf(errU,ERROR,__func__,0,"File: \"%s\": Item value error in "
                  "record %d: \"%s\"!", CPU_INFO_FILE_NAME, rc+1, bp);
          if(procCpuinfoBuf)free(procCpuinfoBuf);
          return cpus;
        }
        *vep='\0';
      }
      /*.....................................................................*/
      if(!strcmp(nbp,"processor"))
      {
        pN=(int)strtol(vbp,(char**)NULL,0);
        if(pN>=cpus.cpuN)
        {
          mPrintf(errU,ERROR,__func__,0,"Unconsistency error: file \"%s\" "
                  "lists more CPUs than the number returned by sysconf()",
                  CPU_INFO_FILE_NAME);
          if(procCpuinfoBuf)free(procCpuinfoBuf);
          return cpus;
        }
      }
      else if(!strcmp(nbp,"vendor_id"))
        cpus.info[pN].vendor_id=strdup(vbp);
      else if(!strcmp(nbp,"cpu family"))
        cpus.info[pN].cpu_family=strdup(vbp);
      else if(!strcmp(nbp,"model"))
        cpus.info[pN].model=strdup(vbp);
      else if(!strcmp(nbp,"model name"))
        cpus.info[pN].model_name=strdup(vbp);
      else if(!strcmp(nbp,"stepping"))
        cpus.info[pN].stepping=strdup(vbp);
      else if(!strcmp(nbp,"cpu MHz"))
        cpus.info[pN].cpu_MHz=strdup(vbp);
      else if(!strcmp(nbp,"cache size"))
      {
        char *p;
        for(p=vbp;*p;p++)if(*p==' ')*p='\0';
        cpus.info[pN].kb_cache_size=strdup(vbp);
      }
      else if(!strcmp(nbp,"physical id"))
        cpus.info[pN].physical_id=strdup(vbp);
      else if(!strcmp(nbp,"siblings"))
        cpus.info[pN].siblings=strdup(vbp);
      else if(!strcmp(nbp,"core id"))
        cpus.info[pN].core_id=strdup(vbp);
      else if(!strcmp(nbp,"cpu cores"))
        cpus.info[pN].cpu_cores=strdup(vbp);
      else if(!strcmp(nbp,"bogomips"))
        cpus.info[pN].bogomips=strdup(vbp);
      /*.....................................................................*/
      if(!cpus.info[pN].vendor_id)cpus.info[pN].vendor_id="N/A";
      /*.....................................................................*/
    }                                              /* if record contains ':' */
    /*-----------------------------------------------------------------------*/
    bp=ep+1;                                    /* next record begin pointer */
  }                                       /* loop over /proc/cpuinfo records */
  /*-------------------------------------------------------------------------*/
  /* mark absent elements as not available */
  for(pN=0;pN<cpus.cpuN;pN++)
  {
    if(!cpus.info[pN].vendor_id)     cpus.info[pN].vendor_id="N/A";
    if(!cpus.info[pN].cpu_family)    cpus.info[pN].cpu_family="N/A";
    if(!cpus.info[pN].model)         cpus.info[pN].model="N/A";
    if(!cpus.info[pN].model_name)    cpus.info[pN].model_name="N/A";
    if(!cpus.info[pN].stepping)      cpus.info[pN].stepping="N/A";
    if(!cpus.info[pN].cpu_MHz)       cpus.info[pN].cpu_MHz="N/A";
    if(!cpus.info[pN].kb_cache_size) cpus.info[pN].kb_cache_size="N/A";
    if(!cpus.info[pN].physical_id)   cpus.info[pN].physical_id="N/A";
    if(!cpus.info[pN].siblings)      cpus.info[pN].siblings="N/A";
    if(!cpus.info[pN].core_id)       cpus.info[pN].core_id="N/A";
    if(!cpus.info[pN].cpu_cores)     cpus.info[pN].cpu_cores="N/A";
    if(!cpus.info[pN].bogomips)      cpus.info[pN].bogomips="N/A";
  }
  /*-------------------------------------------------------------------------*/
  cpus.success=1;
  /*-------------------------------------------------------------------------*/
  /* free the memory buffer allocated by fmcFile2str() */
  if(procCpuinfoBuf)free(procCpuinfoBuf);
  return cpus;
}
/*****************************************************************************/
/* getCpuinfoSensorVersion() - Returns the RCS identifier of this file.      */
/*****************************************************************************/
char *getCpuinfoSensorVersion()
{
  return rcsid;
}
/* ######################################################################### */
