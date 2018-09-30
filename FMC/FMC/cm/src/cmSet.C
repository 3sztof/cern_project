/* ######################################################################### */
/*
 * $Log: cmSet.C,v $
 * Revision 1.14  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.13  2008/10/16 13:44:38  galli
 * minor changes
 *
 * Revision 1.12  2008/10/15 06:42:38  galli
 * bug fixed
 *
 * Revision 1.6  2008/10/07 15:22:05  galli
 * groff manual
 *
 * Revision 1.4  2008/10/06 12:46:01  galli
 * working version
 *
 * Revision 1.1  2008/10/03 22:09:13  galli
 * Initial revision
 */
/* ######################################################################### */
#include <stdio.h>
#define __STDC_FORMAT_MACROS                                       /* PRIu64 */
#include <inttypes.h>                                              /* PRIu64 */
#include <fnmatch.h>                                            /* fnmatch() */
#include <errno.h>
#include <string.h>                                           /* strerror(3) */
#include <ctype.h>                                              /* toupper() */
#include <termios.h>                           /* tcgetattr(3), tcsetattr(3) */
#include <pwd.h>                           /* uid_t, getpwnam(3) getpwuid(3) */
#include <grp.h>                           /* gid_t, getgrnam(3) getgrgid(3) */
#include <sys/stat.h>                      /* struct stat, lstat(2), open(2) */
#include <fcntl.h>                                      /* O_RDONLY, open(2) */
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>
/*---------------------------------------------------------------------------*/
#include "fmcPar.h"                                    /* DIM_CONF_FILE_NAME */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcGroffMacro.h"                             /* macro for man page */
#include "cmSrv.H"                                 /* SRV_NAME, RPC_TIME_OUT */
#include "CmAcl.IC"
#include "FmcUtils.IC"                               /* printServerVersion() */
/*****************************************************************************/
#define oMode (*((uint64_t*)oBuf))
#define oUid  (*((uint64_t*)(oBuf+sizeof(uint64_t))))
#define oGid  (*((uint64_t*)(oBuf+2*sizeof(uint64_t))))
#define oStr  (oBuf+3*sizeof(uint64_t))
/*****************************************************************************/
int deBug=0;
int errU=L_STD;
static char rcsid[]="$Id: cmSet.C,v 1.14 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
int rpcTimeOut=RPC_TIME_OUT;
char sepLine[80];
/* terminal settings */
static struct termios savedTermios;
static int ttysavefd=-1;
static enum{RESET,RAW,CBREAK}ttystate=RESET;
/* thread arbitration */
bool rpcDone;
pthread_mutex_t rpcMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t rpcCond=PTHREAD_COND_INITIALIZER;
/* ######################################################################### */
void usage(int mode);
void shortUsage(void);
void getServerVersion(int nodeHostNameC,char **nodeHostNameV);
int ttyRaw(int fd);                          /* put terminal into a raw mode */
int ttyReset(int fd);                             /* restore terminal's mode */
static void signalHandler(int signo);
/* ######################################################################### */
class ConfFileWriter:public DimRpcInfo
{
  /***************************************************************************/
  public:
  /***************************************************************************/
  void rpcInfoHandler()
  {
    char *rv=getString();
    /*.......................................................................*/
    printf("%s\n",rv);
    /*.......................................................................*/
    /* inform the main thread that we have done */
    rpcDone=true;
    pthread_cond_broadcast(&rpcCond);
    /*.......................................................................*/
  }
  /***************************************************************************/
  ConfFileWriter(char *name, char *noLink)
  :DimRpcInfo(name, rpcTimeOut, noLink)
  {
  };
  /***************************************************************************/
};
/* ######################################################################### */
int main(int argc,char **argv)
{
  /* counters */
  int i=0,j=0;
  /* pointers */
  char *p;
  /* getopt */
  int flag;
  /* dim DNS */
  char *dimDnsNode=NULL;
  /* service browsing */
  int nodeHostNameC=0;
  char **nodeHostNameV=NULL;
  string nodePttn;
  string successRawSvcPttn;
  string successSvcPttn;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  string successSvc;
  size_t b,e;
  int totNodeFoundN=0;
  int pttnNodeFoundN=0;
  string nodeFound;
  string setRpc;
  char *noLink;
  /* input file */
  char *iBuf=NULL;
  size_t iBufSz=0;
  ssize_t iBufRsz=0;
  int nChunk;
  off_t offset;
  char ch;
  mode_t fileMode=0644;
  char *iFileName=NULL;
  struct stat iFileStat;
  int iFD;
  /* command line switch */
  char *fileName=NULL;
  int doPrintServerVersion=0;
  int doForce=0;
  /* output buffer */
  char *oBuf;
  int oBufSz;
  char c;
  uid_t uid=~(uid_t)0;
  gid_t gid=~(gid_t)0;
  /* authentication */
  CmAcl acl;
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
  nodeHostNameC=0;
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt(argc,argv,"h::Vm:vN:p:t:M:fi:u:g:"))!=EOF)
  {
    switch(flag)
    {
      case 'm':
        nodeHostNameC++;
        nodeHostNameV=(char**)realloc(nodeHostNameV,nodeHostNameC*
                                      sizeof(char*));
        nodeHostNameV[nodeHostNameC-1]=optarg;
        break;
      case 'V':
        doPrintServerVersion=1;
        break;
      case 'v':
        deBug++;
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'p':
        fileName=optarg;
        break;
      case 'i':
        iFileName=optarg;
        break;
      case 'f':
        doForce=1;
        break;
      case 'M':
        fileMode=(mode_t)strtoul(optarg,(char**)NULL,8);
        break;
      case 't':
        rpcTimeOut=(int)strtol(optarg,(char**)NULL,0);
        break;
      case 'u':
        uid=(uid_t)strtoul(optarg,(char**)NULL,0);
        break;
      case 'g':
        gid=(gid_t)strtoul(optarg,(char**)NULL,0);
        break;
      case 'h':
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        fprintf(stderr,"\ngetopt(): invalid option \"-%c\"!\n",optopt);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* default: all the nodes */
  if(!nodeHostNameC)
  {
    nodeHostNameC=1;
    nodeHostNameV=(char**)realloc(nodeHostNameV,sizeof(char*));
    nodeHostNameV[nodeHostNameC-1]=(char*)"*";
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
    printf("[INFO] DIM_DNS_NODE: \"%s\" (from -N command-line option).\n",
           dimDnsNode);
  }
  else
  {
    dimDnsNode=getenv("DIM_DNS_NODE");
    if(dimDnsNode)
    {
      printf("[INFO] DIM_DNS_NODE: \"%s\" (from DIM_DNS_NODE environment "
             "variable).\n",dimDnsNode);
    }
    else
    {
      dimDnsNode=getDimDnsNode(dimConfFile,0,1);
      if(dimDnsNode)
      {
        printf("[INFO] DIM_DNS_NODE: \"%s\" (from \""DIM_CONF_FILE_NAME" "
               "file).\n",dimDnsNode);
      }
      else
      {
        fprintf(stderr,"[FATAL]: DIM Name Server (DIM_DNS_NODE) not defined!\n"
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
    fprintf(stderr,"[FATAL] setenv(): %s!\n",strerror(errno));
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  if(doPrintServerVersion)getServerVersion(nodeHostNameC,nodeHostNameV);
  /*-------------------------------------------------------------------------*/
  /* check command line */
  if(!fileName)
  {
    fprintf(stderr,"\nOption \"-p FILE_PATH\" must be specified in the "
            "command line!\n");
    shortUsage();
  }
  /*-------------------------------------------------------------------------*/
  printf("[INFO] Node patterns:  ");
  for(i=0;i<nodeHostNameC;i++)                    /* loop over node patterns */
  {
    printf("\"%s\"",nodeHostNameV[i]);
    if(i<nodeHostNameC-1)printf(", ");
  }
  printf(".\n");
  /*-------------------------------------------------------------------------*/
  /* set the data to be returned if RPC service is not available */
  noLink=(char*)"[ERROR] RPC service not available!";
  /*-------------------------------------------------------------------------*/
  /* get the input either from file or from stdin */
  if(iFileName)                                       /* get input from file */
  {
    /* check input file accessibility */
    if(access(iFileName,R_OK)==-1)
    {
      fprintf(stderr,"[FATAL] Can't access file \"%s\" for reading. access(2) "
              "error: %s!\n",iFileName,strerror(errno));
      return 1;
    }
    /* get input file size */
    if(lstat(iFileName,&iFileStat)<0)
    {
      fprintf(stderr,"[FATAL] Can't get info about file \"%s\". lstat(2) "
              "error: %s!\n",iFileName,strerror(errno));
      return 1;
    }
    /* empty input file */
    if(!iFileStat.st_size)
    {
      fprintf(stderr,"[FATAL] Empty file \"%s\"!\n",iFileName);
      return 1;
    }
    /* allocate buffer to store file */
    iBufSz=(size_t)((float)iFileStat.st_size*1.2+1);
    iBuf=(char*)malloc(iBufSz);
    memset(iBuf,0,iBufSz);
    /* open file */
    iFD=open(iFileName,O_RDONLY);
    if(iFD==-1)                                              /* open failure */
    {
      fprintf(stderr,"[FATAL] Can't open file \"%s\" for reading. open(2) "
              "error: %s!\n",iFileName,strerror(errno));
      return 1;
    }                                                        /* open failure */
    /* read file */
    iBufRsz=read(iFD,iBuf,iBufSz);
    if(iBufRsz==-1)                                          /* read failure */
    {
      fprintf(stderr,"[FATAL] Can't read file \"%s\" for reading. read(2) "
              "error: %s!\n",iFileName,strerror(errno));
      return 1;
    }                                                        /* read failure */
    /* handle incomplete read */
    if(iBufRsz<iFileStat.st_size)                         /* incomplete read */
    {
      fprintf(stderr,"[FATAL] Incomplete read of file \"%s\". Read %lu B read "
              "out of %lu!\n",iFileName,iBufRsz,iFileStat.st_size);
      return 1;
    }                                                     /* incomplete read */
  }                                                   /* get input from file */
  /*-------------------------------------------------------------------------*/
  else                                               /* get input from stdin */
  {
    printf("Type in the content of the configuration file.\n");
    printf("To terminate type Ctrl-D in a new line.\n");
    printf("%s\n",sepLine);
    iBuf=(char*)malloc(BUF_CHUNK_SZ);
    p=iBuf;
    for(nChunk=1;;)
    {
      offset=p-iBuf;
      if(offset>BUF_CHUNK_SZ*nChunk)
      {
        iBuf=(char*)realloc(iBuf,BUF_CHUNK_SZ*++nChunk);
        p=iBuf+offset;
      }
      ch=fgetc(stdin);
      if(ch!=EOF)
      {
        *p++=ch;
      }
      else
      {
        *p='\0';
        break;
      }
    }
    printf("%s\n",sepLine);
  }                                                  /* get input from stdin */
  /*-------------------------------------------------------------------------*/
  printf("Content to be written to file \"%s\":\n",fileName);
  printf("%s\n",sepLine);
  printf("%s",iBuf);
  printf("%s\n",sepLine);
  /*-------------------------------------------------------------------------*/
  /* print the selected nodes and ask confirm */
  if(!doForce)
  {
    printf("Going to write the content above to file \"%s\" into the "
           "following nodes:\n",fileName);
    /*-----------------------------------------------------------------------*/
    /* print the selected nodes */
    totNodeFoundN=0;
    for(i=0,j=0;i<nodeHostNameC;i++)              /* loop over node patterns */
    {
      nodePttn=nodeHostNameV[i];
      nodePttn=FmcUtils::toUpper(nodePttn);
      successRawSvcPttn=string(SVC_HEAD)+"/*/"+SRV_NAME+"/success";
      successSvcPttn=string(SVC_HEAD)+"/"+nodePttn+"/"+SRV_NAME+"/success";
      if(deBug)printf("  Service pattern: \"%s\".\n",successSvcPttn.c_str());
      /*.....................................................................*/
      br.getServices(successRawSvcPttn.c_str());
      while((type=br.getNextService(p,format))!=0)        /* loop over nodes */
      {
        if(!fnmatch(successSvcPttn.c_str(),p,0))  /* if node matches pattern */
        {
          totNodeFoundN++;
          successSvc=p;
          /*.................................................................*/
          /* extract node hostname from "success" service name */
          e=successSvc.find_last_of('/');
          e=successSvc.find_last_of('/',e-1);
          b=successSvc.find_last_of('/',e-1)+1;
          nodeFound=successSvc.substr(b,e-b);
          nodeFound=FmcUtils::toLower(nodeFound);
          /*.................................................................*/
          printf(" %4d: %s\n",totNodeFoundN,nodeFound.c_str());
          /*.................................................................*/
        }                                         /* if node matches pattern */
      }                                                   /* loop over nodes */
    }                                             /* loop over node patterns */
    if(!totNodeFoundN)
    {
      printf("[FATAL] No node found matching node pattern(s):  ");
      for(i=0;i<nodeHostNameC;i++)                /* loop over node patterns */
      {
        printf("\"%s\"",nodeHostNameV[i]);
        if(i<nodeHostNameC-1)printf(", ");
      }
      printf("! Terminating...\n");
      return 1;
    }
    /*-----------------------------------------------------------------------*/
    /* get the confirm [yn] */
    signal(SIGINT,signalHandler);
    signal(SIGQUIT,signalHandler);
    signal(SIGTERM,signalHandler);
    for(;;)
    {
      printf("Sure you want to write the file \"%s\" to the nodes listed "
             "above [yn]? ",fileName);
      fflush(stdout);
      ttyRaw(STDIN_FILENO);
      read(STDIN_FILENO,&c,1);
      ttyReset(STDIN_FILENO);
      c&=0xff;
      if(c=='y'||c=='n')break;
      printf("%c\n",c);
    }
    printf("%c\n",c);
    signal(SIGINT,SIG_DFL);
    signal(SIGQUIT,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
    if(c=='n')
    {
      printf("[WARN] Write operation cancelled on user request! "
             "Terminating...\n");
      return 1;
    }
    printf("[INFO] Write operation confirmed by user.\n");
  }                                                          /* if(!doForce) */
  /*-------------------------------------------------------------------------*/
  /* set the output buffer for the RPC */
  string authString=acl.getAuthString();
  oBufSz=3*sizeof(uint64_t)+authString.size()+1+strlen(fileName)+1+
         strlen(iBuf)+1;
  oBuf=(char*)malloc(oBufSz);
  memset(oBuf,0,oBufSz);
  oMode=(uint64_t)fileMode;
  oUid=(uint64_t)uid;
  oGid=(uint64_t)gid;
  p=oStr;
  snprintf(p,authString.size()+1+strlen(fileName)+1,"%s %s",authString.c_str(),
           fileName);
  p+=authString.size()+1+strlen(fileName)+1;
  sprintf(p,"%s",iBuf);
  /*-------------------------------------------------------------------------*/
  totNodeFoundN=0;
  for(i=0,j=0;i<nodeHostNameC;i++)                /* loop over node patterns */
  {
    printf("[INFO] Processing node pattern: \"%s\".\n",nodeHostNameV[i]);
    nodePttn=nodeHostNameV[i];
    nodePttn=FmcUtils::toUpper(nodePttn);
    successRawSvcPttn=string(SVC_HEAD)+"/*/"+SRV_NAME+"/success";
    successSvcPttn=string(SVC_HEAD)+"/"+nodePttn+"/"+SRV_NAME+"/success";
    if(deBug)printf("  Service pattern: \"%s\".\n",successSvcPttn.c_str());
    /*.......................................................................*/
    br.getServices(successRawSvcPttn.c_str());
    pttnNodeFoundN=0;
    while((type=br.getNextService(p,format))!=0)          /* loop over nodes */
    {
      pthread_mutex_lock(&rpcMutex);
      rpcDone=false;
      pthread_mutex_unlock(&rpcMutex);
      if(!fnmatch(successSvcPttn.c_str(),p,0))    /* if node matches pattern */
      {
        pttnNodeFoundN++;
        totNodeFoundN++;
        successSvc=p;
        /*...................................................................*/
        /* extract node hostname from "success" service name */
        e=successSvc.find_last_of('/');
        e=successSvc.find_last_of('/',e-1);
        b=successSvc.find_last_of('/',e-1)+1;
        nodeFound=successSvc.substr(b,e-b);
        nodeFound=FmcUtils::toLower(nodeFound);
        /*...................................................................*/
        /* invoke the "set" RPC */
        setRpc=string(SVC_HEAD)+"/"+FmcUtils::toUpper(nodeFound)+"/"+SRV_NAME+
               "/set";
        if(deBug)printf("  %3d Contacting Configuration Manager RPC: \"%s\""
                        "...\n",j,setRpc.c_str());
        printf("[INFO] Processing node %d: \"%s\".\n",totNodeFoundN,
               nodeFound.c_str());
        ConfFileWriter confFileWriter((char*)setRpc.c_str(),noLink);
        confFileWriter.setData(oBuf,oBufSz);
        /*...................................................................*/
        /* wait until ConfFileReader::rpcInfoHandler() has done */
        pthread_mutex_lock(&rpcMutex);
        while(!rpcDone)pthread_cond_wait(&rpcCond,&rpcMutex);
        pthread_mutex_unlock(&rpcMutex);
        /*...................................................................*/
      }                                           /* if node matches pattern */
    }                                                     /* loop over nodes */
    if(!pttnNodeFoundN)
    {
      printf("[WARN] No node found for pattern \"%s\"!\n",nodeHostNameV[i]);
    }
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  if(!totNodeFoundN)
  {
    printf("[FATAL] No node found matching node pattern(s):  ");
    for(i=0;i<nodeHostNameC;i++)                  /* loop over node patterns */
    {
      printf("\"%s\"",nodeHostNameV[i]);
      if(i<nodeHostNameC-1)printf(", ");
    }
    printf("! Terminating...\n");
    return 1;
  }
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
int ttyRaw(int fd)                          /* put terminal into a raw mode */
{
  struct termios  buf;
  if(tcgetattr(fd,&savedTermios)<0)return -1;
  buf=savedTermios;                                         /* save settings */
  /* echo off, canonical mode off, extended input processing off */
  buf.c_lflag&=~(ECHO|ICANON|IEXTEN);
  /* no SIGINT on BREAK, CR-to-NL off, input parity check off */
  /* don't strip 8th bit on input, output flow control off */
  buf.c_iflag&=~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
  /* clear size bits, parity checking off */
  buf.c_cflag&=~(CSIZE|PARENB);
  /* set 8 bits/char */
  buf.c_cflag|=CS8;
  /* output processing off */
  buf.c_oflag&=~(OPOST);
  /* 1 byte at a time, no timer */
  buf.c_cc[VMIN]=1;
  buf.c_cc[VTIME]=0;
  if(tcsetattr(fd,TCSAFLUSH,&buf)<0)return -1;
  ttystate=RAW;
  ttysavefd=fd;
  return 0;
}
/*****************************************************************************/
int ttyReset(int fd)                              /* restore terminal's mode */
{
  if (ttystate!=CBREAK && ttystate!=RAW)return 0;
  if(tcsetattr(fd,TCSAFLUSH,&savedTermios)<0)return -1;
  ttystate=RESET;
  return 0;
}
/*****************************************************************************/
static void signalHandler(int signo)
{
  ttyReset(STDIN_FILENO);
  printf("\nSignal %d (%s) received. Operation cancelled. Exiting...\n",signo,
         strsignal(signo));
  _exit(0);
}
/*****************************************************************************/
void getServerVersion(int nodeHostNameC,char **nodeHostNameV)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/process_controller/success */
  FmcUtils::printServerVersion(nodeHostNameC,nodeHostNameV,SVC_HEAD,SRV_NAME,
                               deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/process_controller/success */
  FmcUtils::printServerVersion(nodeHostNameC,nodeHostNameV,"",SRV_NAME,
                               deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(1);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"cmSet [-v...] [-N DIM_DNS_NODE] [-m NODE_PATTERN...] [-f] [-t RPC_TIME_OUT]\n"
"      [-M PERMISSION] [-u UID] [-g GID] [-i INPUT_FILE_PATH] -p FILE_PATH\n"
"cmSet -V [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"cmSet -h\n"
"Try \"cmSet -h\" for more information.\n";
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
"cmSet.man\n"
"\n"
"..\n"
"%s"
".hw NODE_PATTERN \n"
".TH cmSet 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis cmSet\\ \\-\n"
"Write a file to one or more farm nodes through the FMC Configuration "
"Manager\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis cmSet\n"
".ShortOpt[] v...\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] f\n"
".ShortOpt[] t RPC_TIME_OUT\n"
".ShortOpt[] M PERMISSION\n"
".ShortOpt[] u UID\n"
".ShortOpt[] g GID\n"
".ShortOpt[] i INPUT_FILE_PATH\n"
"\\fB-p\\fP \\fIFILE_PATH\\fP\n"
".EndSynopsis\n"
".sp\n"
".Synopsis cmSet\n"
".ShortOpt V\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN\n"
".EndSynopsis\n"
".sp\n"
".Synopsis cmSet\n"
".ShortOpt h\n"
".EndSynopsis\n"
".\n"
".\n"
".SH INTRODUCTION\n"
".\n"
"The FMC \\fBConfiguration Manager\\fP is a tool to \\fBread\\fP, "
"\\fBwrite\\fP and \\fBremove configuration files\\fP (more generally, "
"ASCII files) to/from the farm nodes.\n"
".PP\n"
"The Process Controller Server, \\fBcmSrv\\fP(8), typically runs on each farm "
"node and allows to access the files on the node it is running on.\n"
".PP\n"
"The Process Controller Clients \\fBcmSet\\fP(1), \\fBcmUnset\\fP(1) and "
"\\fBcmGet\\fP(1) can contact one or more Process Controller Servers "
"running on remote nodes to write files, remove files and read files "
"respectively.\n"
".PP\n"
"File access can be \\fBrestricted\\fP in 2 different ways simultaneously: "
"(a) by means of a username/hostname access control and (b) by means of a "
"filename access control. See \\fBcmSrv\\fP(8) for more information.\n"
".PP\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"The command \\fBcmSet\\fP(1) \\fBwrites\\fP the file at path "
"\\fIINPUT_FILE_PATH\\fP on the local host (if the option \\fB-i\\fP "
"\\fIINPUT_FILE_PATH\\fP is specified) or the file got from the standard "
"input (if the option \\fB-i\\fP \\fIINPUT_FILE_PATH\\fP is omitted) to the "
"absolute path \\fIFILE_PATH\\fP into all the farm nodes whose hostname "
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP.\n"
".PP\n"
"If the file content is got from the standard input (option \\fB-i\\fP "
"\\fIINPUT_FILE_PATH\\fP omitted) the user is asked to type the file content "
"into the terminal. To terminate the text input, the character \\fBCtrl-D\\fP "
"must be typed in a new line.\n"
".PP\n"
"Unless the option \\fB-f\\fP (force) is specified, before proceeding writing "
"the files, the list of the nodes matching the wildcard patterns "
"\\fINODE_PATTERN\\fP is shown and a \\fBconfirmation\\fP is asked.\n"
".PP\n"
"The DIM Name Server, looked up to seek for Configuration Managers Servers, "
"can be chosen (in decreasing order of priority) by:\n"
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
".OptDef h \"\"\n"
"Get basic usage help from the command line, then terminate.\n"
".\n"
".OptDef v \"\"\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Operate only with the nodes whose hostname matches at least one of the "
"wildcard patterns \\fINODE_PATTERN\\fP (\\fBdefault\\fP: operate with all "
"the nodes).\n"
".\n"
".OptDef V \"\"\n"
"For each selected node print the FMC Configuration Manager Server version "
"and the FMC version, than terminate.\n"
".\n"
".OptDef p \"\" FILE_PATH (string, mandatory)\n"
"Write to the files at path \\fIFILE_PATH\\fP into the selected nodes.\n"
".\n"
".OptDef i \"\" INPUT_FILE_PATH (string)\n"
"Get the content to be written to files \\fIFILE_PATH\\fP in the selected "
"nodes, from the file at path \\fIINPUT_FILE_PATH\\fP on the local host. "
"\\fBDefault\\fP: get the content from the standard input stream.\n"
".\n"
".OptDef f \"\"\n"
"Do not ask for a confirmation before proceeding writing the files. "
"\\fBDefault\\fP before proceeding writing the files, the list of the nodes "
"matching the wildcard patterns \\fINODE_PATTERN\\fP is shown and a "
"confirmation is asked.\n"
".\n"
".OptDef t \"\" RPC_TIME_OUT (integer)\n"
"Set the timeout for the Remote Procedure Call to \\fIRPC_TIME_OUT\\fP "
".\n"
".OptDef M \"\" PERMISSION (octal integer)\n"
"Set the file access permission of written file to \\fIPERMISSION\\fP. "
"The integer \\fIPERMISSION\\fP can be the bitwise \\fIOR\\fP of the "
"following (\\fBoctal\\fP) numbers:\n"
".PP\n"
".RS\n"
"00001 read permission by others\n"
".br\n"
"00002 write permission by others\n"
".br\n"
"00004 execute/search permission by others\n"
".br\n"
"00010 read permission by group\n"
".br\n"
"00020 write permission by group\n"
".br\n"
"00040 execute/search permission by group\n"
".br\n"
"00100 read permission by owner\n"
".br\n"
"00200 write permission by owner\n"
".br\n"
"00400 execute/search permission by owner\n"
".br\n"
"01000 sticky bit\n"
".br\n"
"02000 set group ID on execution\n"
".br\n"
"04000 set user ID on execution\n"
".RE\n"
".\n"
".OptDef u \"\" UID (integer)\n"
"Set the ownership of the written file to the user identifier \\fIUID\\fP.\n"
".\n"
".OptDef g \"\" GID (integer)\n"
"Set the ownership of the written file to the group identifier \\fIGID\\fP.\n"
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
"Write the text got from the standard input to the file \"/tmp/test.conf\" "
"into all the farm nodes registered with the current DIM name server:\n"
".PP\n"
".ShellCommand cmSet -p /tmp/test.conf\n"
".PP\n"
".\n"
"Write the text got from the standard input to the file \"/tmp/test.conf\" "
"into the farm node \"farm0101\":\n"
".PP\n"
".ShellCommand cmSet -m farm0101 -p /tmp/test.conf\n"
".PP\n"
".\n"
"Write the text got from the standard input to the file \"/tmp/test.conf\" "
"into the farm nodes \"farm0101\" and \"farm0102\":\n"
".PP\n"
".ShellCommand cmSet -m farm0101 -m farm0102 -p /tmp/test.conf\n"
".PP\n"
".\n"
"Use wildcard patterns to select the nodes:\n"
".PP\n"
".ShellCommand cmSet -m \\[dq]farm0[0-5]*\\[dq] -m \\[dq]farm07[135][0-5]"
"\\[dq]\\: -p /tmp/test.conf\n"
".PP\n"
".\n"
"Write the text got from the local file \"tmp_test.conf\" to the files "
"\"/tmp/test.conf\" into the farm nodes \"farm0101\" and \"farm0102\":\n"
".PP\n"
".ShellCommand cmSet -m farm0101 -m farm0102 -i tmp_test.conf -p "
"/tmp/test.conf\n"
".PP\n"
".\n"
"Do not ask for confirmation:\n"
".PP\n"
".ShellCommand cmSet -f -m farm0101 -m farm0102 -i tmp_test.conf\\: "
"-p /tmp/test.conf\n"
".ShellCommand cmSet -f -i etc_tmSrv.allow -p /etc/tmSrv.allow\n"
".ShellCommand cmSet -f -m \\[dq]ctrl*\\[dq] -i etc_pcSrv.allow -p "
"/etc/pcSrv.allow\n"
".ShellCommand cmSet -f -m \\[dq]ctrl*\\[dq] -i etc_pcSrv.init -p "
"/etc/pcSrv.init\n"
".PP\n"
".\n"
"Set the UID to 1206, the GID to 100 and the permission to "
"\\[hy]rwxr\\[hy]xr\\[hy]x:\n"
".PP\n"
".ShellCommand cmSet -m farm0101 -u 1206 -g 100 -M 755 -p /tmp/test.conf\n"
".PP\n"
".\n"
"Set the permission to \\[hy]rwx\\[hy]\\[hy]\\[hy]\\[hy]\\[hy]\\[hy]:\n"
".PP\n"
".ShellCommand cmSet -m farm0101 -u 1206 -g 100 -M 700 -p /tmp/test.conf\n"
".PP\n"
".\n"
"Set the permission to \\[hy]rwxr\\[hy]sr\\[hy]x:\n"
".PP\n"
".ShellCommand cmSet -m farm0101 -u 1206 -g 100 -M 2755 -p /tmp/test.conf\n"
".PP\n"
".\n"
"Set the permission to \\[hy]rwsr\\[hy]sr\\[hy]x:\n"
".PP\n"
".ShellCommand cmSet -m farm0101 -u 1206 -g 100 -M 6755 -p /tmp/test.conf\n"
".PP\n"
".\n"
"Check the version of the Configuration Manager Servers installed on the farm "
"nodes:\n"
".PP\n"
".ShellCommand cmSet -V\n"
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
".BR \\%%cmUnset (1),\n"
".BR \\%%cmGet (1),\n"
".BR \\%%cmSrv (8),\n"
".br\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%ipmiSrv (8),\n"
".BR \\%%pcSrv (8).\n"
".br\n"
".BR \\%%glob (7).\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro,FMC_DATE,FMC_VERSION
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/* ######################################################################### */
