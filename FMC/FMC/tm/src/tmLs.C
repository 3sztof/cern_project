/* ************************************************************************* */
/*
 * $Log: tmLs.C,v $
 * Revision 1.23  2012/12/13 15:54:37  galli
 * minor changes
 *
 * Revision 1.22  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.21  2012/08/30 08:38:33  galli
 * accepts multiple UTGID patterns
 *
 * Revision 1.15  2008/08/27 09:05:58  galli
 * groff manual
 *
 * Revision 1.13  2008/07/02 12:20:40  galli
 * services starts with /FMC
 *
 * Revision 1.11  2007/12/13 15:22:40  galli
 * cmd-line option -V to print server version and FMC version
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.9  2007/10/23 15:54:52  galli
 * usage() prints FMC version
 *
 * Revision 1.8  2007/08/16 12:43:08  galli
 * usage() modified
 *
 * Revision 1.5  2007/06/28 11:42:38  galli
 * 2 minor bugs fixed
 *
 * Revision 1.4  2006/09/08 09:28:51  galli
 * -u option to select utgid
 * -c for compact output format
 *
 * Revision 1.3  2005/08/26 08:50:50  galli
 * more restrictive DIM wildcard pattern to speed-up browsing
 *
 * Revision 1.2  2004/11/21 21:44:31  galli
 * option -m hostname_pattern substitutes option -s svc_pattern
 * avoid use of "--" (end of option-scanning)
 *
 * Revision 1.1  2004/10/15 00:16:23  galli
 * Initial revision
 */
