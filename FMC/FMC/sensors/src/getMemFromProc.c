/*****************************************************************************/
/*
 * $Log: getMemFromProc.c,v $
 * Revision 2.1  2011/11/21 12:23:16  galli
 * Added field: WritebackTmp.
 *
 * Revision 2.0  2011/11/11 15:02:06  galli
 * Added fields: AnonPages, Bounce, HugePages_Rsvd, NFS_Unstable,
 * SReclaimable and SUnreclaim.
 *
 * Revision 1.11  2008/02/04 10:14:13  galli
 * bug fixed
 *
 * Revision 1.8  2008/02/03 22:21:33  galli
 * added parameters: CommitLimit, HugePages_Total, HugePages_Free,
 * Hugepagesize.
 *
 * Revision 1.7  2007/08/10 13:30:56  galli
 * added function getMemSensorVersion()
 * compatible with libFMCutils v 2
 *
 * Revision 1.6  2006/10/23 20:16:19  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.5  2005/11/18 09:48:19  galli
 * return data as array (instead of structure)
 * easier to add new items
 * added functions: getItemsN(), getItemsSz(), getItems()
 *
 * Revision 1.4  2005/11/11 10:23:36  galli
 * return value=~1UL if quantity not found
 *
 * Revision 1.3  2004/10/18 00:18:36  galli
 * Print errors on logger
 * memInfo() function added
 *
 * Revision 1.2  2004/08/10 12:58:00  galli
 * first working version
 *
 * Revision 1.1  2004/08/10 07:23:59  galli
 * Initial revision
 * */
/*****************************************************************************/
/* see also: procps-3.1.13/proc/sysinfo.c                                    */
/*****************************************************************************/
/* If new kernels includes new data, increase MEM_ITEM_N and add the new     */
/* items in memTbl                                                           */
/*****************************************************************************/
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <stdlib.h>                                              /* strtol() */
#include <errno.h>                                                  /* errno */
#include <string.h>                                  /* strerror(), strcmp() */
#include <ctype.h>                                              /* isspace() */
#include <sys/time.h>                                      /* gettimeofday() */
#include <syslog.h>                                              /* syslog() */
#include <sys/types.h>                                             /* open() */
#include <sys/stat.h>                                              /* open() */
#include <fcntl.h>                                                 /* open() */
#include <unistd.h>                                                /* read() */
/*****************************************************************************/
#include "getMemFromProc.h"                                 /* struct memVal */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
#define PROC_MEMINFO_SIZE 1024
#define MEM_ITEM_N 44    /* number of data to be read; included derived ones */
/*****************************************************************************/
/* typedefs */
typedef struct mem_table
{
  const char *name;                                      /* memory type name */
  unsigned long *slot;                              /* slot in return struct */
}mem_table_t;
/*****************************************************************************/
/* global variables */
static int memItemsSz=0;
static char *memItems=NULL;
static char rcsid[]="$Id: getMemFromProc.c,v 2.1 2011/11/21 12:23:16 galli Exp galli $";
/*****************************************************************************/
/* function prototype */
static int cmpMemTblStructs(const void *a,const void *b);
/*****************************************************************************/
  static unsigned long memData[MEM_ITEM_N];
  static const mem_table_t memTbl[]={
  {"Active",         &memData[0]},
  {"ActiveAnon",     &memData[1]},
  {"ActiveCache",    &memData[2]},
  {"AnonPages",      &memData[3]},
  {"Bounce",         &memData[4]},
  {"Buffers",        &memData[5]},
  {"Cached",         &memData[6]},
  {"CommitLimit",    &memData[7]},
  {"Committed_AS",   &memData[8]},
  {"Dirty",          &memData[9]},          /* kB version of vmstat nr_dirty */
  {"HighFree",       &memData[10]},
  {"HighTotal",      &memData[11]},
  {"HugePages_Free", &memData[12]},
  {"HugePages_Rsvd", &memData[13]},
  {"HugePages_Total",&memData[14]},
  {"Hugepagesize",   &memData[15]},
  {"Inact_clean",    &memData[16]},
  {"Inact_dirty",    &memData[17]},
  {"Inact_laundry",  &memData[18]},
  {"Inact_target",   &memData[19]}, 
  {"Inactive",       &memData[20]},
  {"LowFree",        &memData[21]},
  {"LowTotal",       &memData[22]},
  {"LowUsed",        &memData[23]},
  {"Mapped",         &memData[24]},        /* kB version of vmstat nr_mapped */
  {"MemFree",        &memData[25]},
  {"MemShared",      &memData[26]},
  {"MemTotal",       &memData[27]},
  {"MemUsed",        &memData[28]},
  {"NFS_Unstable",   &memData[29]},
  {"PageTables",     &memData[30]},       /* s.a. vmstat nr_page_table_pages */
  {"ReverseMaps",    &memData[31]},       /* s.a. vmstat nr_page_table_pages */
  {"SReclaimable",   &memData[32]},
  {"SUnreclaim",     &memData[33]},
  {"Slab",           &memData[34]},          /* kB version of vmstat nr_slab */
  {"SwapCached",     &memData[35]},
  {"SwapFree",       &memData[36]},
  {"SwapTotal",      &memData[37]},
  {"SwapUsed",       &memData[38]},
  {"VmallocChunk",   &memData[39]},
  {"VmallocTotal",   &memData[40]},
  {"VmallocUsed",    &memData[41]},
  {"Writeback",      &memData[42]},     /* kB version of vmstat nr_writeback */
  {"WritebackTmp",   &memData[43]},     /* kB version of vmstat nr_writeback */
  };
  const int memTblCnt=sizeof(memTbl)/sizeof(mem_table_t);
