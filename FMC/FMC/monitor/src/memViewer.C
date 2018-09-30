/* ************************************************************************* */
/*
 * $Log: memViewer.C,v $
 * Revision 3.2  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 3.1  2011/11/21 14:57:50  galli
 * usage() and shortUsage() rewritten
 *
 * Revision 3.0  2011/11/17 16:15:56  galli
 * Compatible with memSrv v3.0
 *
 * Revision 1.9  2008/10/22 12:28:00  galli
 * uses FmcUtils::printServerVersion()
 *
 * Revision 1.7  2008/02/28 13:45:12  galli
 * #define FLOAT_DATA and #define UINT32_DATA for PVSS compatibility
 *
 * Revision 1.6  2008/02/21 10:17:40  galli
 * uint64_t instead of unsigned long
 *
 * Revision 1.5  2008/02/04 13:47:41  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.3  2007/12/19 16:11:31  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.2  2005/11/18 09:45:34  galli
 * comaptible with memSrv.c 2.5
 *
 * Revision 1.1  2005/11/14 13:50:21  galli
 * Initial revision
 */
/* ************************************************************************* */
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>                                            /* uint64_t */
#include <math.h>                                                  /* fabs() */
#include <values.h>                                               /* FLT_MIN */
#include <getopt.h>                                         /* getopt_long() */
#include <sys/wait.h>                                  /* wait(2) in usage() */
/* ------------------------------------------------------------------------- */
/* DIM */
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                               /* printServerVersion() */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/* ------------------------------------------------------------------------- */
/* in monitor/include */
#define _CLIENT 1
#include "memSrv.h"                             /* SRV_NAME, DET_FIELD_N_ERR */ 
/* ------------------------------------------------------------------------- */
/* MACRO */
#define DATUM_PUBLISHED(isOldServer,val) \
(((isOldServer) && ((val)!=0xfffffffeUL)) || \
(!(isOldServer) && ((val)!=0xfffffffffffffffeULL)))
/* ************************************************************************* */
/* globals */
int deBug=0;
static char rcsid[]="$Id: memViewer.C,v 3.2 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
/* ************************************************************************* */
/* function prototype */
void usage(int mode);
void shortUsage(void);
void getServerVersion(vector<string>hostPttnLs);
/* ************************************************************************* */
int main(int argc,char **argv)
{
  /* counters */
  int i,j,sc1,sc2;
  unsigned k;
  size_t b,e;
  /* pointers */
  char *p,*lp,*up;
  /* service browsing */
  vector<string> hostPttnLs;                          /* host pattern vector */
  vector<string>::iterator hpIt;             /* host pattern vector iterator */
  string hostPttn;
  string successRawSvcPttn;
  string successSvcPttn;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  string successSvc;
  string(nodeFound);
  int nodeFoundN=0;
  string svc;
  char *dimDnsNode=NULL;
  /* no link replacement */
  int nolinkDetDataSz=0;
  int nolinkOldDetDataSz=0;
  uint64_t *nolinkDetData=NULL;
  uint32_t *nolinkOldDetData=NULL;
  int nolinkDetStrArrSz=0;
  char *nolinkDetStrArr=NULL;
  /* retrieved quantities */
  int success=-1;
  string versionStr,version;
  bool isOldServer=false;
  int labelsSz=0;
  char *labels=NULL;
  int unitsSz=0;
  char *units=NULL;
  int dataSz=0;
  vector<uint64_t>data;
  /* printing variables */
  unsigned fieldN;
  unsigned fieldLen[3]={0,0,0};
  unsigned len;
  char *line=NULL;
  int lineLen=0;
  char pFormat[256];
  /* command line switch */
  int doPrintServerVersion=0;
  int compactFormat=0;
  /* getopt */
  int flag;
  /* short command line options */
  const char *shortOptions="vN:m:cVh::";
  /* long command line options */
  static struct option longOptions[]=
  {
    /* ..................................................................... */
    /* standard options */
    {"debug",no_argument,NULL,'v'},
    {"dim-dns",required_argument,NULL,'N'},
    {"hostname",required_argument,NULL,'m'},
    {"compact",no_argument,NULL,'c'},
    /* ..................................................................... */
    /* accessory options */
    {"version",no_argument,NULL,'V'},
    {"help",no_argument,NULL,'h'},
    /* ..................................................................... */
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /* ----------------------------------------------------------------------- */
  /* process command-line options */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt_long(argc,argv,shortOptions,longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      /* ................................................................... */
      /* standard options */
      case 'v': /* -v, --debug */
        deBug++;
        break;
      case 'N': /* -N, --dim-dns */
        dimDnsNode=optarg;
        break;
      case 'm': /* -m, --hostname */
        hostPttnLs.push_back(optarg);
        break;
      case 'c': /* -c, --compact */
        compactFormat=1;
        break;
      /* ................................................................... */
      /* accessory options */
      case 'V': /* -V, --version */
        doPrintServerVersion=1;
        break;
      case 'h': /* -h, -hh, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
        break;
      /* ................................................................... */
      default:
        fprintf(stderr,"\ngetopt_long(): Invalid option \"%s\"!\n",
                argv[optind-1]);
        shortUsage();
        break;
      /* ................................................................... */
    }
  }
  /* ----------------------------------------------------------------------- */
  /* default wildcard for hostPttnLs and logPttnLs */
  if(hostPttnLs.empty())hostPttnLs.push_back("*");
  /* ----------------------------------------------------------------------- */
  /* check hostPttnLs */
  for(hpIt=hostPttnLs.begin();hpIt!=hostPttnLs.end();hpIt++)
  {
    if(hpIt->find_first_of('/')!=string::npos)
    {
      fprintf(stderr,"Illegal NODE_PATTERN "
              "\"%s\"! A NODE_PATTERN specified with the command-line option "
              "\"-m NODE_PATTERN\" or \"--hostname NODE_PATTERN\" must not "
              "contain the '/' character!\n",hpIt->c_str());
      shortUsage();
    }
  }
  /* ----------------------------------------------------------------------- */
  /* check command line non-option argument(s) */
  if(optind<argc)
  {
    i=optind;
    string msg="Unrequired non-option argument(s): ";
    for(i=optind;i<argc;i++)
    {
      if(i!=optind)msg+=", ";
      msg+="\""+string(argv[i])+"\"";
    }
    msg+="!";
    fprintf(stderr,"\n%s\n",msg.c_str());
    shortUsage();
  }
  if(argc-optind!=0)shortUsage();
  /* ----------------------------------------------------------------------- */
  /* set the DIM DNS node */
  printf("\n");
  if(dimDnsNode)
  {
    printf("DIM_DNS_NODE: \"%s\" (from -N command-line option).\n",
           dimDnsNode);
  }
  else
  {
    dimDnsNode=getenv("DIM_DNS_NODE");
    if(dimDnsNode)
    {
      printf("DIM_DNS_NODE: \"%s\" (from DIM_DNS_NODE environment variable).\n",
             dimDnsNode);
    }
    else
    {
      dimDnsNode=getDimDnsNode(DIM_CONF_FILE_NAME,0,1);
      if(dimDnsNode)
      {
        printf("DIM_DNS_NODE: \"%s\" (from \""DIM_CONF_FILE_NAME" file).\n",
               dimDnsNode);
      }
      else
      {
        fprintf(stderr,"\nERROR: DIM Name Server (DIM_DNS_NODE) not defined!\n"
                "DIM Name Server can be defined (in decreasing order of "
                "priority):\n"
                "  1. Specifying the -N DIM_DNS_NODE command-line option;\n"
                "  2. Specifying the DIM_DNS_NODE environment variable;\n"
                "  3. Defining the DIM_DNS_NODE in the file "
                "\""DIM_CONF_FILE_NAME"\".\n");
        exit(1);
      }
    }
  }
  if(setenv("DIM_DNS_NODE",dimDnsNode,1))
  {
    fprintf(stderr,"setenv(): %s!\n",strerror(errno));
    exit(1);
  }
  /* ----------------------------------------------------------------------- */
  if(doPrintServerVersion) getServerVersion(hostPttnLs);
  /* ----------------------------------------------------------------------- */
  if(deBug)compactFormat=0;
  /* ----------------------------------------------------------------------- */
  /* Compose DIM no link error arrays */
  /* old detailed data */
  nolinkOldDetDataSz=DET_FIELD_N_ERR*sizeof(uint32_t);
  nolinkOldDetData=(uint32_t*)malloc(nolinkOldDetDataSz);
  for(i=0;i<DET_FIELD_N_ERR;i++)nolinkOldDetData[i]=0xffffffffUL;
  /* new detailed data */
  nolinkDetDataSz=DET_FIELD_N_ERR*sizeof(uint64_t);
  nolinkDetData=(uint64_t*)malloc(nolinkDetDataSz);
  for(i=0;i<DET_FIELD_N_ERR;i++)nolinkDetData[i]=0xffffffffffffffffULL;
  /* detailed label/units */
  nolinkDetStrArrSz=DET_FIELD_N_ERR*(1+strlen("N/F"));    /* "N/F"=not found */
  nolinkDetStrArr=(char*)malloc(nolinkDetStrArrSz*sizeof(char));
  for(i=0,p=nolinkDetStrArr;i<DET_FIELD_N_ERR;i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
  /* ----------------------------------------------------------------------- */
  if(!compactFormat)
  {
    printf("Node patterns list:  ");
    for(hpIt=hostPttnLs.begin();hpIt<hostPttnLs.end();hpIt++)
    {
      printf("\"%s\"",hpIt->c_str());
      if(*hpIt!=hostPttnLs.back())printf(", ");
    }
    printf(".\n");
  }
  /* ----------------------------------------------------------------------- */
  /* loop over node patterns */
  for(hpIt=hostPttnLs.begin(),j=0;hpIt<hostPttnLs.end();hpIt++)
  {
    hostPttn=*hpIt;
    if(!compactFormat)printf("  Node pattern: \"%s\":\n",hostPttn.c_str());
    hostPttn=FmcUtils::toUpper(hostPttn);
    successRawSvcPttn=string("/FMC/*/")+SRV_NAME+"/success";
    successSvcPttn=string("/FMC/")+hostPttn+"/"+SRV_NAME+"/success";
    if(deBug)printf("  Service pattern: \"%s\"\n",successSvcPttn.c_str());
    /* ..................................................................... */
    /* count the number of the slashes */
    for(k=0,sc1=0;k<successSvcPttn.size();k++)
    {
      if(successSvcPttn.at(k)=='/')sc1++;
    }
    /* ..................................................................... */
    br.getServices(successRawSvcPttn.c_str());
    nodeFoundN=0;
    while((type=br.getNextService(p,format))!=0)          /* loop over nodes */
    {
      if(!fnmatch(successSvcPttn.c_str(),p,0))
      {
        successSvc=p;
        /* ................................................................. */
        /* count the number of the slashes */
        for(k=0,sc2=0;k<successSvc.size();k++)
        {
          if(successSvc.at(k)=='/')sc2++;
        }
        if(sc2!=sc1)continue;
        /* ................................................................. */
        /* extract node hostname from "success" service name */
        e=successSvc.find_last_of('/');                        /* last slash */
        e=successSvc.find_last_of('/',e-1);            /* last-but-one slash */
        for(k=0;k<8;k++)e=successSvc.find_last_of('/',e-1);
        b=successSvc.find_last_of('/',e-1)+1;
        nodeFound=successSvc.substr(b,e-b);
        nodeFound=FmcUtils::toLower(nodeFound);
        /* ................................................................. */
        nodeFoundN++;
        /* ................................................................. */
        /* read success */
        if(deBug)
        {
          printf("  %3d Contacting Service: \"%s\"...\n",j++,
                 successSvc.c_str());
        }
        DimCurrentInfo Success(successSvc.c_str(),-1);
        success=(int)Success.getInt();
        if(success==-1)
        {
          fprintf(stderr,"      Service \"%s\" unreachable!\n",
                  successSvc.c_str());
          continue;
        }
        else if(success==0)
        {
          fprintf(stderr,"Service \"%s\": data not ready!\n",
                  successSvc.c_str());
          continue;
        }
        /* ................................................................. */
        /* read server version */
        svc=string("/FMC/")+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
            "/server_version";
        if(deBug)
        {
          printf("  %3d Contacting Service: \"%s\"...\n",j++,svc.c_str());
        }
        DimCurrentInfo Version(svc.c_str(),(char*)"N/A");
        versionStr=string((char*)Version.getData());
        b=versionStr.find_first_of(' ')+1;
        e=versionStr.find_first_of(' ',b);
        version=versionStr.substr(b,e-b);
        if(strverscmp(version.c_str(),"3.0")>=0)isOldServer=false;
        else isOldServer=true;
        /* ................................................................. */
        /* read labels */
        svc=string("/FMC/")+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
            "/details/labels";
        if(deBug)
        {
          printf("  %3d Contacting Service: \"%s\"...\n",j++,svc.c_str());
        }
        DimCurrentInfo Labels(svc.c_str(),nolinkDetStrArr,nolinkDetStrArrSz);
        labelsSz=(int)Labels.getSize();
        labels=(char*)Labels.getData();
        /* ................................................................. */
        /* read units */
        svc=string("/FMC/")+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
            "/details/units";
        if(deBug)
        {
          printf("  %3d Contacting Service: \"%s\"...\n",j++,svc.c_str());
        }
        DimCurrentInfo Units(svc.c_str(),nolinkDetStrArr,nolinkDetStrArrSz);
        unitsSz=(int)Units.getSize();
        units=(char*)Units.getData();
        /* ................................................................. */
        /* read data */
        svc=string("/FMC/")+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
            "/details/data";
        if(deBug)
        {
          printf("  %3d Contacting Service: \"%s\"...\n",j++,svc.c_str());
        }
        if(isOldServer)
        {
          uint32_t *dataVl;
          DimCurrentInfo Data(svc.c_str(),nolinkOldDetData,nolinkOldDetDataSz);
          dataSz=(int)Data.getSize();
          dataVl=(uint32_t*)Data.getData();
          data.assign(dataVl,dataVl+dataSz);
        }
        else
        {
          uint64_t *dataVl;
          DimCurrentInfo Data(svc.c_str(),nolinkDetData,nolinkDetDataSz);
          dataSz=(int)Data.getSize();
          dataVl=(uint64_t*)Data.getData();
          data.assign(dataVl,dataVl+dataSz);
        }
        /* ................................................................. */
        /* evaluate field number */
        for(fieldN=0,lp=labels;lp<labels+labelsSz;lp+=1+strlen(lp))fieldN++;
        /* ................................................................. */
        /* print data */
        if(!compactFormat)
        {
          /* ............................................................... */
          /* evaluate fields size */
          /* labels */
          for(k=0,lp=labels;k<fieldN;lp+=1+strlen(lp),k++)
          {
            /* datum is not N/A for the node kernel */
            if(DATUM_PUBLISHED(isOldServer,data[k]))
            {
              len=strlen(lp);
              if(fieldLen[0]<len)fieldLen[0]=len;
            }
          }
          /* units */
          for(k=0,up=units;k<fieldN;up+=1+strlen(up),k++)
          {
            /* datum is not N/A for the node kernel */
            if(DATUM_PUBLISHED(isOldServer,data[k]))
            {
              len=strlen(up);
              if(fieldLen[1]<len)fieldLen[1]=len;
            }
          }
          /* data */
          for(k=0;k<fieldN;k++)
          {
            /* datum is not N/A for the node kernel */
            if(DATUM_PUBLISHED(isOldServer,data[k]))
            {
              len=snprintf(NULL,0,"%"PRIu64,data[k]);
              if(fieldLen[2]<len)fieldLen[2]=len;
            }
          }
          /* ............................................................... */
          /* evaluate line separator length */
          lineLen=0;
          for(k=0;k<3;k++)lineLen+=1+fieldLen[k];
          lineLen--;
          line=(char*)realloc(line,1+lineLen);
          memset(line,'-',lineLen);
          line[lineLen]='\0';
          /* ............................................................... */
          printf("    NODE %s:\n",nodeFound.c_str());
          printf("    %s\n",line);
          sprintf(pFormat,"    %%-%ds %%%d"PRIu64" %%-%ds\n",fieldLen[0],
                  fieldLen[2],fieldLen[1]);
          for(k=0,lp=labels,up=units;k<fieldN;k++,lp+=1+strlen(lp),
              up+=1+strlen(up))
          {
            /* datum is not N/A for the node kernel */
            if(DATUM_PUBLISHED(isOldServer,data[k]))
            {
              printf(pFormat,lp,data[k],up);
            }
          }
          printf("    %s\n",line);
          /* ............................................................... */
        }                                              /* if(!compactFormat) */
        else                                            /* if(compactFormat) */
        {
          /* ............................................................... */
          printf("NODE=%s",nodeFound.c_str());
          for(k=0,lp=labels,up=units;k<fieldN;k++,lp+=1+strlen(lp),
              up+=1+strlen(up))
          {
            for(p=lp;*p;p++)
            {
              if(*p==' ')*p='_';    /* avoid spaces to make parsing easier */
            }
            if(DATUM_PUBLISHED(isOldServer,data[k]))
            {
              printf(" %s",lp);
              if(*up)printf("_[%s]=",up);
              printf("%"PRIu64,data[k]);
            }
          }
          printf("\n");
          /* ............................................................... */
        }                                               /* if(compactFormat) */
        /* ................................................................. */
      }                                                     /* if !fnmatch() */
    }                                                     /* loop over nodes */
    if(!nodeFoundN && !compactFormat)
    {
      printf("    No node found for pattern: \"%s\"!\n",hpIt->c_str());
    }
  }        
  /* ----------------------------------------------------------------------- */
  return 0;
}
/* ************************************************************************* */
void getServerVersion(vector<string>hostPttnLs)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnLs,SVC_HEAD,SRV_NAME,deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnLs,"",SRV_NAME,deBug,12,8);
  /* older service name rule */
  FmcUtils::printServerVersion(hostPttnLs,"","mem",deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/* ************************************************************************* */
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"memViewer [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"          [-m | --hostname NODE_PATTERN...]  [-c | --compact]\n"
"memViewer { -V | --version } [-v | --debug]\n"
"          [-N | --dim-dns DIM_DNS_NODE]\n"
"          [-m | --hostname NODE_PATTERN...]\n"
"memViewer { -h | --help }\n"
"\n"
"Try \"memViewer -h\" for more information.\n";
  /* ----------------------------------------------------------------------- */
  fprintf(stderr,"\n%s\n",shortUsageStr);
  exit(1);
}
/* ************************************************************************* */
void usage(int mode)
{
  FILE *fpOut;
  int status;
  const char *formatter;
  /*-------------------------------------------------------------------------*/
  if(!mode)formatter="/usr/bin/groff -man -Tascii | /usr/bin/less -iRs";
  else formatter="/bin/cat";
  fpOut=popen(formatter,"w");
  fprintf(fpOut,
".ig\n"
"memViewer.man\n"
"\n"
"..\n"
"%s"
".hw hostname NODE_PATTERN\n"
".TH memViewer 1  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis memViewer\\ \\-\n"
"Get memory usage statistic information from the FMC Memory Monitor Servers\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis memViewer\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".DoubleOpt[] c compact\n"
".EndSynopsis\n"
".sp\n"
".Synopsis memViewer\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis memViewer\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"Get the information about the \\fBmemory usage\\fP of the farm nodes whose "
"hostname matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP, "
"specified using the \\fB-m\\fP \\fINODE_PATTERN\\fP command line option "
"(\\fBdefault\\fP: all the nodes registered with the DIM name server "
"\\fIDIM_DNS_NODE\\fP).\n"
".PP\n"
"The data about total, used and free memory are partitioned into low memory "
"and high memory. The \\fBhigh memory\\fP is all the memory above 896 MiB of "
"physical memory. The high memory areas can be used by the user-space "
"programs, or for the page-cache (but not for kernel data structures). The "
"kernel must use some tricks to access the high memory (it temporarily maps "
"pages from high memory into the lower page tables), making it slower to "
"access than low memory. The \\fBlow memory\\fP, on the other side, is memory "
"which can be used for everything that high memory can be used for, but it is "
"also available for the kernel's use for its own data structures (e.g. kernel "
"slabs). The kernel cannot work if it is out of low memory.\n"
".PP\n"
"Datum \\fBCommitted_AS\\fP refers to the Linux kernel memory "
"\\fBover-committing\\fP policy. When a process dynamically allocate memory "
"calling the \\fBmalloc\\fP(3) library call, and hence the \\fBbrk\\fP(2) "
"system call, nothing happens, really. Only when the process start using the "
"memory allocated with the \\fBbrk\\fP(2), it get real memory, and just as "
"much it uses. The hope is that programs ask for more than they actually need "
"or that not all the running programs need the memory they have demanded with "
"\\fBbrk\\fP(2) at the same time. If the hope is fulfilled, the kernel can "
"run more programs in the same memory space, or can run a program that "
"requires more virtual memory than is available. If the hope is not "
"fulfilled, an \\fBout-of-memory\\fP (\\fBOOM\\fP) condition occurs: as a "
"consequence, the \\fBOOM killer\\fP is invoked to choose a process "
"(following certain criteria) and kill it. Since kernel version 2.5.30 the "
"over-committing behavior is controlled by the two user-settable parameters "
"in /proc/sys/vm/: \\fBovercommit_memory\\fP and \\fBovercommit_ratio\\fP. "
"Since kernel 2.5.30 the parameter \\fBovercommit_memory\\fP can be set to "
"three different values:\n"
".RS\n"
".TP\n"
"0:\n"
"heuristic over-commit handling (default);\n"
".TP\n"
"1:\n"
"never refuse any brk(2);\n"
".TP\n"
"2:\n"
"strict over-commit. Never commit a virtual address space larger than swap "
"space plus a fraction \\fBovercommit_ratio\\fP (default 50%%) of the "
"physical memory.\n"
".RE\n"
".PP\n"
"Previous kernel allowed only values 0 and 1 for \\fBovercommit_memory\\fP.\n"
".PP\n"
"Data with \\fBVmalloc\\fP prefix concern a mechanism to map physically "
"non-contiguous memory areas to a contiguous area in virtual memory. The size "
"of this region will be always at least \\fIVMALLOC_RESERVE\\fP, which on the "
"x86 is 128 MiB, located between \\fIVMALLOC_START\\fP and \\fIVMALLOC_END\\fP "
"addresses in virtual memory. The page tables in this region are adjusted as "
"necessary to point to physical pages which are allocated with the normal "
"physical page allocator. Since allocations require altering the kernel page "
"tables, there is a limitation on how much memory can be mapped with "
"\\fBvmalloc\\fP() as only the virtual addresses space between "
"\\fIVMALLOC_START\\fP and \\fIVMALLOC_END\\fP is available. Usually, it is "
"only used for storing the swap map information and for loading kernel "
"modules into memory.\n"
".PP\n"
"The DIM Name Server \\fIDIM_DNS_NODE\\fP, looked up to seek for FMC CPU "
"Information Servers, can be chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1.\n"
"Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2.\n"
"Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3.\n"
"Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB"DIM_CONF_FILE_NAME"\\fP\".\n"
".RE\n"
".PP\n"
"Retrieved parameters depend on the kernel version. They are a subset of the "
"following:\n"
".RS\n"
".TP\n"
"\\fBActive\\fP (uint64_t)\n"
"Amount of memory that has been used more recently and usually not reclaimed "
"unless absolutely necessary.\n"
".TP\n"
"\\fBActiveAnon\\fP (uint64_t)\n"
"Amount of program memory that has been used more recently. Specific of Linux "
"kernel 2.4.\n"
".TP\n"
"\\fBActiveCache\\fP (uint64_t)\n"
"Amount of cache memory that has been used more recently. Specific of Linux "
"kernel 2.4.\n"
".TP\n"
"\\fBAnonPages\\fP (uint64_t)\n"
"Non-file backed pages mapped into userspace page tables.\n"
".TP\n"
"\\fBBounce\\fP (uint64_t)\n"
"Memory used for block device \"bounce buffers\".\n"
".TP\n"
"\\fBBuffers\\fP (uint64_t)\n"
"Relatively temporary storage for raw disk blocks shouldn't get tremendously "
"large (20MB or so).\n"
".TP\n"
"\\fBCached\\fP (uint64_t)\n"
"In-memory cache for files read from the disk (the pagecache). Doesn't "
"include SwapCached.\n"
".TP\n"
"\\fBCommitLimit\\fP (uint64_t)\n"
"Based on the overcommit ratio ('vm.overcommit_ratio'), this is the total "
"amount of memory currently available to be allocated on the system. This "
"limit is only adhered to if strict overcommit accounting is enabled (mode 2 "
"in 'vm.overcommit_memory'). The CommitLimit is calculated with the following "
"formula: CommitLimit = ('vm.overcommit_ratio' * Physical RAM) + Swap. For "
"example, on a system with 1 GiB of physical RAM and 7 GiB of swap with a "
"'vm.overcommit_ratio' of 30 it would yield a CommitLimit of 7.3 GiB.\n"
".TP\n"
"\\fBCommitted_AS\\fP (uint64_t)\n"
"The amount of memory presently allocated on the system. The committed memory "
"is a sum of all of the memory which has been allocated by processes, even if "
"it has not been \"used\" by them as of yet. A process which malloc()'s 1 GiB "
"of memory, but only touches 300 MiB of it will only show up as using 300 MiB "
"of memory even if it has the address space allocated for the entire 1 GiB. "
"This 1 GiB is memory which has been \"committed\" to by the VM and can be "
"used at any time by the allocating application. With strict overcommit "
"enabled on the system (mode 2 in 'vm.overcommit_memory'), allocations which "
"would exceed the CommitLimit (detailed above) will not be permitted. This is "
"useful if one needs to guarantee that processes will not fail due to lack of "
"memory once that memory has been successfully allocated.\n"
".TP\n"
"\\fBDirty\\fP (uint64_t)\n"
"The amount of memory which is waiting to get written back to the disk.\n"
".TP\n"
"\\fBHighFree\\fP (uint64_t)\n"
"The amount of High Memory free. High memory is all memory above ~860 MiB of "
"physical memory. High memory areas are for use by userspace programs, or for "
"the pagecache. The kernel must use tricks to access this memory, making it "
"slower to access than lowmem.\n"
".TP\n"
"\\fBHighTotal\\fP (uint64_t)\n"
"The total amount of High Memory. High memory is all memory above ~860 MiB of "
"physical memory. High memory areas are for use by userspace programs, or for "
"the pagecache. The kernel must use tricks to access this memory, making it "
"slower to access than lowmem.\n"
".TP\n"
"\\fBHugePages_Free\\fP (uint64_t)\n"
"The number of the Linux Huge Pages in the pool that are not yet allocated. "
"Hugepages is a mechanism that allows the Linux kernel to utilize the "
"multiple page size capabilities of modern hardware architectures, by using "
"\"Transaction Lookaside Buffers\" (TLB) in the CPU architecture. The default "
"page size is 4 KiB in the x86 architecture, while the Huge Page Size is "
"typically 2048 KiB.\n"
".TP\n"
"\\fBHugePages_Rsvd\\fP (uint64_t)\n"
"The number of huge pages for which a commitment to allocate from the pool "
"has been made, but no allocation has yet been made. Reserved huge pages "
"guarantee that an application will be able to allocate a huge page from the "
"pool of huge pages at fault time. Hugepages is a mechanism that allows the "
"Linux kernel to utilize the multiple page size capabilities of modern "
"hardware architectures, by using \"Transaction Lookaside Buffers\" (TLB) in "
"the CPU architecture. The default page size is 4 KiB in the x86 "
"architecture, while the Huge Page Size is typically 2048 KiB.\n"
".TP\n"
"\\fBHugePages_Total\\fP (uint64_t)\n"
"The size of the pool of huge pages. Hugepages is a mechanism that allows the "
"Linux kernel to utilize the multiple page size capabilities of modern "
"hardware architectures, by using \"Transaction Lookaside Buffers\" (TLB) in "
"the CPU architecture. The default page size is 4 KiB in the x86 "
"architecture, while the Huge Page Size is typically 2048 KiB.\n"
".TP\n"
"\\fBHugepagesize\\fP (uint64_t)\n"
"The size of a Linux Huge Page. Hugepages is a mechanism that allows the "
"Linux kernel to utilize the multiple page size capabilities of modern "
"hardware architectures, by using \"Transaction Lookaside Buffers\" (TLB) in "
"the CPU architecture. The default page size is 4 KiB in the x86 "
"architecture, while the Huge Page Size is typically 2048 KiB.\n"
".TP\n"
"\\fBInact_clean\\fP (uint64_t)\n"
"The amount of memory which has been less recently used and has been already "
"copied to the swap device. Because clean pages are already synchronized with "
"respect to their backing store, the OS can reuse Inact_clean pages without "
"having to write the page to a swap device. Specific of kernel 2.4.\n"
".TP\n"
"\\fBInact_dirty\\fP (uint64_t)\n"
"The amount of memory which has been less recently used and has not yet been "
"copied to the swap device. When memory is required, the OS chooses to steal "
"Inact_clean pages before swapping Inact_dirty pages. Specific of kernel "
"2.4.\n"
".TP\n"
"\\fBInact_laundry\\fP (uint64_t)\n"
"The amount of memory which has been less recently used and which is being "
"copied to the swap device. Specific of kernel 2.4.\n"
".TP\n"
"\\fBInact_target\\fP (uint64_t)\n"
"The amount of memory which OS ought to have, to be possibly reclaimed for "
"other purposes, calculated as the sum of Active, Inact_dirty, and "
"Inact_clean divided by 5. When exceeded, the kernel will not do work to move "
"pages from active to inactive. This is an indicator of when page cleaning "
"should be performed. Specific of kernel 2.4.\n"
".TP\n"
"\\fBInactive\\fP (uint64_t)\n"
"The amount of memory which has been less recently used. It is more eligible "
"to be reclaimed for other purposes. Since kernel 2.5.41+. In kernel 2.4 this "
"value is the sum Inact_dirty + Inact_laundry + Inact_clean.\n"
".TP\n"
"\\fBLowFree\\fP (uint64_t)\n"
"The amount of low memory free. Low memory is memory which can be used for "
"everything that high memory can be used for, but it is also available for "
"the kernel's use for its own data structures. Among many other things, it is "
"where everything from the Slab is allocated. Bad things happen when you're "
"out of low memory.\n"
".TP\n"
"\\fBLowTotal\\fP (uint64_t)\n"
"The total amount of low memory. Low memory is memory which can be used for "
"everything that high memory can be used for, but it is also available for "
"the kernel's use for its own data structures. Among many other things, it is "
"where everything from the Slab is allocated. Bad things happen when you're "
"out of low memory.\n"
".TP\n"
"\\fBLowUsed\\fP (uint64_t)\n"
"The amount of low memory used. Low memory is memory which can be used for "
"everything that high memory can be used for, but it is also available for "
"the kernel's use for its own data structures. Among many other things, it is "
"where everything from the Slab is allocated. Bad things happen when you're "
"out of low memory.\n"
".TP\n"
"\\fBMapped\\fP (uint64_t)\n"
"The amount of memory used to store files which have been mmapped, such as "
"libraries. Since kernel 2.5.41+..\n"
".TP\n"
"\\fBMemFree\\fP (uint64_t)\n"
"The amount of free memory (The sum of LowFree + HighFree).\n"
".TP\n"
"\\fBMemShared\\fP (uint64_t)\n"
"The amount of memory shared between processes. Set to zero since kernel 2.4.\n"
".TP\n"
"\\fBMemTotal\\fP (uint64_t)\n"
"The total usable ram (i.e. physical ram minus a few reserved bits and the "
"kernel binary code). = LowTotal + HighTotal.\n"
".TP\n"
"\\fBMemUsed\\fP (uint64_t)\n"
"The amount of used memory (The difference MemTotal - MemFree).\n"
".TP\n"
"\\fBNFS_Unstable\\fP (uint64_t)\n"
"NFS pages sent to the server, but not yet committed to stable storage.\n"
".TP\n"
"\\fBPageTables\\fP (uint64_t)\n"
"Amount of memory dedicated to the lowest level of page tables.\n"
".TP\n"
"\\fBReverseMaps\\fP (uint64_t)\n"
"The number of reverse mappings performed.\n"
".TP\n"
"\\fBSReclaimable\\fP (uint64_t)\n"
"Part of Slab, that might be reclaimed, such as caches.\n"
".TP\n"
"\\fBSUnreclaim\\fP (uint64_t)\n"
"Part of Slab, that cannot be reclaimed on memory pressure.\n"
".TP\n"
"\\fBSlab\\fP (uint64_t)\n"
"The amount of memory used for in-kernel data structures cache. Since kernel "
"2.5.41+.\n"
".TP\n"
"\\fBSwapCached\\fP (uint64_t)\n"
"Amount of memory that once was swapped out, is swapped back in but still "
"also is in the swapfile (if memory is needed it doesn't need to be swapped "
"out AGAIN because it is already in the swapfile. This saves I/O).\n"
".TP\n"
"\\fBSwapFree\\fP (uint64_t)\n"
"The amount of swap memory free. Swap space is memory which has been "
"evicted from RAM, and is temporarily on the disk.\n"
".TP\n"
"\\fBSwapTotal\\fP (uint64_t)\n"
"The total amount of physical swap space available. Swap space is memory "
"which has been evicted from RAM, and is temporarily on the disk.\n"
".TP\n"
"\\fBSwapUsed\\fP (uint64_t)\n"
"The amount of swap space used. Swap space is memory which has been "
"evicted from RAM, and is temporarily on the disk.\n"
".TP\n"
"\\fBVmallocChunk\\fP (uint64_t)\n"
"Largest contiguous block of vmalloc area which is free. Vmalloc area is a "
"memory area which is seen by the kernel as a contiguous range of addresses "
"in the virtual address space, although the pages are not consecutive in "
"physical memory.\n"
".TP\n"
"\\fBVmallocTotal\\fP (uint64_t)\n"
"Total size of vmalloc memory area. Vmalloc area is a memory area which is "
"seen by the kernel as a contiguous range of addresses in the virtual address "
"space, although the pages are not consecutive in physical memory.\n"
".TP\n"
"\\fBVmallocUsed\\fP (uint64_t)\n"
"Amount of vmalloc memory area which is used. Vmalloc area is a memory area "
"which is seen by the kernel as a contiguous range of addresses in the "
"virtual address space, although the pages are not consecutive in physical "
"memory.\n"
".TP\n"
"\\fBWriteback\\fP (uint64_t)\n"
"Amount of memory which is actively being written back to the disk.\n"
".TP\n"
"\\fBWritebackTmp\\fP (uint64_t)\n"
"Memory used by FUSE for temporary writeback buffers.\n"
".RE\n"
".PP\n"
".\n"
".SH OPTIONS\n"
".\n"
".SS General Options\n"
".\n"
".OptDef h help\n"
"Get basic usage help from the command line, then terminate.\n"
".\n"
".OptDef v debug\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N dim\\[hy]dns DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef m hostname NODE_PATTERN (string, repeatable)\n"
"Print only the memory usage information of the farm nodes whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: print the memory usage information of all the nodes "
"registered with the DIM name server \\fIDIM_DNS_NODE\\fP).\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC Memory Monitor Server version "
"and the FMC version, than terminate.\n"
".\n"
".OptDef c compact\n"
"Print the memory usage information in compact format (easier to "
"parse): one node per line, no separator lines.\n"
".\n"
".SH ENVIRONMENT\n"
".TP\n"
".EnvVar DIM_DNS_NODE \\ (string,\\ mandatory\\ if\\ not\\ defined\\ "
"otherwise,\\ see\\ above)\n"
"Host name of the node which is running the DIM DNS.\n"
".\n"
".TP\n"
".EnvVar LD_LIBRARY_PATH \\ (string,\\ mandatory\\ if\\ not\\ set\\ using\\ "
"ldconfig)\n"
"Must include the path to the libraries \"libdim\" and \"libFMCutils\".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Show the memory sytatistics of all the farm nodes:\n"
".PP\n"
".ShellCommand memViewer\n"
".PP\n"
"Print output in compact format:\n"
".PP\n"
".ShellCommand memViewer -c\n"
".ShellCommand memViewer --compact\n"
".PP\n"
"Show only the memory sytatistics of selected nodes:\n"
".PP\n"
".ShellCommand memViewer -m farm0101 -m farm0102\n"
".ShellCommand memViewer --host farm0101 --host farm0102\n"
".PP\n"
"Show only the memory sytatistics of wildcard-selected nodes:\n"
".PP\n"
".ShellCommand memViewer -m \"farm01*\" -m \"farm02*\"\n"
".ShellCommand memViewer --host \"farm01*\" --host \"farm02*\"\n"
".ShellCommand memViewer -m \"farm010[1357]\"\n" 
".ShellCommand memViewer --hostname \"farm010[1357]\"\n" 
".PP\n"
"Show the server version installed on the farm nodes:\n"
".PP\n"
".ShellCommand memViewer -V\n"
".ShellCommand memViewer --version\n"
".PP\n"
".\n"
". SH AVAILABILITY\n"
".\n"
"Information on how to get the FMC (Farm Monitoring and Control System)\n"
"package and related information is available at the web sites:\n"
".PP\n"
".URL https://\\:lhcbweb.bo.infn.it/\\:twiki/\\:bin/\\:view.cgi/\\:"
"LHCbBologna/\\:FmcLinux \"FMC Linux Development Homepage\".\n"
".PP\n"
".URL http://\\:itcobe.web.cern.ch/\\:itcobe/\\:Projects/\\:Framework/\\:"
"Download/\\:Components/\\:SystemOverview/\\:fwFMC/\\:welcome.html "
"\"CERN ITCOBE: FW Farm Monitor and Control\".\n"
".PP\n"
".URL http://\\:lhcb-daq.web.cern.ch/\\:lhcb-daq/\\:online-rpm-repo/\\:"
"index.html \"LHCb Online RPMs\".\n"
".PP\n"
".\n"
".\n"
".SH AUTHORS\n"
".\n"
".MTO domenico.galli@bo.infn.it \"Domenico Galli\"\n"
".\n"
".\n"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".br\n"
"Package: FMC-%s\n"
".\n"
".SH SEE ALSO:\n"
".ad l\n"
".BR \\%%memSrv (8),\n"
".br\n"
".BR \\%%vmstat (8),\n"
".br\n"
".BR \\%%/proc/meminfo,\n"
".br\n"
".BR \\%%http://www.kernel.org/doc/Documentation/filesystems/proc.txt.\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(0);
}
/*****************************************************************************/