/* ************************************************************************* */
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
#include <string.h>                                              /* memchr() */
#include <getopt.h>                                         /* getopt_long() */
#include <sys/wait.h>                                             /* wait(2) */
/* ------------------------------------------------------------------------- */
/* DIM */
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                               /* printServerVersion() */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcRef.h"                                  /* FMC_URLS FMC_AUTHORS */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/* ------------------------------------------------------------------------- */
/* in tm/include */
#define _CLIENT 1
#include "tmSrv.h"                             /* SRV_NAME, TMLS_FIELD_N_ERR */
/* ************************************************************************* */
/* globals */
int deBug=0;
static char rcsid[]="$Id: tmLs.C,v 1.23 2012/12/13 15:54:37 galli Exp galli $";
char *rcsidP;
/* ************************************************************************* */
/* function prototype */
void usage(int mode);
void shortUsage(void);
void getServerVersion(vector<string>hostPttnLs);
/* ************************************************************************* */
class PsData
{
  public:
  unsigned n;
  string utgid;
  bool isSelected;
};
/* ------------------------------------------------------------------------- */
class FieldSize
{
  public:
  unsigned n;
  unsigned utgid;
  void reset(void)
  {
    n=1;
    utgid=5;
  }
};
/* ************************************************************************* */
/* replace a blank character with an underscore */
char blankUnd(char c)
{
  if(isblank(c))c='_';
  return c;
}
/* ************************************************************************* */
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
  vector<string> utgidPttnLs;                        /* utgid pattern vector */
  vector<string>::iterator ugIt;            /* utgid pattern vector iterator */
  bool utgidFound=false;
  string hostPttn;
  string successRawSvcPttn;
  string successSvcPttn;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  string successSvc;
  int nodeFoundN=0;
  string(nodeFound);
  string svc;
  char *dimDnsNode=NULL;
  /* no link replacement */
  int nolinkStrArrSz=0;
  static char *nolinkStrArr=NULL;
  /* retrieved quantities */
  int success=-1;
  vector<PsData> pss;
  vector<PsData>::iterator pssIt;
  FieldSize fs;
  char *list=NULL;
  char *listP=NULL;
  int psN=0;
  /* command line switch */
  int doPrintServerVersion=0;
  int compactFormat=0;
  int doPrintHeader=0;
  /* printing variables */
  bool isFirstTime=true;
  char printFormatH[128];
  char printFormatD[128];
  /* getopt */
  int flag;
  /* short command line options */
  const char *shortOptions="vN:m:u:cHVh::";
  /* long command line options */
  static struct option longOptions[]=
  {
    /* ..................................................................... */
    /* standard options */
    {"debug",no_argument,NULL,'v'},
    {"dim-dns",required_argument,NULL,'N'},
    {"hostname",required_argument,NULL,'m'},
    {"utgid",required_argument,NULL,'u'},
    {"compact",no_argument,NULL,'c'},
    {"header",no_argument,NULL,'H'},
    /* ..................................................................... */
    /* accessory options */
    {"version",no_argument,NULL,'V'},
    {"help",no_argument,NULL,'h'},
    /* ..................................................................... */
    {NULL,0,NULL,0}
  };
  /* ----------------------------------------------------------------------- */
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /* ----------------------------------------------------------------------- */
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
      case 'u': /* -u, --utgid */
        utgidPttnLs.push_back(optarg);
        break;
      case 'c': /* -c, --compact */
        compactFormat=1;
        break;
      case 'H': /* -H, --header */
        doPrintHeader=1;
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
  /* ----------------------------------------------------------------------- */
  /* default wildcard for hostPttnLs and utgidPttnLs */
  if(hostPttnLs.empty())hostPttnLs.push_back("*");
  if(utgidPttnLs.empty())utgidPttnLs.push_back("*");
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
  /* ----------------------------------------------------------------------- */
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
  /* ----------------------------------------------------------------------- */
  if(doPrintServerVersion)getServerVersion(hostPttnLs);
  /* ----------------------------------------------------------------------- */
  if(deBug)compactFormat=0;
  if(!compactFormat)doPrintHeader=0;
  /* ----------------------------------------------------------------------- */
  /* Compose DIM  no link error string array */
  nolinkStrArrSz=TMLS_FIELD_N_ERR*(1+strlen("(N/A)"));  /* N/A=not available */
  nolinkStrArr=(char*)malloc(nolinkStrArrSz);
  for(i=0,p=nolinkStrArr;i<TMLS_FIELD_N_ERR;i++,p+=(1+strlen(p)))
  {
    strcpy(p,"(N/A)");
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
    printf("UTGID patterns list:  ");
    for(ugIt=utgidPttnLs.begin();ugIt<utgidPttnLs.end();ugIt++)
    {
      printf("\"%s\"",ugIt->c_str());
      if(*ugIt!=utgidPttnLs.back())printf(", ");
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
          printf("  ==> %3d Contacting Service: \"%s\"...\n",j++,
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
        else if(success==0)
        {
          fprintf(stderr,"Service \"%s\": data not ready!\n",
                  successSvc.c_str());
          continue;
        }
        /* ................................................................. */
        /* read data */
        svc=string("/FMC/")+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
            "/list";
        if(deBug)
        {
          printf("  ==> %3d Contacting Service: \"%s\"...\n",j++,svc.c_str());
        }
        DimCurrentInfo Ls(svc.c_str(),nolinkStrArr,nolinkStrArrSz);
        list=(char*)Ls.getData();
        /* ................................................................. */
        /* processing data */
        pss.clear();
        for(psN=0,listP=list;;)                     /* loop over list fields */
        {
          PsData el;
          el.utgid=listP;
          listP=1+strchr(listP,'\0');
          /* check if utgid matches at least 1 of the pattern in utgidPttnLs */
          utgidFound=false;
          for(ugIt=utgidPttnLs.begin();ugIt<utgidPttnLs.end();ugIt++)
          {
            if(!fnmatch(ugIt->c_str(),el.utgid.c_str(),0))
            {
              utgidFound=true;
              break;
            }
          }
          if(utgidFound)
          {
            el.isSelected=true;
            el.n=psN;
            psN++;
          }
          else
          {
            el.isSelected=false;
          }
          pss.push_back(el);
          if(listP>=list+Ls.getSize())break;
        }                                           /* loop over list fields */
        /* ................................................................. */
        /* evaluate field sizes */
        fs.reset();
        for(pssIt=pss.begin();pssIt!=pss.end();pssIt++)
        {
          unsigned sz;
          if(pssIt->isSelected)
          {
            sz=FmcUtils::itos(pssIt->n).length();
            if(fs.n<sz)fs.n=sz;
            sz=(pssIt->utgid).length();
            if(fs.utgid<sz)fs.utgid=sz;
          }
        }
        /* ................................................................. */
        /* define print format */
        snprintf(printFormatH,128,"    %%%ds %%-%ds\n",fs.n,fs.utgid);
        snprintf(printFormatD,128,"    %%%dd %%-%ds\n",fs.n,fs.utgid);
        /* ................................................................. */
        /* print node & process number */
        if(!compactFormat)
        {
          /* check if "*" is among UTGID patterns */
          utgidFound=false;
          for(ugIt=utgidPttnLs.begin();ugIt<utgidPttnLs.end();ugIt++)
          {
            if(!strcmp(ugIt->c_str(),"*"))
            {
              utgidFound=true;
              break;
            }
          }
          if(utgidFound)                         /* "*" among UTGID patterns */
          {
            printf("    Node: \"%s\". %d process(es) have UTGID:\n",
                   nodeFound.c_str(),psN);
          }
          else                               /* "*" not among UTGID patterns */
          {
            printf("    Node: \"%s\". %d process(es) have UTGID matching "
                   "pattern: ",nodeFound.c_str(),psN);
            for(ugIt=utgidPttnLs.begin();ugIt<utgidPttnLs.end();ugIt++)
            {
              printf("\"%s\"",ugIt->c_str());
              if(*ugIt!=utgidPttnLs.back())printf(", ");
            }
            printf(":\n");
          }
        }
        /* ................................................................. */
        /* print data */
        if(!compactFormat)
        {
          /* ............................................................... */
          /* print header line */
          printf(printFormatH,"#","UTGID");
          /* print data */
          for(pssIt=pss.begin();pssIt!=pss.end();pssIt++)   /* loop over pss */
          {
            if(pssIt->isSelected)
            {
              printf(printFormatD,pssIt->n,(pssIt->utgid).c_str());
            }                                           /* if ps is selected */
          }                                                 /* loop over pss */
          /* ............................................................... */
        }                                           /* if not compact format */
        else
        {                                               /* if compact format */
          /* ............................................................... */
          /* print header line */
          if(doPrintHeader && isFirstTime && psN!=0)
          {
            isFirstTime=0;
            printf("HOSTNAME N UTGID UTGID ... \n");
          }
          /* print data */
          printf("%s %d",nodeFound.c_str(),psN);
          for(pssIt=pss.begin();pssIt!=pss.end();pssIt++)   /* loop over pss */
          {
            transform(pssIt->utgid.begin(),
                      pssIt->utgid.end(),
                      pssIt->utgid.begin(),
                      blankUnd);
            if(pssIt->isSelected)
            {
              printf(" %s",(pssIt->utgid).c_str());
            }                                           /* if ps is selected */
          }                                                 /* loop over pss */
          printf("\n");
          /* ............................................................... */
        }                                               /* if compact format */
        /* ................................................................. */
      }                                                     /* if !fnmatch() */
    }                                                     /* loop over nodes */
    /* ..................................................................... */
    if(!nodeFoundN && !compactFormat)
    {
      printf("    No node found for pattern: \"%s\"!\n",hpIt->c_str());
    }
  }                                               /* loop over node patterns */
  /* ----------------------------------------------------------------------- */
  return 0;
}
/* ************************************************************************* */
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
  FmcUtils::printServerVersion(hostPttnLs,"","mem",deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/* ************************************************************************* */
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"       tmLs [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"            [-m | --hostname NODE_PATTERN...]  "
"[-u | --utgid UTGID_PATTERN...]\n"
"            [-c | --compact] [-H | --header]\n"
"       tmLs { -V | --version } [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"            [-m | --hostname NODE_PATTERN...]\n"
"       tmLs { -h | --help }\n"
"\n"
"Try \"tmLs -h\" for more information.\n";
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
  /* ----------------------------------------------------------------------- */
  if(!mode)formatter="/usr/bin/groff -man -Tascii | /usr/bin/less -iRs";
  else formatter="/bin/cat";
  fpOut=popen(formatter,"w");
  fprintf(fpOut,
".ig\n"
"tmLs.man\n"
"\n"
"..\n"
"%s"
".hw no\\[hy]authentication UTGID_PATTERN\n"
".TH tmLs 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis tmLs\\ \\-\n"
"List the process(es) started by FMC Task Manager (short list)\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis tmLs\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".br\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".DoubleOpt[] u utgid UTGID_PATTERN...\n"
".DoubleOpt[] c compact\n"
".DoubleOpt[] H header\n"
".EndSynopsis\n"
".sp\n"
".Synopsis tmLs\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis tmLs\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
"The command \\fBtmLs\\fP lists the processes, started by the FMC Task "
"Manager, whose \\fBUTGID\\fP matches at least one of the wildcard patterns "
"\\fIUTGID_PATTERN\\fP, running on all the nodes whose \\fBhostname\\fP "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP.\n"
".PP\n"
"The list includes \\fBonly\\fP the \\fBUTGID\\fPs of the processes (i.e. The "
"User assigned unique Thread Group Identifier, set by the FMC Task Manager "
"Server and used as a tag to identify the process).\n"
".PP\n"
"To get a more comprehensive list (including COMMAND, PID, OOM_SCORE and "
"STATUS) the command \\fBtmLl\\fP(1) can be used instead.\n"
".PP\n"
"The output can be made more \\fBcompact\\fP by means of the \\fB-c\\fP "
"option.\n"
".PP\n"
"A sample \\fBtmLs\\fP output in standard format is:\n"
".PP\n"
".RS\n"
" Node patterns list:  \\[dq]*\\[dq].\n"
" UTGID patterns list:  \\[dq]*\\[dq].\n"
"   Node pattern: \\[dq]*\\[dq]:\n"
"     Node: \\[dq]farm0704\\[dq]. 16 process(es) have UTGID:\n"
"      # UTGID\n"
"      0 fmcLog\n"
"      1 coalSrv_u\n"
"      2 cpuinfoSrv_u\n"
"      3 cpustatSrv_u\n"
"      4 irqSrv_u\n"
"      5 memSrv_u\n"
"      6 nifSrv_u\n"
"      7 cmSrv_u\n"
"      8 psSrv_u\n"
"      9 tcpipSrv_u\n"
"     10 smartSrv_u\n"
"     11 fsSrv_u\n"
"     12 osSrv_u\n"
"     13 gaudiLog\n"
"     14 sysLog\n"
"     15 tmSrv_u\n"
".RE\n"
".PP\n"
"A sample \\fBtmLs\\fP output in compact format with header (\\fB-cH\\fP "
"option) is:\n"
".PP\n"
".RS\n"
" HOSTNAME N UTGID UTGID ... \n"
" farm0307 16 fmcLog tmSrv_u cmSrv_u coalSrv_u cpuinfoSrv_u\n"
"   cpustatSrv_u irqSrv_u memSrv_u nifSrv_u psSrv_u\n"
"   tcpipSrv_u smartSrv_u fsSrv_u osSrv_u gaudiLog sysLog\n"
" farm0912 17 fmcLog cmSrv_u pcSrv_u coalSrv_u cpuinfoSrv_u\n"
"   cpustatSrv_u irqSrv_u memSrv_u nifSrv_u psSrv_u\n"
"   tcpipSrv_u smartSrv_u fsSrv_u osSrv_u gaudiLog sysLog tmSrv_u\n"
" farm0204 16 fmcLog coalSrv_u cpuinfoSrv_u cpustatSrv_u\n"
"   irqSrv_u memSrv_u nifSrv_u cmSrv_u psSrv_u tcpipSrv_u\n"
"   smartSrv_u fsSrv_u osSrv_u gaudiLog sysLog tmSrv_u\n"
".RE\n"
".PP\n"
"If no \\fIUTGID_PATTERN\\fP is specified, it is set to \"*\".\n"
"If no \\fINODE_PATTERN\\fP is specified, it is set to \"*\". The list\n"
"printed by \\fBtmLs\\fP is updated not only periodically, but also whenever\n"
"a Task Manager command is executed, and immediately after a process\n"
"started by the current Task Manager istance has terminated for\n"
"whatever reason.\n"
".PP\n"
"The DIM Name Server, looked up to seek for Task Manager Servers, can be\n"
"chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB/etc/sysconfig/dim\\fP\".\n"
".RE\n"
".\n"
".\n"
".SH OPTIONS\n"
".\n"
".SS General Options\n"
".\n"
".OptDef v debug\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N dim\\[hy]dns DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".SS Data Presentation Options\n"
".\n"
".OptDef c compact\n"
"Print the process list in a compact format (easier to parse): one host "
"per line, no labels, no units, no separator lines.\n"
".\n"
".OptDef H header\n"
"If the compact output format has been choosen, print a heading line with the "
"output field labels.\n"
".\n"
".SS Data Selection Options\n"
".\n"
".OptDef m hostname NODE_PATTERN (string, repeatable)\n"
"List only process(es) running on the nodes whose hostname\n"
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP\n"
"(\\fBdefault\\fP: list process(es) running at all the nodes).\n"
".\n"
".OptDef u utgid UTGID_PATTERN (string, repeatable)\n"
"List only the process(es) whose UTGID matches at least one of the wildcard "
"pattern \\fIUTGID_PATTERN\\fP (\\fBdefault\\fP: list all the processes "
"started by the Task Manager Servers running at the selected nodes).\n"
".\n"
".SS Accessory Options\n"
".\n"
".OptDef h help\n"
"Get basic usage help from the command line, then terminate.\n"
".\n"
".OptDef V version\n"
"For each selected node, print the FMC Task Manager Server version and the\n"
"FMC package version, than terminate.\n"
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
"List the processes having whatever UTGID, executing at whatever\n"
"node running the Task Manager Server registered with the current DIM\n"
"DNS:\n"
".\n"
".PP\n"
".ShellCommand tmLs\n"
".ShellCommand tmLs -m \\[dq]*\\[dq] -u \\[dq]*\\[dq]\n"
".ShellCommand tmLs -m \\[rs]* -u \\[rs]*\n"
".\n"
".PP\n"
"Produce a more compact output:\n"
".\n"
".PP\n"
".ShellCommand tmLs -c\n"
".\n"
".\n"
".PP\n"
"Produce a more compact output with a header line showing field meanings:\n"
".\n"
".PP\n"
".ShellCommand tmLs -cH\n"
".\n"
".PP\n"
"List only processes having the UTGID \"counter_0\" or \"gaudiLog\" (running "
"at whatever node):\n"
".\n"
".PP\n"
".ShellCommand tmLs -u counter_0 -u gaudiLog\n"
".\n"
".PP\n"
"Use a wildcard pattern to select the UTGIDs:\n"
".\n"
".PP\n"
".ShellCommand tmLs -u \\[dq]count*\\[dq]  -u \\[dq]gaudi*\\[dq]\n"
".ShellCommand tmLs -u count\\[rs]* -u gaudi\\[rs]*\n"
".ShellCommand tmLs -u \\[dq]count*[2-5]\\[dq] -u \\[dq]count*[8-9]\\[dq]\n"
".\n"
".PP\n"
"Limit the list to the processes running on nodes \"farm0101\" and\n"
"\"farm0102\":\n"
".\n"
".PP\n"
".ShellCommand tmLs -m farm0101 -m farm0102\n"
".\n"
".PP\n"
"Use a wildcard pattern to select the nodes:\n"
".\n"
".PP\n"
".ShellCommand tmLs -m \\[dq]farm01*\\[dq]\n"
".ShellCommand tmLs -m farm01\\[rs]*\n"
".ShellCommand tmLs -m \\[dq]farm010[3-7]\\[dq] -m \\[dq]farm020[3-7]\\[dq]\n"
".ShellCommand tmLs -m \\[dq]farm01?[3-7]\\[dq] -m \\[dq]farm02?[3-7]\\[dq]\n"
".\n"
".PP\n"
"Use a wildcard pattern to select both the nodes and the UTGIDs:\n"
".\n"
".PP\n"
".ShellCommand tmLs -m \\[dq]farm01*\\[dq] -m \\[dq]farm02*\\[dq] "
"-u \\[dq]count*\\[dq] -u \\[dq]gaudi*\\[dq]\n"
".ShellCommand tmLs -m farm01\\[rs]* -m farm02\\[rs]* -u count\\[rs]* "
"-u gaudi\\[rs]*\n"
".ShellCommand tmLs -m \\[dq]farm01?[3-7]\\[dq] -m \\[dq]farm02?[3-7]\\[dq] "
"-u \\[dq]count*[2-5]\\[dq]\n"
"%s"
"%s"
".\n"
".\n"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".PP\n"
"Package: FMC-%s\n"
".\n"
".\n"
".SH SEE ALSO\n"
".ad l\n"
".BR \\%%tmLl (1),\n"
".BR \\%%tmStart (1),\n"
".BR \\%%tmKill (1),\n"
".BR \\%%tmStop (1).\n"
".br\n"
".BR \\%%pcAdd (1),\n"
".BR \\%%pcRm (1),\n"
".BR \\%%pcLs (1),\n"
".BR \\%%pcLl (1),\n"
".BR \\%%pcLss (1).\n"
".br\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%pcSrv (8).\n"
".br\n"
".BR \\%%glob (7).\n"
".\n"
,groffMacro,FMC_DATE,FMC_VERSION
,FMC_URLS,FMC_AUTHORS
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/* ************************************************************************* */
