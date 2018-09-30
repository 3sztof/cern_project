/*****************************************************************************/
/*
 * $Log: nifViewer.C,v $
 * Revision 1.16  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.15  2008/10/22 13:02:31  galli
 * minor changes
 *
 * Revision 1.14  2008/10/22 12:32:55  galli
 * uses FmcUtils::printServerVersion()
 *
 * Revision 1.12  2008/02/27 15:12:32  galli
 * bug fixed
 *
 * Revision 1.11  2008/02/27 15:06:34  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.9  2007/12/22 00:16:39  galli
 * bug fixed
 *
 * Revision 1.8  2007/12/21 23:54:33  galli
 * read service /<HOSTNAME>/nif/active, if available
 *
 * Revision 1.7  2007/12/21 13:51:36  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.6  2006/10/26 15:47:49  galli
 * -d option (only view addresses) added
 *
 * Revision 1.5  2006/02/09 11:41:03  galli
 * bug fixed
 *
 * Revision 1.4  2005/11/09 13:10:11  galli
 * compatible with nifSrv.c revision 2.10
 *
 * Revision 1.3  2005/09/20 10:37:46  galli
 * default: contact all hosts
 *
 * Revision 1.2  2005/09/20 09:02:26  galli
 * print also total Eth I/O rates
 *
 * Revision 1.1  2005/09/19 14:10:27  galli
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
#define SRV_NAME "net/ifs"
#define DEV_FIELD_N_ERR 1
#define DET_FLOAT_FIELD_N_ERR 18
#define DET_STR_FIELD_N_ERR 9
#define ETH_FIELD_N_ERR 4
/*****************************************************************************/
/* type definition */
typedef struct detDataStr
{
  char *id;
  float *stat;
  char *name;
  char *busAddress;
  char *hwAddress;
  char *ipAddress;
  char *speed;
  char *duplex;
  char *portType;
  char *autoNegotiation;
  char *linkDetected;
}detDataStr_t;
typedef struct detDataLen
{
  unsigned id;
  unsigned name;
  unsigned busAddress;
  unsigned hwAddress;
  unsigned ipAddress;
  unsigned speed;
  unsigned duplex;
  unsigned portType;
  unsigned autoNegotiation;
  unsigned linkDetected;
}detDataLen_t;
/*****************************************************************************/
/* globals */
int deBug=0;
static char rcsid[]="$Id: nifViewer.C,v 1.16 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
const char *srvName="net/ifs";
/*****************************************************************************/
/* function prototype */
void usage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j,k,l;
  /* pointers */
  char *p=NULL;
  char *lp,*up;
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
  int nolinkDevStrArrS=0;
  char *nolinkDevStrArr=NULL;
  int nolinkDetStrArrS=0;
  char *nolinkDetStrArr=NULL;
  int nolinkDetDataS=0;
  char *nolinkDetData=NULL;
  int nolinkEthStrArrS=0;
  char *nolinkEthStrArr=NULL;
  int nolinkEthDataS=0;
  char *nolinkEthData=NULL;
  int type=0;
  char *format=NULL;
  char *successSvc=NULL;
  char *svc=NULL;
  int svcLen=0;
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  int success=-1;
  char *devices=NULL;
  int devicesSz=0;
  int deviceC=0;
  char **deviceV=NULL;
  DimCurrentInfo *DetLabels=NULL;
  int detLabelsSz=0;
  char *detLabels=NULL;
  int detLabelsRead=0;
  DimCurrentInfo *DetUnits=NULL;
  int detUnitsSz=0;
  char *detUnits=NULL;
  int detUnitsRead=0;
  DimCurrentInfo *EthLabels=NULL;
  int ethLabelsSz=0;
  char *ethLabels=NULL;
  int ethLabelsRead=0;
  DimCurrentInfo *EthUnits=NULL;
  int ethUnitsSz=0;
  char *ethUnits=NULL;
  int ethUnitsRead=0;
  DimCurrentInfo **DetDataBuf=NULL;
  int *detDataBufSz=NULL;
  char **detDataBuf=NULL;
  detDataStr_t *detData=NULL;
  DimCurrentInfo **DetAdataBuf=NULL;
  int *detAdataBufSz=NULL;
  char **detAdataBuf=NULL;
  detDataStr_t *detAdata=NULL;
  DimCurrentInfo **DetMdataBuf=NULL;
  int *detMdataBufSz=NULL;
  char **detMdataBuf=NULL;
  detDataStr_t *detMdata=NULL;
  DimCurrentInfo *EthDataBuf=NULL;
  int ethDataBufSz=0;
  float *ethDataBuf=NULL;
  char *tss=NULL;
  char *tssc=NULL;
  int tssd,tssh,tssm,tsss,tssmils;
  /* printing variables */
  detDataLen_t detDataLen;
  char lineFormat[256]="";
  static int isFirstTime=1;
  char *line=NULL;
  const char *line2="--------------------------------------";
  int lineLen=0;
  /* command line switch */
  int doPrintAve=0;
  int doPrintMax=0;
  int doPrintTot=0;
  int doPrintHeader=0;
  int compactFormat=0;
  int onlyActive=0;
  int onlyAddress=0;
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
    {"total",no_argument,NULL,'T'},
    {"active",no_argument,NULL,'x'},
    {"settings",no_argument,NULL,'s'},
    {NULL,0,NULL,0}
  };
  /* other */
  float errFloat=-1.0;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  while((flag=getopt_long(argc,argv,"m:vVN:hcHaAMTxs",longOptions,NULL))!=EOF)
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
        doPrintTot=!doPrintTot;
        break;
      case 'A': /* -A, --average */
        doPrintAve=!doPrintAve;
        break;
      case 'M': /* -M, --maximum */
        doPrintMax=!doPrintMax;
        break;
      case 'T': /* -T, --total */
        doPrintTot=!doPrintTot;
        break;
      case 'x': /* -x, --active */
        onlyActive=1;
        break;
      case 's': /* -s, --settings */
        onlyAddress=1;
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
  if(onlyAddress)
  {
    doPrintAve=0;
    doPrintMax=0;
    doPrintTot=0;
  }
  /*-------------------------------------------------------------------------*/
  /* Compose DIM no link error arrays */
  /* devices */
  nolinkDevStrArrS=DEV_FIELD_N_ERR*(1+strlen("N/F"));     /* "N/F"=not found */
  nolinkDevStrArr=(char*)malloc(nolinkDevStrArrS);
  for(i=0,p=nolinkDevStrArr;i<DEV_FIELD_N_ERR;i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
  /* detailed labels/units */
  nolinkDetStrArrS=(DET_FLOAT_FIELD_N_ERR+DET_STR_FIELD_N_ERR)*
                   (1+strlen("N/F"));                     /* "N/F"=not found */
  nolinkDetStrArr=(char*)malloc(nolinkDetStrArrS);
  for(i=0,p=nolinkDetStrArr;i<(DET_FLOAT_FIELD_N_ERR+DET_STR_FIELD_N_ERR);
      i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
  /* detailed data */
  nolinkDetDataS=DET_FLOAT_FIELD_N_ERR*sizeof(float)+
                 DET_STR_FIELD_N_ERR*(1+strlen("N/F"));
  nolinkDetData=(char*)malloc(nolinkDetDataS);
  p=nolinkDetData;
  for(i=0;i<DET_FLOAT_FIELD_N_ERR;i++)
  {
    memcpy(p,&errFloat,sizeof(float));
    p+=sizeof(float);
  }
  for(i=0;i<DET_STR_FIELD_N_ERR;i++)
  {
    sprintf(p,"%s","N/F");
    p+=(1+strlen("N/F"));
  }
  /* total ethernet labels/units */
  nolinkEthStrArrS=(DET_FLOAT_FIELD_N_ERR+DET_STR_FIELD_N_ERR)*
                   (1+strlen("N/F"));                     /* "N/F"=not found */
  nolinkEthStrArr=(char*)malloc(nolinkEthStrArrS);
  for(i=0,p=nolinkEthStrArr;i<(DET_FLOAT_FIELD_N_ERR+DET_STR_FIELD_N_ERR);
      i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
  /* total ethernet data */
  nolinkEthDataS=ETH_FIELD_N_ERR*sizeof(float);
  nolinkEthData=(char*)malloc(nolinkEthDataS);
  p=nolinkEthData;
  for(i=0;i<ETH_FIELD_N_ERR;i++)
  {
    memcpy(p,&errFloat,sizeof(float));
    p+=sizeof(float);
  }
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
  lineLen=43;
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
    svcPatt=(char*)realloc(svcPatt,1+snprintf(NULL,0,"/FMC/%s/%s/success",
                           hostNamePattUc,srvName));
    sprintf(svcPatt,"/FMC/%s/%s/success",hostNamePattUc,srvName);
    if(deBug)printf("  Service pattern: \"%s\"\n",svcPatt);
    /*.......................................................................*/
    dimBrPatt=(char*)realloc(dimBrPatt,1+snprintf(NULL,0,"/FMC/*/%s/success",
                             srvName));
    sprintf(dimBrPatt,"/FMC/*/%s/success",srvName);
    br.getServices(dimBrPatt);
    nodeFoundN=0;
    while((type=br.getNextService(successSvc,format))!=0) /* loop over nodes */
    {
      if(!fnmatch(svcPatt,successSvc,0))
      {
        /*...................................................................*/
        nodeFoundN++;
        snprintf(nodeFound,sizeof(nodeFound),"%s",successSvc+strlen("/FMC/"));
        snprintf(ucNodeFound,1+strcspn(nodeFound,"/"),"%s",nodeFound);
        for(p=nodeFound;*p;p++)
        {
          if(*p=='/')
          {
            *p='\0';
            break;
          }
          *p=tolower(*p);
        }
        /*-------------------------------------------------------------------*/
        /* Read data */
        /*...................................................................*/
        /* read Success. If fails then skip the node */
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
        if(!detLabelsRead)
        {
          /* read details/labels */
          svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/details/labels",ucNodeFound,
                            srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"/FMC/%s/%s/details/labels",ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          DetLabels=new DimCurrentInfo(svc,nolinkDetStrArr,nolinkDetStrArrS);
          detLabelsSz=(int)DetLabels->getSize();
          detLabels=(char*)DetLabels->getData();
          if(!strstr(detLabels,"N/F"))detLabelsRead=1;
        }
        /*...................................................................*/
        if(!detUnitsRead)
        {
          /* read details/units */
          svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/details/units",ucNodeFound,
                            srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"/FMC/%s/%s/details/units",ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          DetUnits=new DimCurrentInfo(svc,nolinkDetStrArr,nolinkDetStrArrS);
          detUnitsSz=(int)DetUnits->getSize();
          detUnits=(char*)DetUnits->getData();
          if(!strstr(detUnits,"N/F"))detUnitsRead=1;
        }
        /*...................................................................*/
        if(!ethLabelsRead)
        {
          /* read eth_total/labels */
          svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/eth_total/labels",ucNodeFound,
                            srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"/FMC/%s/%s/eth_total/labels",ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          EthLabels=new DimCurrentInfo(svc,nolinkEthStrArr,nolinkEthStrArrS);
          ethLabelsSz=(int)EthLabels->getSize();
          ethLabels=(char*)EthLabels->getData();
          if(!strstr(ethLabels,"N/F"))ethLabelsRead=1;
        }
        /*...................................................................*/
        /* read eth_total/units */
        if(!ethUnitsRead)
        {
          svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/eth_total/units",ucNodeFound,
                            srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"/FMC/%s/%s/eth_total/units",ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          EthUnits=new DimCurrentInfo(svc,nolinkEthStrArr,nolinkEthStrArrS);
          ethUnitsSz=(int)EthUnits->getSize();
          ethUnits=(char*)EthUnits->getData();
          if(!strstr(ethUnits,"N/F"))ethUnitsRead=1;
        }
        /*...................................................................*/
        if(doPrintMax||doPrintAve)
        {
          /* read Time Since Start/Reset */
          svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/time_since_reset",ucNodeFound,
                            srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"/FMC/%s/%s/time_since_reset",ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          DimCurrentInfo Tss(svc,(char*)"Server unreachable!");
          tss=(char*)realloc(tss,Tss.getSize());
          snprintf(tss,Tss.getSize(),"%s",Tss.getString());
          /* tssc = compact time string [ddd-hh:mm:ss.sss] */
          tssc=(char*)realloc(tssc,1+strlen(tss)-7);
          sscanf(tss,"%dd,%dh,%dm,%d.%ds",&tssd,&tssh,&tssm,&tsss,&tssmils);
          sprintf(tssc,"%03d-%02d:%02d:%02d.%03d",tssd,tssh,tssm,tsss,tssmils);
        }
        /*...................................................................*/
        /* read devices */
        svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/devices",ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"/FMC/%s/%s/devices",ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo Devices(svc,nolinkDevStrArr,nolinkDevStrArrS);
        devicesSz=(int)Devices.getSize();
        devices=(char*)Devices.getData();
        for(k=0,p=devices;p<devices+devicesSz;k++,p+=(1+strlen(p)))
        {
          deviceV=(char**)realloc(deviceV,(k+1)*sizeof(char*));
          deviceV[k]=strdup(p);
        }
        deviceC=k;
        /*...................................................................*/
        /* allocate memory for if data */
        DetDataBuf=(DimCurrentInfo**)realloc(DetDataBuf,
                                              deviceC*sizeof(DimCurrentInfo*));
        detDataBufSz=(int*)realloc(detDataBufSz,deviceC*sizeof(int));
        detDataBuf=(char**)realloc(detDataBuf,deviceC*sizeof(char*));
        detData=(detDataStr_t*)realloc(detData,deviceC*sizeof(detDataStr_t));
        /* read if data */
        for(k=0;k<deviceC;k++)
        {
          if(!strcmp(deviceV[k],"N/F"))break;
          svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/details/%s/data",
                            ucNodeFound,srvName,deviceV[k]);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"/FMC/%s/%s/details/%s/data",ucNodeFound,srvName,
                  deviceV[k]);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          DetDataBuf[k]=new DimCurrentInfo(svc,nolinkDetData,nolinkDetDataS);
          detDataBufSz[k]=(int)DetDataBuf[k]->getSize();
          detDataBuf[k]=(char*)DetDataBuf[k]->getData();
          detData[k].id=deviceV[k];
          detData[k].stat=(float*)detDataBuf[k];
          detData[k].name=(char*)(detDataBuf[k]+18*sizeof(float));
          detData[k].busAddress=detData[k].name+strlen(detData[k].name)+1;
          detData[k].hwAddress=detData[k].busAddress+
                                               strlen(detData[k].busAddress)+1;
          detData[k].ipAddress=detData[k].hwAddress+
                                                strlen(detData[k].hwAddress)+1;
          detData[k].speed=detData[k].ipAddress+strlen(detData[k].ipAddress)+1;
          detData[k].duplex=detData[k].speed+strlen(detData[k].speed)+1;
          detData[k].portType=detData[k].duplex+strlen(detData[k].duplex)+1;
          detData[k].autoNegotiation=detData[k].portType+
                                                 strlen(detData[k].portType)+1;
          detData[k].linkDetected=detData[k].autoNegotiation+
                                          strlen(detData[k].autoNegotiation)+1;
        }
        /*...................................................................*/
        if(doPrintAve)
        {
          /* allocate memory for if average data */
          DetAdataBuf=(DimCurrentInfo**)realloc(DetAdataBuf,
                                              deviceC*sizeof(DimCurrentInfo*));
          detAdataBufSz=(int*)realloc(detAdataBufSz,deviceC*sizeof(int));
          detAdataBuf=(char**)realloc(detAdataBuf,deviceC*sizeof(char*));
          detAdata=(detDataStr_t*)realloc(detAdata,deviceC
                                                        *sizeof(detDataStr_t));
          /* read if average data */
          for(k=0;k<deviceC;k++)
          {
            if(!strcmp(deviceV[k],"N/F"))break;
            svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/details/%s/average_data",
                              ucNodeFound,srvName,deviceV[k]);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"/FMC/%s/%s/details/%s/average_data",ucNodeFound,
                    srvName,deviceV[k]);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
            DetAdataBuf[k]=new DimCurrentInfo(svc,nolinkDetData,nolinkDetDataS);
            detAdataBufSz[k]=(int)DetAdataBuf[k]->getSize();
            detAdataBuf[k]=(char*)DetAdataBuf[k]->getData();
            detAdata[k].id=deviceV[k];
            detAdata[k].stat=(float*)detAdataBuf[k];
            detAdata[k].name=(char*)(detAdataBuf[k]+18*sizeof(float));
            detAdata[k].busAddress=detAdata[k].name+strlen(detAdata[k].name)+1;
            detAdata[k].hwAddress=detAdata[k].busAddress+
                                              strlen(detAdata[k].busAddress)+1;
            detAdata[k].ipAddress=detAdata[k].hwAddress+
                                               strlen(detAdata[k].hwAddress)+1;
            detAdata[k].speed=detAdata[k].ipAddress+
                                               strlen(detAdata[k].ipAddress)+1;
            detAdata[k].duplex=detAdata[k].speed+strlen(detAdata[k].speed)+1;
            detAdata[k].portType=detAdata[k].duplex+
                                                  strlen(detAdata[k].duplex)+1;
            detAdata[k].autoNegotiation=detAdata[k].portType+
                                                strlen(detAdata[k].portType)+1;
            detAdata[k].linkDetected=detAdata[k].autoNegotiation+
                                         strlen(detAdata[k].autoNegotiation)+1;
          }
        }
        /*...................................................................*/
        if(doPrintMax)
        {
          /* allocate memory for if maximum data */
          DetMdataBuf=(DimCurrentInfo**)realloc(DetMdataBuf,
                                              deviceC*sizeof(DimCurrentInfo*));
          detMdataBufSz=(int*)realloc(detMdataBufSz,deviceC*sizeof(int));
          detMdataBuf=(char**)realloc(detMdataBuf,deviceC*sizeof(char*));
          detMdata=(detDataStr_t*)realloc(detMdata,deviceC
                                                        *sizeof(detDataStr_t));
          /* read if maximum data */
          for(k=0;k<deviceC;k++)
          {
            if(!strcmp(deviceV[k],"N/F"))break;
            svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/details/%s/maximum_data",
                              ucNodeFound,srvName,deviceV[k]);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"/FMC/%s/%s/details/%s/maximum_data",ucNodeFound,
                    srvName,deviceV[k]);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
            DetMdataBuf[k]=new DimCurrentInfo(svc,nolinkDetData,nolinkDetDataS);
            detMdataBufSz[k]=(int)DetMdataBuf[k]->getSize();
            detMdataBuf[k]=(char*)DetMdataBuf[k]->getData();
            detMdata[k].id=deviceV[k];
            detMdata[k].stat=(float*)detMdataBuf[k];
            detMdata[k].name=(char*)(detMdataBuf[k]+18*sizeof(float));
            detMdata[k].busAddress=detMdata[k].name+strlen(detMdata[k].name)+1;
            detMdata[k].hwAddress=detMdata[k].busAddress+
                                              strlen(detMdata[k].busAddress)+1;
            detMdata[k].ipAddress=detMdata[k].hwAddress+
                                               strlen(detMdata[k].hwAddress)+1;
            detMdata[k].speed=detMdata[k].ipAddress+
                                               strlen(detMdata[k].ipAddress)+1;
            detMdata[k].duplex=detMdata[k].speed+strlen(detMdata[k].speed)+1;
            detMdata[k].portType=detMdata[k].duplex+
                                                  strlen(detMdata[k].duplex)+1;
            detMdata[k].autoNegotiation=detMdata[k].portType+
                                                strlen(detMdata[k].portType)+1;
            detMdata[k].linkDetected=detMdata[k].autoNegotiation+
                                         strlen(detMdata[k].autoNegotiation)+1;
          }
        }
        /*...................................................................*/
        /* read total ethernet data */
        if(doPrintTot)
        {
          svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/eth_total/data",ucNodeFound,
                            srvName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"/FMC/%s/%s/eth_total/data",ucNodeFound,srvName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
          EthDataBuf=new DimCurrentInfo(svc,nolinkEthData,nolinkEthDataS);
          ethDataBufSz=EthDataBuf->getSize();
          ethDataBuf=(float*)EthDataBuf->getData();
        }
        /*-------------------------------------------------------------------*/
        /* evaluate max string lengths */
        /*...................................................................*/
        if(!compactFormat)
        {
          memset(&detDataLen,0,sizeof(detDataLen));
          for(int k=0;k<deviceC;k++)                       /* loop over nifs */
          {
            unsigned l;
            l=snprintf(NULL,0,"%s",detData[k].id);
            if(detDataLen.id<l)detDataLen.id=l;
            l=snprintf(NULL,0,"%s",detData[k].name);
            if(detDataLen.name<l)detDataLen.name=l;
            l=snprintf(NULL,0,"%s",detData[k].busAddress);
            if(detDataLen.busAddress<l)detDataLen.busAddress=l;
            l=snprintf(NULL,0,"%s",detData[k].hwAddress);
            if(detDataLen.hwAddress<l)detDataLen.hwAddress=l;
            l=snprintf(NULL,0,"%s",detData[k].ipAddress);
            if(detDataLen.ipAddress<l)detDataLen.ipAddress=l;
            l=snprintf(NULL,0,"%s",detData[k].speed);
            if(detDataLen.speed<l)detDataLen.speed=l;
            l=snprintf(NULL,0,"%s",detData[k].duplex);
            if(detDataLen.duplex<l)detDataLen.duplex=l;
            l=snprintf(NULL,0,"%s",detData[k].portType);
            if(detDataLen.portType<l)detDataLen.portType=l;
            l=snprintf(NULL,0,"%s",detData[k].autoNegotiation);
            if(detDataLen.autoNegotiation<l)detDataLen.autoNegotiation=l;
            l=snprintf(NULL,0,"%s",detData[k].linkDetected);
            if(detDataLen.linkDetected<l)detDataLen.linkDetected=l;
          }
        }
        /*-------------------------------------------------------------------*/
        /* Print data */
        /*...................................................................*/
        if(!onlyAddress)
        {
          /*.................................................................*/
          /* print header for compact formet */
          if(isFirstTime && compactFormat && doPrintHeader)
          {
            printf("HOSTNAME Id");
            lp=detLabels;
            up=detUnits;
            for(k=0;k<27;k++,lp+=1+strlen(lp),up+=1+strlen(up))
            {
              if(k>17)
              {
                for(p=lp;*p;p++)if(*p==' ')*p='_';
                for(p=up;*p;p++)if(*p==' ')*p='_';
                if(up[0])printf(" %s_[%s]",lp,up);
                else printf(" %s",lp);
              }
            }
            lp=detLabels;
            up=detUnits;
            for(k=0;k<18;k++,lp+=1+strlen(lp),up+=1+strlen(up))
            {
              for(p=lp;*p;p++)if(*p==' ')*p='_';
              for(p=up;*p;p++)if(*p==' ')*p='_';
              if(up[0])printf(" %s_[%s]",lp,up);
              else printf(" %s",lp);
            }
            if(doPrintAve)
            {
              lp=detLabels;
              up=detUnits;
              for(k=0;k<18;k++,lp+=1+strlen(lp),up+=1+strlen(up))
              {
                for(p=lp;*p;p++)if(*p==' ')*p='_';
                for(p=up;*p;p++)if(*p==' ')*p='_';
                if(up[0])printf(" %s_av_[%s]",lp,up);
                else printf(" %s_av",lp);
              }
            }
            if(doPrintMax)
            {
              lp=detLabels;
              up=detUnits;
              for(k=0;k<18;k++,lp+=1+strlen(lp),up+=1+strlen(up))
              {
                for(p=lp;*p;p++)if(*p==' ')*p='_';
                for(p=up;*p;p++)if(*p==' ')*p='_';
                if(up[0])printf(" %s_max_[%s]",lp,up);
                else printf(" %s_max",lp);
              }
            }
            if(doPrintAve || doPrintMax)
            {
              printf(" Elapsed_time_[ddd-HH:MM:SS.SSS]");
            }
            printf("\n");
            /*...............................................................*/
            if(doPrintTot)
            {
              printf("HOSTNAME total_ethernet");
              lp=ethLabels;
              up=ethUnits;
              for(k=0;k<4;k++,lp+=1+strlen(lp),up+=1+strlen(up))
              {
                for(p=lp;*p;p++)if(*p==' ')*p='_';
                for(p=up;*p;p++)if(*p==' ')*p='_';
                printf(" %s_[%s]",lp,up);
              }
              printf("\n");
            }                                              /* if(doPrintTot) */
            /*...............................................................*/
          }                                               /* if(isFirstTime) */
          /*.................................................................*/
          /* loop over nifs to read IF specific data */
          for(k=0;k<deviceC;k++)                           /* loop over nifs */
          {
            /*...............................................................*/
            /* print IF specific data */
            if(!onlyActive || strcmp(detData[k].linkDetected,"no"))
            {
              if(!compactFormat)
              {
                printf("    NODE: %s, ID: %s, IF: %s, IP: %s\n",nodeFound,
                       detData[k].id,detData[k].name,detData[k].ipAddress);
                printf("    MAC: %s, BUS: %s\n",detData[k].hwAddress,
                       detData[k].busAddress);
                printf("    Speed: %s, Duplex: %s, Port type: %s\n",
                       detData[k].speed,detData[k].duplex,detData[k].portType);
                printf("    Auto-negotiation: %s, Link detected: %s\n",
                       detData[k].autoNegotiation,detData[k].linkDetected);
                if(doPrintAve||doPrintMax)
                {
                  printf("    Elapsed time: %s.\n",tss);
                }
                printf("    %s\n",line);
                printf("     # Name                    Instant");
                if(doPrintAve)printf("   Average");
                if(doPrintMax)printf("   Maximum");
                printf("\n");
                printf("    %s\n",line);
                for(l=0,lp=detLabels,up=detUnits;l<18;
                    l++,lp+=1+strlen(lp),up+=1+strlen(up))
                {
                  if(!onlyActive ||
                     (l==0||l==1||l==4||l==5) ||
                     detData[k].stat[l]>1e-37 ||
                     (doPrintMax && detAdata[k].stat[l]>1e-37)||
                     (doPrintAve && detMdata[k].stat[l]>1e-37))
                  {
                    printf("    %2d %-21s  %8.2e",l,lp,detData[k].stat[l]);
                    if(doPrintAve)printf("  %8.2e",detAdata[k].stat[l]);
                    if(doPrintMax)printf("  %8.2e",detMdata[k].stat[l]);
                    printf(" %s\n",up);
                  }
                
                }
                printf("    %s\n",line);
              }                                        /* if(!compactFormat) */
              else                                          /* compactFormat */
              {
                printf("%s %s %s %s %s %s %s %s %s %s %s",nodeFound,
                       detData[k].id,detData[k].name,detData[k].busAddress,
                       detData[k].hwAddress,detData[k].ipAddress,
                       detData[k].speed,detData[k].duplex,detData[k].portType,
                       detData[k].autoNegotiation,detData[k].linkDetected);
                for(l=0;l<18;l++)printf(" %8.2e",detData[k].stat[l]);
                if(doPrintAve)
                {
                  for(l=0;l<18;l++)printf(" %8.2e",detAdata[k].stat[l]);
                }
                if(doPrintMax)
                {
                  for(l=0;l<18;l++)printf(" %8.2e",detMdata[k].stat[l]);
                }
                if(doPrintAve || doPrintMax)
                {
                  printf(" %s",tssc);
                }
                printf("\n");
              }                                         /* if(compactFormat) */
            }     /* if(!onlyActive || strcmp(detData[k].linkDetected,"no")) */
          }                                                /* loop over nifs */
          /*.................................................................*/
        }                                                /* if(!onlyAddress) */
        else                                              /* if(onlyAddress) */
        {
          /*.................................................................*/
          if(!compactFormat)
          {
            unsigned l;
            printf("    NODE: %s\n",nodeFound);
            l=strlen("Id");
            if(detDataLen.id<l)detDataLen.id=l;
            l=strlen("Name");
            if(detDataLen.name<l)detDataLen.name=l;
            l=strlen("BUS");
            if(detDataLen.busAddress<l)detDataLen.busAddress=l;
            l=strlen("MAC");
            if(detDataLen.hwAddress<l)detDataLen.hwAddress=l;
            l=strlen("IP");
            if(detDataLen.ipAddress<l)detDataLen.ipAddress=l;
            l=strlen("Speed");
            if(detDataLen.speed<l)detDataLen.speed=l;
            l=strlen("Duplex");
            if(detDataLen.duplex<l)detDataLen.duplex=l;
            l=strlen("Port");
            if(detDataLen.portType<l)detDataLen.portType=l;
            l=strlen("Auto-neg");
            if(detDataLen.autoNegotiation<l)detDataLen.autoNegotiation=l;
            l=strlen("Link");
            if(detDataLen.linkDetected<l)detDataLen.linkDetected=l;
            lineLen=detDataLen.id+1+detDataLen.name+1+detDataLen.busAddress+1+
                    detDataLen.hwAddress+1+detDataLen.ipAddress+1+
                    detDataLen.speed+1+detDataLen.duplex+1+
                    detDataLen.portType+1+detDataLen.autoNegotiation+1+
                    detDataLen.linkDetected;
            line=(char*)realloc(line,(1+lineLen)*sizeof(char));
            memset(line,'-',lineLen);
            line[lineLen]='\0';
            printf("    %s\n",line);
            sprintf(lineFormat,"    %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds "
                    "%%-%ds %%-%ds %%-%ds %%-%ds %%-%ds\n",detDataLen.id,
                    detDataLen.name,detDataLen.busAddress,detDataLen.hwAddress,
                    detDataLen.ipAddress,detDataLen.speed,detDataLen.duplex,
                    detDataLen.portType,detDataLen.autoNegotiation,
                    detDataLen.linkDetected);
            printf(lineFormat,"Id","Name","BUS","MAC","IP","Speed","Duplex",
                   "Port","Auto-neg","Link");
            printf("    %s\n",line);
            for(k=0;k<deviceC;k++)                         /* loop over nifs */
            {
              if(!onlyActive || strcmp(detData[k].linkDetected,"no"))
              {
                printf(lineFormat,detData[k].id,detData[k].name,
                       detData[k].busAddress,detData[k].hwAddress,
                       detData[k].ipAddress,detData[k].speed,detData[k].duplex,
                       detData[k].portType,detData[k].autoNegotiation,
                       detData[k].linkDetected);
              }
            }
            printf("    %s\n",line);
          }                                            /* if(!compactFormat) */
          /*.................................................................*/
          else                                          /* if(compactFormat) */
          /*.................................................................*/
          {
            if(doPrintHeader && isFirstTime)
            {
              printf("HOSTNAME Id Name BUS_address MAC_address IP_address "
                     "Speed Duplex Port_type Auto-negotiation "
                     "Link_detected\n");
            }
            for(k=0;k<deviceC;k++)                         /* loop over nifs */
            {
              if(!onlyActive || strcmp(detData[k].linkDetected,"no"))
              {
                printf("%s %s %s %s %s %s %s %s %s %s %s\n",nodeFound,
                       detData[k].id,detData[k].name,detData[k].busAddress,
                       detData[k].hwAddress,detData[k].ipAddress,
                       detData[k].speed,detData[k].duplex,detData[k].portType,
                       detData[k].autoNegotiation,detData[k].linkDetected);
              }
            }
          }                                             /* if(compactFormat) */
          /*.................................................................*/
        }                                                 /* if(onlyAddress) */
        /*...................................................................*/
        /* print summary data */
        if(doPrintTot)
        {
          if(!compactFormat)
          {
            printf("    NODE %s, Total Ethernet I/O rates (sum over IFs):\n",
                   nodeFound);
            printf("    %s\n",line2);
            printf("    Name                 Instant\n");
            printf("    %s\n",line2);
            lp=ethLabels;
            up=ethUnits;
            printf("    total %-14s %8.2e %s\n",lp,ethDataBuf[0],up);
            lp+=1+strlen(lp); up+=1+strlen(up);
            printf("    total %-14s %8.2e %s\n",lp,ethDataBuf[1],up);
            lp+=1+strlen(lp); up+=1+strlen(up);
            printf("    total %-14s %8.2e %s\n",lp,ethDataBuf[2],up);
            lp+=1+strlen(lp); up+=1+strlen(up);
            printf("    total %-14s %8.2e %s\n",lp,ethDataBuf[3],up);
            printf("    %s\n",line2);
          }
          else
          {
            printf("%s total_ethernet",nodeFound);
            for(k=0;k<4;k++)printf(" %8.2e",ethDataBuf[k]);
            printf("\n");
          }
        }
        /* End Print data */
        /*-------------------------------------------------------------------*/
      }                                                     /* if !fnmatch() */
      isFirstTime=0;
      /* free allocated memory */
      for(int k=0;k<deviceC;k++)
      {
        DetDataBuf[k]->~DimCurrentInfo();
        if(deviceV[k])free(deviceV[k]);
      }
      if(EthDataBuf)
      {
        EthDataBuf->~DimCurrentInfo();
        EthDataBuf=NULL;
      }
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
                               deBug,12,8,1);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"",SRV_NAME,
                               deBug,12,8,1);
  /* older service name rule */
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"","nif",
                               deBug,12,8,0);
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
"       nifViewer - Get network interface statistics from the FMC Network\n"
"                   Interface Monitor Servers (nifSrv)\n"
"\n"
"SYNOPSIS\n"
"       nifViewer [-N|--dim-dns DIM_DNS_NODE] [-v|--debug]\n"
"                 [-m|--hostname NODE_PATTERN...]\n"
"                 [-c|--compact [-H|--header]] [-A|--average] [-M|--maximum]\n"
"                 [-T|--total] [-a|--all] [-x|--active] [-s|--settings]\n"
"       nifViewer {-V,--version} [-m|--hostname NODE_PATTERN...]\n"
"       nifViewer {-h,--help}\n"
"\n"
"DESCRIPTION\n"
"       Get the network interface statistics from the FMC Network Interface\n"
"       Monitor Servers of the farm nodes whose hostname matches the\n"
"       wildcard pattern NODE_PATTERN, specified using the -m command line\n"
"       option. In addition to instant values, average and maximum values can\n"
"       be printed by means of --average and --maximum switches, together\n"
"       with the time elapsed since the server start-up or since last server\n"
"       reset (i.e. reset of average and maximum values). Server reset is\n"
"       achieved through the nifReset(FMC) command.\n"
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
"              For each selected node print the FMC Network Interface Monitor\n"
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
"       --total, -T\n"
"              Toggle the printout of the total ethernet thoughput (the sum\n"
"              over all the Ethernet interfaces of the node).\n"
"       --all, -a\n"
"              Toggle all the optional (non-basic) fields. Equivalent to:\n"
"              \"--average --maximum --total\".\n"
"       --active, -x\n"
"              Print only data of active network interfaces (non zero data\n"
"              fields).\n"
"       --settings, -s\n"
"              Do not print statistics. Print only interface name, BUS\n"
"              address, MAC address, IP address, interface Speed, Duplex\n"
"              setting, Port type, Auto-negotiation setting and Link\n"
"              detection.\n"
"\n"
"ENVIRONMENT\n"
"       DIM_DNS_NODE (string, mandatory if not defined otherwise, see above)\n"
"              Host name of the node which is running the DIM DNS.\n"
"       LD_LIBRARY_PATH (string, mandatory if not set using ldconfig)\n"
"              Must include the path to the libraries \"libdim\"\n"
"              and \"libFMCutils\".\n"
"\n"
"EXAMPLES\n"
"       Show the network interface statistics of all the farm nodes:\n"
"              nifViewer\n"
"       Show also average and maximum values and Ethernet summary:\n"
"              nifViewer -a\n"
"              nifViewer --all\n"
"       Print output in compact format with header:\n"
"              nifViewer -acH\n"
"              nifViewer -a -c -H\n"
"              nifViewer --all --compact --header\n"
"              nifViewer --al --co --hea\n"
"       Show also average values:\n"
"              nifViewer --average\n"
"              nifViewer --av\n"
"              nifViewer -A\n"
"       Show also maximum values:\n"
"              nifViewer --maximum\n"
"              nifViewer --ma\n"
"              nifViewer -M\n"
"       Show also Ethernet summary (sum over all the Ethernet interfaces):\n"
"              nifViewer --total\n"
"              nifViewer --to\n"
"              nifViewer -T\n"
"       Show only active network interfaces:\n"
"              nifViewer -ax\n"
"              nifViewer --all --active\n"
"       Do not print statistics. Print only interface settings:\n"
"              nifViewer -s\n"
"              nifViewer --settings\n"
"       Show only the network interface statistics of selected nodes:\n"
"              nifViewer -m farm0101 -m farm0102 --ave\n"
"       Show only the statistics of wildcard-selected nodes:\n"
"              nifViewer -m \"farm01*\" -m \"farm02*\" --ave --max\n"
"              nifViewer -m \"farm010[1357]\" --all\n" 
"       Show the server version installed on the farm nodes:\n"
"              nifViewer -V\n"
"              nifViewer --version\n"
"\n"
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       nifReset(FMC), nifSrv(FMC), ifconfig(8), /proc/net/dev.\n"
"\n"
,rcsidP,FMC_VERSION
         );
  exit(1);
}
/*****************************************************************************/
