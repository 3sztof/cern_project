/*****************************************************************************/
/*
 * $Log: nifReset.C,v $
 * Revision 1.5  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.4  2008/10/09 09:57:26  galli
 * minor changes
 *
 * Revision 1.3  2008/02/28 14:09:28  galli
 * SVCs of the agreement Galli-Gaspar-Mazurov-Rodriguez of 2008/01/28
 *
 * Revision 1.2  2007/12/21 14:20:11  galli
 * Can get DIM_DNS_NODE from: (1) cmd-line option, (2) environment (3) file.
 *
 * Revision 1.1  2005/09/15 14:39:27  galli
 * Initial revision
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include <ctype.h>                                              /* toupper() */
#include <string.h>                                              /* memchr() */
#include <libgen.h>                                            /* basename() */
#include <errno.h>
/*****************************************************************************/
/* DIM */
#include "dic.hxx"                                  /* DimBrowser, DimClient */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcPar.h"                  /* DIM_CONF_FILE_NAME, DIM_DNS_SRV_LIST */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
/*****************************************************************************/
int deBug=0;
static char rcsid[]="$Id: nifReset.C,v 1.5 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
const char *dimConfFile=DIM_CONF_FILE_NAME;
const char *srvName="net/ifs";
/*****************************************************************************/
void usage(void);
/*****************************************************************************/
int main(int argc,char **argv)
{
  /* counters */
  int i;
  /* pointers */
  char *p=NULL;
  /* service browsing */
  int hostNamePattC=0;
  char **hostNamePattV=NULL;
  char *hostNamePattUc=NULL;
  char *cmdPatt=NULL;
  int nodeFoundN=0;
  char nodeFound[128]="";
  char *dimBrPatt=NULL;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  char *cmd=NULL;
  char *dimDnsNode=NULL;
  /* getopt */
  int flag;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  hostNamePattC=0;
  while((flag=getopt(argc,argv,"m:vN:h"))!=EOF)
  {
    switch(flag)
    {
      case 'm':
        hostNamePattC++;
        hostNamePattV=(char**)realloc(hostNamePattV,hostNamePattC*
                                      sizeof(char*));
        hostNamePattV[hostNamePattC-1]=optarg;
        break;
      case 'v':
        deBug++;
        break;
      case 'N':
        dimDnsNode=optarg;
        break;
      case 'h':
        usage();
        break;
      default:
        fprintf(stderr,"getopt(): invalid option \"-%c\"!",optopt);
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
  printf("Node pattern list:  ");
  for(i=0;i<hostNamePattC;i++)                    /* loop over node patterns */
  {
    printf("\"%s\"",hostNamePattV[i]);
    if(i<hostNamePattC-1)printf(", ");
  }
  printf(".\n");
  /*-------------------------------------------------------------------------*/
  for(i=0;i<hostNamePattC;i++)                    /* loop over node patterns */
  {
    printf("  Node pattern: \"%s\":\n",hostNamePattV[i]);
    hostNamePattUc=(char*)realloc(hostNamePattUc,1+strlen(hostNamePattV[i]));
    strcpy(hostNamePattUc,hostNamePattV[i]);
    /* convert to upper case */
    for(p=hostNamePattUc;*p;p++)*p=toupper(*p);
    cmdPatt=(char*)realloc(cmdPatt,1+snprintf(NULL,0,"/FMC/%s/%s/reset",
                           hostNamePattUc,srvName));
    sprintf(cmdPatt,"/FMC/%s/%s/reset",hostNamePattUc,srvName);
    if(deBug)printf("  Command pattern: \"%s\"\n",cmdPatt);
    /*.......................................................................*/
    dimBrPatt=(char*)realloc(dimBrPatt,1+snprintf(NULL,0,"/FMC/*/%s/reset",
                             srvName));
    sprintf(dimBrPatt,"/FMC/*/%s/reset",srvName);
    br.getServices(dimBrPatt);
    nodeFoundN=0;
    while((type=br.getNextService(cmd,format))!=0)        /* loop over nodes */
    {
      if(!fnmatch(cmdPatt,cmd,0))
      {
        /*...................................................................*/
        nodeFoundN++;
        /*...................................................................*/
        snprintf(nodeFound,sizeof(nodeFound),"%s",cmd+strlen("/FMC/"));
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
        /* Issue command */
        if(deBug)printf("Contacting DIM command: \"%s\"...\n",cmd);
        printf("    NODE: \"%s\". Resetting NIF counters...\n",nodeFound);
        DimClient::sendCommand(cmd,0);
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
}
/*****************************************************************************/
void usage()
{
  fprintf(stderr,
"\n"
"NAME\n"
"       nifReset - Reset the counters of the FMC Network Interface Monitor\n"
"                  Servers\n"
"\n"
"SYNOPSIS\n"
"       nifReset [-N DIM_DNS_NODE] [-v] [-m NODE_PATTERN...]\n"
"       nifReset -h\n"
"\n"
"DESCRIPTION\n"
"       Reset the counters used to evaluate the average and the maximum\n"
"       values of the Network Interface statistics on the Servers running\n"
"       on the farm nodes whose hostname matches the wildcard pattern\n"
"       NODE_PATTERN, specified using the -m command line option.\n"
"       DIM Name Server can be chosen (in decreasing order of priority) by:\n"
"              1. Specifying the -N DIM_DNS_NODE command-line option;\n"
"              2. Specifying the DIM_DNS_NODE environment variable;\n"
"              3. Defining the DIM_DNS_NODE in the file \"/etc/sysconfig/dim\""
".\n"
"\n"
"OPTIONS\n"
"       -h     Get basic usage help from the command line.\n"
"       -N DIM_DNS_NODE (string)\n"
"              Use DIM_DNS_NODE as DIM Name Server node.\n"
"       -v     Increase verbosity for debugging.\n"
"       -m NODE_PATTERN (string, repeatable)\n"
"              Reset only the nifSrv servers running on the nodes whose\n"
"              hostname matches the wildcard pattern NODE_PATTERN (default:\n"
"              reset the interrupt servers running on all the nodes).\n"
"\n"
"ENVIRONMENT\n"
"       DIM_DNS_NODE (string, mandatory if not defined otherwise, see above)\n"
"              Host name of the node which is running the DIM DNS.\n"
"       LD_LIBRARY_PATH (string, mandatory if not set using ldconfig)\n"
"              Must include the path to the libraries \"libdim\"\n"
"              and \"libFMCutils\".\n"
"\n"
"EXAMPLES\n"
"       Reset the Network Interface Monitor Servers running on all the farm\n"
"       nodes:\n"
"              nifReset -m \"*\"\n"
"              nifReset\n"
"       Reset only the Network Interface Monitor Servers running on the\n"
"       selected nodes:\n"
"              nifReset -m farm0101 -m farm0102\n"
"       Reset only the Network Interface Monitor Servers running on the\n"
"       wildcard-selected nodes:\n"
"              nifReset -m \"farm01*\" -m \"farm02*\"\n"
"              nifReset -m farm01\\* -m farm02\\*\n"
"              nifReset -m \"farm010[1357]\"\n" 
"\n"
"AUTHOR\n"
"       Domenico Galli <domenico.galli@bo.infn.it>\n"
"\n"
"VERSION\n"
"       Source:  %s\n"
"       Package: FMC-%s\n"
"\n"
"SEE ALSO\n"
"       nifViewer(FMC), nifSrv(FMC).\n"
"\n"
,rcsidP,FMC_VERSION
         );
  exit(1);
}
/*****************************************************************************/
