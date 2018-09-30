/*****************************************************************************/
/*
 * $Log: cpuinfoViewer.C,v $
 * Revision 1.23  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.22  2008/10/22 12:48:05  galli
 * minor changes
 *
 * Revision 1.20  2008/10/21 13:59:38  galli
 * groff manual
 *
 * Revision 1.19  2008/10/21 09:58:17  galli
 * uses FmcUtils::printServerVersion()
 *
 * Revision 1.17  2008/02/04 11:14:35  galli
 * bug fixed
 *
 * Revision 1.15  2008/02/01 08:05:21  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.12  2008/01/24 10:54:50  galli
 * dim SVC format compatible with cpuinfoSrv 2.17
 * separate services for each cpu core
 * as agreeded with Fernando Varela Rodriguez on 2008/01/23
 *
 * Revision 1.9  2007/12/17 16:21:48  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.8  2007/10/10 14:27:34  galli
 * bug fixed
 *
 * Revision 1.5  2007/08/24 22:44:29  galli
 * usage() modified
 *
 * Revision 1.4  2007/08/24 14:45:12  galli
 * service reorganization
 *
 * Revision 1.2  2005/09/26 13:30:52  galli
 * bug fixed
 *
 * Revision 1.1  2005/09/26 13:13:50  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>                                              /* strtod() */
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <getopt.h>                                         /* getopt_long() */
#include <errno.h>                                                  /* errno */
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                               /* printServerVersion() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*---------------------------------------------------------------------------*/
/* in monitor/include */
#include "cpuinfoSrv.h"       /* SRV_NAME, DEV_FIELD_N_ERR, DATA_FIELD_N_ERR */
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: cpuinfoViewer.C,v 1.23 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j,k,l;
  /* pointers */
  char *p=NULL,*lp=NULL,*up=NULL,*dp=NULL;
  /* service browsing */
  int hostPttnC=0;
  char **hostPttnV=NULL;
  char *hostPttn=NULL;
  int rawSuccessSvcPttnLen=0;
  char *rawSuccessSvcPttn=NULL;
  int successSvcPttnLen=0;
  char *successSvcPttn=NULL;
  int hostFoundN=0;
  char host[128]="";
  char ucHost[128]="";
  DimBrowser br;
  static char *nolinkDevStrArr=NULL;
  int nolinkDevStrArrS=0;
  static char *nolinkDataStrArr=NULL;
  int nolinkDataStrArrS=0;
  int type=0;
  char *format=NULL;
  char *svc=NULL;
  int svcLen=0;
  char *successSvc=NULL;
  char *successSvcDup=NULL;
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  int success=-1;
  int fieldN=0;
  char *devices=NULL;
  int devicesSz=0;
  int deviceC=0;
  char **deviceV=NULL;
  char *labels=NULL;
  int labelsSz=0;
  char *units=NULL;
  int unitsSz=0;
  DimCurrentInfo **Data=NULL;
  char **data=NULL;
  int *dataSz=NULL;
  /* printing variables */
  int isFirstTime=1;
  /* command line switch */
  int compactFormat=0;
  int doPrintHeader=0;
  int doPrintServerVersion=0;
  /* getopt */
  int flag;
  static struct option longOptions[]=
  {
    {"hostname",required_argument,NULL,'m'},
    {"dim-dns",required_argument,NULL,'N'},
    {"debug",no_argument,NULL,'v'},
    {"compact",no_argument,NULL,'c'},
    {"header",no_argument,NULL,'H'},
    {"version",no_argument,NULL,'V'},
    {"help",optional_argument,NULL,'h'},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt_long(argc,argv,"m:vN:Vh::cH",longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 'm': /* -m, --hostname */
        hostPttnC++;
        hostPttnV=(char**)realloc(hostPttnV,hostPttnC*sizeof(char*));
        hostPttnV[hostPttnC-1]=optarg;
        break;
      case 'v': /* -v, --debug */
        deBug++;
        break;
      case 'N': /* -N, --dim-dns */
        dimDnsNode=optarg;
        break;
      case 'V': /* -V, --version */
        doPrintServerVersion=1;
        break;
      case 'h': /* -h, -hh, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      case 'c': /* -c, --compact */
        compactFormat=1;
        break;
      case 'H': /* -H, --header */
        doPrintHeader=1;
        break;
      default:
        fprintf(stderr,"\ngetopt(): Invalid option \"%s\"!\n",argv[optind-1]);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* default: all the nodes */
  if(!hostPttnC)
  {
    hostPttnC=1;
    hostPttnV=(char**)realloc(hostPttnV,sizeof(char*));
    hostPttnV[hostPttnC-1]=(char*)"*";
  }
  /*-------------------------------------------------------------------------*/
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
  /*-------------------------------------------------------------------------*/
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
  /*-------------------------------------------------------------------------*/
  if(doPrintServerVersion) getServerVersion(hostPttnC,hostPttnV);
  /*-------------------------------------------------------------------------*/
  if(deBug)compactFormat=0;
  if(!compactFormat)doPrintHeader=0;
  /*-------------------------------------------------------------------------*/
  /* Compose DIM no link error string array */
  /* devices */
  nolinkDevStrArrS=DEV_FIELD_N_ERR*(1+strlen("N/F"));   /* "N/F"=not found */
  nolinkDevStrArr=(char*)malloc(nolinkDevStrArrS);
  for(i=0,p=nolinkDevStrArr;i<DEV_FIELD_N_ERR;i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
  /* data label/units/data */
  nolinkDataStrArrS=DATA_FIELD_N_ERR*(1+strlen("N/F"));   /* "N/F"=not found */
  nolinkDataStrArr=(char*)malloc(nolinkDataStrArrS);
  for(i=0,p=nolinkDataStrArr;i<DATA_FIELD_N_ERR;i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
  /*-------------------------------------------------------------------------*/
  if(!compactFormat)
  {
    printf("Node patterns list:  ");
    for(i=0;i<hostPttnC;i++)                  /* loop over node patterns */
    {
      printf("\"%s\"",hostPttnV[i]);
      if(i<hostPttnC-1)printf(", ");
    }
    printf(".\n");
  }
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<hostPttnC;i++)                /* loop over node patterns */
  {
    if(!compactFormat)printf("  Node pattern: \"%s\":\n",hostPttnV[i]);
    hostPttn=(char*)realloc(hostPttn,1+strlen(hostPttnV[i]));
    strcpy(hostPttn,hostPttnV[i]);
    /* convert to upper case */
    for(p=hostPttn;*p;p++)*p=toupper(*p);
    /* pattern to filter with using fnmatch() */
    successSvcPttnLen=snprintf(NULL,0,"%s/%s/%s/success",SVC_HEAD,hostPttn,
                               SRV_NAME);
    successSvcPttn=(char*)realloc(successSvcPttn,1+successSvcPttnLen);
    sprintf(successSvcPttn,"%s/%s/%s/success",SVC_HEAD,hostPttn,SRV_NAME);
    if(deBug)printf("  Service pattern: \"%s\"\n",successSvcPttn);
    /*.......................................................................*/
    /* pattern to filter with using DimBrowser */
    rawSuccessSvcPttnLen=snprintf(NULL,0,"%s/*/%s/success",SVC_HEAD,SRV_NAME);
    rawSuccessSvcPttn=(char*)realloc(rawSuccessSvcPttn,1+rawSuccessSvcPttnLen);
    snprintf(rawSuccessSvcPttn,1+rawSuccessSvcPttnLen,"%s/*/%s/success",
             SVC_HEAD,SRV_NAME);
    /*.......................................................................*/
    br.getServices(rawSuccessSvcPttn);
    hostFoundN=0;
    while((type=br.getNextService(successSvc,format))!=0) /* loop over nodes */
    {
      if(!fnmatch(successSvcPttn,successSvc,0))
      {
        hostFoundN++;
        successSvcDup=(char*)realloc(successSvcDup,1+strlen(successSvc));
        strcpy(successSvcDup,successSvc);
        /* successSvcDup: "<SVC_HEAD>/<HOST>/<SRV_NAME>/success" */
        p=strrchr(successSvcDup,'/');                          /* last slash */
        *p='\0';
        p=strrchr(successSvcDup,'/');                  /* last-but-one slash */
        *p='\0';
        p=strrchr(successSvcDup,'/');                  /* last-but-two slash */
        *p='\0';
        p=strrchr(successSvcDup,'/');                /* last-but-three slash */
        snprintf(host,sizeof(host),"%s",1+p);             /* host discovered */
        strcpy(ucHost,host);
        for(p=host;*p;p++)*p=tolower(*p);           /* convert to lower case */
        /*...................................................................*/
        /* read Success */
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,successSvc);
        DimCurrentInfo Success(successSvc,-1);
        success=(int)Success.getInt();
        if(success==-1)
        {
          fprintf(stderr,"Service \"%s\" unreachable!\n",successSvc);
          continue;
        }
        else if(success==0)
        {
          fprintf(stderr,"Service \"%s\": data not ready!\n",successSvc);
          continue;
        }
        /*...................................................................*/
        if(!compactFormat)
          printf("    NODE %s:\n",host);
        /*...................................................................*/
        /* read devices */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/devices",SVC_HEAD,ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/devices",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Devices(svc,nolinkDevStrArr,nolinkDevStrArrS);
        devices=(char*)Devices.getData();
        devicesSz=(int)Devices.getSize();
        for(k=0,p=devices;p<devices+devicesSz;k++,p+=(1+strlen(p)))
        {
          deviceV=(char**)realloc(deviceV,(k+1)*sizeof(char*));
          deviceV[k]=strdup(p);
        }
        deviceC=k;
        /*...................................................................*/
        /* read labels */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/labels",SVC_HEAD,ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/labels",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Labels(svc,nolinkDataStrArr,nolinkDataStrArrS);
        labels=(char*)Labels.getData();
        labelsSz=(int)Labels.getSize();
        /*...................................................................*/
        /* read units */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/units",SVC_HEAD,ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/units",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Units(svc,nolinkDataStrArr,nolinkDataStrArrS);
        units=(char*)Units.getData();
        unitsSz=(int)Units.getSize();
        /*...................................................................*/
        /* read data */
        Data=(DimCurrentInfo**)malloc(deviceC*sizeof(DimCurrentInfo*));
        dataSz=(int*)malloc(deviceC*sizeof(int));
        data=(char**)malloc(deviceC*sizeof(char*));
        for(k=0;k<deviceC;k++)
        {
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/data",SVC_HEAD,ucHost,SRV_NAME,
                            deviceV[k]);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/%s/data",SVC_HEAD,ucHost,SRV_NAME,deviceV[k]);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          Data[k]=new DimCurrentInfo(svc,nolinkDataStrArr,nolinkDataStrArrS);
          data[k]=(char*)Data[k]->getData();
          dataSz[k]=(int)Data[k]->getSize();
        }
        /*...................................................................*/
        /* evaluate field number */
        for(lp=labels,fieldN=0;lp<labels+labelsSz;fieldN++)lp+=(1+strlen(lp));
        /*...................................................................*/
        if(!compactFormat)
        {
          /*.................................................................*/
          unsigned labelLen=0;
          unsigned unitLen=0;
          /*.................................................................*/
          /* evaluate maximum label length */
          for(k=0,lp=labels;k<fieldN;k++)
          {
            if(labelLen<strlen(lp))labelLen=strlen(lp);
            lp+=(1+strlen(lp));
          }
          /*.................................................................*/
          /* evaluate maximum unit length */
          for(k=0,up=units;k<fieldN;k++)
          {
            if(unitLen<strlen(up))unitLen=strlen(up);
            up+=(1+strlen(up));
          }
          /*.................................................................*/
          for(l=0;l<deviceC;l++)
          {
            dp=data[l];
            printf("      CORE %02d:\n",l);
            for(k=0,lp=labels,up=units;k<fieldN;k++)
            {
              char flabel[labelLen+2];
              char funit[unitLen+3];
              for(p=lp;*p;p++)*p=toupper(*p);        /* convert to uppercase */
              sprintf(flabel,"%s:",lp);
              if(*up!='\0')sprintf(funit,"[%s]",up);
              else funit[0]='\0';
              printf("        %-*s %*s %s\n",labelLen+1,flabel,unitLen+2,
                     funit,dp);
              lp+=(1+strlen(lp));
              up+=(1+strlen(up));
              dp+=(1+strlen(dp));
            }
          }
          /*.................................................................*/
        }                                              /* if(!compactFormat) */
        else                                            /* if(compactFormat) */
        {
          /*.................................................................*/
          /* print header */
          if(doPrintHeader && isFirstTime)
          {
            isFirstTime=0;
            /* labels */
            printf("HOSTNAME CORE");
            for(k=0,lp=labels,up=units;k<fieldN;k++)
            {
              for(p=lp;*p;p++)
              {
                *p=toupper(*p);                      /* convert to uppercase */
                if(*p==' ')*p='_';    /* avoid spaces to make parsing easier */
              }
              printf(" %s",lp);
              if(*up)printf("_[%s]",up);
              lp+=(1+strlen(lp));
              up+=(1+strlen(up));
            }
            printf("\n");
          }                             /* if(doPrintHeader && isFirstTime)) */
          /*.................................................................*/
          /* print data */
          for(l=0;l<deviceC;l++)
          {
            dp=data[l];
            printf("%s %02d",host,l);
            for(k=0;k<fieldN;k++)
            {
              /* avoid spaces to facilitate output parsing */
              for(p=dp;*p;p++)if(*p==' ')*p='_';
              printf(" %s",dp);
              dp+=(1+strlen(dp));
            }
            printf("\n");
          }
          /*.................................................................*/
        }                                               /* if(compactFormat) */
        /*...................................................................*/
      }                                                     /* if !fnmatch() */
    }                                                     /* loop over nodes */
    if(!hostFoundN && !compactFormat)
    {
      printf("    No node found for pattern: \"%s\"!\n",hostPttnV[i]);
    }
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
void getServerVersion(int hostPttnC,char **hostPttnV)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,SVC_HEAD,SRV_NAME,
                               deBug,12,8,1);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"",SRV_NAME,
                               deBug,12,8,1);
  /* older service name rule */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"","cpuinfo",
                               deBug,12,8,0);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"cpuinfoViewer [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"              [-m | --hostname NODE_PATTERN...] [-c | --compact]\n"
