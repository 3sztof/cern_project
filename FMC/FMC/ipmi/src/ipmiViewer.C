/*****************************************************************************/
/*
 * $Log: ipmiViewer.C,v $
 * Revision 1.23  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.22  2009/01/22 10:17:26  galli
 * do not depend on kernel-devel headers
 *
 * Revision 1.21  2008/10/16 10:19:54  galli
 * minor changes
 *
 * Revision 1.20  2008/10/15 14:35:38  galli
 * condition variable to wait for the I/O thread
 * groff manual
 *
 * Revision 1.18  2007/12/14 14:56:23  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.17  2007/10/23 16:05:44  galli
 * usage() prints FMC version
 *
 * Revision 1.13  2007/10/10 12:00:43  galli
 * multiple host pattern allowed
 *
 * Revision 1.8  2007/08/03 12:55:27  galli
 * added current measurements
 *
 * Revision 1.7  2006/02/22 12:43:28  galli
 * temperature read and printed as float
 *
 * Revision 1.6  2006/02/22 12:25:01  galli
 * rewritten: uses DimInfo instead of DimCurrentInfo
 * 20 times faster
 *
 * Revision 1.4  2006/02/10 14:21:27  galli
 * added timestamps for power status and sensor readings
 *
 * Revision 1.3  2006/02/09 12:19:41  galli
 * prints error codes
 *
 * Revision 1.2  2006/01/20 07:56:03  galli
 * print also power status
 *
 * Revision 1.1  2006/01/19 14:00:03  galli
 * Initial revision
 *
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <getopt.h>                                         /* getopt_long() */
#include <time.h>                                            /* nanosleep(2) */
#include <errno.h>
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
/* DIM */
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
/* in ipmi/include */
#include "ipmiSrv.h"                                             /* SRV_NAME */
#include "ipmiUtils.h"                                        /* error codes */
/*****************************************************************************/
/* globals */
int deBug=0;
char sepLine[80];
/* configuration */
const char *dimConfFile=DIM_CONF_FILE_NAME;
/* revision */
static char rcsid[]="$Id: ipmiViewer.C,v 1.23 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
/* inter-thread communication */
int totNodeFoundC=0;
/* mutex which protects totNodeFoundC */
pthread_mutex_t totNodeFoundMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t totNodeFoundCond=PTHREAD_COND_INITIALIZER;
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
class IpmiInfo:public DimClient
{
  char *hostName;
  int nodeSvcC;
  /*.........................................................................*/
  string  pwStatusSvc;
  DimInfo *PwStatus;
  int     pwStatus;
  /*.........................................................................*/
  string  pwTsSvc, sensorsTsSvc;
  DimInfo *PwTs,   *SensorsTs;
  time_t  pwTs,    sensorsTs;
  /*.........................................................................*/
  string  tempNamesSvc,   fanNamesSvc,   voltNamesSvc,   currNamesSvc;
  string  tempInputSvc,   fanInputSvc,   voltInputSvc,   currInputSvc;
  string  tempUnitsSvc,   fanUnitsSvc,   voltUnitsSvc,   currUnitsSvc;
  string  tempStatusSvc,  fanStatusSvc,  voltStatusSvc,  currStatusSvc;
  DimInfo *TempNames,     *FanNames,     *VoltNames,     *CurrNames;
  DimInfo *TempInput,     *FanInput,     *VoltInput,     *CurrInput;
  DimInfo *TempUnits,     *FanUnits,     *VoltUnits,     *CurrUnits;
  DimInfo *TempStatus,    *FanStatus,    *VoltStatus,    *CurrStatus;
  size_t  tempNamesSize,  fanNamesSize,  voltNamesSize,  currNamesSize;
  size_t  tempInputSize,  fanInputSize,  voltInputSize,  currInputSize;
  size_t  tempUnitsSize,  fanUnitsSize,  voltUnitsSize,  currUnitsSize;
  size_t  tempStatusSize, fanStatusSize, voltStatusSize, currStatusSize;
  char    *tempNames,     *fanNames,     *voltNames,     *currNames;
  char    *tempUnits,     *fanUnits,     *voltUnits,     *currUnits;
  char    *tempStatus,    *fanStatus,    *voltStatus,    *currStatus;
  float   *tempInput,                    *voltInput,     *currInput;
  int                     *fanInput;
  /*-------------------------------------------------------------------------*/
  public:
  /*-------------------------------------------------------------------------*/
  IpmiInfo(char *hostName)
  {
    /*.......................................................................*/
    this->hostName=hostName;
    nodeSvcC=0;
    char *uMsg=(char*)"Server unreachable!";
    /*.......................................................................*/
    pwStatusSvc=string(SVC_HEAD)+"/"+hostName+"/power_status";
    if(deBug)printf("Contacting Service: \"%s\"...\n",pwStatusSvc.c_str());
    PwStatus=new DimInfo(pwStatusSvc.c_str(),-1,this);
    /*.......................................................................*/
    pwTsSvc=string(SVC_HEAD)+"/"+hostName+"/power_status_timestamp";
    if(deBug)printf("Contacting Service: \"%s\"...\n",pwTsSvc.c_str());
    PwTs=new DimInfo(pwTsSvc.c_str(),-1,this);
    sensorsTsSvc=string(SVC_HEAD)+"/"+hostName+"/sensors_timestamp";
    if(deBug)printf("Contacting Service: \"%s\"...\n",sensorsTsSvc.c_str());
    SensorsTs=new DimInfo(sensorsTsSvc.c_str(),-1,this);
    /*.......................................................................*/
    tempNamesSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/temp/names";
    if(deBug)printf("Contacting Service: \"%s\"...\n",tempNamesSvc.c_str());
    TempNames=new DimInfo(tempNamesSvc.c_str(),uMsg,this);
    fanNamesSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/fan/names";
    if(deBug)printf("Contacting Service: \"%s\"...\n",fanNamesSvc.c_str());
    FanNames=new DimInfo(fanNamesSvc.c_str(),uMsg,this);
    voltNamesSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/voltage/names";
    if(deBug)printf("Contacting Service: \"%s\"...\n",voltNamesSvc.c_str());
    VoltNames=new DimInfo(voltNamesSvc.c_str(),uMsg,this);
    currNamesSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/current/names";
    if(deBug)printf("Contacting Service: \"%s\"...\n",currNamesSvc.c_str());
    CurrNames=new DimInfo(currNamesSvc.c_str(),uMsg,this);
    /*.......................................................................*/
    tempInputSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/temp/input";
    if(deBug)printf("Contacting Service: \"%s\"...\n",tempInputSvc.c_str());
    TempInput=new DimInfo(tempInputSvc.c_str(),uMsg,this);
    fanInputSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/fan/input";
    if(deBug)printf("Contacting Service: \"%s\"...\n",fanInputSvc.c_str());
    FanInput=new DimInfo(fanInputSvc.c_str(),uMsg,this);
    voltInputSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/voltage/input";
    if(deBug)printf("Contacting Service: \"%s\"...\n",voltInputSvc.c_str());
    VoltInput=new DimInfo(voltInputSvc.c_str(),uMsg,this);
    currInputSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/current/input";
    if(deBug)printf("Contacting Service: \"%s\"...\n",currInputSvc.c_str());
    CurrInput=new DimInfo(currInputSvc.c_str(),uMsg,this);
    /*.......................................................................*/
    tempUnitsSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/temp/units";
    if(deBug)printf("Contacting Service: \"%s\"...\n",tempUnitsSvc.c_str());
    TempUnits=new DimInfo(tempUnitsSvc.c_str(),uMsg,this);
    fanUnitsSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/fan/units";
    if(deBug)printf("Contacting Service: \"%s\"...\n",fanUnitsSvc.c_str());
    FanUnits=new DimInfo(fanUnitsSvc.c_str(),uMsg,this);
    voltUnitsSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/voltage/units";
    if(deBug)printf("Contacting Service: \"%s\"...\n",voltUnitsSvc.c_str());
    VoltUnits=new DimInfo(voltUnitsSvc.c_str(),uMsg,this);
    currUnitsSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/current/units";
    if(deBug)printf("Contacting Service: \"%s\"...\n",currUnitsSvc.c_str());
    CurrUnits=new DimInfo(currUnitsSvc.c_str(),uMsg,this);
    /*.......................................................................*/
    tempStatusSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/temp/status";
    if(deBug)printf("Contacting Service: \"%s\"...\n",tempStatusSvc.c_str());
    TempStatus=new DimInfo(tempStatusSvc.c_str(),uMsg,this);
    fanStatusSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/fan/status";
    if(deBug)printf("Contacting Service: \"%s\"...\n",fanStatusSvc.c_str());
    FanStatus=new DimInfo(fanStatusSvc.c_str(),uMsg,this);
    voltStatusSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/voltage/status";
    if(deBug)printf("Contacting Service: \"%s\"...\n",voltStatusSvc.c_str());
    VoltStatus=new DimInfo(voltStatusSvc.c_str(),uMsg,this);
    currStatusSvc=string(SVC_HEAD)+"/"+hostName+"/sensors/current/status";
    if(deBug)printf("Contacting Service: \"%s\"...\n",currStatusSvc.c_str());
    CurrStatus=new DimInfo(currStatusSvc.c_str(),uMsg,this);
    /*.......................................................................*/
  }
  /*-------------------------------------------------------------------------*/
  void infoHandler()
  {
    nodeSvcC++;
    /*.......................................................................*/
    DimInfo *curr=getInfo();
    /*.......................................................................*/
    if(curr==PwStatus)
    {
      pwStatus=(int)curr->getInt();
      if(deBug)printf("Service \"%s\" replied.\n",pwStatusSvc.c_str());
    }
    /*.......................................................................*/
    else if(curr==PwTs)
    {
      pwTs=(time_t)curr->getInt();
      if(deBug)printf("Service \"%s\" replied.\n",pwTsSvc.c_str());
    }
    else if(curr==SensorsTs)
    {
      sensorsTs=(time_t)curr->getInt();
      if(deBug)printf("Service \"%s\" replied.\n",sensorsTsSvc.c_str());
    }
    /*.......................................................................*/
    else if(curr==TempNames)
    {
      tempNamesSize=(int)curr->getSize();
      tempNames=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",tempNamesSvc.c_str());
    }
    else if(curr==TempInput)
    {
      tempInputSize=(int)curr->getSize()/sizeof(int);
      tempInput=(float*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",tempInputSvc.c_str());
    }
    else if(curr==TempUnits)
    {
      tempUnitsSize=(int)curr->getSize();
      tempUnits=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",tempUnitsSvc.c_str());
    }
    else if(curr==TempStatus)
    {
      tempStatusSize=(int)curr->getSize();
      tempStatus=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",tempStatusSvc.c_str());
    }
    /*.......................................................................*/
    else if(curr==FanNames)
    {
      fanNamesSize=(int)curr->getSize();
      fanNames=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",fanNamesSvc.c_str());
    }
    else if(curr==FanInput)
    {
      fanInputSize=(int)curr->getSize()/sizeof(int);
      fanInput=(int*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",fanInputSvc.c_str());
    }
    else if(curr==FanUnits)
    {
      fanUnitsSize=(int)curr->getSize();
      fanUnits=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",fanUnitsSvc.c_str());
    }
    else if(curr==FanStatus)
    {
      fanStatusSize=(int)curr->getSize();
      fanStatus=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",fanStatusSvc.c_str());
    }
    /*.......................................................................*/
    else if(curr==VoltNames)
    {
      voltNamesSize=(int)curr->getSize();
      voltNames=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",voltNamesSvc.c_str());
    }
    else if(curr==VoltInput)
    {
      voltInputSize=(int)curr->getSize()/sizeof(int);
      voltInput=(float*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",voltInputSvc.c_str());
    }
    else if(curr==VoltUnits)
    {
      voltUnitsSize=(int)curr->getSize();
      voltUnits=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",voltUnitsSvc.c_str());
    }
    else if(curr==VoltStatus)
    {
      voltStatusSize=(int)curr->getSize();
      voltStatus=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",voltStatusSvc.c_str());
    }
    /*.......................................................................*/
    else if(curr==CurrNames)
    {
      currNamesSize=(int)curr->getSize();
      currNames=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",currNamesSvc.c_str());
    }
    else if(curr==CurrInput)
    {
      currInputSize=(int)curr->getSize()/sizeof(int);
      currInput=(float*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",currInputSvc.c_str());
    }
    else if(curr==CurrUnits)
    {
      currUnitsSize=(int)curr->getSize();
      currUnits=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",currUnitsSvc.c_str());
    }
    else if(curr==CurrStatus)
    {
      currStatusSize=(int)curr->getSize();
      currStatus=(char*)curr->getData();
      if(deBug)printf("Service \"%s\" replied.\n",currStatusSvc.c_str());
    }
    /*.......................................................................*/
    if(nodeSvcC==19)
    {
      char *pN;
      unsigned i;
      char *pU;
      char *pS;
      char *p;
      char *hostname;
      struct tm lPwTs;
      char sPwTs[25]="";
      struct tm lSensorsTs;
      char sSensorsTs[25]="";
      /*.....................................................................*/
      hostname=(char*)malloc(1+strlen(hostName));
      strcpy(hostname,hostName);
      for(p=hostname;*p;p++)*p=tolower(*p);
      /*.....................................................................*/
      localtime_r(&pwTs,&lPwTs);
      strftime(sPwTs,26,"%a %b %d %H:%M:%S %Y",&lPwTs);
      localtime_r(&sensorsTs,&lSensorsTs);
      strftime(sSensorsTs,26,"%a %b %d %H:%M:%S %Y",&lSensorsTs);
      /*.....................................................................*/
      printf("NODE: %s\n",hostname);
      printf("%s\n",sepLine);
      printf("  Power status (%s):\n",sPwTs);
      if(pwStatus>=0)
        printf("    %s\n",pwStatus==0?"OFF":"ON");
      else if(pwStatus==IPMI_NOT_YET_AVAIL)
        printf("    %s\n",IPMI_NOT_YET_AVAIL_SL);
      else if(pwStatus==IPMI_SENSOR_UNREADABLE)
        printf("    %s\n",IPMI_SENSOR_UNREADABLE_SL);
      else if(pwStatus==IPMI_INVALID_RESP)
        printf("    %s\n",IPMI_INVALID_RESP_SL);
      else if(pwStatus==IPMI_NOT_RESP)
        printf("    %s\n",IPMI_NOT_RESP_SL);
      else
        printf("    %s\n","Unknown Error!");
      /*.....................................................................*/
      printf("  Temperatures (%s):\n",sSensorsTs);
      if(!strcmp(tempNames,IPMI_NOT_AVAIL_SL))
      {
        printf("    (No temperature measurements available on the node)\n");
      }
      else
      {
        for(pN=tempNames,i=0,pU=tempUnits,pS=tempStatus;i<tempInputSize;i++)
        {
          printf("    %-16s",pN);
          printf(" %6.1f",tempInput[i]);
          printf("   %-16s",pU);
          printf(" %-16s",pS);
          printf("\n");
          pN=1+strchr(pN,'\0');
          pU=1+strchr(pU,'\0');
          pS=1+strchr(pS,'\0');
        }
      }
      /*.....................................................................*/
      printf("  Fan speeds (%s):\n",sSensorsTs);
      if(!strcmp(fanNames,IPMI_NOT_AVAIL_SL))
      {
        printf("    (No fan speed measurements available on the node)\n");
      }
      else
      {
        for(pN=fanNames,i=0,pU=fanUnits,pS=fanStatus;i<fanInputSize;i++)
        {
          printf("    %-16s",pN);
          printf(" %4d",fanInput[i]);
          printf("     %-16s",pU);
          printf(" %-16s",pS);
          printf("\n");
          pN=1+strchr(pN,'\0');
          pU=1+strchr(pU,'\0');
          pS=1+strchr(pS,'\0');
        }
      }
      /*.....................................................................*/
      printf("  Voltages (%s):\n",sSensorsTs);
      if(!strcmp(voltNames,IPMI_NOT_AVAIL_SL))
      {
        printf("    (No voltage measurements available on the node)\n");
      }
      else
      {
        for(pN=voltNames,i=0,pU=voltUnits,pS=voltStatus;i<voltInputSize;i++)
        {
          printf("    %-16s",pN);
          printf(" %8.3f",voltInput[i]);
          printf(" %-16s",pU);
          printf(" %-16s",pS);
          printf("\n");
          pN=1+strchr(pN,'\0');
          pU=1+strchr(pU,'\0');
          pS=1+strchr(pS,'\0');
        }
      }
      /*.....................................................................*/
      printf("  Currents (%s):\n",sSensorsTs);
      if(!strcmp(currNames,IPMI_NOT_AVAIL_SL))
      {
        printf("    (No current measurements available on the node)\n");
      }
      else
      {
        for(pN=currNames,i=0,pU=currUnits,pS=currStatus;i<currInputSize;i++)
        {
          printf("    %-16s",pN);
          printf(" %8.3f",currInput[i]);
          printf(" %-16s",pU);
          printf(" %-16s",pS);
          printf("\n");
          pN=1+strchr(pN,'\0');
          pU=1+strchr(pU,'\0');
          pS=1+strchr(pS,'\0');
        }
      }
      /*.....................................................................*/
      printf("%s\n",sepLine);
      /*.....................................................................*/
      /* increase the node counter */
      pthread_mutex_lock(&totNodeFoundMutex);
      totNodeFoundC++;
      pthread_mutex_unlock(&totNodeFoundMutex);
      /*.....................................................................*/
      /* inform the main thread of the change in node counter */
      pthread_cond_broadcast(&totNodeFoundCond);
      /*.....................................................................*/
    }
    /*.......................................................................*/
  }
  /*-------------------------------------------------------------------------*/
};
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j;
  /* pointers */
  char *p=NULL;
  /* service browsing */
  int hostPttnC=0;
  char **hostPttnV=NULL;
  char *srvPattern=NULL;                                       /* upper case */
  int svcPatternLen=0;
  char *svcPattern=NULL;
  int rawSvcPatternLen=0;
  char *rawSvcPattern=NULL;
  int nodeFoundN=0;
  int totNodeFoundN=0;
  char nodeFound[128]="";
  char ucNodeFound[128]="";
  DimBrowser br;
  char *format=NULL;
  int type=0;
  char *pwStatusSvc=NULL;
  char *pwStatusSvcDup=NULL;
  char *dimDnsNode=NULL;
  /* command line switch */
  int doPrintServerVersion=0;
  /* other */
  int flag;
  /* long command line options */
  static struct option longOptions[]=
  {
    {"hostname",required_argument,NULL,'m'},
    {"dim-dns",required_argument,NULL,'N'},
    {"debug",no_argument,NULL,'v'},
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
  /* set separator line */
  memset(&sepLine,'-',79);
  sepLine[79]='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt_long(argc,argv,"m:vN:Vh::",longOptions,NULL))!=EOF)
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
      default:
        fprintf(stderr,"\ngetopt(): Invalid option \"%s\"!\n",argv[optind-1]);
        shortUsage();
        break;
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
      dimDnsNode=getDimDnsNode(dimConfFile,0,1);
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
  if(doPrintServerVersion)getServerVersion(hostPttnC,hostPttnV);
  /*-------------------------------------------------------------------------*/
  printf("Node patterns list:  ");
  for(i=0;i<hostPttnC;i++)                    /* loop over node patterns */
  {
    printf("\"%s\"",hostPttnV[i]);
    if(i<hostPttnC-1)printf(", ");
  }
  printf(".\n");
  /*-------------------------------------------------------------------------*/
  totNodeFoundN=0;
  for(i=0,j=0;i<hostPttnC;i++)                    /* loop over node patterns */
  {
    if(deBug)printf("  Node pattern: \"%s\":\n",hostPttnV[i]);
    srvPattern=(char*)realloc(srvPattern,1+strlen(hostPttnV[i]));
    strcpy(srvPattern,hostPttnV[i]);
    /* convert to upper case */
    for(p=srvPattern;*p;p++)*p=toupper(*p);
    svcPatternLen=snprintf(NULL,0,"%s/%s/power_status",SVC_HEAD,srvPattern);
    svcPattern=(char*)realloc(svcPattern,1+svcPatternLen);
    snprintf(svcPattern,1+svcPatternLen,"%s/%s/power_status",SVC_HEAD,
             srvPattern);
    if(deBug)printf("  Service pattern: \"%s\"\n",svcPattern);
    /*.......................................................................*/
    rawSvcPatternLen=snprintf(NULL,0,"%s/*/power_status",SVC_HEAD);
    rawSvcPattern=(char*)realloc(rawSvcPattern,1+rawSvcPatternLen);
    snprintf(rawSvcPattern,1+rawSvcPatternLen,"%s/*/power_status",
             SVC_HEAD);
    /*.......................................................................*/
    br.getServices(rawSvcPattern);
    nodeFoundN=0;
    while((type=br.getNextService(pwStatusSvc,format))!=0)/* loop over nodes */
    {
      if(!fnmatch(svcPattern,pwStatusSvc,0))
      {
        totNodeFoundN++;
        nodeFoundN++;
        pwStatusSvcDup=(char*)realloc(pwStatusSvcDup,1+strlen(pwStatusSvc));
        strcpy(pwStatusSvcDup,pwStatusSvc);
        p=strrchr(pwStatusSvcDup,'/');
        *p='\0';
        p=strrchr(pwStatusSvcDup,'/');
        snprintf(nodeFound,sizeof(nodeFound),"%s",1+p);
        strcpy(ucNodeFound,nodeFound);
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
        if(deBug)printf("Subscribing to host: \"%s\"...\n",nodeFound);
        new IpmiInfo(strdup(ucNodeFound));
        /*...................................................................*/
      }                                                     /* if !fnmatch() */
    }                                                     /* loop over nodes */
    if(!nodeFoundN)
    {
      printf("    No node found for pattern: \"%s\"!\n",hostPttnV[i]);
    }
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  /* wait until all service data are printed */
  if(totNodeFoundN)
  {
    for(;;)
    {
      /* wait for a change in node counter set by the I/O thread */
      pthread_mutex_lock(&totNodeFoundMutex);
      pthread_cond_wait(&totNodeFoundCond,&totNodeFoundMutex);
      pthread_mutex_unlock(&totNodeFoundMutex);
      if(totNodeFoundC>=totNodeFoundN)break;
    }
  }
  /*-------------------------------------------------------------------------*/
  if(deBug)printf("All the %d found nodes have done.\n",totNodeFoundN);
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
"ipmiViewer [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"           [-m | --hostname NODE_PATTERN...]\n"
"ipmiViewer { -V | --version } [-v | --debug]\n"
"           [-N | --dim-dns DIM_DNS_NODE]\n"
"           [-m | --hostname NODE_PATTERN...]\n"
"ipmiViewer { -h | --help }\n"
"\n"
"Try \"ipmiViewer -h\" for more information.\n";
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
"ipmiViewer.man\n"
"\n"
"..\n"
"%s"
".hw hostname NODE_PATTERN\n"
".TH ipmiViewer 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis ipmiViewer\\ \\-\n"
"Print the Power Status (on/off) and the Sensor Status (temperatures, fan "
"speeds, etc.) of the farm PC(s), got by means of the FMC Power Manager "
"Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis ipmiViewer\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis ipmiViewer\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis ipmiViewer\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH INTRODUCTION\n"
"The FMC Power Manager is a tool which allows, in an OS-independent manner, "
"to \\fBswitch-on\\fP, \\fBswitch-off\\fP, and \\fBpower-cycle\\fP the farm "
"nodes, and to \\fBmonitor\\fP their physical condition: \\fBpower status\\fP "
"(on/off), and sensor information retrieved through the I2C bus "
"(\\fBtemperatures\\fP, \\fBfan speeds\\fP, \\fBvoltages\\fP and "
"\\fBcurrents\\fP).\n"
".PP\n"
"The FMC Power Manager can operate on the farm nodes whose motherboards and "
"network interface cards implement the \\fBIPMI\\fP (Intelligent Platform "
"Management Interface) specifications, version 1.5 or subsequent, and copes "
"with several IPMI limitations.\n"
".PP\n"
"The FMC Power Manager Server, \\fBipmiSrv\\fP(8) typically runs on a few "
"control PCs (each one watching up to 200-1000 farm nodes) and uses the IPMI "
"protocol to communicate with the BMC (Baseboard Management Controller) of "
"the farm nodes, and the DIM network communication layer to communicate with "
"the Power Manager Clients.\n"
".PP\n"
"The Power Manager Clients \\fBpwSwitch\\fP(1), \\fBipmiViewer\\fP(1) and "
"\\fBipmiViewer\\fP(1) can contact one or more Power Manager Servers, "
"running on remote Control PCs, to switch on/off the farm worker nodes "
"controlled by these Control PCs and to retrieve their physical condition.\n"
".PP\n"
"The FMC Power Manager Server \\fBipmiSrv\\fP(8) accesses the farm node BMCs "
"both periodically and on user demand: it inquires \\fBperiodically\\fP the "
"farm node BMCs to get their power status and sensor information (and keeps "
"the information and its time stamp to be able to answer immediately to a "
"client request); it sends commands (power on, power off, power cycle, etc.) "
"to the farm node BMCs \\fBon user request\\fP.\n"
".PP\n"
"The Power Manager Server copes with the long IPMI response time by "
"\\fBparallelly accessing each node\\fP from a different short-living thread. "
"Every IPMI access request by \\fBipmiSrv\\fP(8) starts, by default, as many "
"short-living threads as the number of the controlled nodes and each "
"short-living thread accesses one node only. In case of a huge number of "
"configured nodes (>~\\ 500), the number of short-living threads can be "
"limited: in this running condition the IPMI accesses which exceed the "
"maximum thread number are queued up.\n"
".PP\n"
"The Power Manager Server copes also with the IPMI limitation to be able to "
"process only one access at a time, by \\fBarbitrating\\fP among the \\fBIPMI "
"accesses to the same node\\fP. In case of overlapping commands or "
"and periodical inquiries, the Power Manager Server \\fBenqueues "
"commands\\fP, in order for all the received commands to be executed, one at "
"a time, exactly in the same order they were issued, and \\fBcancel "
"periodical sensor inquiries\\fP, in order to avoid indefinite thread "
"pile-up for not-responding IPMI interfaces.\n"
".PP\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The \\fBipmiViewer\\fP(1) command prints to the standard output stream the "
"\\fBpower status\\fP (\\fIon/off\\fP) and the \\fBsensor status\\fP "
"(\\fItemperatures\\fP, \\fIfan speeds\\fP, \\fIvoltages\\fP and "
"\\fIcurrents\\fP) of the farm nodes, got from the Power Manager Servers "
"registered with the DIM name server \\fIDIM_DNS_NODE\\fP.\n"
".PP\n"
"The DIM Name Server \\fIDIM_DNS_NODE\\fP, looked up to seek for FMC Power "
"Manager Servers, can be chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB%s\\fP\".\n"
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
"Print only the power status and the sensor status of the PCs whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: print the power status and the sensor status of all the "
"controlled PCs).\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC Configuration Manager Server version "
"and the FMC version, than terminate.\n"
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
"Print the power status and the sensor status of all the controlled PCs:\n"
".PP\n"
".ShellCommand ipmiViewer\n"
".PP\n"
".\n"
"Print the power status and the sensor status of the PC farm0101:\n"
".PP\n"
".ShellCommand ipmiViewer -m farm0101\n"
".PP\n"
".\n"
"Use wildcard patterns to select PCs:\n"
".PP\n"
".ShellCommand ipmiViewer -m \\[dq]farm01*\\[dq]\n"
".ShellCommand ipmiViewer -m \\[dq]farm01*\\[dq] -m \\[dq]farm02*\\[dq]\n"
".ShellCommand ipmiViewer -m \\[dq]farm010[1357]\\[dq]\n"
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
".BR \\%%pwSwitch (1),\n"
".BR \\%%pwStatus (1),\n"
".BR \\%%ipmiSrv (8).\n"
".br\n"
".BR \\%%ipmitool (1).\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro
,FMC_DATE,FMC_VERSION
,DIM_CONF_FILE_NAME
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(0);
}
/*****************************************************************************/
