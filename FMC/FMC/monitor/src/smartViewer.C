/*****************************************************************************/
/*
 * $Log: smartViewer.C,v $
 * Revision 1.11  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.10  2008/10/22 11:15:20  galli
 * minor changes
 *
 * Revision 1.7  2007/12/24 11:06:36  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.4  2006/08/01 13:15:35  galli
 * works also with SCSI disks
 *
 * Revision 1.2  2006/05/04 12:43:21  galli
 * bug fixed
 *
 * Revision 1.1  2006/05/04 12:38:37  galli
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
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>
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
#define MEM_ITEM_N 33    /* number of data to be read; included derived ones */
#define SRV_NAME "disk"
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: smartViewer.C,v 1.11 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
const char *srvName="disk";
/*****************************************************************************/
void usage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,k;
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
  static float nolinkUl[MEM_ITEM_N];
  static int nolinkL[1]={-1};
  static int nolinkU[1]={~0};
  static uint64_t nolinkULL[1]={~0ULL};
  int type=0;
  char *format=NULL;
  char *successSvc=NULL;
  char *successSvcDup=NULL;
  char *svc=NULL;
  int svcLen=0;
  char *dimDnsNode=NULL;
  /* retrieved quantities */
  int success=-1;
  int diskN=0;
  char *diskNameLs=NULL;
  int diskNameLsSz=0;
  char *diskName=NULL;
  uint64_t size=~0x0ULL;
  char *diskType=NULL;
  char *deviceModel=NULL;
  char *serialNumber=NULL;
  char *transportProtocol=NULL;
  char *firmwareVersion=NULL;
  int ataVersion=0;
  char *ataStandard=NULL;
  int attrN=0;
  int *attrId=NULL;
  int attrNameSz=0;
  char *attrNameLs=NULL;
  char *attrName=NULL;
  int attrUnitsSz=0;
  char *attrUnitsLs=NULL;
  char *attrUnits=NULL;
  unsigned *attrFlag=NULL;
  unsigned *ataAttrValue=NULL;
  uint64_t *scsiAttrValue=NULL;
  unsigned *attrWorst=NULL;
  unsigned *attrThresh=NULL;
  int attrTypeSz=0;
  char *attrTypeLs=NULL;
  char *attrType=NULL;
  int attrUpdatedSz=0;
  char *attrUpdatedLs=NULL;
  char *attrUpdated=NULL;
  int attrWhenFailedSz=0;
  char *attrWhenFailedLs=NULL;
  char *attrWhenFailed=NULL;
  uint64_t *attrRawValue=NULL;
  int errN=0;
  int errNameSz=0;
  char *errNameLs=NULL;
  char *errName=NULL;
  uint64_t *errRValue=NULL;
  uint64_t *errWValue=NULL;
  uint64_t *errVValue=NULL;
  /* printing variables */
  char *line1=NULL;
  int lineLen1=0;
  char *line2=NULL;
  int lineLen2=0;
  /* command line switch */
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
    {NULL,0,NULL,0}
  };
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  while((flag=getopt_long(argc,argv,"m:vVN:h",longOptions,NULL))!=EOF)
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
  printf("Node patterns list:  ");
  for(i=0;i<hostNamePattC;i++)                  /* loop over node patterns */
  {
    printf("\"%s\"",hostNamePattV[i]);
    if(i<hostNamePattC-1)printf(", ");
  }
  printf(".\n");
  /*-------------------------------------------------------------------------*/
  /* compose line separator */
  lineLen1=102;
  line1=(char*)realloc(line1,(1+lineLen1)*sizeof(char));
  memset(line1,'=',lineLen1);
  line1[lineLen1]='\0';
  lineLen2=lineLen1-2;
  line2=(char*)realloc(line2,(1+lineLen2)*sizeof(char));
  memset(line2,'-',lineLen2);
  line2[lineLen2]='\0';
  /*-------------------------------------------------------------------------*/
  for(i=0;i<MEM_ITEM_N;i++)nolinkUl[i]=~0UL;
  /*-------------------------------------------------------------------------*/
  for(i=0,k=0;i<hostNamePattC;i++)                /* loop over node patterns */
  {
    printf("  Node pattern: \"%s\":\n",hostNamePattV[i]);
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
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,successSvc);
        DimCurrentInfo Success(successSvc,-1);
        success=(int)Success.getInt();
        if(success!=1)
        {
          fprintf(stderr,"Service \"%s\" unreachable!\n",successSvc);
          continue;
        }
        /*...................................................................*/
        /* read disk mumber */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/N",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/N",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
        DimCurrentInfo DiskN(svc,-1);
        diskN=(int)DiskN.getInt();
        printf("    %s\n",line1);
        printf("    NODE: %s. Disk number %d:\n",nodeFound,diskN);
        printf("    %s\n",line1);
        if(diskN==-1)
        {
          fprintf(stderr,"Invalid disk number %d!\n",diskN);
          continue;
        }
        /*...................................................................*/
        /* read disk names */
        svcLen=1+snprintf(NULL,0,"%s/%s/%s/name",SVC_HEAD,ucNodeFound,srvName);
        svc=(char*)realloc(svc,svcLen);
        sprintf(svc,"%s/%s/%s/name",SVC_HEAD,ucNodeFound,srvName);
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
        DimCurrentInfo Name(svc,(char*)"Server unreachable!");
        diskNameLsSz=(int)Name.getSize();
        diskNameLs=(char*)Name.getData();
        /*...................................................................*/
        for(i=0,diskName=diskNameLs;diskName<diskNameLs+diskNameLsSz;
            diskName=1+strchr(diskName,'\0'),i++)
        {
          printf("      Disk: %s:\n",diskName);
          /*.................................................................*/
          /* read disk sizes */
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/info/size",SVC_HEAD,ucNodeFound,srvName,
                            diskName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/%s/info/size",SVC_HEAD,ucNodeFound,srvName,diskName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
          DimCurrentInfo Size(svc,(longlong)~0x0ULL);
          size=(uint64_t)Size.getLonglong();
          printf("      Size: %f GiB = %"PRIu64" B\n",
                 (float)size/1024./1024./1024.,size);
          /*.................................................................*/
          /* read disk type */
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/info/type",SVC_HEAD,ucNodeFound,srvName,
                            diskName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/%s/info/type",SVC_HEAD,ucNodeFound,srvName,diskName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
          DimCurrentInfo DiskType(svc,(char*)"Server unreachable!");
          diskType=(char*)DiskType.getString();
          printf("      Type: %s\n",diskType);
          /*.................................................................*/
          /* read disk device model */
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/info/deviceModel",SVC_HEAD,ucNodeFound,
                            srvName,diskName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/%s/info/deviceModel",SVC_HEAD,ucNodeFound,srvName,
                  diskName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
          DimCurrentInfo DeviceModel(svc,(char*)"Server unreachable!");
          deviceModel=(char*)DeviceModel.getString();
          printf("      Device model: \"%s\"\n",deviceModel);
          /*.................................................................*/
          /* read disk serial number */
          svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/info/serialNumber",SVC_HEAD,ucNodeFound,
                            srvName,diskName);
          svc=(char*)realloc(svc,svcLen);
          sprintf(svc,"%s/%s/%s/%s/info/serialNumber",SVC_HEAD,ucNodeFound,srvName,
                  diskName);
          if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
          DimCurrentInfo SerialNumber(svc,(char*)"Server unreachable!");
          serialNumber=(char*)SerialNumber.getString();
          printf("      Serial number: \"%s\"\n",serialNumber);
          /*.................................................................*/
          if(!strcmp(diskType,"SCSI"))
          {
            /*...............................................................*/
            /* read disk transport protocol */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/info/transportProtocol",
                              SVC_HEAD,ucNodeFound,srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/info/transportProtocol",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo TransportProtocol(svc,(char*)"Server unreachable!");
            transportProtocol=(char*)TransportProtocol.getString();
            printf("      Transport protocol: \"%s\"\n",transportProtocol);
            /*...............................................................*/
            /* read disk smart attribute number */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/N",SVC_HEAD,ucNodeFound,srvName,
                              diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/N",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrN(svc,-1);
            attrN=(int)AttrN.getInt();
            printf("      Attribute number: %d",attrN);fflush(stdout);
            if(deBug)printf("\n");
            /*...............................................................*/
            /* read disk smart attribute names */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/name",SVC_HEAD,ucNodeFound,srvName,
                              diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/name",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrName(svc,(char*)"Server unreachable!");
            attrNameSz=(int)AttrName.getSize();
            attrNameLs=(char*)AttrName.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute values */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/value",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/value",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrValue(svc,nolinkULL,attrN*sizeof(uint64_t));
            scsiAttrValue=(uint64_t*)AttrValue.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute units */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/units",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/units",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrUnits(svc,(char*)"Server unreachable!");
            attrUnitsSz=(int)AttrUnits.getSize();
            attrUnitsLs=(char*)AttrUnits.getData();
            if(!deBug){printf(".\n");}
            /*...............................................................*/
            /* print SCSI attributes */
            printf("        # ATTRIBUTE_NAME                                 "
                   "                             VALUE UNITS\n");
            attrName=attrNameLs;
            attrUnits=attrUnitsLs;
            for(int j=0;j<attrN;j++)
            {
              printf("      %3d %-60s %20"PRIu64" %s\n",
                     j,attrName,scsiAttrValue[j],
                     attrUnits);
              attrName=1+strchr(attrName,'\0');
              if(attrName==(char*)1||attrName>attrNameLs+attrNameSz)break;
              attrUnits=1+strchr(attrUnits,'\0');
              if(attrUnits==(char*)1||attrUnits>attrUnitsLs+attrUnitsSz)break;
            }                                        /* loop over attributes */
            /*...............................................................*/
            /* read disk smart error type number */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/err/N",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/err/N",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo ErrN(svc,-1);
            errN=(int)ErrN.getInt();
            printf("      Error type number: %d",errN);fflush(stdout);
            if(deBug)printf("\n");
            /*...............................................................*/
            /* read disk smart error names */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/err/name",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/err/name",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo ErrName(svc,(char*)"Server unreachable!");
            errNameSz=(int)ErrName.getSize();
            errNameLs=(char*)ErrName.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart read error value */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/err/value/read",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/err/value/read",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo ErrRValue(svc,nolinkULL,errN*sizeof(uint64_t));
            errRValue=(uint64_t*)ErrRValue.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart write error value */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/err/value/write",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/err/value/write",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo ErrWValue(svc,nolinkULL,errN*sizeof(uint64_t));
            errWValue=(uint64_t*)ErrWValue.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart verify error value */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/err/value/verify",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/err/value/verify",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo ErrVValue(svc,nolinkULL,errN*sizeof(uint64_t));
            errVValue=(uint64_t*)ErrVValue.getData();
            if(!deBug){printf(".\n");}
            /*...............................................................*/
            /* print SCSI errors */
            printf("        # ERROR TYPE                                     "
                   "  READ                WRITE               VERIFY\n");
            errName=errNameLs;
            for(int j=0;j<errN;j++)
            {
              printf("      %3d %-32s %20"PRIu64" %20"PRIu64" %20"PRIu64"\n",
                     j,errName,errRValue[j],errWValue[j],errVValue[j]);
              errName=1+strchr(errName,'\0');
              if(errName==(char*)1||errName>errNameLs+errNameSz)break;
            }                                            /* loop over errors */
            /*...............................................................*/
          }
          else if(!strcmp(diskType,"ATA")||!strcmp(diskType,"SATA"))
          {
            /*...............................................................*/
            /* read disk firmware version */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/info/firmwareVersion",
                              SVC_HEAD,ucNodeFound,srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/info/firmwareVersion",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo FirmwareVersion(svc,(char*)"Server unreachable!");
            firmwareVersion=(char*)FirmwareVersion.getString();
            printf("      Firmware version: \"%s\"\n",firmwareVersion);
            /*...............................................................*/
            /* read disk ata version */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/info/ataVersion",
                              SVC_HEAD,ucNodeFound,srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/info/ataVersion",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AtaVersion(svc,-1);
            ataVersion=(int)AtaVersion.getInt();
            printf("      Ata version: %d\n",ataVersion);
            /*...............................................................*/
            /* read disk ata standard */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/info/ataStandard",
                              SVC_HEAD,ucNodeFound,srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/info/ataStandard",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AtaStandard(svc,(char*)"Server unreachable!");
            ataStandard=(char*)AtaStandard.getString();
            printf("      Ata standard: \"%s\"\n",ataStandard);
            /*...............................................................*/
            /* read disk smart attribute number */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/N",SVC_HEAD,ucNodeFound,srvName,
                              diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/N",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrN(svc,-1);
            attrN=(int)AttrN.getInt();
            printf("      Attribute number: %d",attrN);fflush(stdout);
            if(deBug)printf("\n");
            /*...............................................................*/
            /* read disk smart attribute Ids */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/id",SVC_HEAD,ucNodeFound,srvName,
                              diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/id",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrId(svc,nolinkL,attrN*sizeof(int));
            attrId=(int*)AttrId.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute names */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/name",SVC_HEAD,ucNodeFound,srvName,
                              diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/name",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrName(svc,(char*)"Server unreachable!");
            attrNameSz=(int)AttrName.getSize();
            attrNameLs=(char*)AttrName.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute flag */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/flag",SVC_HEAD,ucNodeFound,srvName,
                              diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/flag",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrFlag(svc,nolinkU,attrN*sizeof(int));
            attrFlag=(unsigned*)AttrFlag.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute value */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/value",SVC_HEAD,ucNodeFound,srvName,
                              diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/value",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrValue(svc,nolinkU,attrN*sizeof(int));
            ataAttrValue=(unsigned*)AttrValue.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute worst */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/worst",SVC_HEAD,ucNodeFound,srvName,
                              diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/worst",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrWorst(svc,nolinkU,attrN*sizeof(int));
            attrWorst=(unsigned*)AttrWorst.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute theshold */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/threshold",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/threshold",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrThresh(svc,nolinkU,attrN*sizeof(int));
            attrThresh=(unsigned*)AttrThresh.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute type */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/type",SVC_HEAD,ucNodeFound,srvName,
                              diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/type",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrType(svc,(char*)"Server unreachable!");
            attrTypeSz=(int)AttrType.getSize();
            attrTypeLs=(char*)AttrType.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute updated */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/updated",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/updated",SVC_HEAD,ucNodeFound,srvName,diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrUpdated(svc,(char*)"Server unreachable!");
            attrUpdatedSz=(int)AttrUpdated.getSize();
            attrUpdatedLs=(char*)AttrUpdated.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute whenFailed */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/whenFailed",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/whenFailed",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrWhenFailed(svc,(char*)"Server unreachable!");
            attrWhenFailedSz=(int)AttrWhenFailed.getSize();
            attrWhenFailedLs=(char*)AttrWhenFailed.getData();
            if(!deBug){printf(".");fflush(stdout);}
            /*...............................................................*/
            /* read disk smart attribute rawValue */
            svcLen=1+snprintf(NULL,0,"%s/%s/%s/%s/attr/rawValue",SVC_HEAD,ucNodeFound,
                              srvName,diskName);
            svc=(char*)realloc(svc,svcLen);
            sprintf(svc,"%s/%s/%s/%s/attr/rawValue",SVC_HEAD,ucNodeFound,srvName,
                    diskName);
            if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",k++,svc);
            DimCurrentInfo AttrRawValue(svc,nolinkULL,attrN*sizeof(uint64_t));
            attrRawValue=(uint64_t*)AttrRawValue.getData();
            if(!deBug){printf(".\n");}
            /*...............................................................*/
            /* print (S)ATA attributes */
            printf("        # ID# ATTRIBUTE_NAME           FLAG  VAL WOR THR   "
                   "TYPE   UPDATED WHEN_FAILED            RAW_VALUE\n");
            attrName=attrNameLs;
            attrType=attrTypeLs;
            attrUpdated=attrUpdatedLs;
            attrWhenFailed=attrWhenFailedLs;
            for(int j=0;j<attrN;j++)
            {
              printf("      %3d %3d %-23s %#6.4x %3d %3d %3d %-8s %-7s %-11s "
                     "%20"PRIu64"\n",
                     j,attrId[j],attrName,attrFlag[j],ataAttrValue[j],
                     attrWorst[j],attrThresh[j],attrType,attrUpdated,
                     attrWhenFailed,attrRawValue[j]);
              attrName=1+strchr(attrName,'\0');
              if(attrName==(char*)1||attrName>attrNameLs+attrNameSz)break;
              attrType=1+strchr(attrType,'\0');
              if(attrType==(char*)1||attrType>attrTypeLs+attrTypeSz)break;
              attrUpdated=1+strchr(attrUpdated,'\0');
              if(attrUpdated==(char*)1||attrUpdated>attrUpdatedLs+attrUpdatedSz)
                break;
              attrWhenFailed=1+strchr(attrWhenFailed,'\0');
              if(attrWhenFailed==(char*)1||
                 attrWhenFailed>attrWhenFailedLs+attrWhenFailedSz)
                break;
            }                                        /* loop over attributes */
          }                                            /* if diskType (S)ATA */
          if(i<diskN-1)printf("      %s\n",line2);
        }                                                 /* loop over disks */
        /*...................................................................*/
      }                                                     /* if !fnmatch() */
    }                                                     /* loop over nodes */
    if(!nodeFoundN)
    {
      printf("    No node found for pattern: \"%s\"!\n",hostNamePattV[i]);
    }
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  return 0;
}                                                                  /* main() */
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
"       smartViewer - Get the SMART statistics from the FMC SMART Monitor\n"
"                     Servers (smartSrv)\n"
"\n"
"SYNOPSIS\n"
"       smartViewer [-N|--dim-dns DIM_DNS_NODE] [-v|--debug]\n"
"                   [-m|--hostname NODE_PATTERN...]\n"
"       smartViewer {-V,--version} [-m|--hostname NODE_PATTERN...]\n"
"       smartViewer {-h,--help}\n"
"\n"
"DESCRIPTION\n"
"       Get the SMART statistics from the FMC SMART Monitor Servers of the\n"
"       farm nodes whose hostname matches the wildcard pattern NODE_PATTERN,\n"
"       specified using the -m command line option.\n"
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
"              For each selected node print the FMC SMART Monitor Server\n"
"              version, than exit.\n"
"       --dim-dns, -N DIM_DNS_NODE (string)\n"
"              Use DIM_DNS_NODE as DIM Name Server node.\n"
"       --debug, -v\n"
"              Increase verbosity for debugging.\n"
"       --hostname, -m NODE_PATTERN (string, repeatable)\n"
"              Get information from the nodes whose hostname matches the\n"
"              wildcard pattern NODE_PATTERN (default: get information from\n"
"              all the nodes).\n"
"\n"
"ENVIRONMENT\n"
"       DIM_DNS_NODE (string, mandatory if not defined otherwise, see above)\n"
"              Host name of the node which is running the DIM DNS.\n"
"       LD_LIBRARY_PATH (string, mandatory if not set using ldconfig)\n"
"              Must include the path to the libraries \"libdim\"\n"
"              and \"libFMCutils\".\n"
"\n"
"EXAMPLES\n"
"       Show the SMART statistics of all the farm nodes:\n"
"              smartViewer\n"
"       Show only the SMART statistics of selected nodes:\n"
"              smartViewer -m farm0101 -m farm0102\n"
"       Show only the statistics of wildcard-selected nodes:\n"
"              smartViewer -m \"farm01*\" -m \"farm02*\"\n"
"              smartViewer -m \"farm010[1357]\"\n" 
"       Show the server version installed on the farm nodes:\n"
"              smartViewer -V\n"
"              smartViewer --version\n"
"\n"
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       smartSrv(FMC), smartctl(8), /proc/partitions.\n"
"\n"
,rcsidP,FMC_VERSION
         );
  exit(1);
}
/*****************************************************************************/
