/* ######################################################################### */
/*
 * $Log: osViewer.C,v $
 * Revision 1.13  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.12  2008/10/22 06:47:00  galli
 * minor changes
 *
 * Revision 1.7  2008/10/21 07:45:10  galli
 * groff manual
 *
 * Revision 1.6  2008/10/21 06:35:50  galli
 * uses FmcUtils::printServerVersion()
 *
 * Revision 1.2  2008/01/29 15:16:39  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.1  2008/01/23 11:10:38  galli
 * Initial revision
 */
/* ######################################################################### */
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
#include "osSrv.h"                                  /* SRV_NAME, FIELD_N_ERR */
/* ######################################################################### */
int deBug=0;
static char rcsid[]="$Id: osViewer.C,v 1.13 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/* ######################################################################### */
int main(int argc,char **argv)
{
  /* counters */
  int i,j,k;
  /* pointers */
  char *p=NULL,*lp=NULL,*dp=NULL;
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
  int type=0;
  char *format=NULL;
  char *svc=NULL;
  int svcLen=0;
  char *successSvc=NULL;
  char *successSvcDup=NULL;
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  char *nolinkStrArr=NULL;
  int nolinkStrArrSz=0;
  int success=-1;
  int fieldN=0;
  char *labels=NULL;
  int labelsSz=0;
  char *data=NULL;
  int dataSz=0;
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
        hostPttnV=(char**)realloc(hostPttnV,hostPttnC*
                                      sizeof(char*));
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
  nolinkStrArrSz=FIELD_N_ERR*(1+strlen("N/F"));           /* "N/F"=not found */
  nolinkStrArr=(char*)malloc(nolinkStrArrSz);
  for(i=0,p=nolinkStrArr;i<FIELD_N_ERR;i++,p+=(1+strlen(p)))strcpy(p,"N/F");
  /*-------------------------------------------------------------------------*/
  if(!compactFormat)
  {
    printf("Node patterns list:  ");
    for(i=0;i<hostPttnC;i++)                      /* loop over node patterns */
    {
      printf("\"%s\"",hostPttnV[i]);
      if(i<hostPttnC-1)printf(", ");
    }
    printf(".\n");
  }
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<hostPttnC;i++)                    /* loop over node patterns */
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
        if(!compactFormat)
          printf("    NODE %s:\n",host);
        /*...................................................................*/
        /* read labels */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/labels",SVC_HEAD,ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/labels",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Labels(svc,nolinkStrArr,nolinkStrArrSz);
        labels=(char*)Labels.getData();
        labelsSz=(int)Labels.getSize();
        /*...................................................................*/
        /* read data */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/data",SVC_HEAD,ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/data",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Data(svc,nolinkStrArr,nolinkStrArrSz);
        data=(char*)Data.getData();
        dataSz=(int)Data.getSize();
        /*...................................................................*/
        /* evaluate field number */
        for(lp=labels,fieldN=0;lp<labels+labelsSz;fieldN++)lp+=(1+strlen(lp));
        /*...................................................................*/
        if(!compactFormat)
        {
          /*.................................................................*/
          unsigned labelLen=0;
          /*.................................................................*/
          /* evaluate maximum label length: labelLen */
          for(k=0,lp=labels;k<fieldN;k++)
          {
            if(labelLen<strlen(lp))labelLen=strlen(lp);
            lp+=(1+strlen(lp));
          }
          /*.................................................................*/
          /* print labels+data */
          for(k=0,lp=labels,dp=data;k<fieldN;k++)
          {
            char flabel[labelLen+2];
            //for(p=lp;*p;p++)*p=toupper(*p);          /* convert to uppercase */
            *lp=toupper(*lp);
            sprintf(flabel,"%s:",lp);
            printf("      %-*s %s\n",labelLen+1,flabel,dp);
            lp+=(1+strlen(lp));
            dp+=(1+strlen(dp));
          }
          /*.................................................................*/
        }                                              /* if(!compactFormat) */
        else                                            /* if(compactFormat) */
        {
          /*.................................................................*/
          /* print labels */
          if(doPrintHeader && isFirstTime)
          {
            isFirstTime=0;
            printf("HOSTNAME");
            for(k=0,lp=labels;k<fieldN;k++)
            {
              *lp=toupper(*lp);
              for(p=lp;*p;p++)
              {
                //*p=toupper(*p);                      /* convert to uppercase */
                if(*p==' ')*p='_';    /* avoid spaces to make parsing easier */
              }
              printf(" %s",lp);
              lp+=(1+strlen(lp));
            }
            printf("\n");
          }
          /*.................................................................*/
          /* print data */
          printf("%s",host);
          for(k=0,dp=data;k<fieldN;k++)
          {
            for(p=dp;*p;p++)
            {
              if(*p==' ')*p='_';      /* avoid spaces to make parsing easier */
            }
            printf(" %s",dp);
            dp+=(1+strlen(dp));
          }
          printf("\n");
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
                               deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"",SRV_NAME,
                               deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"osViewer [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"         [-m | --hostname NODE_PATTERN...] [-c | --compact] [-H | --header]\n"
"osViewer { -V | --version } [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"         [-m | --hostname NODE_PATTERN...]\n"
"osViewer { -h | --help }\n"
"\n"
"Try \"osViewer -h\" for more information.\n";
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
"osViewer.man\n"
"\n"
"..\n"
"%s"
".hw hostname NODE_PATTERN\n"
".TH osViewer 1  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis osViewer\\ \\-\n"
"Get the Operating System information about the farm PCs from the FMC "
"Operating System Information Servers\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis osViewer\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".DoubleOpt[] c compact\n"
".DoubleOpt[] H header\n"
".EndSynopsis\n"
".sp\n"
".Synopsis osViewer\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis osViewer\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"Get the Operating System information about the farm nodes whose hostname "
"matches the wildcard pattern \\fINODE_PATTERN\\fP, specified using the "
"\\fB-m\\fP \\fINODE_PATTERN\\fP command line option (\\fBdefault\\fP: all "
"the nodes registered with the DIM name server \\fIDIM_DNS_NODE\\fP). "
"Information includes:\n"
".TP\n"
"\\fBName\\fP (string)\n"
"The OS name (e.g.: \"Linux\").\n"
".TP\n"
"\\fBDistribution\\fP (string)\n"
"The OS distribution (e.g.: \"Scientific Linux CERN SLC release 4.6 "
"(Beryllium)\").\n"
".TP\n"
"\\fBKernel\\fP (string)\n"
"The OS kernel (e.g.: \"2.6.9-55.0.12.EL.cernsmp\").\n"
".TP\n"
"\\fBRelease\\fP (string)\n"
"The OS release (e.g.: \"#1 SMP Mon Nov 5 17:20:17 CET 2007\").\n"
".TP\n"
"\\fBMachine\\fP (string)\n"
"The machine architecture (e.g.: \"x86_64\", \"i686\").\n"
".TP\n"
"\\fBLastBootUpTime\\fP (string)\n"
"The date and time of the last reboot (e.g.: \"2012 Apr 19 Thu 09:20:09 CEST\").\n"
".TP\n"
"\\fBLocalDateTime\\fP (string)\n"
"The local date and time of the node (e.g.: \"2012 May 08 Tue 16:38:29 CEST\").\n"
".PP\n"
"The DIM Name Server \\fIDIM_DNS_NODE\\fP, looked up to seek for FMC OS "
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
"Print only the Operating System information of the farm nodes whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: print the Operating System information of all the "
"nodes registered with the DIM name server \\fIDIM_DNS_NODE\\fP).\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC OS Information Server version "
"and the FMC version, than terminate.\n"
".\n"
".OptDef c compact\n"
"Print the Operating System information in compact format (easier to parse): "
"one farm node per line, no labels, spaces inside fields replaced by "
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
"Print the Operating System information of all the farm nodes:\n"
".PP\n"
".ShellCommand osViewer\n"
".PP\n"
".\n"
"Print output in compact format with header:\n"
".PP\n"
".ShellCommand osViewer -c -H\n"
".ShellCommand osViewer -cH\n"
".ShellCommand osViewer --comp --hea\n"
".PP\n"
".\n"
"Print only the Operating System information of selected nodes:\n"
".PP\n"
".ShellCommand osViewer -m farm0101 -m farm0102\n"
".ShellCommand osViewer -host farm0101 -host farm0102\n"
".PP\n"
".\n"
"Print only the Operating System information of wildcard-selected nodes:\n"
".PP\n"
".ShellCommand osViewer -m \\[dq]farm01*\\[dq] -m \\[dq]farm02*\\[dq]\n"
".ShellCommand osViewer -m \\[dq]farm01?[1357]\\[dq]\n"
".ShellCommand osViewer -m \\[dq]farm01?[1-5]\\[dq]\n"
".ShellCommand osViewer --host \\[dq]farm01[0-5][1-36-9]\\[dq]\n"
".PP\n"
".\n"
"Print the Server version and the FMC package version of the running FMC "
"Operating System Information servers registered with the current DIM name "
"server:\n"
".PP\n"
".ShellCommand osViewer -V\n"
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
".BR \\%%osSrv (8).\n"
".br\n"
".BR \\%%uname (1),\n"
".BR \\%%uname (2).\n"
".br\n"
".BR \\%%/proc/sys/kernel/ostype,\n"
".BR \\%%/proc/sys/kernel/osrelease,\n"
".BR \\%%/proc/sys/kernel/version,\n"
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
/* ######################################################################### */
