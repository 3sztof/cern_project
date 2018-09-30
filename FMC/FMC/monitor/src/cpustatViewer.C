/*****************************************************************************/
/*
 * $Log: cpustatViewer.C,v $
 * Revision 1.16  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.15  2008/10/22 12:50:29  galli
 * minor changes
 *
 * Revision 1.12  2008/02/20 23:25:53  galli
 * memory leak fixed
 *
 * Revision 1.10  2008/02/04 11:03:15  galli
 * bug fixed
 *
 * Revision 1.8  2008/02/02 22:31:15  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.5  2007/10/10 14:29:08  galli
 * bug fixed
 *
 * Revision 1.4  2007/09/07 09:32:59  galli
 * compatible with cpustatSrv 2.11
 * rewritten
 *
 * Revision 1.2  2005/11/14 14:12:59  galli
 * compatible with cpustatSrv 2.3
 *
 * Revision 1.1  2005/09/27 11:53:13  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>                                                  /* errno */
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
/*****************************************************************************/
#define DEV_FIELD_N_ERR 1
#define CTXT_FIELD_N_ERR 1
#define LOAD_FIELD_N_ERR 7
#define SRV_NAME "cpu/stat"
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: cpustatViewer.C,v 1.16 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
const char *srvName="cpu/stat";
/*****************************************************************************/
void usage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j,k,l,m;
  /* pointers */
  char *p,*lp,*up;
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
  int nolinkCtxtStrArrS=0;
  char *nolinkCtxtStrArr=NULL;
  int nolinkCtxtFloatS=0;
  float *nolinkCtxtFloat=NULL;
  int nolinkLoadStrArrS=0;
  char *nolinkLoadStrArr=NULL;
  int nolinkLoadFloatS=0;
  float *nolinkLoadFloat=NULL;
  int type=0;
  char *format=NULL;
  char *successSvc=NULL;
  int svcLen=0;
  char *svc=NULL;
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  int success=-1;
  char *devices=NULL;
  int devicesSz=0;
  int deviceC=0;
  char **deviceV=NULL;
  char *ctxtLabels=NULL;
  int ctxtLabelsSz=0;
  char *ctxtUnits=NULL;
  int ctxtUnitsSz=0;
  DimCurrentInfo **CtxtData=NULL;
  int *ctxtDataSz=NULL;
  float **ctxtData=NULL;
  int loadLabelsSz=0;
  char *loadLabels=NULL;
  int loadUnitsSz=0;
  char *loadUnits=NULL;
  DimCurrentInfo **LoadData=NULL;
  int *loadDataSz=NULL;
  float **loadData=NULL;
  /* printing variables */
  int ctxtFieldN;
  int loadFieldN;
  unsigned *fieldLen=NULL;
  unsigned len;
  char pFormat[256];
  int isFirstTime=1;
  char *line=NULL;
  int lineLen=0;
  /* command line switch */
  int doPrintServerVersion=0;
  int doPrintSepStates=0;
  int compactFormat=0;
  int doPrintHeader=0;
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
    {"separate",no_argument,NULL,'s'},
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  while((flag=getopt_long(argc,argv,"m:vVN:hcHas",longOptions,NULL))!=EOF)
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
        doPrintSepStates=!doPrintSepStates;
        break;
      case 's': /* -s, --separate */
        doPrintSepStates=!doPrintSepStates;
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
  /* Compose DIM no link error arrays */
  /* devices */
  nolinkDevStrArrS=DEV_FIELD_N_ERR*(1+strlen("N/F"));   /* "N/F"=not found */
  nolinkDevStrArr=(char*)malloc(nolinkDevStrArrS);
  for(i=0,p=nolinkDevStrArr;i<DEV_FIELD_N_ERR;i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
  /* ctxt data */
  nolinkCtxtFloatS=CTXT_FIELD_N_ERR*sizeof(float);
  nolinkCtxtFloat=(float*)malloc(nolinkCtxtFloatS);
  for(i=0;i<CTXT_FIELD_N_ERR;i++)nolinkCtxtFloat[i]=-1.0;  /* -1.0=not found */
  /* ctxt labels/units */
  nolinkCtxtStrArrS=CTXT_FIELD_N_ERR*(1+strlen("N/F"));   /* "N/F"=not found */
  nolinkCtxtStrArr=(char*)malloc(nolinkCtxtStrArrS);
  for(i=0,p=nolinkCtxtStrArr;i<CTXT_FIELD_N_ERR;i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
  /* load data */
  nolinkLoadFloatS=LOAD_FIELD_N_ERR*sizeof(float);
  nolinkLoadFloat=(float*)malloc(nolinkLoadFloatS);
  for(i=0;i<LOAD_FIELD_N_ERR;i++)nolinkLoadFloat[i]=-1.0;  /* -1.0=not found */
  /* load labels/units */
  nolinkLoadStrArrS=LOAD_FIELD_N_ERR*(1+strlen("N/F"));   /* "N/F"=not found */
  nolinkLoadStrArr=(char*)malloc(nolinkLoadStrArrS);
  for(i=0,p=nolinkLoadStrArr;i<LOAD_FIELD_N_ERR;i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
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
        /* read ctxt/labels */
        svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/ctxt/labels",ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"/FMC/%s/%s/ctxt/labels",ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo CtxtLabels(svc,nolinkCtxtStrArr,nolinkCtxtStrArrS);
        ctxtLabelsSz=(int)CtxtLabels.getSize();
        ctxtLabels=(char*)CtxtLabels.getData();
        /*...................................................................*/
        /* read ctxt/units */
        svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/ctxt/units",ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"/FMC/%s/%s/ctxt/units",ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo CtxtUnits(svc,nolinkCtxtStrArr,nolinkCtxtStrArrS);
        ctxtUnitsSz=(int)CtxtUnits.getSize();
        ctxtUnits=(char*)CtxtUnits.getData();
        /*...................................................................*/
        /* read ctxt/data */
        CtxtData=(DimCurrentInfo**)realloc(CtxtData,sizeof(DimCurrentInfo*));
        ctxtDataSz=(int*)realloc(ctxtDataSz,sizeof(int));
        ctxtData=(float**)realloc(ctxtData,sizeof(float*));
        svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/ctxt/data",ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"/FMC/%s/%s/ctxt/data",ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        CtxtData[0]=new DimCurrentInfo(svc,nolinkCtxtFloat,nolinkCtxtFloatS);
        ctxtDataSz[0]=(int)CtxtData[0]->getSize();
        ctxtData[0]=(float*)CtxtData[0]->getData();
        /*...................................................................*/
        /* read load/labels */
        svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/load/labels",ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"/FMC/%s/%s/load/labels",ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo LoadLabels(svc,nolinkLoadStrArr,nolinkLoadStrArrS);
        loadLabelsSz=(int)LoadLabels.getSize();
        loadLabels=(char*)LoadLabels.getData();
        /*...................................................................*/
        /* read load/units */
        svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/load/units",ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"/FMC/%s/%s/load/units",ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        DimCurrentInfo LoadUnits(svc,nolinkLoadStrArr,nolinkLoadStrArrS);
        loadUnitsSz=(int)LoadUnits.getSize();
        loadUnits=(char*)LoadUnits.getData();
        /*...................................................................*/
        LoadData=(DimCurrentInfo**)realloc(LoadData,
                                          (deviceC+1)*sizeof(DimCurrentInfo*));
        loadDataSz=(int*)realloc(loadDataSz,(deviceC+1)*sizeof(int));
        loadData=(float**)realloc(loadData,(deviceC+1)*sizeof(float*));
        /*...................................................................*/
        /* read load/average/data */
        svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/load/average/data",ucNodeFound,
                          srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"/FMC/%s/%s/load/average/data",ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
        LoadData[0]=new DimCurrentInfo(svc,nolinkLoadFloat,nolinkLoadFloatS);
        loadDataSz[0]=(int)LoadData[0]->getSize();
        loadData[0]=(float*)LoadData[0]->getData();
        /*...................................................................*/
        /* read load/core_XX/data */
        if(doPrintSepStates)
        {
          for(k=0;k<deviceC;k++)
          {
            if(!strcmp(deviceV[k],"N/F"))break;
            svcLen=1+snprintf(NULL,0,"/FMC/%s/%s/load/%s/data",
                              ucNodeFound,srvName,deviceV[k]);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"/FMC/%s/%s/load/%s/data",ucNodeFound,srvName,
                    deviceV[k]);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,svc);
            LoadData[k+1]=new DimCurrentInfo(svc,nolinkLoadFloat,
                                             nolinkLoadFloatS);
            loadDataSz[k+1]=(int)LoadData[k+1]->getSize();
            loadData[k+1]=(float*)LoadData[k+1]->getData();
          }
        }
        /*...................................................................*/
        /* evaluate field numbers */
        ctxtFieldN=0;
        for(lp=ctxtLabels;lp<ctxtLabels+ctxtLabelsSz;lp+=(1+strlen(lp)))
        {
          ctxtFieldN++;
        }
        loadFieldN=0;
        for(lp=loadLabels;lp<loadLabels+loadLabelsSz;lp+=(1+strlen(lp)))
        {
          loadFieldN++;
        }
        /*...................................................................*/
        if(!compactFormat)
        {
          /*.................................................................*/
          printf("    NODE %s:\n",nodeFound);
          /*.................................................................*/
          /* evaluate fields size */
          /* core# + ctxtFieldN + loadFieldN */
          fieldLen=(unsigned*)realloc(fieldLen,(1+ctxtFieldN+loadFieldN)*
                                      sizeof(unsigned));
          memset(fieldLen,0,(1+ctxtFieldN+loadFieldN)*sizeof(unsigned));
          /* cpu id */
          fieldLen[0]=strlen("AVER");                                /* data */
          if(fieldLen[0]<strlen("CORE"))fieldLen[0]=strlen("CORE"); /* label */
          if(doPrintSepStates)
          {
            len=snprintf(NULL,0,"%02d",deviceC-1);
            if(fieldLen[0]<len)fieldLen[0]=len;                  /* opt data */
          }
          /* ctxt rate */
          for(l=0;l<ctxtFieldN;l++)
          {
            fieldLen[1+l]=snprintf(NULL,0,"%.2f",ctxtData[0][l]);    /* data */
          }
          lp=ctxtLabels;
          up=ctxtUnits;
          for(l=0;l<ctxtFieldN;l++)
          {
            if(fieldLen[1+l]<strlen(lp))fieldLen[1+l]=strlen(lp);   /* label */
            lp+=(1+strlen(lp));
            if(fieldLen[1+l]<strlen(up))fieldLen[1+l]=strlen(up);    /* unit */
            up+=(1+strlen(up));
          }
          /* load fields */
          /* data */
          for(l=0;l<loadFieldN;l++)
          {
            fieldLen[1+ctxtFieldN+l]=snprintf(NULL,0,"%.2f",loadData[0][l]);
          }
          /* opt data */
          if(doPrintSepStates)
          {
            for(k=0,m=0;k<deviceC+1;k++)
            {
              for(l=0;l<loadFieldN;l++,m++)
              {
                len=snprintf(NULL,0,"%.2f",loadData[k][l]);
                if(fieldLen[1+ctxtFieldN+l]<len)fieldLen[1+ctxtFieldN+l]=len;
              }
            }
          }
          lp=loadLabels;
          up=loadUnits;
          for(l=0;l<loadFieldN;l++)
          {
            /* label */
            len=strlen(lp);
            if(fieldLen[1+ctxtFieldN+l]<len)fieldLen[1+ctxtFieldN+l]=len;
            lp+=(1+strlen(lp));
            /* unit */
            len=2+strlen(up);                                          /* [] */
            if(fieldLen[1+ctxtFieldN+l]<len)fieldLen[1+ctxtFieldN+l]=len;
            up+=(1+strlen(up));
          }
          /*.................................................................*/
          /* evaluate line separator length */
          lineLen=0;
          for(l=0;l<loadFieldN+2;l++)lineLen+=(1+fieldLen[l]);
          lineLen--;
          line=(char*)realloc(line,(1+lineLen)*sizeof(char));
          memset(line,'-',lineLen);
          line[lineLen]='\0';
          /*.................................................................*/
          /* print header */
          printf("    ");
          printf("%s\n",line);
          /* first line */
          printf("    ");
          /* core N */
          sprintf(pFormat,"%%%ds",fieldLen[0]);
          printf(pFormat,"CORE");
          /* ctxt rate */
          lp=ctxtLabels;
          for(l=0;l<ctxtFieldN;l++)
          {
            sprintf(pFormat," %%%ds",fieldLen[1+l]);
            for(p=lp;*p;p++)
            {
              *p=toupper(*p);                        /* convert to uppercase */
              if(*p==' ')*p='_';      /* avoid spaces to make parsing easier */
            }
            printf(pFormat,lp);
            lp+=(1+strlen(lp));
          }
          /* load fields */
          lp=loadLabels;
          for(l=0;l<loadFieldN;l++)
          {
            sprintf(pFormat," %%%ds",fieldLen[1+ctxtFieldN+l]);
            for(p=lp;*p;p++)
            {
              *p=toupper(*p);                        /* convert to uppercase */
              if(*p==' ')*p='_';      /* avoid spaces to make parsing easier */
            }
            printf(pFormat,lp);
            lp+=(1+strlen(lp));
          }
          printf("\n");
          /* second line */
          printf("    ");
          /* core N */
          sprintf(pFormat,"%%%ds",fieldLen[0]);
          printf(pFormat,"");
          /* ctxt rate */
          up=ctxtUnits;
          for(l=0;l<ctxtFieldN;l++)
          {
            char funits[fieldLen[1+l]];
            sprintf(pFormat," %%%ds",fieldLen[1+l]);
            for(p=up;*p;p++)
            {
              if(*p==' ')*p='_';      /* avoid spaces to make parsing easier */
            }
            sprintf(funits,"[%s]",up);
            printf(pFormat,funits);
            up+=(1+strlen(up));
          }
          /* load fields */
          up=loadUnits;
          for(l=0;l<loadFieldN;l++)
          {
            char funits[fieldLen[1+ctxtFieldN+l]];
            sprintf(pFormat," %%%ds",fieldLen[1+ctxtFieldN+l]);
            for(p=up;*p;p++)
            {
              if(*p==' ')*p='_';      /* avoid spaces to make parsing easier */
            }
            sprintf(funits,"[%s]",up);
            printf(pFormat,funits);
            up+=(1+strlen(up));
          }
          printf("\n");
          printf("    ");
          printf("%s\n",line);
          /*.................................................................*/
          /* print data */
          /* average CPU state */
          printf("    ");
          /* cpu N */
          sprintf(pFormat,"%%%ds",fieldLen[0]);
          printf(pFormat,"AVER");
          /* ctxt rate */
          for(l=0;l<ctxtFieldN;l++)
          {
            sprintf(pFormat," %%%d.2f",fieldLen[1+l]);
            printf(pFormat,ctxtData[0][l]);
          }
          /* load fields */
          for(l=0;l<loadFieldN;l++)
          {
            sprintf(pFormat," %%%d.2f",fieldLen[1+ctxtFieldN+l]);
            printf(pFormat,loadData[0][l]);
          }
          printf("\n");
          /* separate CPU state */
          if(doPrintSepStates)
          {
            char deviceCS[8];
            for(k=0;k<deviceC;k++)
            {
              printf("    ");
              /* cpu N */
              snprintf(deviceCS,8,"%02d",k);
              sprintf(pFormat,"%%%ds",fieldLen[0]);
              printf(pFormat,deviceCS);
              /* ctxt rate (empty) */
              for(l=0;l<ctxtFieldN;l++)
              {
                sprintf(pFormat," %%%ds",fieldLen[1+l]);
                printf(pFormat,"");
              }
              /* load fields */
              for(l=0;l<loadFieldN;l++)
              {
                sprintf(pFormat," %%%d.2f",fieldLen[1+ctxtFieldN+l]);
                printf(pFormat,loadData[k+1][l]);
              }
              printf("\n");
            }
          }
          printf("    ");
          printf("%s\n",line);
          /*.................................................................*/
        }                                              /* if(!compactFormat) */
        else                                            /* if(compactFormat) */
        {
          /*.................................................................*/
          if(doPrintHeader && isFirstTime)
          {
            isFirstTime=0;
            /* hostname */
            printf("%s","HOSTNAME");
            /* core N */
            printf(" %s","CORE");
            /* load fields */
            lp=loadLabels;
            up=loadUnits;
            for(l=0;l<loadFieldN;l++)
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
            /* ctxt rate */
            lp=ctxtLabels;
            up=ctxtUnits;
            for(l=0;l<ctxtFieldN;l++)
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
          }                              /* if(doPrintHeader && isFirstTime) */
          /*.................................................................*/
          /* print data */
          /* average CPU state */
          /* hostname */
          printf("%s",nodeFound);
          /* cpu N */
          printf(" %s","AV");
          /* load fields */
          for(l=0;l<loadFieldN;l++)
          {
            printf(" %.2f",loadData[0][l]);
          }
          /* ctxt rate */
          for(l=0;l<ctxtFieldN;l++)
          {
            printf(" %.2f",ctxtData[0][l]);
          }
          printf("\n");
          /* separate CPU state */
          if(doPrintSepStates)
          {
            char deviceCS[8];
            for(k=0;k<deviceC;k++)
            {
              /* hostname */
              printf("%s",nodeFound);
              /* cpu N */
              snprintf(deviceCS,8,"%02d",k);
              printf(" %s",deviceCS);
              /* load fields */
              for(l=0;l<loadFieldN;l++)
              {
                printf(" %.2f",loadData[k+1][l]);
              }
              printf("\n");
            }
          }
          /*.................................................................*/
        }                                               /* if(compactFormat) */
        /*...................................................................*/
      }                                                     /* if !fnmatch() */
      /* free allocated memory */
      for(int k=0;k<deviceC;k++)if(deviceV[k])free(deviceV[k]);
    }                                                     /* loop over nodes */
    /*.......................................................................*/
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
  FmcUtils::printServerVersion(hostPttnC,hostPttnV,"","cpustat",
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
"       cpustatViewer - Get CPU state information from the FMC CPU state\n"
"                       Monitor Servers\n"
"\n"
"SYNOPSIS\n"
"       cpustatViewer [-N|--dim-dns DIM_DNS_NODE] [-v|--debug]\n"
"                     [-m|--hostname NODE_PATTERN...]\n"
"                     [-c|--compact [-H|--header]] [-s|--separate]\n"
"                     [-a|--all]\n"
"       cpustatViewer {-V,--version} [-m|--hostname NODE_PATTERN...]\n"
"       cpustatViewer {-h,--help}\n"
"\n"
"DESCRIPTION\n"
"       Get the context switch rate and the CPU states from the farm nodes\n"
"       whose hostname matches the wildcard pattern NODE_PATTERN, specified\n"
"       using the -m command line option. In addition to global node values,\n"
"       separate CPU values can be printed by means of --separate switch.\n"
"       Printed data are the context switch rates measured in Hz and the\n"
"       fraction of time spent by the CPUs performing different kinds of\n"
"       work:\n"
"              ctxt_rate\n"
"                     Total number of context switches per second across all\n"
"                     the CPUs.\n"
"              user   Time percentage the CPU has spent in normal processes\n"
"                     executing in user mode.\n"
"              system Time percentage the CPU has spent in normal processes\n"
"                     executing in kernel mode.\n"
"              nice   Time percentage the CPU has spent in niced processes\n"
"                     in user mode. Time spent in niced tasks will also be\n"
"                     counted in system and user time.\n"
"              idle   Time percentage the CPU has spent not working.\n"
"              iowait Time percentage the CPU has spent in waiting for I/O to\n"
"                     complete.\n"
"              irq    Time percentage the CPU has spent in servicing\n"
"                     interrupts (top halves). Available only with kernel\n"
"                     version >=2.5 (with previous kernel version this number\n"
"                     is set to zero).\n"
"              softirq\n"
"                     Time percentage the CPU has spent in servicing softirqs\n"
"                     (bottom halves). Available only with kernel version\n"
"                     >=2.5 (with previous kernel version this number is set\n"
"                     to zero).\n"
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
"              For each selected node print the FMC CPU state Monitor\n"
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
"       --separate, -s\n"
"              Toggle the printout of the separate CPU states.\n"
"       --maximum, -M\n"
"              Toggle the printout of the maximum values.\n"
"       --all, -a\n"
"              Toggle all the optional (non-basic) fields. Equivalent to:\n"
"              \"--separate\".\n"
"\n"
"ENVIRONMENT\n"
"       DIM_DNS_NODE (string, mandatory if not defined otherwise, see above)\n"
"              Host name of the node which is running the DIM DNS.\n"
"       LD_LIBRARY_PATH (string, mandatory if not set using ldconfig)\n"
"              Must include the path to the libraries \"libdim\"\n"
"              and \"libFMCutils\".\n"
"\n"
"EXAMPLES\n"
"       Show the CPU states of all the farm nodes:\n"
"              cpustatViewer\n"
"       Show also average and maximum values:\n"
"              cpustatViewer -a\n"
"              cpustatViewer --all\n"
"       Print output in compact format with header:\n"
"              cpustatViewer -acH\n"
"              cpustatViewer -a -c -H\n"
"              cpustatViewer --all --compact --header\n"
"              cpustatViewer --al --co --hea\n"
"       Show also separate CPU states:\n"
"              cpustatViewer --separate\n"
"              cpustatViewer --se\n"
"              cpustatViewer -s\n"
"       Show only the CPU states of selected nodes:\n"
"              cpustatViewer -m farm0101 -m farm0102 --sep\n"
"       Show only the CPU states of wildcard-selected nodes:\n"
"              cpustatViewer -m \"farm01*\" -m \"farm02*\" --sep\n"
"              cpustatViewer -m \"farm010[1357]\" --all\n" 
"       Show the server version installed on the farm nodes:\n"
"              cpustatViewer -V\n"
"              cpustatViewer --version\n"
"\n"
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       cpustatSrv(FMC), proc(5), top(1), /proc/stat.\n"
"       /usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt."
"\n"
,rcsidP,FMC_VERSION
         );
  exit(1);
}
/*****************************************************************************/
