/*****************************************************************************/
/*
 * $Log: irqViewer.C,v $
 * Revision 1.13  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.12  2008/10/22 12:57:28  galli
 * minor changes
 *
 * Revision 1.9  2007/12/19 13:39:13  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.8  2007/10/10 14:31:12  galli
 * bug fixed
 *
 * Revision 1.7  2007/09/05 20:33:23  galli
 * usage() modified
 *
 * Revision 1.4  2007/09/05 08:30:52  galli
 * option -c, --compact for compact output
 * option -H, --header for a header in compact output
 *
 * Revision 1.2  2006/02/09 11:42:18  galli
 * working version
 *
 * Revision 1.1  2005/09/27 14:21:43  galli
 * Initial revision
 *
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
#define FIELD_N_ERR 5  /* number of fields for each process in case of error */
#define MAX_HOSTNAME_L 12                          /* maximum hostame length */
#define SRV_NAME "IRQ"
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: irqViewer.C,v 1.13 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
const char *srvName=SRV_NAME;
/*****************************************************************************/
void usage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j,k,l;
  /* pointers */
  char *p,*pp;
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
  int nolinkStrArrSz=0;
  static char *nolinkStrArr=NULL;
  int type=0;
  char *format=NULL;
  char *svc=NULL;
  char *successSvc=NULL;
  char *successSvcDup=NULL;
  int svcLen=0;
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  int success=-1;
  int cpuN;
  char *tss;
  char *tssc=NULL;
  int tssd,tssh,tssm,tsss,tssmils;
  char *data=NULL;
  int dataSz;
  int fieldN;
  int srcN;
  /* printing variables */
  char **labelV1=NULL;
  char **labelV2=NULL;
  char **labelV3=NULL;
  unsigned cntLen,devLen,numLen,dataLen;
  char pFormat[256];
  char *line=NULL;
  int lineLen=0;
  /* command line switch */
  int doPrintAve=0;
  int doPrintMax=0;
  int doPrintHeader=0;
  int compactFormat=0;
  int doPrintServerVersion=0;
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
  /* Compose DIM  no link error string array */
  nolinkStrArrSz=FIELD_N_ERR*(1+strlen("N/F"));           /* "N/F"=not found */
  nolinkStrArr=(char*)malloc(nolinkStrArrSz);
  for(i=0,p=nolinkStrArr;i<FIELD_N_ERR;i++,p+=(1+strlen(p)))strcpy(p,"N/F");
  /*-------------------------------------------------------------------------*/
  if(!compactFormat)
  {
    printf("Node pattern list:  ");
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
        /* read CPU number */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/cpuN",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/cpuN",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo CpuN(svc,-1);
        cpuN=(int)CpuN.getInt();
        if(cpuN==-1)
        {
          fprintf(stderr,"Invalid CPU number %d!\n",cpuN);
          continue;
        }
        /*...................................................................*/
        if(!compactFormat)
          printf("    NODE %s [%d CPU core(s)/thread(s)].\n",nodeFound,cpuN);
          printf("    Time since counters reset: %s.\n",tss);
        /*...................................................................*/
        /* read data */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/data",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/data",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Data(svc,nolinkStrArr,nolinkStrArrSz);
        data=(char*)Data.getData();
        dataSz=(int)Data.getSize();
        /*...................................................................*/
        /* evaluate field number (fieldN) */
        fieldN=2+3*cpuN;
        /*...................................................................*/
        /* evaluate interrupt sources number (srcN) */
        for(p=data,srcN=0;p<data+dataSz;srcN++)p+=(1+strlen(p));
        srcN/=fieldN;
        /*...................................................................*/
        /* define labels */
        labelV1=(char**)realloc(labelV1,(fieldN+1)*sizeof(char*));
        labelV2=(char**)realloc(labelV2,(fieldN+1)*sizeof(char*));
        labelV3=(char**)realloc(labelV3,(fieldN+1)*sizeof(char*));
        labelV1[0]=strdup("#");
        labelV2[0]=strdup("");
        labelV3[0]=strdup("");
        labelV1[1]=strdup("device");
        labelV2[1]=strdup("");
        labelV3[1]=strdup("");
        labelV1[2]=strdup("IRQ");
        labelV2[2]=strdup("");
        labelV3[2]=strdup("");
        for(k=0;k<cpuN;k++)
        {
          asprintf(&labelV1[3+k],"CPU%02d",k);
          asprintf(&labelV2[3+k],"instant");
          asprintf(&labelV3[3+k],"[irq/s]");
          asprintf(&labelV1[3+cpuN+k],"CPU%02d",k);
          asprintf(&labelV2[3+cpuN+k],"average");
          asprintf(&labelV3[3+cpuN+k],"[irq/s]");
          asprintf(&labelV1[3+2*cpuN+k],"CPU%02d",k);
          asprintf(&labelV2[3+2*cpuN+k],"maximum");
          asprintf(&labelV3[3+2*cpuN+k],"[irq/s]");
        }
        /*...................................................................*/
        /* evaluate fields size */
        cntLen=snprintf(NULL,0,"%d",srcN-1);
        if(cntLen<strlen("#"))cntLen=strlen("#");
        for(k=0,p=data,devLen=0,numLen=0;k<srcN;k++)
        {
          for(l=0;l<fieldN;l++)
          {
            if(l==0)
            {
              if(devLen<strlen(p))devLen=strlen(p);
            }
            else if(l==1)
            {
              if(numLen<strlen(p))numLen=strlen(p);
            }
            p+=(1+strlen(p));
          }
        }
        if(devLen<strlen("device"))devLen=strlen("device");
        if(numLen<strlen("IRQ"))numLen=strlen("IRQ");
        dataLen=9;  /* 0.000e+00 */
        /*...................................................................*/
        /* evaluate line separator length */
        lineLen=0;
        lineLen+=cntLen;
        lineLen+=(1+devLen);
        lineLen+=(1+numLen);
        for(l=0;l<cpuN;l++)lineLen+=(1+dataLen);
        if(doPrintAve)for(l=0;l<cpuN;l++)lineLen+=(1+dataLen);
        if(doPrintMax)for(l=0;l<cpuN;l++)lineLen+=(1+dataLen);
        /* compose line separator */
        line=(char*)realloc(line,(1+lineLen)*sizeof(char));
        memset(line,'-',lineLen);
        line[lineLen]='\0';
        /*...................................................................*/
        /* print header */
        /* first line */
        if(!compactFormat || doPrintHeader)
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
            printf(pFormat,"HOSTNAME");
          }
          if(!compactFormat)printf("    ");
          sprintf(pFormat,"%%%ds",cntLen);
          printf(pFormat,labelV1[0]);
          sprintf(pFormat," %%-%ds",devLen);
          printf(pFormat,labelV1[1]);
          sprintf(pFormat," %%%ds",numLen);
          printf(pFormat,labelV1[2]);
          for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV1[l+3]);
          }
          if(doPrintAve)for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV1[l+3+cpuN]);
          }
          if(doPrintMax)for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV1[l+3+2*cpuN]);
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
          printf(pFormat,labelV2[0]);
          sprintf(pFormat," %%-%ds",devLen);
          printf(pFormat,labelV2[1]);
          sprintf(pFormat," %%%ds",numLen);
          printf(pFormat,labelV2[2]);
          for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV2[l+3]);
          }
          if(doPrintAve)for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV2[l+3+cpuN]);
          }
          if(doPrintMax)for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV2[l+3+2*cpuN]);
          }
          printf("\n");
          /* third line */
          if(!compactFormat)printf("    ");
          else
          {
            /* hostname */
            sprintf(pFormat,"%%-%ds ",MAX_HOSTNAME_L);
            printf(pFormat,"");
          }
          sprintf(pFormat,"%%%ds",cntLen);
          printf(pFormat,labelV3[0]);
          sprintf(pFormat," %%-%ds",devLen);
          printf(pFormat,labelV3[1]);
          sprintf(pFormat," %%%ds",numLen);
          printf(pFormat,labelV3[2]);
          for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV3[l+3]);
          }
          if(doPrintAve)for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV3[l+3+cpuN]);
          }
          if(doPrintMax)for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,labelV3[l+3+2*cpuN]);
          }
          printf("\n");
        }
        if(!compactFormat)
        {
          printf("    ");
          printf("%s\n",line);
        }
        /*...................................................................*/
        /* print data */
        for(k=0,p=data;k<srcN;k++)
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
          sprintf(pFormat," %%-%ds",devLen);
          /* avoid spaces to facilitate output parsing */
          if(compactFormat)for(pp=p;*pp;pp++)if(*pp==' ')*pp='-';
          printf(pFormat,p);
          p+=(1+strlen(p));
          sprintf(pFormat," %%%ds",numLen);
          printf(pFormat,p);
          p+=(1+strlen(p));
          for(l=0;l<cpuN;l++)
          {
            sprintf(pFormat," %%%ds",dataLen);
            printf(pFormat,p);
            p+=(1+strlen(p));
          }
          for(l=0;l<cpuN;l++)
          {
            if(doPrintAve)
            {
              sprintf(pFormat," %%%ds",dataLen);
              printf(pFormat,p);
            }
            p+=(1+strlen(p));
          }
          for(l=0;l<cpuN;l++)
          {
            if(doPrintMax)
            {
              sprintf(pFormat," %%%ds",dataLen);
              printf(pFormat,p);
            }
            p+=(1+strlen(p));
          }
          printf("\n");
        }
        if(!compactFormat)
        {
          printf("    ");
          printf("%s\n",line);
        }
        /*...................................................................*/
        for(k=0;k<fieldN+1;k++)
        {
          if(labelV1[k])free(labelV1[k]);
          if(labelV2[k])free(labelV2[k]);
          if(labelV3[k])free(labelV3[k]);
        }
        /*...................................................................*/
      }                             /* if(!fnmatch(svcPatt,successSvc,0)) */
    }                                                     /* loop over nodes */
    /*.......................................................................*/
    if(!nodeFoundN)
    {
      printf("    No node found for pattern: \"%s\"!\n",hostNamePattV[i]);
    }
    /*.......................................................................*/
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
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"","irq",
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
"       irqViewer - Get interrupt rates information from the FMC IRQ\n"
"                   Monitor Servers\n"
"\n"
"SYNOPSIS\n"
"       irqViewer [-N|--dim-dns DIM_DNS_NODE] [-v|--debug]\n"
"                 [-m|--hostname NODE_PATTERN...]\n"
"                 [-c|--compact [-H|--header]] [-A|--average] [-M|--maximum]\n"
"                 [-a|--all]\n"
"       irqViewer {-V,--version} [-m|--hostname NODE_PATTERN...]\n"
"       irqViewer {-h,--help}\n"
"\n"
"DESCRIPTION\n"
"       Get the information about the hard interrupt rates on the farm nodes\n"
"       whose hostname matches the wildcard pattern NODE_PATTERN, specified\n"
"       using the -m command line option. In addition to instant values,\n"
"       average and maximum values can be printed by means of --average and\n"
"       --maximum switches, together with the time elapsed since the server\n"
"       start-up or since last server reset (i.e. reset of average and\n"
"       maximum values). Server reset is achieved through the irqReset(FMC)\n"
"       command.\n"
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
"              For each selected node print the FMC Interrupt Monitor Server\n"
"              version and sensor version, than exit.\n"
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
"       Show the instant values:\n"
"              irqViewer\n"
"       Show also average and maximum values:\n"
"              irqViewer -a\n"
"              irqViewer --all\n"
"       Print output in compact format with header:\n"
"              irqViewer -acH\n"
"              irqViewer -a -c -H\n"
"              irqViewer --all --compact --header\n"
"              irqViewer --al --co --hea\n"
"       Show also average values:\n"
"              irqViewer --average\n"
"              irqViewer --av\n"
"              irqViewer -A\n"
"       Show also maximum values:\n"
"              irqViewer --maximum\n"
"              irqViewer --ma\n"
"              irqViewer -M\n"
"       Show only interrupt raised on selected nodes:\n"
"              irqViewer -m farm0101 -m farm0102 --ave\n"
"       Show only interrupt raised on wildcard-selected nodes:\n"
"              irqViewer -m \"farm01*\" -m \"farm02*\" --ave --max\n"
"              irqViewer -m \"farm010[1357]\" --all\n" 
"       Show the server version installed on the farm nodes:\n"
"              irqViewer -V\n"
"              irqViewer --version\n"
"\n"
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       irqReset(FMC), irqSrv(FMC), coalSrv(FMC), proc(5).\n"
"       /proc/interrupts.\n"
"       /usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt.\n"
"\n"
,rcsidP,FMC_VERSION
         );
  exit(1);
}
/*****************************************************************************/
