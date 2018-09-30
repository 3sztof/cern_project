/*****************************************************************************/
/*
 * $Log: pwSwitch.C,v $
 * Revision 1.8  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.7  2008/10/16 10:13:42  galli
 * groff manual
 *
 * Revision 1.6  2008/10/09 09:44:31  galli
 * minor changes
 *
 * Revision 1.5  2007/12/14 15:39:53  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.4  2007/10/23 16:05:44  galli
 * usage() prints FMC version
 *
 * Revision 1.2  2007/06/21 13:09:06  galli
 * Prompt for wildcard operations.
 * -f command line option to skip prompt
 * -d delay_time command line option
 * -m node_pattern command line option is repeatable.
 *
 * Revision 1.1  2005/04/18 23:39:08  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <argz.h>                         /* argz_create(), argz_stringify() */
#include <ctype.h>                                              /* toupper() */
#include <libgen.h>                                            /* basename() */
#include <math.h>                                      /* floor(3), round(3) */
#include <stdlib.h>                                               /* exit(3) */
#include <signal.h>                                             /* signal(2) */
#include <termios.h>                           /* tcgetattr(3), tcsetattr(3) */
#include <unistd.h>                                /* STDIN_FILENO, _exit(2) */
#include <errno.h>
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>                                  /* DimBrowser, DimClient */
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "fmcVersion.h"                                       /* FMC version */
#include "FmcUtils.IC"                               /* printServerVersion() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*---------------------------------------------------------------------------*/
/* in ipmi/include */
#include "ipmiSrv.h"
/*****************************************************************************/
void usage(int mode);
void shortUsage(void);
int getLocalOpt(int *argcP,char ***argvP,const char* optS,int withArg,
                char **optArg);
