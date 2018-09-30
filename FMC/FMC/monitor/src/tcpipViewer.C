/*****************************************************************************/
/*
 * $Log: tcpipViewer.C,v $
 * Revision 1.11  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.10  2008/10/22 10:02:42  galli
 * minor changes
 *
 * Revision 1.8  2007/12/24 00:13:35  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.7  2005/09/20 10:39:22  galli
 * default: contact all hosts
 *
 * Revision 1.5  2005/09/13 12:12:45  galli
 * accept wildcards in hostname
 * print time since start in average and maximum compact view
 *
 * Revision 1.4  2005/09/12 22:47:36  galli
 * usage() modified
 *
 * Revision 1.3  2005/09/12 22:35:47  galli
 * added cmdline switches to print in compact format
 *
 * Revision 1.2  2005/09/11 23:54:15  galli
 * cmdline syntax changed
 *
 * Revision 1.1  2005/08/27 08:10:27  galli
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
#define SRV_NAME "tcpip"
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: tcpipViewer.C,v 1.11 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
const char *srvName="tcpip";
/*****************************************************************************/
void usage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j;
  /* pointers */
  char *p=NULL;
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
  float nolink10[10]={-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0};
  float nolink20[20]={-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,
                      -1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0};
  int type=0;
  char *format=NULL;
  char *successSvc=NULL;
  char *successSvcDup=NULL;
  char *svc=NULL;
  int svcLen=0;
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  int success=-1;
  char *tss=NULL;
  char *tssc=NULL;
  int tssd,tssh,tssm,tsss,tssmils;
  DimCurrentInfo *Rate=NULL,*Ratio=NULL;
  DimCurrentInfo *Arate=NULL,*Aratio=NULL;
  DimCurrentInfo *Mrate=NULL,*Mratio=NULL;
  float *rate=NULL;
  float *arate=NULL;
  float *mrate=NULL;
  float *ratio=NULL;
  float *aratio=NULL;
  float *mratio=NULL;
  /* printing variables */
  static int isFirstTime=1;
  const char *nrate[10]={"InReceivesRate      ","InDeliversRate      ",
                         "ForwDatagramsRate   ","OutRequestsRate     ",
                         "ReasmReqdsRate      ","FragReqdsRate       ",
                         "InSegsRate          ","OutSegsRate         ",
                         "InDatagramsRate     ","OutDatagramsRate    "};
  const char *nratio[20]={"InHdrErrorsFrac     ","InAddrErrorsFrac    ",
                          "InUnknownProtosFrac ","InDiscardsFrac      ",
                          "ForwDatagramsFrac   ","InDeliversFrac      ",
                          "ReasmReqdsFrac      ","ReasmTimeoutFrac    ",
                          "ReasmFailsFrac      ","ReasmOKsFrac        ",
                          "OutDiscardsFrac     ","OutNoRoutesFrac     ",
                          "FragReqdsFrac       ","FragFailsFrac       ",
                          "FragCreatesFrac     ","RetransSegsFrac     ",
                          "OutRstsFrac         ","InErrsFrac          ",
                          "NoPortsFrac         ","InErrorsFrac        "};
  char *line=NULL;
  int lineLen=0;
  /* command line switch */
  int doPrintAve=0;
  int doPrintMax=0;
  int doPrintHeader=0;
  int compactFormat=0;
  int doSuppressZeros=0;
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
    {"no-zero",no_argument,NULL,'z'},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  while((flag=getopt_long(argc,argv,"m:vVN:hcHaAMz",longOptions,NULL))!=EOF)
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
      case 'z': /* -z, --no-zero */
        doSuppressZeros=1;
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
  /* compose line separator */
  //lineLen=36;
  lineLen=55;
  if(doPrintAve)lineLen+=10;
  if(doPrintMax)lineLen+=10;
  line=(char*)realloc(line,(1+lineLen)*sizeof(char));
  memset(line,'-',lineLen);
  line[lineLen]='\0';
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
        /* read Success. If fails then skip the node */
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,successSvc);
        DimCurrentInfo Success(successSvc,-1);
        success=(int)Success.getInt();
        if(success!=1)
        {
          fprintf(stderr,"Service \"%s\" unreachable!\n",successSvc);
          continue;
        }
        /*...................................................................*/
        if(doPrintMax||doPrintAve)
        {
          /* read Time Since Start/Reset */
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/tss",SVC_HEAD,ucNodeFound,srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/tss",SVC_HEAD,ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          DimCurrentInfo Tss(svc,(char*)"Server unreachable!");
          tss=strdup((char*)Tss.getString());
          /* tssc = compact time string [ddd-hh:mm:ss.sss] */
          tssc=(char*)malloc(1+strlen(tss)-7);
          sscanf(tss,"%dd,%dh,%dm,%d.%ds",&tssd,&tssh,&tssm,&tsss,&tssmils);
          sprintf(tssc,"%03d-%02d:%02d:%02d.%03d",tssd,tssh,tssm,tsss,tssmils);
        }
        /*...................................................................*/
        /* read data */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/data-rate",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/data-rate",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        Rate=new DimCurrentInfo(svc,nolink10,10*sizeof(float));
        rate=(float*)Rate->getData();
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/data-ratio",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/data-ratio",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        Ratio=new DimCurrentInfo(svc,nolink20,20*sizeof(float));
        ratio=(float*)Ratio->getData();
        if(doPrintAve)
        {
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/data-arate",SVC_HEAD,ucNodeFound,srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/data-arate",SVC_HEAD,ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          Arate=new DimCurrentInfo(svc,nolink10,10*sizeof(float));
          arate=(float*)Arate->getData();
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/data-aratio",SVC_HEAD,ucNodeFound,srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/data-aratio",SVC_HEAD,ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          Aratio=new DimCurrentInfo(svc,nolink20,20*sizeof(float));
          aratio=(float*)Aratio->getData();
        }
        if(doPrintMax)
        {
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/data-mrate",SVC_HEAD,ucNodeFound,srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/data-mrate",SVC_HEAD,ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          Mrate=new DimCurrentInfo(svc,nolink10,10*sizeof(float));
          mrate=(float*)Mrate->getData();
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/data-mratio",SVC_HEAD,ucNodeFound,srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/data-mratio",SVC_HEAD,ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          Mratio=new DimCurrentInfo(svc,nolink20,20*sizeof(float));
          mratio=(float*)Mratio->getData();
        }
        /*...................................................................*/
        /* print data */
        if(!compactFormat)
        {
          if(doPrintAve||doPrintMax)
          {
            printf("    NODE: %s. Elapsed time: %s.\n",nodeFound,tss);
          }
          else
          {
            printf("    NODE: %s.\n",nodeFound);
          }
          printf("    %s\n",line);
          printf("    Ip rates                     Instant");
          if(doPrintAve)printf("   Average");
          if(doPrintMax)printf("   Maximum");
          printf("\n");
          for(i=0;i<6;i++)
          {
            if(!doSuppressZeros || 
               i==0 || i==1 || i==3 ||
               rate[i] ||
               (doPrintAve && arate[i]) ||
               (doPrintMax && mrate[i]))
            {
              printf("    %2d %s    %8.3e",i,nrate[i],rate[i]);
              if(doPrintAve)printf(" %8.3e",arate[i]);
              if(doPrintMax)printf(" %8.3e",mrate[i]);
              if(i==4)printf(" fragments/s");
              else printf(" datagrams/s");
              printf("\n");
            }
          }
          printf("    Ip ratios                    Instant");
          if(doPrintAve)printf("   Average");
          if(doPrintMax)printf("   Maximum");
          printf("\n");
          for(i=0;i<15;i++)
          {
            if(!doSuppressZeros ||
               i==5 ||
               ratio[i] ||
               (doPrintAve && aratio[i]) ||
               (doPrintMax && mratio[i]))
            {
              printf("    %2d %s    %8.3e",i,nratio[i],ratio[i]);
              if(doPrintAve)printf(" %8.3e",aratio[i]);
              if(doPrintMax)printf(" %8.3e",mratio[i]);
              if(i==6||i==14)printf(" fragments/datagram");
              printf("\n");
            }
          }
          printf("    %s\n",line);
          printf("    TCP rates:                   Instant");
          if(doPrintAve)printf("   Average");
          if(doPrintMax)printf("   Maximum");
          printf("\n");
          for(i=6;i<8;i++)
          {
            printf("    %2d %s    %8.3e",i-6,nrate[i],rate[i]);
            if(doPrintAve)printf(" %8.3e",arate[i]);
            if(doPrintMax)printf(" %8.3e",mrate[i]);
            printf(" segments/s");
            printf("\n");
          }
          if(!doSuppressZeros ||
             ratio[15] || ratio[16] || ratio[17] ||
             (doPrintAve && (aratio[15] || aratio[16] || aratio[17])) ||
             (doPrintMax && (mratio[15] || mratio[16] || mratio[17])))
          {
            printf("    TCP ratios:                  Instant");
            if(doPrintAve)printf("   Average");
            if(doPrintMax)printf("   Maximum");
            printf("\n");
          }
          for(i=15;i<18;i++)
          {
            if(!doSuppressZeros ||
               ratio[i] ||
               (doPrintAve && aratio[i]) ||
               (doPrintMax && mratio[i]))
            {
              printf("    %2d %s    %8.3e",i-15,nratio[i],ratio[i]);
              if(doPrintAve)printf(" %8.3e",aratio[i]);
              if(doPrintMax)printf(" %8.3e",mratio[i]);
              printf("\n");
            }
          }
          printf("    %s\n",line);
          printf("    UDP rates:                   Instant");
          if(doPrintAve)printf("   Average");
          if(doPrintMax)printf("   Maximum");
          printf("\n");
          for(i=8;i<10;i++)
          {
            printf("    %2d %s    %8.3e",i-8,nrate[i],rate[i]);
            if(doPrintAve)printf(" %8.3e",arate[i]);
            if(doPrintMax)printf(" %8.3e",mrate[i]);
            printf(" datagrams/s");
            printf("\n");
          }
          if(!doSuppressZeros ||
             ratio[18] || ratio[19] ||
             (doPrintAve && (aratio[18] || aratio[19])) ||
             (doPrintMax && (mratio[18] || mratio[19])))
          {
            printf("    UDP ratios:                  Instant");
            if(doPrintAve)printf("   Average");
            if(doPrintMax)printf("   Maximum");
            printf("\n");
          }
          for(i=18;i<20;i++)
          {
            if(!doSuppressZeros ||
               ratio[i] ||
               (doPrintAve && aratio[i]) ||
               (doPrintMax && mratio[i]))
            {
              printf("    %2d %s    %8.3e",i-18,nratio[i],ratio[i]);
              if(doPrintAve)printf(" %8.3e",aratio[i]);
              if(doPrintMax)printf(" %8.3e",mratio[i]);
              printf("\n");
            }
          }
          printf("    %s\n",line);
        }
        else                                                /* compactFormat */
        {
          if(doPrintHeader && isFirstTime)
          {
            char temp[80];
            isFirstTime=0;
            printf("NODE:");
            for(i=0;i<10;i++)
            {
              /* suppress the trailing spaces */
              strcpy(temp,nrate[i]);
              *strchr(temp,' ')='\0';
              printf(" %s",temp);
            }
            for(i=0;i<20;i++)
            {
              /* chop off the trailing spaces */
              strcpy(temp,nratio[i]);
              *strchr(temp,' ')='\0';
              printf(" %s",temp);
            }
            if(doPrintAve)
            {
              for(i=0;i<10;i++)
              {
                /* suppress the trailing spaces */
                strcpy(temp,nrate[i]);
                *strchr(temp,' ')='\0';
                printf(" ave_%s",temp);
              }
              for(i=0;i<20;i++)
              {
                /* chop off the trailing spaces */
                strcpy(temp,nratio[i]);
                *strchr(temp,' ')='\0';
                printf(" ave_%s",temp);
              }
            }                                              /* if(doPrintAve) */
            if(doPrintMax)
            {
              for(i=0;i<10;i++)
              {
                /* suppress the trailing spaces */
                strcpy(temp,nrate[i]);
                *strchr(temp,' ')='\0';
                printf(" max_%s",temp);
              }
              for(i=0;i<20;i++)
              {
                /* chop off the trailing spaces */
                strcpy(temp,nratio[i]);
                *strchr(temp,' ')='\0';
                printf(" max_%s",temp);
              }
            }                                              /* if(doPrintMax) */
            printf("\n");
          }
          printf("%s:",nodeFound);
          for(i=0;i<10;i++)printf(" %8.3e",rate[i]);
          for(i=0;i<20;i++)printf(" %8.3e",ratio[i]);
          if(doPrintAve)
          {
            for(i=0;i<10;i++)printf(" %8.3e",arate[i]);
            for(i=0;i<20;i++)printf(" %8.3e",aratio[i]);
          }
          if(doPrintMax)
          {
            for(i=0;i<10;i++)printf(" %8.3e",mrate[i]);
            for(i=0;i<20;i++)printf(" %8.3e",mratio[i]);
          }
          if(doPrintAve||doPrintMax)printf(" %s",tssc);
          printf("\n");
        }                                                   /* compactFormat */
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
"       tcpipViewer - Get the TCP/IP stack statistics from the FMC TCP/IP\n"
"                     Stack Monitor Servers (tcpipSrv)\n"
"\n"
"SYNOPSIS\n"
"       tcpipViewer [-N|--dim-dns DIM_DNS_NODE] [-v|--debug]\n"
"                   [-m|--hostname NODE_PATTERN...]\n"
"                   [-c|--compact [-H|--header]]\n"
"                   [-A|--average] [-M|--maximum] [-a|--all]\n"
"                   [-z|--no-zero]\n"
"       tcpipViewer {-V,--version} [-m|--hostname NODE_PATTERN...]\n"
"       tcpipViewer {-h,--help}\n"
"\n"
"DESCRIPTION\n"
"       Get the TCP/IP stack statistics from the FMC TCP/IP Stack\n"
"       Monitor Servers of the farm nodes whose hostname matches the\n"
"       wildcard pattern NODE_PATTERN, specified using the -m command line\n"
"       option. In addition to instant values, average and maximum values can\n"
"       be printed by means of --average and --maximum switches, together\n"
"       with the time elapsed since the server start-up or since last server\n"
"       reset (i.e. reset of average and maximum values). Server reset is\n"
"       achieved through the tcpipReset(FMC) command.\n"
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
"              For each selected node print the FMC TCP/IP stack Monitor\n"
"              server version, than exit.\n"
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
"       --no-zero, -z\n"
"              Reduce the number of zero data fields printed.\n"
"\n"
"ENVIRONMENT\n"
"       DIM_DNS_NODE (string, mandatory if not defined otherwise, see above)\n"
"              Host name of the node which is running the DIM DNS.\n"
"       LD_LIBRARY_PATH (string, mandatory if not set using ldconfig)\n"
"              Must include the path to the libraries \"libdim\"\n"
"              and \"libFMCutils\".\n"
"\n"
"EXAMPLES\n"
"       Show the TCP/IP stack statistics of all the farm nodes:\n"
"              tcpipViewer\n"
"       Show also average and maximum values:\n"
"              tcpipViewer -a\n"
"              tcpipViewer --all\n"
"       Print output in compact format with header:\n"
"              tcpipViewer -acH\n"
"              tcpipViewer -a -c -H\n"
"              tcpipViewer --all --compact --header\n"
"              tcpipViewer --al --co --hea\n"
"       Show also average values:\n"
"              tcpipViewer --average\n"
"              tcpipViewer --av\n"
"              tcpipViewer -A\n"
"       Show also maximum values:\n"
"              tcpipViewer --maximum\n"
"              tcpipViewer --ma\n"
"              tcpipViewer -M\n"
"       Reduce the number of zero data fields printed:\n"
"              tcpipViewer -az\n"
"              tcpipViewer --all --no-zero\n"
"       Show only the TCP/IP stack statistics of selected nodes:\n"
"              tcpipViewer -m farm0101 -m farm0102 --ave\n"
"       Show only the statistics of wildcard-selected nodes:\n"
"              tcpipViewer -m \"farm01*\" -m \"farm02*\" --ave --max\n"
"              tcpipViewer -m \"farm010[1357]\" --all\n" 
"       Show the server version installed on the farm nodes:\n"
"              tcpipViewer -V\n"
"              tcpipViewer --version\n"
"\n"
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       tcpipReset(FMC), tcpipSrv(FMC), netstat(8), /proc/net/snmp.\n"
"\n"
,rcsidP,FMC_VERSION
         );
  exit(1);
}
/*****************************************************************************/
