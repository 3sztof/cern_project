/*****************************************************************************/
/*
 * $Log: coalViewer.C,v $
 * Revision 1.16  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.15  2008/10/22 12:45:29  galli
 * uses FmcUtils::printServerVersion()
 *
 * Revision 1.8  2007/12/14 16:09:47  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.6  2007/09/05 20:44:44  galli
 * usage() modified
 *
 * Revision 1.5  2007/09/05 15:17:15  galli
 * rewritten
 *
 * Revision 1.3  2007/08/23 13:41:32  galli
 * added function getServerVersion()
 * usage() modified
 *
 * Revision 1.1  2006/10/21 15:05:47  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#include <getopt.h>                                         /* getopt_long() */
/*****************************************************************************/
/* DIM */
#include "dic.hxx"                                  /* DimBrowser, DimClient */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                               /* printServerVersion() */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
/* ------------------------------------------------------------------------- */
#define MAX_HOSTNAME_L 12                          /* maximum hostame length */
#define SRV_NAME "Coalescence"
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: coalViewer.C,v 1.16 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
const char *srvName="Coalescence";
/*****************************************************************************/
void usage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j,k;
  /* pointers */
  char *p,*lp;
  /* service browsing */
  int hostNamePattC=0;
  char **hostNamePattV=NULL;
  char *hostNamePattUc=NULL;
  char *svcPatt=NULL;
  int nodeFoundN=0;
  char nodeFound[128]="";
  char ucNodeFound[128]="";
  char *dimBrPatt=NULL;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  char *svc=NULL;
  char *successSvc=NULL;
  char *successSvcDup=NULL;
  int svcLen=0;
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  int success=-1;
  int ifN=0;
  char *tss;
  char *tssc=NULL;
  int tssd,tssh,tssm,tsss,tssmils;
  int dataSz;
  float *data;
  /* printing variables */
  const char *labelV[7]={"HOSTNAME","#","Interface","Instant","Average",
                         "Maximum","Elapsed_time"};
  const char *unitV[7]={"","","","[frame/irq]","[frame/irq]","[frame/irq]",
                        "[ddd-hh:mm:ss.sss]"};
  unsigned cntLen,ifLen,dataLen,tsscLen;
  char *ifLabel=NULL;
  int ifLabelSz=0;
  /* retrieved quantities */
  char pFormat[256];
  char *line=NULL;
  int lineLen=0;
  /* command line switch */
  int doPrintAve=0;
  int doPrintMax=0;
  int doPrintHeader=0;
  int compactFormat=0;
  int doPrintServerVersion=0;
  static int isFirstTime=1;
  /* getopt */
  int flag;
  /* long command line options */
  static struct option longOptions[]=
  {
    {"hostname",required_argument,NULL,'m'},
    {"debug",no_argument,NULL,'v'},
    {"version",no_argument,NULL,'V'},
    {"dim-dns",required_argument,NULL,'N'},
    {"help",no_argument,NULL,'h'},
    {"compact",no_argument,NULL,'c'},
    {"header",no_argument,NULL,'H'},
    {"all",no_argument,NULL,'a'},
    {"average",no_argument,NULL,'A'},
    {"maximum",no_argument,NULL,'M'},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  while((flag=getopt_long(argc,argv,"m:vVN:hcHaAM",longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 'm': /* -m, --hostname */
        hostNamePattC++;
        hostNamePattV=(char**)realloc(hostNamePattV,hostNamePattC*
                                      sizeof(char*));
        hostNamePattV[hostNamePattC-1]=optarg;
        break;
      case 'v': /* -v, --debug */
        deBug++;
        break;
      case 'V': /* -V, --version */
        doPrintServerVersion=1;
        break;
      case 'N': /* -N, --dim-dns */
        dimDnsNode=optarg;
        break;
      case 'h': /* -h, --help */
        usage();
        break;
      case 'c': /* -c, --compact */
        compactFormat=1;
        break;
      case 'H': /* -H, --header */
        doPrintHeader=1;
        break;
      case 'a': /* -a, --all */
        doPrintAve=!doPrintAve;
        doPrintMax=!doPrintMax;
        break;
      case 'A': /* -A, --average */
        doPrintAve=!doPrintAve;
        break;
      case 'M': /* -M, --maximum */
        doPrintMax=!doPrintMax;
        break;
      default:
        fprintf(stderr,"getopt_long(): Invalid option \"%s\"!\n",
                argv[optind-1]);
        usage();
        break;
    }
  }
  /* default: all the nodes */
  if(!hostNamePattC)
  {
    hostNamePattC=1;
    hostNamePattV=(char**)realloc(hostNamePattV,sizeof(char*));
    hostNamePattV[hostNamePattC-1]=(char*)"*";
  }
  if(argc-optind!=0)
  {
    fprintf(stderr,"\nUnrequired non-option arguments!\n");
    usage();
  }
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
      dimDnsNode=getDimDnsNode(dimConfFile,0,1);
      if(dimDnsNode)
      {
        printf("DIM_DNS_NODE: \"%s\" (from \"/etc/sysconfig/dim\" file).\n",
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
                "\"/etc/sysconfig/dim\".\n");
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
  if(doPrintServerVersion) getServerVersion(hostNamePattC,hostNamePattV);
  /*-------------------------------------------------------------------------*/
  if(deBug)compactFormat=0;
  if(!compactFormat)doPrintHeader=0;
  /*-------------------------------------------------------------------------*/
  if(!compactFormat)
  {
    printf("Node patterns list:  ");
    for(i=0;i<hostNamePattC;i++)                  /* loop over node patterns */
    {
      printf("\"%s\"",hostNamePattV[i]);
      if(i<hostNamePattC-1)printf(", ");
    }
    printf(".\n");
  }
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<hostNamePattC;i++)                /* loop over node patterns */
  {
    if(!compactFormat)printf("  Node pattern: \"%s\":\n",hostNamePattV[i]);
    hostNamePattUc=(char*)realloc(hostNamePattUc,1+strlen(hostNamePattV[i]));
    strcpy(hostNamePattUc,hostNamePattV[i]);
    /* convert to upper case */
    for(p=hostNamePattUc;*p;p++)*p=toupper(*p);
    svcPatt=(char*)realloc(svcPatt,1+snprintf(NULL,0,"%s/%s/%s/success",
                           SVC_HEAD,hostNamePattUc,srvName));
    sprintf(svcPatt,"%s/%s/%s/success",SVC_HEAD,hostNamePattUc,srvName);
    if(deBug)printf("  Service pattern: \"%s\"\n",svcPatt);
    /*.......................................................................*/
    dimBrPatt=(char*)realloc(dimBrPatt,1+snprintf(NULL,0,"%s/*/%s/success",
                             SVC_HEAD,srvName));
    sprintf(dimBrPatt,"%s/*/%s/success",SVC_HEAD,srvName);
    br.getServices(dimBrPatt);
    nodeFoundN=0;
    while((type=br.getNextService(successSvc,format))!=0) /* loop over nodes */
    {
      if(!fnmatch(svcPatt,successSvc,0))
      {
        nodeFoundN++;
        successSvcDup=(char*)realloc(successSvcDup,1+strlen(successSvc));
        strcpy(successSvcDup,successSvc);
        /* successSvcDup: "<SVC_HEAD>/<HOST>/<SRV_NAME>/success" */
        p=strrchr(successSvcDup,'/');                          /* last slash */
        *p='\0';
        p=strrchr(successSvcDup,'/');                  /* last-but-one slash */
        *p='\0';
        p=strrchr(successSvcDup,'/');                  /* last-but-two slash */
        snprintf(nodeFound,sizeof(nodeFound),"%s",1+p);
        snprintf(ucNodeFound,1+strcspn(nodeFound,"/"),"%s",nodeFound);
        for(p=nodeFound;*p;p++) *p=tolower(*p);     /* convert to lower case */
        /*...................................................................*/
        /* read Success */
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,successSvc);
        DimCurrentInfo Success(successSvc,-1);
        success=(int)Success.getInt();
        if(success!=1)
        {
          fprintf(stderr,"Service \"%s\" unreachable!\n",successSvc);
          continue;
        }
        /*...................................................................*/
        /* read network interface number */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/ifN",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/ifN",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo IfN(svc,-1);
        ifN=(int)IfN.getInt();
        /*...................................................................*/
        /* read Time Since Start/Reset */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/tss",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/tss",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Tss(svc,(char*)"Server unreachable!");
        tss=(char*)Tss.getString();
        /* tssc = compact time string [ddd-hh:mm:ss.sss] */
        tssc=(char*)malloc(1+strlen(tss)-7);
        sscanf(tss,"%dd,%dh,%dm,%d.%ds",&tssd,&tssh,&tssm,&tsss,&tssmils);
        sprintf(tssc,"%03d-%02d:%02d:%02d.%03d",tssd,tssh,tssm,tsss,tssmils);
        /*...................................................................*/
        /* read network interface labels */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/labels",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/labels",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo IfLabel(svc,(char*)"Server unreachable!");
        ifLabelSz=(int)IfLabel.getSize();
        ifLabel=(char*)IfLabel.getData();
        /*...................................................................*/
        /* read data */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/data",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/data",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Data(svc,-1);
        dataSz=(int)Data.getSize();
        data=(float*)Data.getData();
        /*...................................................................*/
        /* evaluate fields size */
        cntLen=snprintf(NULL,0,"%d",ifN-1);
        if(!compactFormat||doPrintHeader)
          if(cntLen<strlen("#"))cntLen=strlen("#");
        for(k=0,p=ifLabel,ifLen=0;k<ifN;k++)
        {
          if(ifLen<strlen(p))ifLen=strlen(p);
          p+=(1+strlen(p));
        }
        if(!compactFormat||doPrintHeader)
          if(ifLen<strlen(labelV[2]))ifLen=strlen(labelV[2]);
        dataLen=9;                                              /* 1.680e+00 */
        if(!compactFormat||doPrintHeader)dataLen=11;          /* [frame/irq] */
        tsscLen=16;                                      /* 000-01:46:20.421 */
        if(!compactFormat||doPrintHeader)tsscLen=18;   /* [ddd-hh:mm:ss.sss] */
        /*...................................................................*/
        /* evaluate line separator length */
        lineLen=cntLen+1+ifLen+1+dataLen+1;
        if(doPrintAve)lineLen+=(dataLen+1);
        if(doPrintMax)lineLen+=(dataLen+1);
        if(doPrintAve||doPrintMax)lineLen+=(tsscLen+1);
        lineLen--;
        /* compose line separator */
        line=(char*)realloc(line,(1+lineLen)*sizeof(char));
        memset(line,'-',lineLen);
        line[lineLen]='\0';
        /*...................................................................*/
        if(!compactFormat)printf("    NODE %s:\n",nodeFound);
        /*...................................................................*/
        /* print header */
        /* first line */
        if(!compactFormat || (doPrintHeader && isFirstTime))
        {
          if(!compactFormat)
          {
            printf("    ");
            printf("%s\n",line);
          }
          else
          {
            /* hostname */
            sprintf(pFormat,"%%-%ds ",MAX_HOSTNAME_L);
            printf(pFormat,labelV[0]);
          }
          if(!compactFormat)printf("    ");
          sprintf(pFormat,"%%%ds",cntLen);
          printf(pFormat,labelV[1]);
          sprintf(pFormat," %%-%ds",ifLen);
          printf(pFormat,labelV[2]);
          sprintf(pFormat," %%%ds",dataLen);
          printf(pFormat,labelV[3]);
          if(doPrintAve)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV[4]);
          }
          if(doPrintMax)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV[5]);
          }
          if(doPrintAve||doPrintMax)
          {
            sprintf(pFormat," %%-%ds",tsscLen);
            printf(pFormat,labelV[6]);
          }
          printf("\n");
          /* second line */
          if(!compactFormat)printf("    ");
          else
          {
            /* hostname */
            sprintf(pFormat,"%%-%ds ",MAX_HOSTNAME_L);
            printf(pFormat,"");
          }
          sprintf(pFormat,"%%%ds",cntLen);
          printf(pFormat,unitV[1]);
          sprintf(pFormat," %%-%ds",ifLen);
          printf(pFormat,unitV[2]);
          sprintf(pFormat," %%%ds",dataLen);
          printf(pFormat,unitV[3]);
          if(doPrintAve)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,unitV[4]);
          }
          if(doPrintMax)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,unitV[5]);
          }
          if(doPrintAve||doPrintMax)
          {
            sprintf(pFormat," %%-%ds",tsscLen);
            printf(pFormat,unitV[6]);
          }
          printf("\n");
          if(!compactFormat)
          {
            printf("    ");
            printf("%s\n",line);
          }
          isFirstTime=0;
        }
        /*...................................................................*/
        /* print data */
        for(k=0,lp=ifLabel;k<ifN;k++)
        {
          if(!compactFormat)printf("    ");
          else
          {
            /* hostname */
            sprintf(pFormat,"%%-%ds ",MAX_HOSTNAME_L);
            printf(pFormat,nodeFound);
          }
          sprintf(pFormat,"%%%dd",cntLen);
          printf(pFormat,k);
          sprintf(pFormat," %%-%ds",ifLen);
          printf(pFormat,lp);
          lp+=(1+strlen(lp));
          sprintf(pFormat," %%%ds%%9.3e",dataLen-9);
          printf(pFormat,"",data[k]);
          if(doPrintAve)
          {
            sprintf(pFormat," %%%ds%%9.3e",dataLen-9);
            printf(pFormat,"",data[k+ifN]);
          }
          if(doPrintMax)
          {
            sprintf(pFormat," %%%ds%%9.3e",dataLen-9);
            printf(pFormat,"",data[k+2*ifN]);
          }
          if(doPrintAve||doPrintMax)
          {
            sprintf(pFormat," %%-%ds",tsscLen);
            printf(pFormat,tssc);
          }
          printf("\n");
        }
        if(!compactFormat)
        {
          printf("    ");
          printf("%s\n",line);
        }
        /*...................................................................*/
      }                                                     /* if !fnmatch() */
    }                                                     /* loop over nodes */
    if(!nodeFoundN && !compactFormat)
    {
      printf("    No node found for pattern: \"%s\"!\n",hostNamePattV[i]);
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
                               deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"",SRV_NAME,
                               deBug,12,8);
  /* older service name rule */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"","coalescence",
                               deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