"              [-H | --header]\n"
"cpuinfoViewer { -V | --version } [-v | --debug]\n"
"              [-N | --dim-dns DIM_DNS_NODE]\n"
"              [-m | --hostname NODE_PATTERN...]\n"
"cpuinfoViewer { -h | --help }\n"
"\n"
"Try \"cpuinfoViewer -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  fprintf(stderr,"\n%s\n",shortUsageStr);
  exit(1);
}
/*****************************************************************************/
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
"cpuinfoViewer.man\n"
"\n"
"..\n"
"%s"
".hw hostname NODE_PATTERN\n"
".TH cpuinfoViewer 1  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis cpuinfoViewer\\ \\-\n"
"Get the CPU information about the farm PCs from the FMC CPU Information "
"Servers\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis cpuinfoViewer\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".DoubleOpt[] c compact\n"
".DoubleOpt[] H header\n"
".EndSynopsis\n"
".sp\n"
".Synopsis cpuinfoViewer\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis cpuinfoViewer\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"Get the information about the CPUs of the farm PCs whose hostname "
"matches the wildcard pattern \\fINODE_PATTERN\\fP, specified using the "
"\\fB-m\\fP \\fINODE_PATTERN\\fP command line option (\\fBdefault\\fP: all "
"the nodes registered with the DIM name server \\fIDIM_DNS_NODE\\fP). "
"Information includes:\n"
".TP\n"
"\\fBVENDOR ID\\fP (string)\n"
"The CPU brand identifier string. E.g.: GenuineIntel, AuthenticAMD, "
"CyrixInstead, GenuineTMx86.\n"
".TP\n"
"\\fBFAMILY\\fP (integer)\n"
"The CPU family identifier. Value 6 stands for Intel Pentium III, AMD K7, "
"Cyrix M2, VIA C3; value 15 stands for Intel Pentium IV and AMD K8, etc.\n"
".TP\n"
"\\fBMODEL\\fP (integer)\n"
"The CPU model identifier. E.g.: value 1 can stand for P4 0.18 um, value 2 "
"can stand for P4 0.13 um, value 3 can stand for P4 0.09 um, value 5 can "
"stand for AMD K8 Opteron 0.13 um.\n"
".TP\n"
"\\fBMODEL NAME\\fP (string)\n"
"The CPU model identifier string. E.g.: \"Intel(R) Pentium(R) III CPU family "
"1133MHz\", \"Intel(R) Xeon(TM) CPU 2.40GHz\", \"AMD Athlon (TM) 64 "
"Processor 3700+\".\n"
".TP\n"
"\\fBSTEPPING\\fP (integer)\n"
"The sub-version identifier (a.k.a. revision number) of a CPU. The stepping "
"number increase with manufactory improvement or bug fixes of the same CPU "
"model.\n"
".TP\n"
"\\fBCLOCK\\fP (float)\n"
"The clock frequency of the CPU, measured in MHz (megahertz).\n"
".TP\n"
"\\fBCACHE SIZE\\fP (integer)\n"
"The CPU cache size, measured in KiB (kibibytes).\n"
".TP\n"
"\\fBHYPER-THREAD\\fP (boolean)\n"
"The Hyper-threading capability of the physical CPU package.\n"
".TP\n"
"\\fBMULTI-CORE\\fP (boolean)\n"
"The Multi-core capability of the physical CPU package.\n"
".TP\n"
"\\fBPHYSICAL ID\\fP (integer)\n"
"The physical package ID of the logical CPU.\n"
".TP\n"
"\\fBSIBLINGS\\fP (integer)\n"
"The number of logical processors (including \\fBboth threads and cores\\fP) "
"in the physical package currently in use by the Operating System.\n"
".TP\n"
"\\fBCORE ID\\fP (integer)\n"
"The core ID of the logical CPU.\n"
".TP\n"
"\\fBCPU CORES\\fP (integer)\n"
"The total number of cores in the physical package currently in use by the "
"Operating System.\n"
".TP\n"
"\\fBBOGOMIPS\\fP (float)\n"
"This number is a very raw measure of the CPU computational speed. Etymology "
"has origin by the words bogus (something which is a fake, not genuine) and "
"MIPS. MIPS is an acronym for \"Millions of Instruction per Second\" and is "
"the number of millions of machine code instruction which a CPU is able to "
"execute in a second. Bogomips are measured at boot time by running a program "
"loop which does absolutely nothing (hence the definition of Bogomips as "
"\"the number of million times per second a processor can do absolutely "
"nothing\"). Bogomips ranges from 0.02 (Intel 8088 at 4.77 MHz) to 6717.44 "
"(Intel Pentium 4 at 3.4 GHz). Bogomips/CPU clock ratio ranges from 0.004 "
"(Intel 8088) to 3.97 (Intel Xeon with hyper-threading).\n"
".PP\n"
"The DIM Name Server \\fIDIM_DNS_NODE\\fP, looked up to seek for FMC CPU "
"Information Servers, can be chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB"DIM_CONF_FILE_NAME"\\fP\".\n"
".RE\n"
".\n"
".\n"
".SH OPTIONS\n"
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
"Print only the CPU information of the farm nodes whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: print the CPU information of all the "
"nodes registered with the DIM name server \\fIDIM_DNS_NODE\\fP).\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC CPU Information Server version "
"and the FMC version, than terminate.\n"
".\n"
".OptDef c compact\n"
"Print the CPU information in compact format (easier to parse): "
"one core per line, no labels, spaces inside fields replaced by "
"underscores.\n"
".\n"
".OptDef H header\n"
"If the compact output format has been choosen, print a heading line with the "
"output field labels.\n"
".\n"
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
".\n"
".SH EXAMPLES\n"
".\n"
"Print the CPU information about all the farm nodes:\n"
".PP\n"
".ShellCommand cpuinfoViewer\n"
".PP\n"
".\n"
"Print output in compact format with header:\n"
".PP\n"
".ShellCommand cpuinfoViewer -c -H\n"
".ShellCommand cpuinfoViewer -cH\n"
".ShellCommand cpuinfoViewer --comp --hea\n"
".PP\n"
".\n"
"Print only the CPU information of selected nodes:\n"
".PP\n"
".ShellCommand cpuinfoViewer -m farm0101 -m farm0102\n"
".ShellCommand cpuinfoViewer -host farm0101 -host farm0102\n"
".PP\n"
".\n"
"Print only the CPU information of wildcard-selected nodes:\n"
".PP\n"
".ShellCommand cpuinfoViewer -m \\[dq]farm01*\\[dq] -m \\[dq]farm02*\\[dq]\n"
".ShellCommand cpuinfoViewer -m \\[dq]farm01?[1357]\\[dq]\n"
".ShellCommand cpuinfoViewer -m \\[dq]farm01?[1-5]\\[dq]\n"
".ShellCommand cpuinfoViewer --host \\[dq]farm01[0-5][1-36-9]\\[dq]\n"
".PP\n"
".\n"
"Print the Server version and the FMC package version of the running FMC "
"CPU Information servers registered with the current DIM name server:\n"
".PP\n"
".ShellCommand cpuinfoViewer -V\n"
".PP\n"
".\n"
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
".\n"
".SH SEE ALSO:\n"
".ad l\n"
".BR \\%%cpuinfoSrv (8).\n"
".br\n"
".BR \\%%/proc/cpuinfo,\n"
".BR \\%%/usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt.\n"
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