/*****************************************************************************/
struct memVal readMem(int deBug,int errU)
{
  static struct memVal mems={0,rcsid,memData};
  static int isFirstCall=1;
  char memFileName[]="/proc/meminfo";
  int memFileFD;
  char buf[PROC_MEMINFO_SIZE];
  int readBytes;
  char namebuf[16];                       /* big enough to hold any row name */
  mem_table_t findme={namebuf,NULL};
  char *head;
  char *tail;
  mem_table_t *found;
  int i=0;
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  if(isFirstCall)
  {
    isFirstCall=0;
    /* fill memItemsSz and memItems */
    for(i=0;i<MEM_ITEM_N;i++)memItemsSz+=(1+strlen(memTbl[i].name));
    memItems=(char*)malloc(memItemsSz*sizeof(char));
    for(i=0,p=memItems;i<MEM_ITEM_N;i++)
    {
      strcpy(p,memTbl[i].name);
      p+=1+strlen(memTbl[i].name);
    }
  }
  /*-------------------------------------------------------------------------*/
  /* initialize mem.data */
  for(i=0;i<MEM_ITEM_N;i++)mems.data[i]=~1UL;
  /*-------------------------------------------------------------------------*/
  memFileFD=open(memFileName,O_RDONLY);
  if(memFileFD==-1)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s",memFileName,
            strerror(errno));
    return mems;
  }
  if(lseek(memFileFD,0L,SEEK_SET)<0)
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot lseek file \"%s\": %s",memFileName,
            strerror(errno));
    return mems;
  }
  if((readBytes=read(memFileFD,buf,sizeof(buf)-1))<0) 
  {
    mPrintf(errU,ERROR,__func__,0,"Cannot read file \"%s\": %s",memFileName,
            strerror(errno));
    return mems;
  }
  close(memFileFD);
  /*-------------------------------------------------------------------------*/
  buf[readBytes]='\0';
  if(deBug)mPrintf(errU,DEBUG,__func__,0,"buf=\"%s\".",buf);
  head=buf;
  for(;;)
  {
    tail=strchr(head,':');
    if(!tail)break;
    *tail='\0';
    if(strlen(head)>=sizeof(namebuf))
    {
      /* next line */
      head=tail+1;
      tail=strchr(head,'\n');
      if(!tail)break;
      head=tail+1;
      continue;
    }
    strcpy(namebuf,head);
    found=bsearch(&findme,memTbl,memTblCnt,sizeof(mem_table_t),
                  cmpMemTblStructs);
    head=tail+1;
    if(!found)
    {
      /* next line */
      tail=strchr(head,'\n');
      if(!tail)break;
      head=tail+1;
      continue;
    }
    *(found->slot)=strtoul(head,&tail,10);
    tail=strchr(head,'\n');
    if(!tail)break;
    head=tail+1;
  }
  /*-------------------------------------------------------------------------*/
  /* derived values */
  if(*vp("LowTotal")==~1UL || *vp("LowTotal")==0)
  {
    /* low==main except with large-memory support */
    *vp("LowTotal")=*vp("MemTotal");
    *vp("LowFree")=*vp("MemFree");
  }
  if(*vp("Inactive")==~1UL)
  {
    *vp("Inactive")=*vp("Inact_dirty")+*vp("Inact_clean")+*vp("Inact_laundry");
  }
  *vp("SwapUsed")=*vp("SwapTotal")-*vp("SwapFree");
  *vp("MemUsed")=*vp("MemTotal")-*vp("MemFree");
  *vp("LowUsed")=*vp("LowTotal")-*vp("LowFree");
  /*-------------------------------------------------------------------------*/
  mems.success=1;
  /*-------------------------------------------------------------------------*/
  return mems;
}
/*****************************************************************************/
unsigned long *vp(char *key)
{
  mem_table_t findme={key,NULL};
  mem_table_t *found;
  /*-------------------------------------------------------------------------*/
  found=bsearch(&findme,memTbl,memTblCnt,sizeof(mem_table_t),cmpMemTblStructs);
  if(!found)return NULL;
  else return found->slot;
}
/*****************************************************************************/
int getItemsN(void)
{
  return MEM_ITEM_N;
}
/*****************************************************************************/
int getItemsSz(void)
{
  return memItemsSz;
}
/*****************************************************************************/
char *getItems(void)
{
  return memItems;
}
/*****************************************************************************/
static int cmpMemTblStructs(const void *a, const void *b)
{
  return strcmp(((const mem_table_t*)a)->name,
                ((const mem_table_t*)b)->name);
}
/*****************************************************************************/
/* getMemSensorVersion() - Returns the RCS identifier of this file.           */
/*****************************************************************************/
char *getMemSensorVersion()
{
  return rcsid;
}
/*****************************************************************************/