void usage(void)
{
  fprintf(stderr,
"\n"
"NAME\n"
"       coalViewer - Get network interrupt coalescence information from the\n"
"                    FMC Coalescence Monitor Servers\n"
"\n"
"SYNOPSIS\n"
"       coalViewer [-N|--dim-dns DIM_DNS_NODE] [-v|--debug]\n"
"                  [-m|--hostname NODE_PATTERN...]\n"
"                  [-c|--compact [-H|--header]] [-A|--average] [-M|--maximum]\n"
"                  [-a|--all]\n"
"       coalViewer {-V,--version} [-m|--hostname NODE_PATTERN...]\n"
"       coalViewer {-h,--help}\n"
"\n"
"DESCRIPTION\n"
"       Get the network interrupt coalescence factors, - i.e. the ratios\n"
"       between the number of frames sent/received by the network interfaces\n"
"       and the corresponding number of interrupts raised by the network\n"
"       interface cards - from the farm nodes whose hostname matches the\n"
"       wildcard pattern NODE_PATTERN, specified using the -m command line\n"
"       option. In addition to instan values, average and maximum values can\n"
"       be printed by means of --average and --maximum switches, together\n"
"       with the time elapsed since the server start-up or since last server\n"
"       reset (i.e. reset of average and maximum values). Server reset is\n"
"       achieved through the coalReset(FMC) command.\n"
"       DIM Name Server can be chosen (in decreasing order of priority) by:\n"
"              1. Specifying the -N DIM_DNS_NODE command-line option;\n"
"              2. Specifying the DIM_DNS_NODE environment variable;\n"
"              3. Defining the DIM_DNS_NODE in the file \"/etc/sysconfig/dim\""
".\n"
"\n"
"OPTIONS\n"
"       --help, -h\n"
"              Get basic usage help from the command line.\n"
"       --version, -V\n"
"              For each selected node print the FMC Coalescence Monitor\n"
"              server version and sensor version, than exit.\n"
"       --dim-dns, -N DIM_DNS_NODE (string)\n"
"              Use DIM_DNS_NODE as DIM Name Server node.\n"
"       --debug, -v\n"
"              Increase verbosity for debugging.\n"
"       --hostname, -m NODE_PATTERN (string, repeatable)\n"
"              Get information from the nodes whose hostname matches the\n"
"              wildcard pattern NODE_PATTERN (default: get information from\n"
"              all the nodes).\n"
"       --compact, -c\n"
"              Print output in compact format (easier to parse).\n"
"       --header, -H\n"
"              Print a header for the compact format.\n"
"       --average, -A\n"
"              Toggle the printout of the average values.\n"
"       --maximum, -M\n"
"              Toggle the printout of the maximum values.\n"
"       --all, -a\n"
"              Toggle all the optional (non-basic) fields. Equivalent to:\n"
"              \"--average --maximum\".\n"
"\n"
"ENVIRONMENT\n"
"       DIM_DNS_NODE (string, mandatory if not defined otherwise, see above)\n"
"              Host name of the node which is running the DIM DNS.\n"
"       LD_LIBRARY_PATH (string, mandatory if not set using ldconfig)\n"
"              Must include the path to the libraries \"libdim\"\n"
"              and \"libFMCutils\".\n"
"\n"
"EXAMPLES\n"
"       Show the coalescence factors of all the farm nodes:\n"
"              coalViewer\n"
"       Show also average and maximum values:\n"
"              coalViewer -a\n"
"              coalViewer --all\n"
"       Print output in compact format with header:\n"
"              coalViewer -acH\n"
"              coalViewer -a -c -H\n"
"              coalViewer --all --compact --header\n"
"              coalViewer --al --co --hea\n"
"       Show also average values:\n"
"              coalViewer --average\n"
"              coalViewer --av\n"
"              coalViewer -A\n"
"       Show also maximum values:\n"
"              coalViewer --maximum\n"
"              coalViewer --ma\n"
"              coalViewer -M\n"
"       Show only the coalescence factors of selected nodes:\n"
"              coalViewer -m farm0101 -m farm0102 --ave\n"
"       Show only the coalescence factors of wildcard-selected nodes:\n"
"              coalViewer -m \"farm01*\" -m \"farm02*\" --ave --max\n"
"              coalViewer -m \"farm010[1357]\" --all\n" 
"       Show the server version installed on the farm nodes:\n"
"              coalViewer -V\n"
"              coalViewer --version\n"
"\n"
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       coalReset(FMC), coalSrv(FMC), nifSrv(FMC), irqSrv(FMC).\n"
"\n"
,rcsidP,FMC_VERSION
         );
  exit(1);
}
/*****************************************************************************/
