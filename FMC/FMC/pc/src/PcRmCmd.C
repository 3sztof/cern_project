/* ######################################################################### */
/*
 * $Log: PcRmCmd.C,v $
 * Revision 1.4  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.3  2008/09/17 13:53:09  galli
 * takes cluster, acl, lsSvc, llSvcand lssSvc from globals
 *
 * Revision 1.2  2008/09/16 09:35:05  galli
 * PcRmCmd::commandHandler() calls llSvc::updateService()
 * PcRmCmd::commandHandler() calls lssSvc::updateService()
 *
 * Revision 1.1  2008/09/15 07:42:13  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcRmCmd.IC"
/* ######################################################################### */
/* macro to free-up the heap memory mallocated by PcRmCmd::commandHandler()  */
#define FREE_RM_CMD_HANDLER_HEAP \
do{ \
  if(rArgz) \
  { \
    free(rArgz); \
    rArgz=NULL; \
    rArgz_len=0; \
  } \
  if(rArgv) \
  { \
    free(rArgv); \
    rArgv=NULL; \
    rArgc=0; \
  } \
}while(0)
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
extern PcCluster cluster;
extern PcAcl acl;
extern PcLsSvc *lsSvc;
extern PcLlSvc *llSvc;
extern PcLssSvc *lssSvc;
/* ######################################################################### */
/* process the "rm" DIM command */
void PcRmCmd::commandHandler()
{
  /* vector of node wildcard patterns */
  vector<string>nodePttn;
  /* iterator on nodePttn */
  vector<string>::iterator nIt;
  /* UTGID pattern */
  string utgidPttn;
  /* used by strtol(3) */
  char *endptr=NULL;
  /* used by argz_create_sep(3), argz_count(3), argz_extract(3) */
  char *rArgz=NULL;
  size_t rArgz_len=0;
  char **rArgv=NULL;
  int rArgc=0;
  /* used by getopt(3) */
  int flag;
  /* termination parameters */
  unsigned signo;                                       /* first signal sent */
  unsigned s9delay;             /* delay of possible second signal (SIGKILL) */
  /* characters allowed in a hostname wildcard pattern */
  const char *allowedChars="abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "0123456789-.?!*[]";
  /*-------------------------------------------------------------------------*/
  /* get the received command string rcvCmd */
  string rcvCmd(getString());         /* getString() inherited by DimCommand */
  /* chech authentication */
  if(!acl.isAllowed("pcRm",rcvCmd))return;
  /*-------------------------------------------------------------------------*/
  /* Convert the command NUL-terminated string "rcvCmd.c_str()" into an argz */
  /* vector (char *rArgz, size_t rArgz_len) by breaking it up at every       */
  /* occurrence of the separator ' ' (ASCII 32). "rcvCmd.c_str()" untouched. */
  /* e.g.:                                                                   */
  /* cmd:   "-a 1 -b 22 333\0"                                               */
  /* rArgz: "-a\01\0-b\022\0333\0\0"                                         */
  /* rArgz_len: 16                                                           */
  if(argz_create_sep(rcvCmd.c_str(),32,&rArgz,&rArgz_len))         /* malloc */
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": argz_create_sep(): %s!",
            rcvCmd.c_str(),strerror(errno));
    FREE_RM_CMD_HANDLER_HEAP;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* convert the argz vector into argv format for getopt() processing.       */
  /* e.g. (cfr. prev. example):                                              */
  /* rArgc: 5                                                                */
  /* rArgv: 0x9000, 0x9003, 0x9005, 0x9008, 0x900b, 0x0.                     */
  rArgc=argz_count(rArgz,rArgz_len);
  rArgv=(char**)malloc((rArgc+1)*sizeof(char*));                   /* malloc */
  argz_extract(rArgz,rArgz_len,rArgv);
  /*-------------------------------------------------------------------------*/
  /* default values for termination parameters */
  signo=15;                                               /* default SIGTERM */
  s9delay=4;      /* wait 4 s befor sending possible second signal (SIGKILL) */
  /*-------------------------------------------------------------------------*/
  /* process DIM command arguments */
  optind=0;
  opterr=0;
  while((flag=getopt(rArgc,rArgv,"+s:d:m:"))!=EOF)
  {
    switch(flag)
    {
      case 'm':
        nodePttn.push_back(string(optarg));
        break;
      case 's':
        signo=(unsigned)strtoul(optarg,&endptr,0);;
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid value \"%s\" "
                  "specified as argument of the command line option "
                  "\"-s SIGNAL\"! SIGNAL must be an unsigned integer!",
                  rcvCmd.c_str(),optarg);
          FREE_RM_CMD_HANDLER_HEAP;
          return;
        }
        break;
      case 'd':
        s9delay=(unsigned)strtoul(optarg,&endptr,0);;
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid value \"%s\" "
                  "specified as argument of the command line option "
                  "\"-d KILL_9_DELAY\"! KILL_9_DELAY must be an unsigned "
                  "integer!",rcvCmd.c_str(),optarg);
          FREE_RM_CMD_HANDLER_HEAP;
          return;
        }
        break;
      default:
        mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": getopt(): invalid option "
                "\"-%c\"!",rcvCmd.c_str(),optopt);
        FREE_RM_CMD_HANDLER_HEAP;
        return;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check command line non-option arguments */
  if(optind==rArgc)
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Missing UTGID_PATTERN! The "
            "\"UTGID_PATTERN\" non-option argument is mandatory!",
            rcvCmd.c_str());
    FREE_RM_CMD_HANDLER_HEAP;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* check command line options */
  /* default for no specified hostname patterns */
  if(nodePttn.empty())nodePttn.push_back("*");
  /* check for illegal hostnames */
  for(nIt=nodePttn.begin();nIt!=nodePttn.end();nIt++)
  {
    if(nIt->size()==0 || nIt->at(0)=='-' ||
       nIt->find_first_not_of(allowedChars)!=nIt->npos)
    {
      mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid node pattern \"%s\" "
              "specified as argument of the command line option "
              "\"-m NODE_PATTERN\"!",rcvCmd.c_str(),nIt->c_str());
      FREE_RM_CMD_HANDLER_HEAP;
      return;
    }
  }
  /* check for numeric ranges */
  /* check signal number */
  if(signo<1 || signo>(unsigned)SIGRTMAX)
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Signal %d, specified as "
            "argument of the command line option \"-s SIGNAL\" is not "
            "allowed! Only values 1..%d are allowed for SIGNAL!",
            rcvCmd.c_str(),signo,SIGRTMAX);
    FREE_RM_CMD_HANDLER_HEAP;
    return;
  }
  if(s9delay<1 || s9delay>86400)
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Delay %d, specified as "
            "argument of the command line option \"-d KILL_9_DELAY\" is not "
            "allowed! Only values 1..86400 are allowed for KILL_9_DELAY!",
            rcvCmd.c_str(),s9delay);
    FREE_RM_CMD_HANDLER_HEAP;
    return;
  }
  /*-------------------------------------------------------------------------*/
  utgidPttn=rArgv[optind];
  /*-------------------------------------------------------------------------*/
  /* free-up heap the memory mallocated by PcRmCmd::commandHandler()         */
  FREE_RM_CMD_HANDLER_HEAP;
  /*-------------------------------------------------------------------------*/
  cluster.rmSchPs(utgidPttn,nodePttn,signo,s9delay,rcvCmd.c_str());
  cluster.printSchPs(DEBUG,"Process list: ",";  ",".","(",", ",")");
  lsSvc->updateService();
  llSvc->updateService();
  lssSvc->updateService();
  return;
}
/* ######################################################################### */