int ttyRaw(int fd);                          /* put terminal into a raw mode */
int ttyReset(int fd);                             /* restore terminal's mode */
static void signalHandler(int signo);
void getServerVersion(int hostPttnC,char **hostPttnV);
/*****************************************************************************/
/* globals */
int deBug=0;
/* terminal settings */
static struct termios savedTermios;
static int ttysavefd=-1;
static enum{RESET,RAW,CBREAK}ttystate=RESET;
/* revision */
static char rcsid[]="$Id: pwSwitch.C,v 1.8 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
/* configuration */
const char *dimConfFile=DIM_CONF_FILE_NAME;
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i;
  /* pointers */
  char *p=NULL;
  char *s=NULL;
  /* service browsing */
  int hostPttnC=0;
  char **hostPttnV=NULL;
  char *srvPattern=NULL;
  unsigned rawCmdPatternLen=0;
  char *rawCmdPattern=NULL;
  unsigned cmdPatternLen=0;
  char *cmdPattern=NULL;
  DimBrowser br;
  char *format=NULL;
  int type=0;
  int totNodeFoundN=0;
  int pttnNodeFoundN=0;
  char nodeFound[128]="";
  char ucNodeFound[128]="";
  char *cmd=NULL;
  char *cmdDup=NULL;
  char *dimDnsNode=NULL;
  /* command line switch */
  int found=0;
  int doForce=0;
  double delay=0.0;
  time_t delaySec=0;
  long delayNSec=0;
  struct timespec delayTS={0,0};
  int wildcardsPresent=0;
  char action[64];
  char c;
  int doPrintServerVersion=0;
  /* command line args */
  char *argz=0;
  size_t argz_len=0;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  if(argc==1)                                                /* no arguments */
  {
    fprintf(stderr,"\nCMD (non-option) argument required!\n");
    shortUsage();
  }
  if(getLocalOpt(&argc,&argv,"-h",0,&s))usage(0);
  if(getLocalOpt(&argc,&argv,"--help",0,&s))usage(0);
  if(getLocalOpt(&argc,&argv,"-hh",0,&s))usage(1);
  if(getLocalOpt(&argc,&argv,"-V",0,&s))doPrintServerVersion=1;
  if(getLocalOpt(&argc,&argv,"-f",0,&s))doForce=1;
  if(getLocalOpt(&argc,&argv,"-v",0,&s))deBug++;
  if(getLocalOpt(&argc,&argv,"-d",1,&s)==1)
  {
    delay=(double)strtof(s,(char**)NULL);
  }
  if(getLocalOpt(&argc,&argv,"-N",1,&s))dimDnsNode=s;
  for(hostPttnC=0;;)
  {
    found=getLocalOpt(&argc,&argv,"-m",1,&s);
    if(found==-1)shortUsage();
    else if(found==0)break;
    else
    {
      hostPttnC++;
      hostPttnV=(char**)realloc(hostPttnV,hostPttnC*sizeof(char*));
      hostPttnV[hostPttnC-1]=s;
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
  if(argc!=2 && !doPrintServerVersion)
  {
    if(argc>2)
    {
      fprintf(stderr,"\nGot %d non-option arguments: ",argc-1);
      for(int i=1;i<argc;i++)
      {
        if(i==argc-1)fprintf(stderr,"\"%s\".",argv[i]);
        else fprintf(stderr,"\"%s\", ",argv[i]);
      }
      fprintf(stderr,"\nOnly one non-option argument required (CMD)!\n");
    }
    else
    {
      fprintf(stderr,"\nCMD (non-option) argument required!\n");
    }
    shortUsage();
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
  /* handling of -V option */
  if(doPrintServerVersion)getServerVersion(hostPttnC,hostPttnV);
  /*-------------------------------------------------------------------------*/
  /* check the non-option argument */
  if(strcmp(argv[1],"on") && strcmp(argv[1],"off") && 
     strcmp(argv[1],"cycle") && strcmp(argv[1],"soft_off") &&
     strcmp(argv[1],"hard_reset") && strcmp(argv[1],"pulse_diag"))
  {
    fprintf(stderr,"\nInvalid CMD: %s!\n",argv[1]);
    shortUsage();
  }
  /*-------------------------------------------------------------------------*/
  /* process non-option argument, defining action */
  if(!strcmp(argv[1],"on"))strcpy(action,"power on");
  else if(!strcmp(argv[1],"off"))strcpy(action,"power off");
  else if(!strcmp(argv[1],"cycle"))strcpy(action,"power cycle");
  else if(!strcmp(argv[1],"soft_off"))strcpy(action,"soft shutdown via ACPI");
  else if(!strcmp(argv[1],"hard_reset"))
                                       strcpy(action,"pulse the reset signal");
  else if(!strcmp(argv[1],"pulse_diag"))
                           strcpy(action,"pulse a diagnostic interrupt (NMI)");
  /*-------------------------------------------------------------------------*/
  /* convert delay double -> struct timespec */
  delaySec=(time_t)floor(delay);
  delayNSec=(long)round((delay-floor(delay))*1e9);
  if(delayNSec>=1000000000)
  {
    delayNSec-=1000000000;
    delaySec+=1;
  }
  delayTS.tv_sec=delaySec;
  delayTS.tv_nsec=delayNSec;
  /*-------------------------------------------------------------------------*/
  /* chech if there are wildchars in hostnames */
  for(i=0;i<hostPttnC;i++)                        /* loop over node patterns */
  {
    if(strcspn(hostPttnV[i],"*[]?!")!=strlen(hostPttnV[i]))
    {
      wildcardsPresent=1;
      break;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* print selected nodes and ask confirm */
  if(!doForce && wildcardsPresent)
  {
    /*-----------------------------------------------------------------------*/
    printf("Going to %s the following nodes:\n",action);
    for(i=0,totNodeFoundN=0;i<hostPttnC;i++)      /* loop over node patterns */
    {
      srvPattern=(char*)realloc(srvPattern,1+strlen(hostPttnV[i]));
      strcpy(srvPattern,hostPttnV[i]);
      /*.....................................................................*/
      /* convert to upper case */
      for(p=srvPattern;*p;p++)*p=toupper(*p);
      /*.....................................................................*/
      cmdPatternLen=snprintf(NULL,0,"%s/%s/power_switch",SVC_HEAD,srvPattern);
      cmdPattern=(char*)realloc(cmdPattern,1+cmdPatternLen);
      snprintf(cmdPattern,1+cmdPatternLen,"%s/%s/power_switch",SVC_HEAD,
               srvPattern);
      /*.....................................................................*/
      rawCmdPatternLen=snprintf(NULL,0,"%s/*/power_switch",SVC_HEAD);
      rawCmdPattern=(char*)realloc(rawCmdPattern,1+rawCmdPatternLen);
      snprintf(rawCmdPattern,1+rawCmdPatternLen,"%s/*/power_switch",
               SVC_HEAD);
      /*.....................................................................*/
      br.getServices(rawCmdPattern);
      pttnNodeFoundN=0;
      while((type=br.getNextService(cmd,format))!=0)      /* loop over nodes */
      {
        if(!fnmatch(cmdPattern,cmd,0))               /* node matches pattern */
        {
          cmdDup=(char*)realloc(cmdDup,1+strlen(cmd));
          strcpy(cmdDup,cmd);
          p=strrchr(cmdDup,'/');
          *p='\0';
          p=strrchr(cmdDup,'/');
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
          printf(" %4d: %s\n",1+totNodeFoundN,nodeFound);
          pttnNodeFoundN++;
          totNodeFoundN++;
        }                                         /* if node matches pattern */
      }                                                   /* loop over nodes */
      /*.....................................................................*/
      if(!pttnNodeFoundN)
      {
        printf("       (no node found matching pattern \"%s\")\n",hostPttnV[i]);
      }
      /*.....................................................................*/
    }                                             /* loop over node patterns */
    /*-----------------------------------------------------------------------*/
    if(!totNodeFoundN)
    {
      printf("(No node found matching node pattern(s):  ");
      for(i=0;i<hostPttnC;i++)                    /* loop over node patterns */
      {
        printf("\"%s\"",hostPttnV[i]);
        if(i<hostPttnC-1)printf(", ");
      }
      printf(".)\nNo action taken!\n");
      exit(1);
    }
    /*-----------------------------------------------------------------------*/
    /* get the confirm [yn] */
    signal(SIGINT,signalHandler);
    signal(SIGQUIT,signalHandler);
    signal(SIGTERM,signalHandler);
    for(;;)
    {
      printf("Sure you want to %s all the nodes above [yn]? ",action);
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
      printf("%s operation cancelled on user request! Exiting...\n",action);
      exit(1);
    }
    printf("%s operation confirmed by user.\n",action);
  }
  /*-------------------------------------------------------------------------*/
  /* prepare command line to be passed to ipmiSrv */
  argz_create(argv+1,&argz,&argz_len);
  argz_stringify(argz,argz_len,32);
  if(argz==NULL)shortUsage();
  /*-------------------------------------------------------------------------*/
  for(i=0,totNodeFoundN=0;i<hostPttnC;i++)        /* loop over node patterns */
  {
    srvPattern=(char*)realloc(srvPattern,1+strlen(hostPttnV[i]));
    strcpy(srvPattern,hostPttnV[i]);
    /*.......................................................................*/
    /* convert to upper case */
    for(p=srvPattern;*p;p++)*p=toupper(*p);
    /*.......................................................................*/
    cmdPatternLen=snprintf(NULL,0,"%s/%s/power_switch",SVC_HEAD,srvPattern);
    cmdPattern=(char*)realloc(cmdPattern,1+cmdPatternLen);
    snprintf(cmdPattern,1+cmdPatternLen,"%s/%s/power_switch",SVC_HEAD,
             srvPattern);
    /*.......................................................................*/
    rawCmdPatternLen=snprintf(NULL,0,"%s/*/power_switch",SVC_HEAD);
    rawCmdPattern=(char*)realloc(rawCmdPattern,1+rawCmdPatternLen);
    snprintf(rawCmdPattern,1+rawCmdPatternLen,"%s/*/power_switch",SVC_HEAD);
    /*.......................................................................*/
    br.getServices(rawCmdPattern);
    pttnNodeFoundN=0;
    while((type=br.getNextService(cmd,format))!=0)        /* loop over nodes */
    {
      if(!fnmatch(cmdPattern,cmd,0))                 /* node matches pattern */
      {
        cmdDup=(char*)realloc(cmdDup,1+strlen(cmd));
        strcpy(cmdDup,cmd);
        p=strrchr(cmdDup,'/');
        *p='\0';
        p=strrchr(cmdDup,'/');
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
        printf("Sending IPMI %s command to node %4d: %s...\n",action,
               1+totNodeFoundN,nodeFound);
        DimClient::sendCommand(cmd,argz);
        pttnNodeFoundN++;
        totNodeFoundN++;
        if(delaySec||delayNSec)
        {
          delayTS.tv_sec=delaySec;
          delayTS.tv_nsec=delayNSec;
          nanosleep(&delayTS,NULL);
        }
      }                                           /* if node matches pattern */
    }                                                     /* loop over nodes */
    /*.......................................................................*/
    if(!pttnNodeFoundN)
    {
      printf("No node found matching pattern \"%s\"!\n",hostPttnV[i]);
    }
    /*.......................................................................*/
  }                                               /* loop over node patterns */
  /*-------------------------------------------------------------------------*/
  if(!totNodeFoundN)
  {
    printf("No node found matching node pattern(s):  ");
    for(i=0;i<hostPttnC;i++)                      /* loop over node patterns */
    {
      printf("\"%s\"",hostPttnV[i]);
      if(i<hostPttnC-1)printf(", ");
    }
    printf(".\nNo action taken!\n");
    exit(1);
  }
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
/* Look for the switch optS (e.g.: -C, --help) in the command line, return   */
/* its eventual argument in optArg and strip the switch and its eventual     */
/* argument from the command line.                                           */
/* If withArg=0 no argument is expected to follow the switch.                */
/* If withArg=1 an argument is expected to follow the switch.                */
/* Return value:                                                             */
/*   0 option not found                                                      */
/*   1 option found                                                          */
/*  -1 error                                                                 */
/* Example:                                                                  */
/*   main(int argc,char **argv)                                              */
/*   {                                                                       */
/*     int found=0;                                                          */
/*     char *pcSrvHostName=NULL;                                             */
/*     found=getLocalOpt(&argc,&argv,"-C",1,&pcSrvHostName);                 */
/*   }                                                                       */
/*****************************************************************************/
int getLocalOpt(int *argcP,char ***argvP,const char* optS,int withArg,
                char **optArg)
{
  int newArgc=*argcP;
  char **newArgv=*argvP;
  int i,j;
  int found=0;
  int found2=0;
  /*-------------------------------------------------------------------------*/
  /* find option */
  for(i=0;i<*argcP;i++)                               /* loop over arguments */
  {
    if(!strcmp((*argvP)[i],optS))
    {
      found++;
      if(!withArg)break;
      if(!(i+1<*argcP))                                 /* no more arguments */
      {
        fprintf(stderr,"\nERROR: option \"%s\" requires an argument!\n\n",optS);
        return -1;
      }
      else if((*argvP)[i+1][0]=='-')           /* following arg start with - */
      {
        fprintf(stderr,"\nERROR: option \"%s\" requires an argument!\n\n",optS);
        return -1;
      }
      else 
      {
        *optArg=strdup((*argvP)[i+1]);
      }
      break;
    }
  }                                                   /* loop over arguments */
  /*-------------------------------------------------------------------------*/
  if(!found)return found;
  /*-------------------------------------------------------------------------*/
  /* strip found option from command line */
    newArgc=*argcP-1-!!withArg;
    newArgv=(char**)malloc((1+newArgc)*sizeof(char*));
    newArgv[newArgc]=NULL;
    for(i=0,j=0,found2=0;;)
    {
      if(i>=*argcP)break;
      if(!found2 && !strcmp((*argvP)[i],optS))
      {
        i+=(1+!!withArg);
        found2++;
      }
      newArgv[j]=(*argvP)[i];
      i++;
      j++;
    }
  *argcP=newArgc;
  *argvP=newArgv;
  return found;
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
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"pwSwitch [-v] [-f] [-N DIM_DNS_NODE] [-m NODE_PATTERN...]\n"
"         [-d DELAY_TIME] CMD\n"
"pwSwitch -V [-v] [-N DIM_DNS_NODE] [-m CTRL_PC_PATTERN...]\n"
"pwSwitch { -h | --help }\n"
"\n"
"CMD = on | off | cycle | soft_off | hard_reset | pulse_diag\n"
"\n"
"Try \"pwSwitch -h\" for more information.\n";
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
"pwSwitch.man\n"
"\n"
"..\n"
"%s"
".hw hostname NODE_PATTERN\n"
".TH pwSwitch 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".Synopsis pwSwitch\\ \\-\n"
"Issue an IPMI Power Command (power on, power off, power cycle, etc.) to the "
"farm PC(s) through the FMC Power Manager Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis pwSwitch\n"
".ShortOpt[] v\n"
".ShortOpt[] f\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m NODE_PATTERN...\n"
".ShortOpt[] d DELAY_TIME\n"
"\\fICMD\\fP\n"
".EndSynopsis\n"
".sp\n"
".Synopsis pwSwitch\n"
".ShortOpt V\n"
".ShortOpt[] v\n"
".ShortOpt[] N DIM_DNS_NODE\n"
".ShortOpt[] m CTRL_PC_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis pwSwitch\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".PP\n"
"\\fICMD\\fP = on | off | cycle | soft_off | hard_reset | pulse_diag\n"
".PP\n"
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
"The Power Manager Clients \\fBpwSwitch\\fP(1), \\fBpwStatus\\fP(1) and "
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
".\n"
".SH DESCRIPTION\n"
"The \\fBpwSwitch\\fP(1) command issues the \\fBIPMI chassis power "
"command\\fP \\fICMD\\fP to all the farm nodes (controlled by a Power Manager "
"Server) whose hostname matches at least one of the wildcard patter "
"\\fINODE_PATTERN\\fP (by \\fBdefault\\fP: \\[dq]*\\[dq]).\n"
".PP\n"
"The IPMI chassis power commands which can be issued through the "
"\\fBpwSwitch\\fP(1) command (non-option argument \\fICMD\\fP) are:\n"
".TP\n"
"\\fBon\\fP:\n"
"Power-up the node\n"
".TP\n"
"\\fBoff\\fP:\n"
"Power down the node into soft off (S4/S5 state). This command does not "
"initiate a clean shutdown of the operating system prior to powering down "
"the system.\n"
".TP\n"
"\\fBcycle\\fP:\n"
"Provides a power off interval of at least 1 s. No action should occur if the "
"node power is in S4/S5 state.\n"
".TP\n"
"\\fBsoft_off\\fP:\n"
"Initiate a soft-shutdown of the OS via ACPI. This can be done in a number of "
"ways, commonly by simulating an overtemperture or by simulating a power "
"button press. It is necessary to have OS support for ACPI and some sort of "
"daemon watching for soft power events.\n"
".TP\n"
"\\fBhard_reset\\fP:\n"
"Pulse the system reset signal.\n"
".TP\n"
"\\fBpulse_diag\\fP:\n"
"Pulse a diagnostic interrupt (NMI) directly to the processor(s). This is "
"typically used to cause the operating system to do a diagnostic dump (OS "
"dependent).\n"
".PP\n"
"Unless the option \\fB-f\\fP (force) is specified, before proceeding "
"issueing the command, the list of the nodes matching the wildcard patterns "
"\\fINODE_PATTERN\\fP is shown and a \\fBconfirmation\\fP is asked.\n"
".PP\n"
"By means of the option \\fB-d\\fP \\fIDELAY_TIME\\fP, commands issued to "
"different farm nodes can be spaced out by the time interval "
"\\fIDELAY_TIME\\fP, in order to limit the \\fBinrush current\\fP).\n"
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
".OptDef v \"\"\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N \"\" DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef m \"\" NODE_PATTERN (string, repeatable)\n"
"Send the command only to the controlled farm PCs whose hostname matches at "
"least one of the wildcard patterns \\fINODE_PATTERN\\fP (\\fBdefault\\fP: "
"send the command to all the controlled farm PCs).\n"
".\n"
".OptDef V \"\"\n"
"For each selected node print the FMC Power Manager Server version and the "
"FMC version, than terminate.\n"
".\n"
".OptDef f \"\"\n"
"Do not prompt for confirmation before sending a command to a group of nodes "
"defined by means of a wildcard pattern (\\fBdefault\\fP: always ask "
"confirmation before sending a command to a group of nodes defined by means "
"of a wildcard pattern).\n"
".\n"
".OptDef d \"\" DELAY_TIME (float)\n"
"Space out the commands dispatched to different farm PCs by the delay "
"\\fIDELAY_TIME\\fP, in order to limit the \\fBinrush current\\fP "
"(\\fBdefault\\fP: no delay).\n"
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
"Switch on all the controlled PCs:\n"
".PP\n"
".ShellCommand pwSwitch on\n"
".ShellCommand pwSwitch -m \\[rs]* on\n"
".ShellCommand pwSwitch -m \\[dq]*\\[dq] on\n"
".PP\n"
".\n"
"Switch off all the controlled PCs:\n"
".PP\n"
".ShellCommand pwSwitch off\n"
".ShellCommand pwSwitch -m \\[rs]* off\n"
".ShellCommand pwSwitch -m \\[dq]*\\[dq] off\n"
".PP\n"
".\n"
"Power cycle all the controlled PCs:\n"
".PP\n"
".ShellCommand pwSwitch cycle\n"
".ShellCommand pwSwitch -m \\[rs]* cycle\n"
".ShellCommand pwSwitch -m \\[dq]*\\[dq] cycle\n"
".PP\n"
".\n"
"Switch on, switch off and power cycle only the farm node farm0107:\n"
".PP\n"
".ShellCommand pwSwitch -m farm0107 on\n"
".ShellCommand pwSwitch -m farm0107 off\n"
".ShellCommand pwSwitch -m farm0107 cycle\n"
".PP\n"
".\n"
"Power cycle the three farm nodes farm0107, farm0314 and farm0705:\n"
".PP\n"
".ShellCommand pwSwitch -m farm0107 -m farm0314 -m farm0705 cycle\n"
".PP\n"
".\n"
"Use a wildcard pattern to select the farm nodes:\n"
".PP\n"
".ShellCommand pwSwitch -m \\[dq]farm*\\[dq] off\n"
".ShellCommand pwSwitch -m farm\\[rs]* cycle\n"
".ShellCommand pwSwitch -m \\[dq]farm01[3-7]?\\[dq] on\n"
".ShellCommand pwSwitch -m \\[dq]farm01[1357]1\\[dq] off\n"
".ShellCommand pwSwitch -m farm010\\[rs]? -m farm011\\[rs]? on\n"
".ShellCommand pwSwitch -m \\[dq]farm01??\\[dq] -m \\[dq]farm02??\\[dq] -m "
"\\[dq]farm03??\\[dq] on\n"
".PP\n"
".\n"
"Space out the dispatch of the power-on command to different nodes by a delay of 0.5 seconds:\n"
".PP\n"
".ShellCommand pwSwitch -d 0.5 -m \\[dq]farm01*\\[dq] -m \\[dq]ctrl01*\\[dq] "
"on\n"
".PP\n"
".\n"
"Do not ask confirmation for a wildcard command:\n"
".PP\n"
".ShellCommand pwSwitch -f -m \\[dq]farm01*\\[dq] -m \\[dq]ctrl01*\\[dq] off\n"
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
".BR \\%%pwStatus (1),\n"
".BR \\%%ipmiViewer (1),\n"
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
