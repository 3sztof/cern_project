/*****************************************************************************/
/*
 * $Log: psViewer.C,v $
 * Revision 2.4  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 2.3  2009/02/06 16:06:13  galli
 * minor changes
 *
 * Revision 2.0  2009/01/16 21:53:59  galli
 * compatible with psSrv.c Revision 3.7
 *
 * Revision 1.17  2007/12/21 15:04:07  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.16  2007/12/12 12:33:11  galli
 * getServerVersion() changed
 *
 * Revision 1.15  2007/12/12 11:05:32  galli
 * Added cmd-line option -C|--select-cmd CMD_PATTERN
 * Added cmd-line option -U|--select-utgid UTGID_PATTERN
 *
 * Revision 1.14  2007/10/10 14:44:51  galli
 * bug fixed
 *
 * Revision 1.13  2007/09/03 22:12:45  galli
 * read types, labels and units from server
 *
 * Revision 1.11  2007/08/23 11:58:35  galli
 * option -c, --compact for compact output
 * option -H, --header for a header in compact output
 *
 * Revision 1.10  2007/08/22 13:48:35  galli
 * added function getServerVersion()
 *
 * Revision 1.9  2007/08/22 09:57:11  galli
 * minor changes
 *
 * Revision 1.8  2007/08/22 09:36:14  galli
 * usage() modified
 *
 * Revision 1.7  2007/08/22 07:37:04  galli
 * added data: pgid (alias pgrp)
 *
 * Revision 1.6  2007/08/21 13:16:02  galli
 * added data: vm_lock, vm_data, vm_stack, vm_exe, vm_lib
 *
 * Revision 1.1  2007/08/17 09:10:03  galli
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
#include <sys/wait.h>                                  /* wait(2) in usage() */
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
#include "psSrv.h"                  /* SRV_NAME, FIELD_N_ERR, MAX_HOSTNAME_L */
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: psViewer.C,v 2.4 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
int fmcVersionStr2Int(const char *s);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j,k,l,m;
  /* pointers */
  char *p,*lp,*rp,*fp,*tp,*up;
  /* service browsing */
  int hostPttnC=0;
  char **hostPttnV=NULL;
  char *hostPttn=NULL;
  int successSvcPttnLen=0;
  char *successSvcPttn=NULL;
  int hostFoundN=0;
  char host[128]="";
  char ucHost[128]="";
  int rawSuccessSvcPttnLen=0;
  char *rawSuccessSvcPttn=NULL;
  DimBrowser br;
  static char *nolinkStrArr=NULL;
  int nolinkStrArrSz=0;
  int type=0;
  char *format=NULL;
  char *successSvc=NULL;
  char *successSvcDup=NULL;
  char *svc=NULL;
  int svcLen=0;
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  int success=-1;
  char *srvFmcVersionStr=NULL;
  int srvFmcVersionCode=0;
  char *labels=NULL;
  int labelsSz=0;
  char *units=NULL;
  int unitsSz=0;
  char *types=NULL;
  int typesSz=0;
  int fieldN=0;
  int psN=0;                                               /* process number */
  int tkN=0;                                                  /* task number */
  int prN=0;                                               /* printed number */
  char *data=NULL;
  int dataSz;
  /* line separator */
  char *line=NULL;
  int lineLen=0;
  /* print format and variable */
  char funit[64];
  char pFormat[256];
  char *cmd=NULL;
  char *utgid=NULL;
  /* retrieved quantities maximum length */
  unsigned *fieldLen=NULL;
  char *fieldAlign=NULL;
  unsigned cntLen;
  /* command line switch */
  /*   standard options */
  int compactFormat=0;
  int doPrintHeader=0;
  /*   data selection options */
  int doPrintIds=0;
  int doPrintOwner=0;
  int doPrintSched=0;
  int doPrintStat=0;
  int doPrintSize=0;
  int doPrintSig=0;
  int doPrintTime=0;
  int doPrintCmd=0;
  unsigned forcedCmdlineLen=0;
  /*   process selection options */
  int doPrintThreads=0;
  char *taskType=NULL;
  int onlyUtgid=0;
  char *selectUtgid=NULL;
  char *selectCmd=NULL;
  /*   accessory options */
  int doPrintServerVersion=0;
  /* getopt */
  int flag;
  /* short command line options */
  const char *shortOptions="vN:m:cHatuU:C:Vh::";
  /* long command line options */
  static struct option longOptions[]=
  {
    /* ..................................................................... */
    /* standard options */
    {"debug",no_argument,NULL,'v'},
    {"dim-dns",required_argument,NULL,'N'},
    {"hostname",required_argument,NULL,'m'},
    {"compact",no_argument,NULL,'c'},
    {"header",no_argument,NULL,'H'},
    /* ..................................................................... */
    /* data selection options */
    {"all",no_argument,NULL,'a'},
    {"ids",no_argument,NULL,1},
    {"owner",no_argument,NULL,2},
    {"sched",no_argument,NULL,3},
    {"stat",no_argument,NULL,4},
    {"size",no_argument,NULL,5},
    {"sig",no_argument,NULL,6},
    {"time",no_argument,NULL,7},
    {"cmd",no_argument,NULL,8},
    {"cmdlen",required_argument,NULL,9},
    /* ..................................................................... */
    /* process selection options */
    {"threads",no_argument,NULL,'t'},
    {"utgid-only",no_argument,NULL,'u'},
    {"select-utgid",required_argument,NULL,'U'},
    {"select-cmd",required_argument,NULL,'C'},
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
  /*-------------------------------------------------------------------------*/
  /* process command-line arguments */
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
        hostPttnC++;
        hostPttnV=(char**)realloc(hostPttnV,hostPttnC*
                                      sizeof(char*));
        hostPttnV[hostPttnC-1]=optarg;
        break;
      case 'c': /* -c, --compact */
        compactFormat=1;
        break;
      case 'H': /* -H, --header */
        doPrintHeader=1;
        break;
      /* ................................................................... */
      /* data selection options */
      case 'a': /* -a, --all */
        doPrintIds=!doPrintIds;
        doPrintOwner=!doPrintOwner;
        doPrintSched=!doPrintSched;
        doPrintStat=!doPrintStat;
        doPrintSize=!doPrintSize;
        doPrintSig=!doPrintSig;
        doPrintTime=!doPrintTime;
        doPrintCmd=!doPrintCmd;
        break;
      case 1:   /* --ids */
        doPrintIds=!doPrintIds;
        break;
      case 2:   /* --owner */
        doPrintOwner=!doPrintOwner;
        break;
      case 3:   /* --sched */
        doPrintSched=!doPrintSched;
        break;
      case 4:   /* --stat */
        doPrintStat=!doPrintStat;
        break;
      case 5:   /* --size */
        doPrintSize=!doPrintSize;
        break;
      case 6:   /* --sig */
        doPrintSig=!doPrintSig;
        break;
      case 7:   /* --time */
        doPrintTime=!doPrintTime;
        break;
      case 8:   /* --cmd */
        doPrintCmd=!doPrintCmd;
        break;
      case 9:   /* --cmdlen */
        forcedCmdlineLen=atoi(optarg);;
        break;
      /* ................................................................... */
      /* process selection options */
      case 't': /* -t, --threads */
        doPrintThreads=1;
        break;
      case 'u': /* -u, --utgid-only */
        onlyUtgid=1;
        break;
      case 'U': /* -U, --select-utgid */
        selectUtgid=optarg;
        break;
      case 'C': /* -C, --select-cmd */
        selectCmd=optarg;
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
  if(doPrintThreads)taskType=(char*)"tasks";
  else taskType=(char*)"processes";
  /*-------------------------------------------------------------------------*/
  /* Compose DIM  no link error string array */
  nolinkStrArrSz=FIELD_N_ERR*(1+strlen("N/F"));           /* "N/F"=not found */
  nolinkStrArr=(char*)malloc(nolinkStrArrSz);
  for(i=0,p=nolinkStrArr;i<FIELD_N_ERR;i++,p+=(1+strlen(p)))strcpy(p,"N/F");
  /*-------------------------------------------------------------------------*/
  if(!compactFormat)
  {
    printf("Node pattern list:  ");
    for(i=0;i<hostPttnC;i++)                  /* loop over node patterns */
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
    /*.......................................................................*/
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
          if(compactFormat)
          {
            printf("%-*s [ERROR] Service \"%s\" unreachable!\n",MAX_HOSTNAME_L,
                   host,successSvc);
          }
          else
          {
            printf("    NODE %s: [ERROR] Service \"%s\" unreachable!\n",host,
                   successSvc);
          }
          continue;
        }
        else if(success==0)
        {
          if(compactFormat)
          {
            printf("%-*s [ERROR] Data not ready!\n",MAX_HOSTNAME_L,host);
          }
          else
          {
            printf("    NODE %s: [ERROR] Data not ready!\n",host);
          }
          continue;
        }
        /*...................................................................*/
        /* read fmc_version */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/fmc_version",SVC_HEAD,ucHost,
                          SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/fmc_version",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo FmcVersion(svc,-1);
        srvFmcVersionStr=(char*)FmcVersion.getString();
        srvFmcVersionCode=fmcVersionStr2Int(srvFmcVersionStr);
        if(srvFmcVersionCode<fmcVersionStr2Int("3.9.6"))
        {
          if(compactFormat)
          {
            printf("%-*s [ERROR] Obsolete psSrv Server (FMC-%s < FMC-3.9.6)!"
                   "\n",MAX_HOSTNAME_L,host,srvFmcVersionStr);
          }
          else
          {
            printf("    NODE %s: [ERROR] Obsolete psSrv Server (FMC-%s < "
                   "FMC-3.9.6)!\n",host,srvFmcVersionStr);
          }
          continue;
        }
        /*...................................................................*/
        /* read nprocs */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/nprocs",SVC_HEAD,ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/nprocs",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Nprocs(svc,-1);
        psN=(int)Nprocs.getInt();
        /*...................................................................*/
        /* read ntasks */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/ntasks",SVC_HEAD,ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/ntasks",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Ntasks(svc,-1);
        tkN=(int)Ntasks.getInt();
        /*...................................................................*/
        /* read labels */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/details/labels",SVC_HEAD,
                          ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/details/labels",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Labels(svc,nolinkStrArr,nolinkStrArrSz);
        labels=(char*)Labels.getData();
        labelsSz=(int)Labels.getSize();
        /*...................................................................*/
        /* read units */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/details/units",SVC_HEAD,
                          ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/details/units",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Units(svc,nolinkStrArr,nolinkStrArrSz);
        units=(char*)Units.getData();
        unitsSz=(int)Units.getSize();
        /*...................................................................*/
        /* read types */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/details/types",SVC_HEAD,
                          ucHost,SRV_NAME);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/details/types",SVC_HEAD,ucHost,SRV_NAME);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Types(svc,nolinkStrArr,nolinkStrArrSz);
        types=(char*)Types.getData();
        typesSz=(int)Types.getSize();
        /*...................................................................*/
        /* read data */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/details/%s/data",SVC_HEAD,ucHost,
                          SRV_NAME,taskType);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/details/%s/data",SVC_HEAD,ucHost,SRV_NAME,
                taskType);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Data(svc,nolinkStrArr,nolinkStrArrSz);
        data=(char*)Data.getData();
        dataSz=(int)Data.getSize();
        /*...................................................................*/
        /* evaluate field number (fieldN) */
        for(p=labels,fieldN=0;p<labels+labelsSz;fieldN++)p+=(1+strlen(p));
        /*...................................................................*/
        /* Evaluate the printed task number (prN) which should be equal      */
        /* either to psN or to tkN, but in fact can be slightly different    */
        /* since services are updated at different times.                    */
        for(p=data,prN=0;p<data+dataSz;prN++)p+=(1+strlen(p));
        prN/=fieldN;
        /*...................................................................*/
        /* Correct psN or tkN                                                */
        if(doPrintThreads)tkN=prN;                        /* print task data */
        else psN=prN;                                  /* print process data */
        /*...................................................................*/
        if(!compactFormat)
        {
          printf("    NODE %s: process number: %d;  task number = %d.\n",host,
                 psN,tkN);
        }
        /*...................................................................*/
        /* allocate space for maximum string lengths and alignment */
        fieldLen=(unsigned*)realloc(fieldLen,fieldN*sizeof(unsigned));
        memset(fieldLen,0,fieldN*sizeof(unsigned));
        fieldAlign=(char*)realloc(fieldAlign,fieldN*sizeof(int));
        memset(fieldAlign,0,fieldN*sizeof(char));
        /*...................................................................*/
        /* 1 - evaluate fields length (fieldLen[fieldN]) */
        for(k=0,p=labels;k<fieldN;k++)        /* loop over labels SVC fields */
        {
          if(fieldLen[k]<strlen(p))fieldLen[k]=strlen(p);
          p+=(1+strlen(p));
        }                                     /* loop over labels SVC fields */
        for(k=0,p=units;k<fieldN;k++)          /* loop over units SVC fields */
        {
          if(fieldLen[k]<2+strlen(p))fieldLen[k]=2+strlen(p);          /* [] */
          p+=(1+strlen(p));
        }                                      /* loop over units SVC fields */
        for(l=0,m=0,rp=data;l<prN;l++)         /* loop over data SVC records */
        {
          /* find UTGID */
          utgid=(char*)"N/A";
          for(k=0,fp=rp,lp=labels;k<fieldN;k++) /* loop over data SVC fields */
          {
            if(!strcmp(lp,"UTGID"))
            {
              utgid=fp;
              break;
            }
            fp+=(1+strlen(fp));
            lp+=(1+strlen(lp));
          }                                     /* loop over data SVC fields */
          /* find CMD */
          cmd=(char*)"N/A";
          for(k=0,fp=rp,lp=labels;k<fieldN;k++) /* loop over data SVC fields */
          {
            if(!strcmp(lp,"CMD"))
            {
              cmd=fp;
              break;
            }
            fp+=(1+strlen(fp));
            lp+=(1+strlen(lp));
          }                                     /* loop over data SVC fields */
          /* select processes */
          if(onlyUtgid && !strcmp(utgid,"N/A"))
          {
            /* skip process */
            for(k=0,fp=rp;k<fieldN;k++)fp+=(1+strlen(fp));
          }
          else if(selectUtgid && fnmatch(selectUtgid,utgid,0))
          {
            /* skip process */
            for(k=0,fp=rp;k<fieldN;k++)fp+=(1+strlen(fp));
          }
          else if(selectCmd && fnmatch(selectCmd,cmd,0))
          {
            /* skip process */
            for(k=0,fp=rp;k<fieldN;k++)fp+=(1+strlen(fp));
          }
          else                                      /* process to be printed */
          {
            for(k=0,fp=rp;k<fieldN;k++)         /* loop over data SVC fields */
            {
              if(fieldLen[k]<strlen(fp))fieldLen[k]=strlen(fp);
              fp+=(1+strlen(fp));
            }                                   /* loop over data SVC fields */
            m++;
          }                                         /* process to be printed */
          rp=fp;
        }                                      /* loop over data SVC records */
        /* limit CMDLINE length */
        for(k=0,lp=labels;k<fieldN;k++)        /* loop over label SVC fields */
        {
          if(!strcmp(lp,"CMDLINE"))
          {
            if(doPrintCmd && forcedCmdlineLen)fieldLen[k]=forcedCmdlineLen;
            if(fieldLen[k]<strlen(lp))fieldLen[k]=strlen(lp);
            break;
          }
          lp+=(1+strlen(lp));
        }                                      /* loop over label SVC fields */
        /* counter length */
        cntLen=snprintf(NULL,0,"%d",m-1);
        /*...................................................................*/
        /* 2 - evaluate fields alignment (left for strings, right for nums) */
        for(k=0,p=data;k<fieldN;k++) /* loop over 1st record data SVC fields */
        {
          char *endP;
          strtod(p,&endP);                /* try to convert string to double */
          if(p!=endP && *endP=='\0')
          {
            fieldAlign[k]='R';                       /* success: align right */
          }
          else                         /* failed to convert string to double */
          {
            strtol(p,&endP,16);             /* try to convert string to long */
            if(p!=endP && *endP=='\0')
            {
              fieldAlign[k]='R';                     /* success: align right */
            }
            else
            {
              fieldAlign[k]='L';    /* failed to convert to long: align left */
            }
          }
          p+=(1+strlen(p));
        }                            /* loop over 1st record data SVC fields */
        /*...................................................................*/
        /* 3 - evaluate line separator length */
        lineLen=0;
        lineLen+=cntLen;
        for(k=0,tp=types;k<fieldN;k++)          /* loop over type SVC fields */
        {
          if(!strcmp(tp,"basic")||
             (doPrintIds && !strcmp(tp,"ids"))||
             (doPrintOwner && !strcmp(tp,"owner"))||
             (doPrintSched && !strcmp(tp,"sched"))||
             (doPrintStat && !strcmp(tp,"stat"))||
             (doPrintSize && !strcmp(tp,"size"))||
             (doPrintSig && !strcmp(tp,"signal"))||
             (doPrintTime && !strcmp(tp,"time"))||
             (doPrintCmd && !strcmp(tp,"cmdline")))
          {
            lineLen+=(1+fieldLen[k]);
          }
          tp+=(1+strlen(tp));
        }                                       /* loop over type SVC fields */
        /* compose line separator */
        line=(char*)realloc(line,(1+lineLen)*sizeof(char));
        memset(line,'-',lineLen);
        line[lineLen]='\0';
        /*...................................................................*/
        /* 4 - print header */
        if(!compactFormat)printf("%s\n",line);
        if(!compactFormat || doPrintHeader)
        {
          /* label line */
          if(compactFormat)
          {
            /* hostname */
            sprintf(pFormat,"%%-%ds ",MAX_HOSTNAME_L);
            printf(pFormat,"HOSTNAME");
          }
          /* process counter */
          sprintf(pFormat,"%%%ds",cntLen);
          printf(pFormat,"#");
          for(k=0,lp=labels,tp=types;k<fieldN;k++)  /* over label SVC fields */
          {
            if(!strcmp(tp,"basic")||
               (doPrintIds && !strcmp(tp,"ids"))||
               (doPrintOwner && !strcmp(tp,"owner"))||
               (doPrintSched && !strcmp(tp,"sched"))||
               (doPrintStat && !strcmp(tp,"stat"))||
               (doPrintSize && !strcmp(tp,"size"))||
               (doPrintSig && !strcmp(tp,"signal"))||
               (doPrintTime && !strcmp(tp,"time"))||
               (doPrintCmd && !strcmp(tp,"cmdline")))
            {
              if(fieldAlign[k]=='L')sprintf(pFormat," %%-%ds",fieldLen[k]);
              else sprintf(pFormat," %%%ds",fieldLen[k]);
              /* avoid spaces to facilitate output parsing */
              for(p=lp;*p;p++)if(*p==' ')*p='-';
              printf(pFormat,lp);
            }
            lp+=(1+strlen(lp));
            tp+=(1+strlen(tp));
          }                                    /* loop over label SVC fields */
          printf("\n");
          /* units line */
          if(compactFormat)
          {
            /* hostname */
            sprintf(pFormat,"%%-%ds ",MAX_HOSTNAME_L);
            printf(pFormat,"");
          }
          /* process counter */
          sprintf(pFormat,"%%%ds",cntLen);
          printf(pFormat,"");
          for(k=0,up=units,tp=types;k<fieldN;k++)   /* over label SVC fields */
          {
            if(!strcmp(tp,"basic")||
               (doPrintIds && !strcmp(tp,"ids"))||
               (doPrintOwner && !strcmp(tp,"owner"))||
               (doPrintSched && !strcmp(tp,"sched"))||
               (doPrintStat && !strcmp(tp,"stat"))||
               (doPrintSize && !strcmp(tp,"size"))||
               (doPrintSig && !strcmp(tp,"signal"))||
               (doPrintTime && !strcmp(tp,"time"))||
               (doPrintCmd && !strcmp(tp,"cmdline")))
            {
              if(fieldAlign[k]=='L')sprintf(pFormat," %%-%ds",fieldLen[k]);
              else sprintf(pFormat," %%%ds",fieldLen[k]);
              if(*up!='\0')sprintf(funit,"[%s]",up);
              else funit[0]='\0';
              if(compactFormat)for(p=funit;*p;p++)if(*p==' ')*p='-';
              printf(pFormat,funit);
            }
            up+=(1+strlen(up));
            tp+=(1+strlen(tp));
          }                                    /* loop over label SVC fields */
          printf("\n");
        }                             /* if(!compactFormat || doPrintHeader) */
        if(!compactFormat)printf("%s\n",line);
        /*...................................................................*/
        /* 5 - print data */
        for(l=0,m=0,rp=data;l<prN;l++) /* loop over data SVC records (procs) */
        {
          /* find UTGID */
          utgid=(char*)"N/A";
          for(k=0,fp=rp,lp=labels;k<fieldN;k++)     /* over label SVC fields */
          {
            if(!strcmp(lp,"UTGID"))
            {
              utgid=fp;
              break;
            }
            fp+=(1+strlen(fp));
            lp+=(1+strlen(lp));
          }                                         /* over label SVC fields */
          /* find CMD */
          cmd=(char*)"N/A";
          for(k=0,fp=rp,lp=labels;k<fieldN;k++)     /* over label SVC fields */
          {
            if(!strcmp(lp,"CMD"))
            {
              cmd=fp;
              break;
            }
            fp+=(1+strlen(fp));
            lp+=(1+strlen(lp));
          }                                         /* over label SVC fields */
          if(onlyUtgid && !strcmp(utgid,"N/A"))
          {
            /* skip process */
            for(k=0,fp=rp;k<fieldN;k++)fp+=(1+strlen(fp));
          }
          else if(selectUtgid && fnmatch(selectUtgid,utgid,0))
          {
            /* skip process */
            for(k=0,fp=rp;k<fieldN;k++)fp+=(1+strlen(fp));
          }
          else if(selectCmd && fnmatch(selectCmd,cmd,0))
          {
            /* skip process */
            for(k=0,fp=rp;k<fieldN;k++)fp+=(1+strlen(fp));
          }
          else
          {
            if(compactFormat)
            {
              /* hostname */
              sprintf(pFormat,"%%-%ds ",MAX_HOSTNAME_L);
              printf(pFormat,host);
            }
            /* process counter */
            sprintf(pFormat,"%%%dd",cntLen);
            printf(pFormat,m);
            for(k=0,fp=rp,tp=types;k<fieldN;k++)/* loop over type SVC fields */
            {
              if(!strcmp(tp,"basic")||
                 (doPrintIds && !strcmp(tp,"ids"))||
                 (doPrintOwner && !strcmp(tp,"owner"))||
                 (doPrintSched && !strcmp(tp,"sched"))||
                 (doPrintStat && !strcmp(tp,"stat"))||
                 (doPrintSize && !strcmp(tp,"size"))||
                 (doPrintSig && !strcmp(tp,"signal"))||
                 (doPrintTime && !strcmp(tp,"time")))
              {
                if(fieldAlign[k]=='L')sprintf(pFormat," %%-%ds",fieldLen[k]);
                else sprintf(pFormat," %%%ds",fieldLen[k]);
                /* avoid spaces to facilitate output parsing */
                if(compactFormat && fieldAlign[k]=='L')
                  for(p=fp;*p;p++)if(*p==' ')*p='-';
                printf(pFormat,fp);
              }
              if(doPrintCmd && !strcmp(tp,"cmdline"))
              {
                if(!forcedCmdlineLen)printf(" %s",fp);
                else
                {
                  /* cut string at fieldLen[k] */
                  sprintf(pFormat," %%-%d.%ds",fieldLen[k],fieldLen[k]);
                  printf(pFormat,fp);
                }
              }
              fp+=(1+strlen(fp));
              tp+=(1+strlen(tp));
            }                                   /* loop over type SVC fields */ 
            printf("\n");
            m++;
          }                         /* if(!onlyUtgid || strcmp(utgid,"N/A")) */
          rp=fp;
        }                          /* loop over data SVC records (processes) */
        /*...................................................................*/
        if(!compactFormat)printf("%s\n",line);
      }                         /* if(!fnmatch(successSvcPttn,successSvc,0)) */
    }                                                     /* loop over nodes */
    /*.......................................................................*/
    if(!hostFoundN)
    {
      printf("    No node found for pattern: \"%s\"!\n",hostPttnV[i]);
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
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"","procs",
                               deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
int fmcVersionStr2Int(const char *s)
{
  int code=0;
  char str[32];
  char *b,*e;
  /*-------------------------------------------------------------------------*/
  if(!s)return 0;
  snprintf(str,32,"%s",s);                               /* duplicate string */
  b=str;
  e=strchr(b,'.');
  if(!e)return 0;
  else *e='\0';
  code+=1000000*atoi(b);
  b=e+1;
  e=strchr(b,'.');
  if(!e)return 0;
  else *e='\0';
  code+=1000*atoi(b);
  b=e+1;
  e=(char*)memchr(b,'\0',4);
  if(!e)return 0;
  if(b==e)return 0;
  code+=atoi(b);
  return code;
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"psViewer [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"         [-m | --hostname NODE_PATTERN...] [-c | --compact]\n"
"         [-H | --header] [-a | --all] [--ids] [--owner] [--sched]\n"
"         [--stat] [--size] [--sig] [--time] [--cmd] [--cmdlen LEN]\n"
"         [-t | --threads] [-u | --utgid-only]\n"
"         [-U | --select-utgid UTGID_PATTERN]\n"
"         [-C | --select-cmd CMD_PATTERN]\n"
"psViewer { -V | --version } [-v | --debug]\n"
"         [-N | --dim-dns DIM_DNS_NODE]\n"
"         [-m | --hostname NODE_PATTERN...]\n"
"psViewer { -h | --help }\n"
"\n"
"Try \"psViewer -h\" for more information.\n";
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
"psViewer.man\n"
"\n"
"..\n"
"%s"
".hw hostname NODE_PATTERN\n"
".TH psViewer 1  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis psViewer\\ \\-\n"
"Get information and statistics about the processes or the tasks running on the farm nodes from the FMC Process Monitor "
"Servers\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis psViewer\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".DoubleOpt[] c compact\n"
".DoubleOpt[] H header\n"
".DoubleOpt[] a all\n"
".LongOpt[] ids\n"
".LongOpt[] owner\n"
".LongOpt[] sched\n"
".LongOpt[] stat\n"
".LongOpt[] size\n"
".LongOpt[] sig\n"
".LongOpt[] time\n"
".LongOpt[] cmd\n"
".LongOpt[] cmdlen LEN\n"
".DoubleOpt[] t threads\n"
".DoubleOpt[] u utgid\\[hy]only\n"
".DoubleOpt[] U select\\[hy]utgid UTGID_PATTERN\n"
".DoubleOpt[] C select\\[hy]cmd CMD_PATTERN\n"
".EndSynopsis\n"
".sp\n"
".Synopsis psViewer\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis psViewer\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"Get the information and the statistics about the \\fBprocesses\\fP -- if the "
"command line option \\fB-t\\fP is not specified -- or about the "
"\\fBthreads\\fP (aka \\fBlight-weight processes\\fP or \\fBtasks\\fP) -- if "
"the command line option \\fB-t\\fP is specified -- running on the farm nodes "
"whose hostname matches at least one of the wildcard patterns "
"\\fINODE_PATTERN\\fP, specified using the \\fB-m\\fP \\fINODE_PATTERN\\fP "
"command line option (\\fBdefault\\fP: all the nodes registered with the DIM "
"name server \\fIDIM_DNS_NODE\\fP).\n"
".PP\n"
"Thread-specifid cumulative data (e.g. CPU usage or minor/major page fault "
"rate) refer to the single thread if threads are printed, while are the "
"\\fBsum\\fP over the component threads if the processes are printed.\n"
".PP\n"
"Basic information (provided without any other command line option) "
"includes 6 fields:\n"
".TP\n"
"\\fBCMD\\fP (alias COMM, COMMAND, UCMD, UCOMM)\n"
"Command name, i.e. the basename (w/o path) of the executable image file, "
"without arguments, eventually truncated at 15 characters.\n"
".TP\n"
"\\fBUTGID\\fP\n"
"FMC User-assigned unique Thread Group Identifier, if available (e.g. if the "
"process has been started by the FMC Task Manager or has the variable UTGID "
"defined in the process environment). Otherwise \"N/A\" is printed.\n"
".TP\n"
"\\fBTID\\fP (alias LWP, SPID)\n"
"Thread Identifier, aka Light Weight Process Identifier. The \\fBTID\\fP of "
"the thread group leader is equal to the \\fBTGID\\fP (Thread Group "
"Identifier, the old \\fBPID\\fP). When processes (as opposite to threads) "
"are printed, \\fBTID\\fP always coincide with \\fBTGID\\fP (\\fBPID\\fP).\n"
".TP\n"
"\\fBUSER\\fP (alias EUSER, UNAME)\n"
"\\fBEffective\\fP user name. Used for all the security checks. The only user "
"name of the process that normally has any effect.\n"
".TP\n"
"\\fBTTY\\fP (alias TT, TNAME)\n"
"The name of the controlling tty (terminal) if available, a question mark "
"(\\fB?\\fP) otherwise.\n"
".TP\n"
"\\fBNLWP\\fP (alias THCNT)\n"
"Number of Lightweight Processes (Threads) in the process (thread group).\n"
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
"Print only the process/task information of the farm nodes whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: print the process/task information of all the "
"nodes registered with the DIM name server \\fIDIM_DNS_NODE\\fP).\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC Process Monitor Server version "
"and the FMC version, than terminate.\n"
".\n"
".OptDef c compact\n"
"Print the process/task information in compact format (easier to parse): "
"one process/task per line, no labels, no units, no separator lines.\n"
".\n"
".OptDef H header\n"
"If the compact output format has been choosen, print a heading line with the "
"output field labels.\n"
".\n"
".SS Process selection Options\n"
".\n"
".OptDef u utgid\\[hy]only\n"
"Show only processes which have the \\fBUTGID\\fP set (e.g. processes started "
"by the FMC Task Manager or processes which have the variable \\fBUTGID\\fP "
"defined in the process environment).\n"
".\n"
".OptDef U select\\[hy]utgid UTGID_PATTERN (string)\n"
"Show only processes which have the \\fBUTGID\\fP set and whose \\fBUTGID\\fP "
"matches the wildcard pattern \\fIUTGID_PATTERN\\fP.\n"
".\n"
".OptDef C select\\[hy]cmd CMD_PATTERN (string)\n"
"Show only processes whose command (basename of the executable image file "
"w/o arguments), cut at 15 characters, matches the wildcard pattern "
"\\fICMD_PATTERN\\fP.\n"
".\n"
".SS Data selection Options\n"
".\n"
".OptDef \"\" ids\n"
"Toggle the printout of the following 5 process identifier fields:\n"
".RS\n"
".TP\n"
"\\fBTGID\\fP (alias PID)\n"
"Thread Group Identifier (the former PID, Process Identifier). The term "
"\\fBprocess\\fP is synonymous of \\fBthread group\\fP.\n"
".TP\n"
"\\fBPPID\\fP\n"
"Parent Process Identifier, i.e. the Thread Group Identifier of the parent "
"process.\n"
".TP\n"
"\\fBPGID\\fP (alias PGRP)\n"
"Process Group Identifierr, i.e. the Thread Group Identifier of the process "
"group leader. Every process is member of a unique process group. The "
"processes of one pipeline form a single process group. Among the process "
"groups in a session, at most one can be the \\fBforeground process group\\fP "
"of that session. The tty (teminal) input and tty signals (signals generated "
"by Ctrl-C, Ctrl-Z, etc.) go to processes in this foreground process group. "
"All process groups in a session that are not foreground process group are "
"\\fBbackground process groups\\fP.\n"
".TP\n"
"\\fBSID\\fP (alias SESS, SESSION)\n"
"Session Identifier, i.e. the Thread Group Identifier of the session leader. "
"Every process group is in a unique session. Every session may have a "
"controlling tty (terminal), that is also the controlling tty of each one of "
"its member processes. A session is often set up by a login process.\n"
".TP\n"
"\\fBTPGID\\fP\n"
"Terminal Process Group Identifier, i.e. the Process Group Identifier of the "
"process which currently owns the tty (terminal) that the process is "
"connected to, or -1 if the process is not connected to a tty.\n"
".RE\n"
".\n"
".\n"
".OptDef \"\" owner\n"
"Toggle the printout -- in addition to the \\fBUSER\\fP (alias EUSER, "
"effective user name) field, which is always printed -- of the following 7 "
"process owner user/group fields:\n"
".RS\n"
".TP\n"
"\\fBFUSER\\fP\n"
"\\fBFilesystem access\\fP user name. Used only in file-system access. Can be "
"set to any of the current effective, saved or real user names. Processes "
"with \"root\" as effective user name can set their filesystem access user "
"name to an arbitrary value. Used, e.g., by the Linux's user-space NFS "
"server.\n"
".TP\n"
"\\fBRUSER\\fP\n"
"\\fBReal\\fP user name. Checked only when a process attempts to change its "
"effective user name. Any process may change its effective user name to the "
"same value as either its saved or real user name. Only processes with "
"\"root\" as effective user name may change their effective user name to an "
"arbitrary value. Used, e.g., by \"ftpd\" and \"passwd\" programs.\n"
".TP\n"
"\\fBSUSER\\fP\n"
"\\fBSaved\\fP user name. Checked only when a process attempts to change its "
"effective user name. Any process may change its effective user name to the "
"same value as either its saved or real user name. Only processes with "
"\"root\" as effective user name may change their effective user name to an "
"arbitrary value. Used, e.g., by \"ftpd\" and \"passwd\" programs.\n"
".TP\n"
"\\fBGROUP\\fP (alias EGROUP)\n"
"\\fBEffective\\fP group name. Used for all the security checks. The only "
"group name of the process that normally has any effect.\n"
".TP\n"
"\\fBFGROUP\\fP\n"
"\\fBFilesystem access\\fP group name. Used only in file-system access. Can "
"be set to any of the current effective, saved or real group names. Processes "
"with \"root\" as effective group name can set their filesystem access group "
"name to an arbitrary value. Used, e.g., by the Linux's user-space NFS "
"server.\n"
".TP\n"
"\\fBRGROUP\\fP\n"
"\\fBReal\\fP group name. Checked only when a process attempts to change its "
"effective group name. Any process may change its effective group name to the "
"same value as either its saved or real group name. Only processes with "
"\"root\" as effective group name may change their effective group name to an "
"arbitrary value. Used, e.g., by \"ftpd\" and \"passwd\" programs.\n"
".TP\n"
"\\fBSGROUP\\fP\n"
"\\fBSaved\\fP group name. Checked only when a process attempts to change its "
"effective group name. Any process may change its effective group name to the "
"same value as either its saved or real group name. Only processes with "
"\"root\" as effective group name may change their effective group name to an "
"arbitrary value. Used, e.g., by \"ftpd\" and \"passwd\" programs.\n"
".RE\n"
".\n"
".OptDef \"\" sched\n"
"Toggle the printout of the following 4 process scheduling fields:\n"
".RS\n"
".TP\n"
"\\fBSCH\\fP (alias CLS, POL)\n"
"Scheduling class of the process. Possible values of this fields are:\n"
".RS\n"
".TP\n"
"\\fBTS\\fP\n"
"\\fISCHED_OTHER\\fP, the default Linux time-sharing scheduler, with a "
"dynamic priority based on the nice level.\n"
".TP\n"
"\\fBFF\\fP\n"
"\\fISCHED_FIFO\\fP, the static-priority Linux real-time FIFO scheduler, "
"without time slicing.\n"
".TP\n"
"\\fBRR\\fP\n"
"\\fISCHED_RR\\fP, the static-priority Linux real-time Round-Robin "
"scheduler.\n"
".TP\n"
"\\fBN/A\\fP\n"
"Not reported.\n"
".TP\n"
"\\fB?\\fP\n"
"Unknown value.\n"
".RE\n"
".TP\n"
"\\fBRTPRIO\\fP\n"
"Static (real-time) priority of the process. Only value 0 is allowed for TS processes. For FF and RR real-time processes, allowed values are in the range 1...99 (0 is the least favorable priority, 99 is the most favorable\n" "priority).\n"
".TP\n"
"\\fBNICE\\fP (alias NI)\n"
"The nice level of the process, used by the TS Linux scheduler to compute the dynamic priority. Allowed values for nice level are in the range -20...19 (-20 corresponds to the most favorable scheduling; 19 corresponds to the least favorable scheduling).\n"
".TP\n"
"\\fBPRIO\\fP (alias PRI, PR)\n"
"Kernel scheduling priority. Possible values are in the range -100...39 (-100 is the most favorable priority, 39 is the least favorable priority). TS processes have PRIO in the range 0...39, FF and RR processes have PRIO in the range -100...-1.\n"
".RE\n"
".\n"
".OptDef \"\" stat\n"
"Toggle the printout of the following 6 process state fields:\n"
".RS\n"
".TP\n"
"\\fB%%CPU\\fP (alias PCPU, CP, C)\n"
"The task's share of the CPU time since the last update, expressed as a "
"percentage of the total CPU time per processor. \\fBMaximum\\fP: 100%% x "
"CPU_core_number.\n"
".TP\n"
"\\fB%%MEM\\fP (alias PMEM)\n"
"The ratio of the process's resident set size to the physical memory on the "
"machine, expressed as a percentage. \\fBMaximum\\fP: 100%%.\n"
".TP\n"
"\\fBMIN_FLT\\fP\n"
"The rate (page faults per second) of page faults the process has made which "
"have not required loading a memory page from disk.\n"
".TP\n"
"\\fBMAJ_FLT\\fP\n"
"The rate (page faults per second) of page faults the process has made which "
"have required loading a memory page from disk.\n"
".TP\n"
"\\fBSTAT\\fP (alias S)\n"
"The multi-characters process state of the task. The meaning of the first "
"character is:\n"
".RS\n"
".TP\n"
"\\fBD\\fP\n"
"Uninterruptible sleep (usually I/O);\n"
".TP\n"
"\\fBR\\fP\n"
"Running or runnable (on run queue);\n"
".TP\n"
"\\fBS\\fP\n"
"Interruptible sleep (waiting for an event to complete);\n"
".TP\n"
"\\fBT\\fP\n"
"Stopped, either by a job control signal or because it is being traced;\n"
".TP\n"
"\\fBX\\fP\n"
"Dead (should never be seen);\n"
".TP\n"
"\\fBZ\\fP\n"
"Defunct (\"zombie\") process, terminated but not reaped by its parent.\n"
".RE\n"
".PP\n"
".RS\n"
"The meaning of the following characters is:\n"
".TP\n"
"\\fB<\\fP\n"
"Higher-priority (not nice to other users);\n"
".TP\n"
"\\fBN\\fP\n"
"Lower-priority (nice to other users);\n"
".TP\n"
"\\fBL\\fP\n"
"Has pages locked into memory (for real-time and custom I/O);\n"
".TP\n"
"\\fBs\\fP\n"
"Is a session leader;\n"
".TP\n"
"\\fBl\\fP\n"
"Is multi-threaded (using CLONE_THREAD, like NPTL pthreads does);\n"
".TP\n"
"\\fB+\\fP\n"
"Is in the foreground process group.\n"
".RE\n"
".TP\n"
"\\fBPSR\\fP (alias CPUID, P)\n"
"The processor that process is currently assigned to. It is useful to check "
"the operation of process-to-CPU affinity setting by the FMC Task Manager.\n"
".RE\n"
".\n"
".OptDef \"\" size\n"
"Toggle the printout of the following 8 process size fields:\n"
".RS\n"
".TP\n"
"\\fBVSIZE\\fP (alias VSZ, VIRT)\n"
"The total size of the process's memory footprint. This includes the text "
"segment, stack, static variables, data segment, and pages which are shared "
"with other processes.\n"
".TP\n"
"\\fBLOCK\\fP\n"
"The amount of the process's memory which is currently locked by the kernel. "
"Locked memory cannot be swapped out.\n"
".TP\n"
"\\fBRSS\\fP (alias RSZ, RES, RESIDENT)\n"
"The kernel's estimate of the resident set size for this process.\n"
".TP\n"
"\\fBDATA\\fP\n"
"The amount of memory used for data by the process. It includes static "
"variables and the data segment, but excludes the stack.\n"
".TP\n"
"\\fBSTACK\\fP\n"
"The amount of memory consumed by the process's stack.\n"
".TP\n"
"\\fBEXE\\fP\n"
"The size of the process's executable pages, excluding shared pages.\n"
".TP\n"
"\\fBLIB\\fP\n"
"The size of the shared memory pages mapped into the process's address space. "
"This excludes pages shared using System V style IPC.\n"
".TP\n"
"\\fBSHARE\\fP (alias SHRD, SHR)\n"
"The size of the pages of shared (mmap-ed) memory.\n"
".RE\n"
".\n"
".OptDef \"\" sig\n"
"Toggle the printout of the following 4 process signal mask fields:\n"
".RS\n"
".TP\n"
"\\fBIGNORED\\fP (alias SIGIGNORE)\n"
"The 64-bit mask of the ignored signals.\n"
".TP\n"
"\\fBPENDING\\fP (alias SIGPEND)\n"
"The 64-bit mask of the pending signals.\n"
".TP\n"
"\\fBCATCHED\\fP (alias SIGCATCH, CAUGHT)\n"
"The 64-bit mask of the caught signals.\n"
".TP\n"
"\\fBBLOCKED\\fP (alias SIGBLOCKED)\n"
"The 64-bit mask of the blocked signals.\n"
".RE\n"
".\n"
".OptDef \"\" time\n"
"Toggle the printout of the following 3 process time fields:\n"
".RS\n"
".TP\n"
"\\fBSTARTED\\fP (alias STARTTIME, START)\n"
"The process's start time.\n"
".TP\n"
"\\fBELAPSED\\fP (alias ELAPSEDTIME)\n"
"The time elapsed since the process was started.\n"
".TP\n"
"\\fBCPUTIME\\fP (alias TIME)\n"
"The process's cumulative CPU time.\n"
".RE\n"
".\n"
".OptDef \"\" cmd\n"
"Toggle the printout of the field:\n"
".RS\n"
".TP\n"
"\\fBCMDLINE\\fP (alias ARGS, CMD, COMMAND)\n"
"The complete command line with its arguments (up to PATH_MAX = 4096 "
"characters). The output in this column may contain spaces.\n"
".RE\n"
".\n"
".OptDef \"\" cmdlen LEN (integer)\n"
"If the \\fICMDLINE\\fP field has to be printed, cut it at \\fILEN\\fP "
"characters.\n"
".\n"
".OptDef a all\n"
"Toggle all the optional (non-basic) fields. Equivalent to: \"--ids --owner "
"--sched --stat --size --sig --time --cmd\".\n"
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
"Show the basic fields:\n"
".PP\n"
".ShellCommand psViewer\n"
".PP\n"
"Show all the fields:\n"
".PP\n"
".ShellCommand psViewer -a\n"
".ShellCommand psViewer --all\n"
".PP\n"
"Print output in compact format with header:\n"
".PP\n"
".ShellCommand psViewer -acH\n"
".ShellCommand psViewer -a -c -H\n"
".ShellCommand psViewer --all --compact --header\n"
".ShellCommand psViewer --a --co --hea\n"
".PP\n"
"Show only the processes which have the UTGID set:\n"
".PP\n"
".ShellCommand psViewer -au\n"
".ShellCommand psViewer --all --utgid\n"
".PP\n"
"Show basic + scheduling fields:\n"
".PP\n"
".ShellCommand psViewer --sched\n"
".ShellCommand psViewer --sc\n"
".PP\n"
"Show basic + scheduling + state + size fields:\n"
".PP\n"
".ShellCommand psViewer --sched --stat --size\n"
".ShellCommand psViewer --sc --st --siz\n"
".PP\n"
"Show all but signal fields:\n"
".PP\n"
".ShellCommand psViewer --all --sig\n"
".PP\n"
"Show all but signal and command-line fields:\n"
".PP\n"
".ShellCommand psViewer --all --sig --cmd\n"
".PP\n"
"Show the CMDLINE field and cut it at 30 characters:\n"
".PP\n"
".ShellCommand psViewer --cmd --cmdlen 30\n"
".PP\n"
"Show all but signal field and cut the CMDLINE field at 30 characters:\n"
".PP\n"
".ShellCommand psViewer --all --sig --cmdlen 30\n"
".PP\n"
"Show only processes running on selected nodes:\n"
".PP\n"
".ShellCommand psViewer -m farm0101 -m farm0102 -u --sched\n"
".PP\n"
"Show only processes running on wildcard-selected nodes:\n"
".PP\n"
".ShellCommand psViewer -m \\[dq]farm01*\\[dq] -m \\[dq]farm02*\\[dq] -u "
"--sched --stat\n"
".ShellCommand psViewer -m \\[dq]farm010[1357]\\[dq] -u --size\n" 
".PP\n"
"Show the server version installed on the farm nodes:\n"
".PP\n"
".ShellCommand psViewer -V\n"
".ShellCommand psViewer --version\n"
".PP\n"
"\n"
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
".BR \\%%psSrv (8),\n"
".BR \\%%psSetProperties (1).\n"
".br\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%tmStart (1).\n"
".br\n"
".BR \\%%top (1),\n"
".BR \\%%ps (1),\n"
".BR \\%%proc (5).\n"
".br\n"
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
