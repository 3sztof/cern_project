/* ************************************************************************* */
/*
 * $Log: fsViewer.C,v $
 * Revision 2.6  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 2.5  2011/11/17 16:14:40  galli
 * minor changes
 *
 * Revision 2.1  2011/11/14 15:53:21  galli
 * bug fixed
 *
 * Revision 2.0  2011/11/07 11:33:56  galli
 * Compatible with fsSrv version 2.3
 *
 * Revision 1.12  2008/10/22 12:23:27  galli
 * uses FmcUtils::printServerVersion()
 *
 * Revision 1.7  2008/02/20 23:30:18  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.6  2007/12/19 12:57:18  galli
 * Added cmd-line switches -M, -G, -T to express values in MiB, GiB and TiB.
 *
 * Revision 1.5  2007/12/19 10:01:26  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.4  2007/08/08 09:15:12  galli
 * usage() changed
 *
 * Revision 1.3  2006/08/04 14:14:37  galli
 * advanced view added
 *
 * Revision 1.2  2006/08/03 14:30:24  galli
 * percentage evaluation like in df
 *
 * Revision 1.1  2006/08/03 13:32:00  galli
 * Initial revision
 */
/* ************************************************************************* */
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>                                            /* uint64_t */
#include <errno.h>                                                  /* errno */
#include <getopt.h>                                         /* getopt_long() */
#include <sys/wait.h>                                  /* wait(2) in usage() */
/* ------------------------------------------------------------------------- */
/* DIM */
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                               /* printServerVersion() */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/* ------------------------------------------------------------------------- */
/* in monitor/include */
#define _CLIENT 1
#include "fsSrv.h"                              /* SRV_NAME, DEV_FIELD_N_ERR */
/* ************************************************************************* */
/* type definition */
/* ------------------------------------------------------------------------- */
typedef struct fs_data
{
  string id;
  string name;
  string type;
  string mountPt;
  uint64_t minFree;
  uint64_t totalRoot;
  uint64_t totalUser;
  uint64_t used;
  uint64_t availRoot;
  uint64_t availUser;
  float usedRootP;
  float usedUserP;
  string lastUpdate;
  string status;
}fs_data_t;
/* ------------------------------------------------------------------------- */
typedef struct fs_float_data
{
  double minFree;
  double totalRoot;
  double totalUser;
  double used;
  double availRoot;
  double availUser;
}fs_float_data_t;
/* ------------------------------------------------------------------------- */
typedef struct fs_data_len
{
  unsigned id;
  unsigned name;
  unsigned type;
  unsigned mountPt;
  unsigned minFree;
  unsigned totalRoot;
  unsigned totalUser;
  unsigned used;
  unsigned availRoot;
  unsigned availUser;
  unsigned usedRootP;
  unsigned usedUserP;
  unsigned lastUpdate;
  unsigned status;
}fs_data_len_t;
/*****************************************************************************/
/* globals */
int deBug=0;
static char rcsid[]="$Id: fsViewer.C,v 2.6 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
/*****************************************************************************/
/* function prototype */
void usage(int mode);
void shortUsage(void);
void getServerVersion(vector<string>hostPttnLs);
/*****************************************************************************/
/* replace a blank character with an underscore */
char blankUnd(char c)
{
  if(isblank(c))c='_';
  return c;
}
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i,j,sc1,sc2;
  unsigned k;
  size_t b,e;
  /* pointers */
  char *p=NULL;
  /* service browsing */
  vector<string> hostPttnLs;                          /* host pattern vector */
  vector<string>::iterator hpIt;             /* host pattern vector iterator */
  string hostPttn;
  string successRawSvcPttn;
  string successSvcPttn;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  string successSvc;
  string(nodeFound);
  int nodeFoundN=0;
  string svc;
  char *dimDnsNode=NULL;
  /* no link replacement */
  int nolinkDevStrArrS=0;
  char *nolinkDevStrArr=NULL;
  int nolinkDataS=0;
  char *nolinkData=NULL;
  /* retrieved quantities */
  int success=-1;
  string versionStr,version;
  bool isOldServer=false;
  char *devices=NULL;
  int devicesSz=0;
  vector<string> deviceV;
  vector<fs_data_t> fsData;
  fs_float_data_t *fsFdata=NULL;
  /* printing variables */
  char lineFormat[256]="";
  int isFirstTime=1;
  fs_data_len_t dataLen;
  char *line=NULL;
  int lineLen=0;
  /* command line switch */
  int doPrintServerVersion=0;
  int advancedView=0;
  char units='K';
  int compactFormat=0;
  int doPrintHeader=0;
  /* getopt */
  int flag;
  /* short command line options */
  const char *shortOptions="vN:m:cHaKMGTVh::";
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
    {"advanced",no_argument,NULL,'a'},
    {"KiB",no_argument,NULL,'K'},
    {"MiB",no_argument,NULL,'M'},
    {"GiB",no_argument,NULL,'G'},
    {"TiB",no_argument,NULL,'T'},
    /* ..................................................................... */
    /* accessory options */
    {"version",no_argument,NULL,'V'},
    {"help",no_argument,NULL,'h'},
    /* ..................................................................... */
    {NULL,0,NULL,0}
  };
  /* other */
  uint64_t errUint64=~0ULL;
  float errFloat=-1.0;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
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
        hostPttnLs.push_back(optarg);
        break;
      case 'c': /* -c, --compact */
        compactFormat=1;
        break;
      case 'H': /* -H, --header */
        doPrintHeader=1;
        break;
      /* ................................................................... */
      /* data presentation options */
      case 'a': /* -a, --all */
        advancedView=1;
        break;
      case 'K': /* -K, --KiB */
        units='K';
        break;
      case 'M': /* -M, --MiB */
        units='M';
        break;
      case 'G': /* -G, --GiB */
        units='G';
        break;
      case 'T': /* -T, --TiB */
        units='T';
        break;
      /* ................................................................... */
      /* accessory options */
      case 'V':  /* -V, --version */
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
  /* ----------------------------------------------------------------------- */
  /* default wildcard for hostPttnLs and logPttnLs */
  if(hostPttnLs.empty())hostPttnLs.push_back("*");
  /* ----------------------------------------------------------------------- */
  /* check hostPttnLs */
  for(hpIt=hostPttnLs.begin();hpIt!=hostPttnLs.end();hpIt++)
  {
    if(hpIt->find_first_of('/')!=string::npos)
    {
      fprintf(stderr,"Illegal NODE_PATTERN "
              "\"%s\"! A NODE_PATTERN specified with the command-line option "
              "\"-m NODE_PATTERN\" or \"--hostname NODE_PATTERN\" must not "
              "contain the '/' character!\n",hpIt->c_str());
      shortUsage();
    }
  }
  /* ----------------------------------------------------------------------- */
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
  if(doPrintServerVersion) getServerVersion(hostPttnLs);
  /*-------------------------------------------------------------------------*/
  if(deBug)compactFormat=0;
  if(!compactFormat)doPrintHeader=0;
  /*-------------------------------------------------------------------------*/
  /* Compose DIM no link error arrays */
  /* devices */
  nolinkDevStrArrS=DEV_FIELD_N_ERR*(1+strlen("N/F"));     /* "N/F"=not found */
  nolinkDevStrArr=(char*)malloc(nolinkDevStrArrS);
  for(i=0,p=nolinkDevStrArr;i<DEV_FIELD_N_ERR;i++,p+=(1+strlen(p)))
    strcpy(p,"N/F");
  /* data */
  nolinkDataS=6*sizeof(uint64_t)+2*sizeof(float)+4*(1+strlen("N/F"));
  nolinkData=(char*)malloc(nolinkDataS);
  p=nolinkData;
  for(i=0;i<6;i++)
  {
    memcpy(p,&errUint64,sizeof(uint64_t));
    p+=sizeof(uint64_t);
  }
  for(i=0;i<2;i++)
  {
    memcpy(p,&errFloat,sizeof(float));
    p+=sizeof(float);
  }
  for(i=0;i<4;i++)
  {
    sprintf(p,"%s","N/F");
    p+=(1+strlen("N/F"));
  }
  /* ----------------------------------------------------------------------- */
  if(!compactFormat)
  {
    printf("Node patterns list:  ");
    for(hpIt=hostPttnLs.begin();hpIt<hostPttnLs.end();hpIt++)
    {
      printf("\"%s\"",hpIt->c_str());
      if(*hpIt!=hostPttnLs.back())printf(", ");
    }
    printf(".\n");
  }
  /* ----------------------------------------------------------------------- */
  /* loop over node patterns */
  for(hpIt=hostPttnLs.begin(),j=0;hpIt<hostPttnLs.end();hpIt++)
  {
    hostPttn=*hpIt;
    if(!compactFormat)printf("  Node pattern: \"%s\":\n",hostPttn.c_str());
    hostPttn=FmcUtils::toUpper(hostPttn);
    successRawSvcPttn=string("/FMC/*/")+SRV_NAME+"/success";
    successSvcPttn=string("/FMC/")+hostPttn+"/"+SRV_NAME+"/success";
    if(deBug)printf("  Service pattern: \"%s\"\n",successSvcPttn.c_str());
    /* ..................................................................... */
    /* count the number of the slashes */
    for(k=0,sc1=0;k<successSvcPttn.size();k++)
    {
      if(successSvcPttn.at(k)=='/')sc1++;
    }
    /* ..................................................................... */
    br.getServices(successRawSvcPttn.c_str());
    nodeFoundN=0;
    while((type=br.getNextService(p,format))!=0)          /* loop over nodes */
    {
      if(!fnmatch(successSvcPttn.c_str(),p,0))
      {
        successSvc=p;
        /* ................................................................. */
        /* count the number of the slashes */
        for(k=0,sc2=0;k<successSvc.size();k++)
        {
          if(successSvc.at(k)=='/')sc2++;
        }
        if(sc2!=sc1)continue;
        /* ................................................................. */
        /* extract node hostname from "success" service name */
        e=successSvc.find_last_of('/');                        /* last slash */
        e=successSvc.find_last_of('/',e-1);            /* last-but-one slash */
        for(k=0;k<8;k++)e=successSvc.find_last_of('/',e-1);
        b=successSvc.find_last_of('/',e-1)+1;
        nodeFound=successSvc.substr(b,e-b);
        nodeFound=FmcUtils::toLower(nodeFound);
        /* ................................................................. */
        nodeFoundN++;
        /* ................................................................. */
        /* read success */
        if(deBug)
        {
          printf("  %3d Contacting Service: \"%s\"...\n",j++,
                 successSvc.c_str());
        }
        DimCurrentInfo Success(successSvc.c_str(),-1);
        success=(int)Success.getInt();
        if(success==-1)
        {
          fprintf(stderr,"      Service \"%s\" unreachable!\n",
                  successSvc.c_str());
          continue;
        }
        if(deBug && success<-1)
        {
          unsigned insuccessMask=-success;
          if(insuccessMask&0x2)
          {
            fprintf(stderr,"      Node %s: data not ready for at least 1 "
                    "filesystem!\n",nodeFound.c_str());
          }
          if(insuccessMask&0x4)
          {
            fprintf(stderr,"      Node \"%s\": at least 1 filesystem stalled!"
                    "\n",nodeFound.c_str());
          }
        }
        /* ................................................................. */
        /* read server version */
        svc=string("/FMC/")+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
            "/server_version";
        if(deBug)
        {
          printf("  %3d Contacting Service: \"%s\"...\n",j++,svc.c_str());
        }
        DimCurrentInfo Version(svc.c_str(),(char*)"N/A");
        versionStr=string((char*)Version.getData());
        b=versionStr.find_first_of(' ')+1;
        e=versionStr.find_first_of(' ',b);
        version=versionStr.substr(b,e-b);
        if(strverscmp(version.c_str(),"2.0")>=0)isOldServer=false;
        else isOldServer=true;
        /* ................................................................. */
        /* read devices */
        svc=string("/FMC/")+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
            "/devices";
        if(deBug)
        {
          printf("  %3d Contacting Service: \"%s\"...\n",j++,svc.c_str());
        }
        DimCurrentInfo Devices(svc.c_str(),nolinkDevStrArr,nolinkDevStrArrS);
        devicesSz=(int)Devices.getSize();
        devices=(char*)Devices.getData();
        deviceV.clear();
        for(k=0,p=devices;p<devices+devicesSz;k++,p+=(1+strlen(p)))
        {
          deviceV.push_back(p);
        }
        /* ................................................................. */
        fsData.clear();
        for(k=0;k<deviceV.size();k++)               /* loop over filesystems */
        {
          fs_data_t fd;
          int fsDataBufSz;
          char *fsDataBuf;
          char *p;
          if(deviceV[k]=="N/F")break;
          svc=string("/FMC/")+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
              "/details/"+deviceV[k]+"/data";
          if(deBug)
          {
            printf("  %3d Contacting Service: \"%s\"...\n",j++,svc.c_str());
          }
          DimCurrentInfo FsDataBuf(svc.c_str(),nolinkData,nolinkDataS);
          fsDataBufSz=(int)FsDataBuf.getSize();
          fsDataBuf=(char*)FsDataBuf.getData();
          fd.id=deviceV[k];
          if(isOldServer)
          {
            fd.minFree=(uint64_t)(*(float*)(fsDataBuf));
            fd.totalRoot=(uint64_t)(*(float*)(fsDataBuf+1*sizeof(float)));
            fd.totalUser=(uint64_t)(*(float*)(fsDataBuf+2*sizeof(float)));
            fd.used=(uint64_t)(*(float*)(fsDataBuf+3*sizeof(float)));
            fd.availRoot=(uint64_t)(*(float*)(fsDataBuf+4*sizeof(float)));
            fd.availUser=(uint64_t)(*(float*)(fsDataBuf+5*sizeof(float)));
            fd.usedRootP=*(float*)(fsDataBuf+6*sizeof(float));
            fd.usedUserP=*(float*)(fsDataBuf+7*sizeof(float));
            p=fsDataBuf+8*sizeof(float);
            fd.name=(char*)(p);
            p+=fd.name.size()+1;
            fd.type=(char*)(p);
            p+=fd.type.size()+1;
            fd.mountPt=(char*)(p);
            p+=fd.mountPt.size()+1;
            fd.lastUpdate=(char*)(p);
            p+=fd.lastUpdate.size()+1;
            fd.status="N/A";
          }
          else
          {
            fd.minFree=*(uint64_t*)(fsDataBuf);
            fd.totalRoot=*(uint64_t*)(fsDataBuf+sizeof(uint64_t));
            fd.totalUser=*(uint64_t*)(fsDataBuf+2*sizeof(uint64_t));
            fd.used=*(uint64_t*)(fsDataBuf+3*sizeof(uint64_t));
            fd.availRoot=*(uint64_t*)(fsDataBuf+4*sizeof(uint64_t));
            fd.availUser=*(uint64_t*)(fsDataBuf+5*sizeof(uint64_t));
            fd.usedRootP=*(float*)(fsDataBuf+6*sizeof(uint64_t));
            fd.usedUserP=*(float*)(fsDataBuf+6*sizeof(uint64_t)+sizeof(float));
            p=fsDataBuf+6*sizeof(uint64_t)+2*sizeof(float);
            fd.name=(char*)(p);
            p+=fd.name.size()+1;
            fd.type=(char*)(p);
            p+=fd.type.size()+1;
            fd.mountPt=(char*)(p);
            p+=fd.mountPt.size()+1;
            fd.lastUpdate=(char*)(p);
            p+=fd.lastUpdate.size()+1;
            fd.status=(char*)(p);
            p+=fd.status.size()+1;
          }
          fsData.push_back(fd);
        }                                           /* loop over filesystems */
        /* ................................................................. */
        fsFdata=(fs_float_data_t*)realloc(fsFdata,
                                       deviceV.size()*sizeof(fs_float_data_t));
        /*...................................................................*/
        /* print node and time */
        if(!compactFormat)
        {
          printf("    NODE \"%s\":\n",nodeFound.c_str());
        }
        /* ................................................................. */
        /* unit conversion */
        if(units=='K')
        {
          for(size_t k=0;k<deviceV.size();k++)      /* loop over filesystems */
          {
            fsFdata[k].minFree=(double)fsData[k].minFree;
            fsFdata[k].totalRoot=(double)fsData[k].totalRoot;
            fsFdata[k].totalUser=(double)fsData[k].totalUser;
            fsFdata[k].used=(double)fsData[k].used;
            fsFdata[k].availRoot=(double)fsData[k].availRoot;
            fsFdata[k].availUser=(double)fsData[k].availUser;
          }
        }
        if(units=='M')
        {
          for(size_t k=0;k<deviceV.size();k++)      /* loop over filesystems */
          {
            fsFdata[k].minFree=(double)fsData[k].minFree/1024.;
            fsFdata[k].totalRoot=(double)fsData[k].totalRoot/1024.;
            fsFdata[k].totalUser=(double)fsData[k].totalUser/1024.;
            fsFdata[k].used=(double)fsData[k].used/1024.;
            fsFdata[k].availRoot=(double)fsData[k].availRoot/1024.;
            fsFdata[k].availUser=(double)fsData[k].availUser/1024.;
          }
        }
        else if(units=='G')
        {
          for(size_t k=0;k<deviceV.size();k++)      /* loop over filesystems */
          {
            fsFdata[k].minFree=(double)fsData[k].minFree/1048576.;
            fsFdata[k].totalRoot=(double)fsData[k].totalRoot/1048576.;
            fsFdata[k].totalUser=(double)fsData[k].totalUser/1048576.;
            fsFdata[k].used=(double)fsData[k].used/1048576.;
            fsFdata[k].availRoot=(double)fsData[k].availRoot/1048576.;
            fsFdata[k].availUser=(double)fsData[k].availUser/1048576.;
          }
        }
        else if(units=='T')
        {
          for(size_t k=0;k<deviceV.size();k++)      /* loop over filesystems */
          {
            fsFdata[k].minFree=(double)fsData[k].minFree/1073741824.;
            fsFdata[k].totalRoot=(double)fsData[k].totalRoot/1073741824.;
            fsFdata[k].totalUser=(double)fsData[k].totalUser/1073741824.;
            fsFdata[k].used=(double)fsData[k].used/1073741824.;
            fsFdata[k].availRoot=(double)fsData[k].availRoot/1073741824.;
            fsFdata[k].availUser=(double)fsData[k].availUser/1073741824.;
          }
        }
        /* ................................................................. */
        /* evaluate max string length */
        if(!compactFormat)
        {
          memset(&dataLen,0,sizeof(dataLen));
          for(size_t k=0;k<deviceV.size();k++)      /* loop over filesystems */
          {
            unsigned l;
            l=snprintf(NULL,0,"%s",fsData[k].id.c_str());
            if(dataLen.id<l)dataLen.id=l;
            l=snprintf(NULL,0,"%s",fsData[k].name.c_str());
            if(dataLen.name<l)dataLen.name=l;
            l=snprintf(NULL,0,"%s",fsData[k].type.c_str());
            if(dataLen.type<l)dataLen.type=l;
            l=snprintf(NULL,0,"%s",fsData[k].mountPt.c_str());
            if(dataLen.mountPt<l)dataLen.mountPt=l;
            if(units=='K')l=snprintf(NULL,0,"%"PRIu64,fsData[k].totalRoot);
            else l=snprintf(NULL,0,"%.2f",fsFdata[k].totalRoot);
            if(dataLen.totalRoot<l)dataLen.totalRoot=l;
            if(units=='K')l=snprintf(NULL,0,"%"PRIu64,fsData[k].totalUser);
            else l=snprintf(NULL,0,"%.2f",fsFdata[k].totalUser);
            if(dataLen.totalUser<l)dataLen.totalUser=l;
            if(units=='K')l=snprintf(NULL,0,"%"PRIu64,fsData[k].minFree);
            else l=snprintf(NULL,0,"%.2f",fsFdata[k].minFree);
            if(dataLen.minFree<l)dataLen.minFree=l;
            if(units=='K')l=snprintf(NULL,0,"%"PRIu64,fsData[k].used);
            else l=snprintf(NULL,0,"%.2f",fsFdata[k].used);
            if(dataLen.used<l)dataLen.used=l;
            if(units=='K')l=snprintf(NULL,0,"%"PRIu64,fsData[k].availRoot);
            else l=snprintf(NULL,0,"%.2f",fsFdata[k].availRoot);
            if(dataLen.availRoot<l)dataLen.availRoot=l;
            if(units=='K')l=snprintf(NULL,0,"%"PRIu64,fsData[k].availUser);
            else l=snprintf(NULL,0,"%.2f",fsFdata[k].availUser);
            if(dataLen.availUser<l)dataLen.availUser=l;
            dataLen.usedRootP=6;                                   /* 100.0% */
            dataLen.usedUserP=6;                                   /* 100.0% */
            dataLen.lastUpdate=12;                           /* Oct12-134923 */
            l=snprintf(NULL,0,"%s",fsData[k].status.c_str());
            if(dataLen.status<l)dataLen.status=l;
          }                                         /* loop over filesystems */
          /*.................................................................*/
          if(!advancedView)                                   /* simple View */
          {
            /*...............................................................*/
            if(dataLen.id<strlen("Id"))dataLen.id=strlen("Id");
            if(dataLen.name<strlen("Filesystem"))
              dataLen.name=strlen("Filesystem");
            if(dataLen.type<strlen("Type"))dataLen.type=strlen("Type");
            if(dataLen.totalRoot<strlen("1Ki-blocks"))
              dataLen.totalRoot=strlen("1Ki-blocks");
            if(dataLen.used<strlen("Used"))dataLen.used=strlen("Used");
            if(dataLen.availUser<strlen("Available"))
              dataLen.availUser=strlen("Available");
            if(dataLen.usedRootP<strlen("Use%"))
              dataLen.usedRootP=strlen("Use%");
            if(dataLen.mountPt<strlen("Mounted on"))
              dataLen.mountPt=strlen("Mounted on");
            if(dataLen.status<strlen("Status"))
              dataLen.status=strlen("Status");
            /*...............................................................*/
            // s = str( format(" %d %d ") % 11 % 22 );
            // http://www.boost.org/libs/format/index.html
            /* evaluate line separator length */
            lineLen=dataLen.id+1+dataLen.name+1+dataLen.type+1+
                    dataLen.totalRoot+1+dataLen.used+1+dataLen.availUser+1+
                    dataLen.usedRootP+1+dataLen.mountPt+1+dataLen.status;
            line=(char*)realloc(line,(1+lineLen)*sizeof(char));
            memset(line,'-',lineLen);
            line[lineLen]='\0';
            /*...............................................................*/
            /* print filesystem header */
            sprintf(lineFormat,"    %%-%ds %%-%ds %%-%ds %%%ds %%%ds %%%ds "
                    "%%%ds %%-%ds %%-%ds\n",dataLen.id,dataLen.name,
                    dataLen.type,dataLen.totalRoot,dataLen.used,
                    dataLen.availUser,dataLen.usedRootP,dataLen.mountPt,
                    dataLen.status);
            printf("    %s\n",line);
            if(units=='K')
              printf(lineFormat,"Id","Filesystem","Type","1Ki-blocks","Used",
                     "Available","Use%","Mounted on","Status");
            else if(units=='M')
              printf(lineFormat,"Id","Filesystem","Type","1Mi-blocks","Used",
                     "Available","Use%","Mounted on","Status");
            else if(units=='G')
              printf(lineFormat,"Id","Filesystem","Type","1Gi-blocks","Used",
                     "Available","Use%","Mounted on","Status");
            else if(units=='T')
              printf(lineFormat,"Id","Filesystem","Type","1Ti-blocks","Used",
                     "Available","Use%","Mounted on","Status");
            printf("    %s\n",line);
            /*...............................................................*/
            /* print filesystem data */
            if(units=='K')
            {
              sprintf(lineFormat,"    %%-%ds %%-%ds %%-%ds %%%d"PRIu64" %%%d"
                      PRIu64" %%%d"PRIu64" %%5.1f%%%% %%-%ds %%-%ds\n",
                      dataLen.id,dataLen.name,dataLen.type,dataLen.totalRoot,
                      dataLen.used,dataLen.availUser,dataLen.mountPt,
                      dataLen.status);
            }
            else
            {
              sprintf(lineFormat,"    %%-%ds %%-%ds %%-%ds %%%d.2f %%%d.2f "
                      "%%%d.2f %%5.1f%%%% %%-%ds %%-%ds\n",dataLen.id,
                      dataLen.name,dataLen.type,dataLen.totalRoot,dataLen.used,
                      dataLen.availUser,dataLen.mountPt,dataLen.status);
            }
            for(size_t k=0;k<deviceV.size();k++)    /* loop over filesystems */
            {
              if(units=='K')
              {
                printf(lineFormat,fsData[k].id.c_str(),fsData[k].name.c_str(),
                       fsData[k].type.c_str(),fsData[k].totalRoot,
                       fsData[k].used,fsData[k].availUser,fsData[k].usedUserP,
                       fsData[k].mountPt.c_str(),fsData[k].status.c_str());
              }
              else
              {
                printf(lineFormat,fsData[k].id.c_str(),fsData[k].name.c_str(),
                       fsData[k].type.c_str(),fsFdata[k].totalRoot,
                       fsFdata[k].used,fsFdata[k].availUser,fsData[k].usedUserP,
                       fsData[k].mountPt.c_str(),fsData[k].status.c_str());
              }
            }                                       /* loop over filesystems */
            printf("    %s\n",line);
            /*...............................................................*/
          }                                                   /* simple View */
          else                                              /* advanced View */
          {
            /*...............................................................*/
            if(dataLen.id<strlen("Id"))dataLen.id=strlen("Id");
            if(dataLen.name<strlen("Filesystem"))
              dataLen.name=strlen("Filesystem");
            if(dataLen.type<strlen("Type"))dataLen.type=strlen("Type");
            if(dataLen.minFree<strlen("Minfree"))
              dataLen.minFree=strlen("Minfree");
            if(dataLen.totalRoot<strlen("TotalRoot"))
              dataLen.totalRoot=strlen("TotalRoot");
            if(dataLen.totalUser<strlen("TotalUser"))
              dataLen.totalUser=strlen("TotalUser");
            if(dataLen.used<strlen("Used"))
              dataLen.used=strlen("Used");
            if(dataLen.availRoot<strlen("AvailRoot"))
              dataLen.availRoot=strlen("AvailRoot");
            if(dataLen.availUser<strlen("AvailUser"))
              dataLen.availUser=strlen("AvailUser");
            if(dataLen.usedRootP<strlen("UseR%"))
              dataLen.usedRootP=strlen("UseR%");
            if(dataLen.usedUserP<strlen("UseU%"))
              dataLen.usedUserP=strlen("UseU%");
            if(dataLen.mountPt<strlen("Mounted on"))
              dataLen.mountPt=strlen("Mounted on");
            if(dataLen.lastUpdate<strlen("Last update"))
              dataLen.lastUpdate=strlen("Last update");
            if(dataLen.status<strlen("Status"))
              dataLen.status=strlen("Status");
            /*...............................................................*/
            /* evaluate line separator length */
            lineLen=dataLen.id+1+dataLen.name+1+dataLen.type+1+
                    dataLen.minFree+1+dataLen.totalRoot+1+dataLen.totalUser+1+
                    dataLen.used+1+dataLen.availRoot+1+dataLen.availUser+1+
                    dataLen.usedRootP+1+dataLen.usedUserP+1+dataLen.mountPt+1+
                    dataLen.lastUpdate+1+dataLen.status;
            line=(char*)realloc(line,(1+lineLen)*sizeof(char));
            memset(line,'-',lineLen);
            line[lineLen]='\0';
            /*...............................................................*/
            /* print filesystem header */
            printf("    %s\n",line);
            if(units=='K')
              printf("    Units: KiB = 1024 B\n");
            else if(units=='M')
              printf("    Units: MiB = 1048576 B\n");
            else if(units=='G')
              printf("    Units: GiB = 1073741824 B\n");
            else if(units=='T')
              printf("    Units: TiB = 1099511627776 B\n");
            sprintf(lineFormat,"    %%-%ds %%-%ds %%-%ds %%%ds %%%ds %%%ds "
                    "%%%ds %%%ds %%%ds %%%ds %%%ds %%-%ds %%-%ds %%-%ds\n",
                    dataLen.id,dataLen.name,dataLen.type,dataLen.minFree,
                    dataLen.totalRoot,dataLen.totalUser,dataLen.used,
                    dataLen.availRoot,dataLen.availUser,dataLen.usedRootP,
                    dataLen.usedUserP,dataLen.mountPt,dataLen.lastUpdate,
                    dataLen.status);
            printf(lineFormat,"Id","Filesystem","Type","Minfree","TotalRoot",
                   "TotalUser","Used","AvailRoot","AvailUser","UseR%","UseU%",
                   "Mounted on","Last update","Status");
            printf("    %s\n",line);
            /*...............................................................*/
            /* print filesystem data */
            if(units=='K')
            {
              sprintf(lineFormat,"    %%-%ds %%-%ds %%-%ds %%%d"PRIu64" %%%d"
                      PRIu64" %%%d"PRIu64" %%%d"PRIu64" %%%d"PRIu64" %%%d"
                      PRIu64" %%5.1f%%%% %%5.1f%%%% %%-%ds %%-%ds %%-%ds\n",
                      dataLen.id,dataLen.name,dataLen.type,dataLen.minFree,
                      dataLen.totalRoot,dataLen.totalUser,dataLen.used,
                      dataLen.availRoot,dataLen.availUser,dataLen.mountPt,
                      dataLen.lastUpdate,dataLen.status);
            }
            else
            {
              sprintf(lineFormat,"    %%-%ds %%-%ds %%-%ds %%%d.2f %%%d.2f "
                      "%%%d.2f %%%d.2f %%%d.2f %%%d.2f %%5.1f%%%% %%5.1f%%%% "
                      "%%-%ds %%-%ds %%-%ds\n",dataLen.id,dataLen.name,
                      dataLen.type,dataLen.minFree,dataLen.totalRoot,
                      dataLen.totalUser,dataLen.used,dataLen.availRoot,
                      dataLen.availUser,dataLen.mountPt,dataLen.lastUpdate,
                      dataLen.status);
            }
            for(size_t k=0;k<deviceV.size();k++)    /* loop over filesystems */
            {
              if(units=='K')
              {
                printf(lineFormat,fsData[k].id.c_str(),fsData[k].name.c_str(),
                       fsData[k].type.c_str(),fsData[k].minFree,
                       fsData[k].totalRoot,fsData[k].totalUser,fsData[k].used,
                       fsData[k].availRoot,fsData[k].availUser,
                       fsData[k].usedRootP,fsData[k].usedUserP,
                       fsData[k].mountPt.c_str(),fsData[k].lastUpdate.c_str(),
                       fsData[k].status.c_str());
              }
              else
              {
                printf(lineFormat,fsData[k].id.c_str(),fsData[k].name.c_str(),
                       fsData[k].type.c_str(),fsFdata[k].minFree,
                       fsFdata[k].totalRoot,fsFdata[k].totalUser,
                       fsFdata[k].used,fsFdata[k].availRoot,
                       fsFdata[k].availUser,fsData[k].usedRootP,
                       fsData[k].usedUserP,fsData[k].mountPt.c_str(),
                       fsData[k].lastUpdate.c_str(),fsData[k].status.c_str());
              }
            }                                       /* loop over filesystems */
            printf("    %s\n",line);
            /*...............................................................*/
          }                                                 /* advanced View */
          /*.................................................................*/
        }                                              /* if(!compactFormat) */
        else                                            /* if(compactFormat) */
        {
          /*.................................................................*/
          if(!advancedView)                                   /* simple View */
          {
            if(doPrintHeader && isFirstTime)
            {
              isFirstTime=0;
              printf("HOSTNAME Id Filesystem Type 1%ci-blocks_[%ciB] "
                     "Used_[%ciB] Available_[%ciB] Use%%_[%%] Mounted_on "
                     "Time[mmmdd-HHMMSS] Status\n",
                     units,units,units,units);
            }
            for(size_t k=0;k<deviceV.size();k++)
            {
              /* avoid spaces to make parsing easier */
              transform(fsData[k].name.begin(),
                        fsData[k].name.end(),
                        fsData[k].name.begin(),
                        blankUnd);
              transform(fsData[k].type.begin(),
                        fsData[k].type.end(),
                        fsData[k].type.begin(),
                        blankUnd);
              transform(fsData[k].mountPt.begin(),
                        fsData[k].mountPt.end(),
                        fsData[k].mountPt.begin(),
                        blankUnd);
              transform(fsData[k].status.begin(),
                        fsData[k].status.end(),
                        fsData[k].status.begin(),
                        blankUnd);
              if(units=='K')
              {
                printf("%s %s %s %s %"PRIu64" %"PRIu64" %"PRIu64" %.1f %s %s "
                       "%s\n",nodeFound.c_str(),fsData[k].id.c_str(),
                       fsData[k].name.c_str(),fsData[k].type.c_str(),
                       fsData[k].totalRoot,fsData[k].used,fsData[k].availUser,
                       fsData[k].usedUserP,fsData[k].mountPt.c_str(),
                       fsData[k].lastUpdate.c_str(),fsData[k].status.c_str());
              }
              else
              {
                printf("%s %s %s %s %.2f %.2f %.2f %.1f %s %s %s\n",
                       nodeFound.c_str(),fsData[k].id.c_str(),
                       fsData[k].name.c_str(),fsData[k].type.c_str(),
                       fsFdata[k].totalRoot,fsFdata[k].used,
                       fsFdata[k].availUser,fsData[k].usedUserP,
                       fsData[k].mountPt.c_str(),fsData[k].lastUpdate.c_str(),
                       fsData[k].status.c_str());
              }
            }
          }                                                   /* simple View */
          /*.................................................................*/
          else                                              /* advanced View */
          {
            if(doPrintHeader && isFirstTime)
            {
              isFirstTime=0;
              printf("HOSTNAME Id Filesystem Type Minfree_[%ciB] "
                     "TotalRoot_[%ciB] TotalUser_[%ciB] Used_[%ciB] "
                     "AvailRoot_[%ciB] AvailUser_[%ciB] UseR_[%%] UseU_[%%] "
                     "Mounted_on Time[mmmdd-HHMMSS] Status\n",units,units,
                     units,units,units,units);
            }
            for(size_t k=0;k<deviceV.size();k++)
            {
              /* avoid spaces to make parsing easier */
              transform(fsData[k].name.begin(),
                        fsData[k].name.end(),
                        fsData[k].name.begin(),
                        blankUnd);
              transform(fsData[k].type.begin(),
                        fsData[k].type.end(),
                        fsData[k].type.begin(),
                        blankUnd);
              transform(fsData[k].mountPt.begin(),
                        fsData[k].mountPt.end(),
                        fsData[k].mountPt.begin(),
                        blankUnd);
              transform(fsData[k].status.begin(),
                        fsData[k].status.end(),
                        fsData[k].status.begin(),
                        blankUnd);
              if(units=='K')
              {
                printf("%s %s %s %s %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64" %"
                       PRIu64" %"PRIu64" %.1f %.1f %s %s %s\n",
                       nodeFound.c_str(),fsData[k].id.c_str(),
                       fsData[k].name.c_str(),fsData[k].type.c_str(),
                       fsData[k].minFree,fsData[k].totalRoot,
                       fsData[k].totalUser,fsData[k].used,
                       fsData[k].availRoot,fsData[k].availUser,
                       fsData[k].usedRootP,fsData[k].usedUserP,
                       fsData[k].mountPt.c_str(),fsData[k].lastUpdate.c_str(),
                       fsData[k].status.c_str());
              }
              else
              {
                printf("%s %s %s %s %.2f %.2f %.2f %.2f %.2f %.2f %.1f %.1f "
                       "%s %s %s\n",nodeFound.c_str(),fsData[k].id.c_str(),
                       fsData[k].name.c_str(),fsData[k].type.c_str(),
                       fsFdata[k].minFree,fsFdata[k].totalRoot,
                       fsFdata[k].totalUser,fsFdata[k].used,
                       fsFdata[k].availRoot,fsFdata[k].availUser,
                       fsData[k].usedRootP,fsData[k].usedUserP,
                       fsData[k].mountPt.c_str(),fsData[k].lastUpdate.c_str(),
                       fsData[k].status.c_str());
              }
            }
          }
        }                                               /* if(compactFormat) */
        /*...................................................................*/
      }                                                     /* if !fnmatch() */
    }                                                     /* loop over nodes */
    if(!nodeFoundN && !compactFormat)
    {
      printf("    No node found for pattern: \"%s\"!\n",hpIt->c_str());
    }
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  return 0;
}                                                                  /* main() */
/*****************************************************************************/
void getServerVersion(vector<string>hostPttnLs)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnLs,SVC_HEAD,SRV_NAME,deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnLs,"",SRV_NAME,deBug,12,8);
  /* older service name rule */
  FmcUtils::printServerVersion(hostPttnLs,"","fs",deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/* ######################################################################### */
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"fsViewer [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"         [-m | --hostname NODE_PATTERN...]  [-c | --compact]\n"
"         [-H | --header] [-a | --advanced] [-K | --KiB] [-M | --MiB]\n"
"         [-G | --GiB] [-T | --TiB]\n"
"fsViewer { -V | --version } [-v | --debug]\n"
"         [-N | --dim-dns DIM_DNS_NODE]\n"
"         [-m | --hostname NODE_PATTERN...]\n"
"fsViewer { -h | --help }\n"
"\n"
"Try \"fsViewer -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  fprintf(stderr,"\n%s\n",shortUsageStr);
  exit(1);
}
/* ************************************************************************* */
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
"fsViewer.man\n"
"\n"
"..\n"
"%s"
".hw hostname NODE_PATTERN\n"
".TH fsViewer 1  "FMC_DATE" \"FMC-"FMC_VERSION"\" "
"\"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis fsViewer\\ \\-\n"
"Get filesystem space usage from the DIM File System Monitor Servers\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis fsViewer\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".DoubleOpt[] c compact\n"
".DoubleOpt[] H header\n"
".DoubleOpt[] a advanced\n"
".DoubleOpt[] K KiB\n"
".DoubleOpt[] M MiB\n"
".DoubleOpt[] G GiB\n"
".DoubleOpt[] T TiB\n"
".EndSynopsis\n"
".sp\n"
".Synopsis fsViewer\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis fsViewer\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"Get the \\fBspace usage\\fP of the \\fBfilesystems\\fP mounted on the farm "
"nodes whose hostname matches at least one of the wildcard patterns "
"\\fINODE_PATTERN\\fP, specified using the \\fB-m\\fP \\fINODE_PATTERN\\fP "
"command line option (\\fBdefault\\fP: all the nodes registered with the DIM "
"name server \\fIDIM_DNS_NODE\\fP).\n"
".PP\n"
"Values are expressed in KiB (kibiBytes, 1 KiB=1024 Bytes), unless "
"command-line switches \\fB-M\\fP, \\fB-G\\fP or \\fB-T\\fP are specified.\n"
".PP\n"
"Usage information provided includes, in \\fBstandard mode\\fP (omitting "
"\\fB-a\\fP command-line switch), the following 8 fields (the same of the "
"\\fBdf\\fP(1) command):\n"
".TP\n"
"\\fBId\\fP\n"
"Filesystem identifier (fs_00, fs_01, ...).\n"
".TP\n"
"\\fBFilesystem\\fP\n"
"Filesystem device name (e.g.: /dev/hda1, /dev/sda1, etc.).\n"
".TP\n"
"\\fBType\\fP\n"
"Filesystem type (ext3, ntfs, nfs, afs, gpfs, etc.).\n"
".TP\n"
"\\fB1Ki-blocks\\fP\n"
"Total size of the filesystem space accessible to the user \"root\".\n"
".TP\n"
"\\fBUsed\\fP\n"
"Size of the used filesystem space.\n"
".TP\n"
"\\fBAvailable\\fP\n"
"Size of the filesystem space available to non-privileged users.\n"
".TP\n"
"\\fBUse%%\\fP\n"
"Used  percentage of the total filesystem size accessible by a non-privileged "
"user (=Used*100/TotalUser).\n"
".TP\n"
"\\fBMounted on\\fP\n"
"Filesystem mount-point (e.g.: /, /boot, /home, etc.).\n"
".PP\n"
"In \\fBadvanced mode\\fP (specifying the \\fB-a\\fP command-line switch) the "
"following 14 fields are provided:\n"
".TP\n"
"\\fBId\\fP\n"
"Filesystem identifier (fs_00, fs_01, ...).\n"
".TP\n"
"\\fBFilesystem\\fP\n"
"Filesystem device name (e.g.: /dev/hda1, /dev/sda1, etc.).\n"
".TP\n"
"\\fBType\\fP\n"
"Filesystem type (ext3, ntfs, nfs, afs, gpfs, etc.).\n"
".TP\n"
"\\fBMinfree\\fP\n"
"The size of the filesystem space available to the user \"root\" but "
"unavailable to any non-privileged users (=TotalRoot-Used-AvailUser).\n"
".TP\n"
"\\fBTotalRoot\\fP\n"
"Total size of the filesystem space accessible to the user \"root\".\n"
".TP\n"
"\\fBTotalUser\\fP\n"
"Total size of the filesystem space accessible to non-privileged users "
"(=Used+AvailUser).\n"
".TP\n"
"\\fBUsed\\fP\n"
"Size of the used filesystem space.\n"
".TP\n"
"\\fBAvailRoot\\fP\n"
"Size of the filesystem space available to the user \"root\" "
"(=TotalRoot-Used).\n"
".TP\n"
"\\fBAvailUser\\fP\n"
"Size of the filesystem space available to non-privileged users.\n"
".TP\n"
"\\fBUseR%%\\fP\n"
"Used  percentage of the total filesystem size accessible by the user "
"\"root\" (=Used*100/TotalRoot).\n"
".TP\n"
"\\fBUseU%%\\fP\n"
"Used  percentage of the total filesystem size accessible by a non-privileged "
"user (=Used*100/TotalUser).\n"
".TP\n"
"\\fBMounted on\\fP\n"
"Filesystem mount-point (e.g.: /, /boot, /home, etc.).\n"
".TP\n"
"\\fBLast update\\fP\n"
"The time corresponding to the last data access (e.g.: Nov07-122230).\n"
".TP\n"
"\\fBStatus\\fP\n"
"The filesystem status at the last data access. Can be:\n"
".br\n"
"\"\\fBOK\\fP\";\n"
".br\n"
"\"\\fBSTALLED since <N> s\\fP\";\n"
".br\n"
"\"\\fBERROR: <error message>\\fP\".\n"
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
"Print only the filesystem space usage information of the farm nodes whose "
"hostname matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: print the filesystem space usage information of all the "
"nodes registered with the DIM name server \\fIDIM_DNS_NODE\\fP).\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC Process Monitor Server version "
"and the FMC version, than terminate.\n"
".\n"
".OptDef c compact\n"
"Print the filesystem space usage information in compact format (easier to "
"parse): one filesystem per line, no labels, no units, no separator lines.\n"
".\n"
".OptDef H header\n"
"If the compact output format has been choosen, print a heading line with the "
"output field labels.\n"
".\n"
".SS Data Presentation Options\n"
".\n"
".OptDef a advanced\n"
"Advanced 14-field printout (see above).\n"
".\n"
".OptDef K KiB\n"
"Express values in KiB (kibiBytes, 1 KiB=1024 B)\n"
".\n"
".OptDef M MiB\n"
"Express values in MiB (mebiBytes, 1 MiB=1048576 B)\n"
".\n"
".OptDef G GiB\n"
"Express values in GiB (gibiBytes, 1 GiB=1073741824 B)\n"
".\n"
".OptDef T TiB\n"
"Express values in TiB (tebiBytes, 1 TiB=1099511627776 B)\n"
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
"Standard printout:\n"
".PP\n"
".ShellCommand fsViewer\n"
".PP\n"
"Advanced printout:\n"
".PP\n"
".ShellCommand fsViewer -a\n"
".ShellCommand fsViewer --advanced\n"
".ShellCommand fsViewer --adv\n"
".PP\n"
"Print output in compact format with header:\n"
".PP\n"
".ShellCommand fsViewer -acH\n"
".ShellCommand fsViewer -a -c -H\n"
".ShellCommand fsViewer --adv --compact --header\n"
".ShellCommand fsViewer --a --co --hea\n"
".PP\n"
"Show only filesystems mounted on selected nodes:\n"
".PP\n"
".ShellCommand fsViewer -m farm0101 -m farm0102 --adv\n"
".PP\n"
"Show only filesystems mounted on wildcard-selected nodes:\n"
".PP\n"
".ShellCommand fsViewer -m \\[dq]farm01*\\[dq] -m \\[dq]farm02*\\[dq] "
"--adv\n"
".ShellCommand fsViewer -m \\[dq]farm010[1357]\\[dq] --adv\n" 
".PP\n"
"Show the server version installed on the farm nodes:\n"
".PP\n"
".ShellCommand fsViewer -V\n"
".ShellCommand fsViewer --version\n"
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
".BR \\%%fsSrv (8).\n"
".br\n"
".BR \\%%setmntent (3),\n"
".BR \\%%getmntent_r (3),\n"
".BR \\%%endmntent (3),\n"
".BR \\%%statfs (2),\n"
".br\n"
".BR \\%%df (1),\n"
".BR \\%%fs (5),\n"
".BR \\%%fstab (5).\n"
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
