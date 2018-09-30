/* ######################################################################### */
/*
 * $Log: PcCluster.C,v $
 * Revision 1.17  2012/12/04 13:23:07  galli
 * oomScoreAdj data in getSchPsLl()
 *
 * Revision 1.16  2012/12/04 11:23:03  galli
 * Support for oomScoreAdj
 *
 * Revision 1.15  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.14  2008/09/26 20:30:03  galli
 * added method bool findNode(const string&)
 * method addSchPs(): added 4th argument (bool startIt=true)
 * added method void addSchPs(string &,bool)
 * method readNodes skip duplicate hostnames
 * added method readInit()
 * added method printUpdateCnt()
 *
 * Revision 1.13  2008/09/24 09:07:44  galli
 * minor changes
 *
 * Revision 1.12  2008/09/23 07:44:15  galli
 * memory leak fixed (PcProcess proliferation)
 *
 * Revision 1.11  2008/09/22 08:00:24  galli
 * utils ltos() and ultosx() moved to FmcUtils.C
 * added methods isTmSubscribed() and setTmSubscribed()
 *
 * Revision 1.10  2008/09/19 21:11:12  galli
 * added methods: getTmSrvNames(), setRunPss(), checkRunning(), printRunPss()
 *
 * Revision 1.9  2008/09/17 13:19:15  galli
 * no more access to acl
 * added method PcCluster::getSchPsLss
 * PcCluster::addSchPs gets arguments as references
 * bug fixed in PcCluster::ultosx
 *
 * Revision 1.7  2008/09/16 06:44:28  galli
 * added getSchPsLl(), ltos() and ultosx() methods
 *
 * Revision 1.6  2008/09/15 08:39:44  galli
 * PcCluster::getSchPsLs() returns (none) if no processes in a node
 *
 * Revision 1.5  2008/09/15 07:56:48  galli
 * included directives moved to PcCluster.IC
 * changed addSchPs() and rmSchPs() methods
 * added getSchPsLs() and itos() methods
 *
 * Revision 1.4  2008/09/12 09:38:25  galli
 * added method PcCluster::addSchPs()
 * added method PcCluster::rmSchPs()
 * added method PcCluster::printSchPs()
 *
 * Revision 1.3  2008/09/10 07:21:11  galli
 * vector<PcNode*>nodes -> vector<PcNode>nodes
 *
 * Revision 1.1  2008/09/09 06:30:38  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcCluster.IC"
/* ######################################################################### */
/* macro to free-up the heap memory mallocated by method                     */
/* PcCluster::addSchPs(string &)                                             */
#define FREE_ADD_CMD_HANDLER_HEAP \
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
/* ######################################################################### */
/* get methods */
/*****************************************************************************/
vector<string>PcCluster::getTmSrvNames()
{
  vector<string>tmSrvNames;
  vector<PcNode>::iterator ndIt;
  /*-------------------------------------------------------------------------*/
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)/* loop over cluster nodes */
  {
    tmSrvNames.push_back(ndIt->getTmSrvName());
  }
  return tmSrvNames;
}
/*****************************************************************************/
/* Get the list of processes scheduled on the nodes of this Cluster          */
string PcCluster::getSchPsLs(string numSep,string psSep,string nodeSep)
{
  vector<PcNode>::iterator ndIt;
  list<PcProcess> schPss;
  list<PcProcess>::iterator psIt;
  string s;
  /*-------------------------------------------------------------------------*/
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)/* loop over cluster nodes */
  {
    s+=ndIt->getName();
    s+=numSep;
    schPss=ndIt->getSchPss();    /* makes a copy of schPss list out of mutex */
    s+=FmcUtils::itos(schPss.size());
    s+=numSep;
    if(!schPss.size())
    {
      s+="(none)"+psSep;
    }
    else
    {
      for(psIt=schPss.begin();psIt!=schPss.end();psIt++)
      {
        s+=psIt->getUtgid();
        s+=psSep;
      }
    }
    s.replace(s.end()-1,s.end(),nodeSep);
  }
  return s;
}
/*****************************************************************************/
/* Get the list of processes scheduled on the nodes of this Cluster (with    */
/* start-up parameter details)                                               */
string PcCluster::getSchPsLl(string numSep,string psSep,string nodeSep,
                             string detOpen,string detClose,string detSep)
{
  vector<PcNode>::iterator ndIt;
  list<PcProcess> schPss;
  list<PcProcess>::iterator psIt;
  string s;
  const char *ss[3]={"SCHED_OTHER","SCHED_FIFO","SCHED_RR"};
  /*-------------------------------------------------------------------------*/
  /* loop over cluster nodes */
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    s+=ndIt->getName();
    s+=numSep;
    schPss=ndIt->getSchPss();    /* makes a copy of schPss list out of mutex */
    s+=FmcUtils::itos(schPss.size());
    s+=numSep;
    if(!schPss.size())
    {
      s+="(none)"+psSep;
    }
    else                                 /* scheduled process list not empty */
    {
      /* loop over scheduled processes */
      for(psIt=schPss.begin();psIt!=schPss.end();psIt++)
      {
        s+=psIt->getUtgid();
        s+=detOpen;
        /*...................................................................*/
        /* wd  */
        if(psIt->getWd().size())s+="wd=\""+psIt->getWd()+"\""+detSep;
        /* path */
        s+="path=\""+psIt->getPath()+"\""+detSep;
        /* args */
        if(psIt->getArgs().size())s+="args=\""+psIt->getArgs()+"\""+detSep;
        /* cleanEnv */
        if(psIt->isCleanEnv())s+="cleanEnv,";
        /* envp */
        if(psIt->getEnvp().size())s+=psIt->getEnvs(",")+detSep;
        /* daemon */
        if(psIt->isDaemon())s+="daemon,";
        /* scheduler */
        s+=string("scheduler=")+ss[psIt->getScheduler()]+detSep;
        /* nice */
        s+="nice="+FmcUtils::itos(psIt->getNice())+detSep;
        /* rtprio */
        s+="rtprio="+FmcUtils::itos(psIt->getRtPrio())+detSep;
        /* affinity mask */
        s+="affMask="+FmcUtils::ultosx(psIt->getAffMask())+detSep;
        /* OOM score adjust */
        s+="oomScoreAdj="+FmcUtils::itos(psIt->getOomScoreAdj())+detSep;
        /* user */
        if(psIt->getUser().size())s+="user=\""+psIt->getUser()+"\""+detSep;
        /* group */
        if(psIt->getGroup().size())s+="group=\""+psIt->getGroup()+"\""+detSep;
        /* rdrStdout */
        if(psIt->isRdrStdout())s+="redirect_stdout,";
        /* rdrStderr */
        if(psIt->isRdrStderr())s+="redirect_stderr,";
        /* outFIFO */
        if(psIt->getOutFIFO().size())
          s+="outFIFO=\""+psIt->getOutFIFO()+"\""+detSep;
        /* errFIFO */
        if(psIt->getErrFIFO().size())
          s+="errFIFO=\""+psIt->getErrFIFO()+"\""+detSep;
        /* noDrop */
        if(psIt->isNoDrop())s+="no-drop,";
        /* maxStartN */
        s+="maxStartN="+FmcUtils::itos(psIt->getMaxStartN())+detSep;
        /* disPeriod */
        s+="disPeriod="+FmcUtils::ltos(psIt->getDisPeriod())+" s"+detSep;
        /* checkPeriod */
        s+="checkPeriod="+FmcUtils::ltos(psIt->getCheckPeriod())+" s";
        /*...................................................................*/
        s+=detClose;
        s+=psSep;
      }                                     /* loop over scheduled processes */
    }                                    /* scheduled process list not empty */
    s.replace(s.end()-1,s.end(),nodeSep);
  }                                               /* loop over cluster nodes */
  return s;
}
/*****************************************************************************/
/* Get the list of processes scheduled on the nodes of this Cluster (with    */
/* (re)start details)                                                        */
string PcCluster::getSchPsLss(string numSep,string psSep,string nodeSep,
                             string detOpen,string detClose,string detSep)
{
  vector<PcNode>::iterator ndIt;
  list<PcProcess> schPss;
  list<PcProcess>::iterator psIt;
  string s;
  /*-------------------------------------------------------------------------*/
  /* loop over cluster nodes */
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    s+=ndIt->getName();
    s+=numSep;
    schPss=ndIt->getSchPss();    /* makes a copy of schPss list out of mutex */
    s+=FmcUtils::itos(schPss.size());
    s+=numSep;
    if(!schPss.size())
    {
      s+="(none)"+psSep;
    }
    else                                 /* scheduled process list not empty */
    {
      /* loop over scheduled processes */
      for(psIt=schPss.begin();psIt!=schPss.end();psIt++)
      {
        s+=psIt->getUtgid();
        s+=detOpen;
        /*...................................................................*/
        /* disable */
        if(!psIt->isRcEnabled())s+="disabled,";
        /* start number */
        s+="(re)start="+FmcUtils::itos(psIt->getStartCnt())+numSep;
        /* start times */
        if(psIt->getStartCnt())
        {
          s+=psIt->getStartTimesStr(detSep);
        }
        else
        {
          s+="(no start)";
        }
        /*...................................................................*/
        s+=detClose;
        s+=psSep;
      }                                     /* loop over scheduled processes */
    }                                    /* scheduled process list not empty */
    s.replace(s.end()-1,s.end(),nodeSep);
  }                                               /* loop over cluster nodes */
  return s;
}
/*****************************************************************************/
bool PcCluster::isTmSubscribed(const string &node)
{
  vector<PcNode>::iterator ndIt;
  bool found=false;
  bool tmSubscribed=false;
  /*-------------------------------------------------------------------------*/
  /* loop over the nodes of the cluster */
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    if(ndIt->getName()==node)
    {
      found=true;
      tmSubscribed=ndIt->isTmSubscribed();
      break;
    }
  }
  if(!found)
  {
     mPrintf(errU,WARN,METHOD,0,"Node \"%s\" not found!",node.c_str());
  }
  return tmSubscribed;
}
/*****************************************************************************/
bool PcCluster::findNode(const string &node)
{
  vector<PcNode>::iterator ndIt;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    if(ndIt->getName()==node)
    {
      found=true;
      break;
    }
  }
  return found;
}
/*****************************************************************************/
/* set methods                                                               */
/*****************************************************************************/
/* Add the node hostName to the list of the nodes for this Cluster           */
void PcCluster::addNode(string hostName)
{
  PcNode newNode(hostName);
  nodes.push_back(newNode);
}
/*****************************************************************************/
/* Add the process ps to all the cluster nodes whose hostname matches at     */
/* least one of the wildcard pattern nodePttn.                               */
/* The "cmd" argument is used only for diagnostic messages                   */
void PcCluster::addSchPs(PcProcess &ps, vector<string> &nodePttn, string &cmd,
                         bool startIt)
{
  vector<PcNode>::iterator ndIt;
  vector<string>::iterator npIt;
  int i;
  /*-------------------------------------------------------------------------*/
  /* loop over the node wildcard patterns passed to the "add" command */
  for(npIt=nodePttn.begin();npIt!=nodePttn.end();npIt++)
  {
    /* loop over the nodes of the cluster */
    for(ndIt=nodes.begin(),i=0;ndIt!=nodes.end();ndIt++)
    {
      if(!fnmatch(npIt->c_str(),ndIt->getName().c_str(),0))
      {
        ndIt->addSchPs(ps,cmd,startIt);
        i++;
      }
    }                                  /* loop over the nodes of the cluster */
    if(!i)
    {
      mPrintf(errU,WARN,METHOD,0,"CMD: \"%s\": No node controlled by this "
              "Ctrl-Pc matches the node pattern \"%s\"!",cmd.c_str(),
              npIt->c_str());
    }
  }      /* loop over the node wildcard patterns passed to the "add" command */
}
/*****************************************************************************/
void PcCluster::addSchPs(string &cmdLine,bool startIt)
{
  /* vector of node wildcard patterns */
  vector<string>nodePttn;
  /* iterator on nodePttn */
  vector<string>::iterator nIt;
  /* used by strtol(3) */
  char *endptr=NULL;
  /* used by argz_create_sep(3), argz_count(3), argz_extract(3) */
  char *rArgz=NULL;
  size_t rArgz_len=0;
  char **rArgv=NULL;
  int rArgc=0;
  /* used by getopt(3) */
  int flag;
  /* characters allowed in a hostname wildcard pattern */
  const char *allowedChars="abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "0123456789-.?!*[]";
  /* scheduler names */
  const char *ss[3]={"SCHED_OTHER","SCHED_FIFO","SCHED_RR"};
  const char *dimCmdOpts="+dcu:w:s:p:r:n:g:D:oeO:E:Am:M:K:X:a:k:";
  /*-------------------------------------------------------------------------*/
  /* Convert the command NUL-terminated string "cmdLine.c_str()" into an     */
  /* argz vector (char *rArgz, size_t rArgz_len) by breaking it up at every  */
  /* occurrence of the separator ' ' (ASCII 32). "cmdLine.c_str()"           */
  /* untouched.                                                              */
  /* e.g.:                                                                   */
  /* cmd:   "-a 1 -b 22 333\0"                                               */
  /* rArgz: "-a\01\0-b\022\0333\0\0"                                         */
  /* rArgz_len: 16                                                           */
  if(argz_create_sep(cmdLine.c_str(),32,&rArgz,&rArgz_len))        /* malloc */
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": argz_create_sep(): %s!",
            cmdLine.c_str(),strerror(errno));
    FREE_ADD_CMD_HANDLER_HEAP;
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
  PcProcess newPs;
  /*-------------------------------------------------------------------------*/
  /* process DIM command arguments */
  optind=0;
  opterr=0;
  while((flag=getopt(rArgc,rArgv,dimCmdOpts))!=EOF)
  {
    switch(flag)
    {
      case 'm':
        nodePttn.push_back(string(optarg));
        break;
      case 'c':
        newPs.setCleanEnv(true);
        break;
      case 'd':
        newPs.setDaemon(true);
        break;
      case 'o':
        newPs.setRdrStdout(true);
        break;
      case 'e':
        newPs.setRdrStderr(true);
        break;
      case 'O':
        newPs.setOutFIFO(string(optarg));
        break;
      case 'E':
        newPs.setErrFIFO(string(optarg));
        break;
      case 'A':
        newPs.setNoDrop(true);
        break;
      case 'u':
        newPs.setUtgid(string(optarg));
        break;
      case 'w':
        newPs.setWd(string(optarg));
        break;
      case 's':
        newPs.setScheduler((int)strtol(optarg,&endptr,0));
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid SCHEDULER \"%s\" "
                  "specified as argument of the command line option "
                  "\"-s SCHEDULER\"! SCHEDULER must be a integer in the range "
                  "0..2: SCHED_OTHER=0, SCHED_FIFO=1 and SCHED_RR=2 !",
                  cmdLine.c_str(),optarg);
          FREE_ADD_CMD_HANDLER_HEAP;
          return;
        }
        break;
      case 'p':
        newPs.setNice((int)strtol(optarg,&endptr,0));
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid NICE_LEVEL \"%s\" "
                  "specified as argument of the command line option "
                  "\"-p NICE_LEVEL\"! NICE_LEVEL must be a integer in the "
                  "range -20..19 !",cmdLine.c_str(),optarg);
          FREE_ADD_CMD_HANDLER_HEAP;
          return;
        }
        break;
      case 'r':
        newPs.setRtPrio((int)strtol(optarg,&endptr,0));
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid RT_PRIORITY "
                  "\"%s\" specified as argument of the command line option "
                  "\"-r RT_PRIORITY\"! RT_PRIORITY  must be a integer in the "
                  "range 0..99 !",cmdLine.c_str(),optarg);
          FREE_ADD_CMD_HANDLER_HEAP;
          return;
        }
        break;
      case 'k':
        newPs.setOomScoreAdj((int)strtol(optarg,&endptr,0));
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid OOM_SCORE_ADJ "
                  "\"%s\" specified as argument of the command line option "
                  "\"-k OOM_SCORE_ADJ\"! OOM_SCORE_ADJ must be a integer in "
                  "the range -999..1000 !",cmdLine.c_str(),optarg);
          FREE_ADD_CMD_HANDLER_HEAP;
          return;
        }
        break;
      case 'a':
        newPs.addCpu((unsigned)strtol(optarg,&endptr,0));
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid CPU_NUM \"%s\" "
                  "specified as argument of the command line option "
                  "\"-a CPU_NUM\"! CPU_NUM must be an unsigned integer!",
                  cmdLine.c_str(),optarg);
          FREE_ADD_CMD_HANDLER_HEAP;
          return;
        }
        break;
      case 'n':
        newPs.setUser(string(optarg));
        break;
      case 'g':
        newPs.setGroup(string(optarg));
        break;
      case 'D':
        newPs.putEnv(optarg);
        break;
      case 'M':
        newPs.setMaxStartN((int)strtol(optarg,&endptr,0));
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid MAX_START_N "
                  "parameter \"%s\" specified as argument of the command "
                  "line option \"-M MAX_START_N\"! The MAX_START_N parameter "
                  "must be an unsigned integer!",cmdLine.c_str(),optarg);
          FREE_ADD_CMD_HANDLER_HEAP;
          return;
        }
        break;
      case 'K':
        newPs.setCheckPeriod((long)strtol(optarg,&endptr,0));
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid CHECK_PERIOD "
                  "parameter \"%s\" specified as argument of the command "
                  "line option \"-K CHECK_PERIOD\"! The CHECK_PERIOD "
                  "parameter must be an unsigned integer.",cmdLine.c_str(),
                  optarg);
          FREE_ADD_CMD_HANDLER_HEAP;
          return;
        }
        break;
      case 'X':
        newPs.setDisPeriod((long)strtol(optarg,&endptr,0));
        if(endptr && *endptr)
        {
          mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid DIS_PERIOD "
                  "parameter \"%s\" specified as argument of the command "
                  "line option \"-X DIS_PERIOD\"! The DIS_PERIOD parameter "
                  "must be an unsigned integer!",cmdLine.c_str(),optarg);
          FREE_ADD_CMD_HANDLER_HEAP;
          return;
        }
        break;
      default:
        mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": getopt(): invalid option "
                "\"-%c\"!",cmdLine.c_str(),optopt);
        FREE_ADD_CMD_HANDLER_HEAP;
        return;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check command line non-option arguments */
  if(optind==rArgc)
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Missing program PATH! The "
            "\"PATH\" non-option argument is mandatory!",cmdLine.c_str());
    FREE_ADD_CMD_HANDLER_HEAP;
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
              "\"-m NODE_PATTERN\"!",cmdLine.c_str(),nIt->c_str());
      FREE_ADD_CMD_HANDLER_HEAP;
      return;
    }
  }
  /* chech for missing option arguments */
  if(newPs.getUtgid().size() && newPs.getUtgid().at(0)=='-')
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid utgid \"%s\" "
            "specified as argument of the (mandatory) command line option "
            "\"-u UTGID\"!",cmdLine.c_str(),newPs.getUtgid().c_str());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  if(newPs.getWd().size() && newPs.getWd().at(0)=='-')
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid working directory "
            "\"%s\" specified as argument of the command line option "
            "\"-w WD\"!",cmdLine.c_str(),newPs.getWd().c_str());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  if(newPs.getOutFIFO().size() && newPs.getOutFIFO().at(0)=='-')
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid output FIFO \"%s\" "
            "specified as argument of the command line option "
            "\"-O OUT_FIFO\"!",cmdLine.c_str(),newPs.getOutFIFO().c_str());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  if(newPs.getErrFIFO().size() && newPs.getErrFIFO().at(0)=='-')
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid error FIFO \"%s\" "
            "specified as argument of the command line option "
            "\"-E ERR_FIFO\"!",cmdLine.c_str(),newPs.getErrFIFO().c_str());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  if(newPs.getUser().size() && newPs.getUser().at(0)=='-')
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid user name \"%s\" "
            "specified as argument of the command line option "
            "\"-n USER_NAME\"!",cmdLine.c_str(),newPs.getUser().c_str());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  if(newPs.getGroup().size() && newPs.getGroup().at(0)=='-')
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Invalid group name \"%s\" "
            "specified as argument of the command line option "
            "\"-g GROUP_NAME\"!",cmdLine.c_str(),newPs.getGroup().c_str());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  /* check for numeric ranges */
  if(newPs.getScheduler()<0 || newPs.getScheduler()>2)
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Unknown scheduler %d specified "
            "as argument of the command line option \"-s SCHEDULER\"! "
            "SCHEDULER must be a integer in the range 0..2: SCHED_OTHER=0, "
            "SCHED_FIFO=1 and SCHED_RR=2 ! ",cmdLine.c_str(),
            newPs.getScheduler());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  if(newPs.getScheduler()==0 && newPs.getRtPrio()!=0)
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Real-time priority %d specified "
            "with the command-line option \"-r RT_PRIORITY\" not allowed for "
            "the SCHED_OTHER=0 time sharing scheduler specified with the "
            "command-line option \"-s SCHEDULER\"! Only 0 real-time "
            "priority is allowed for SCHED_OTHER time sharing scheduler!",
            cmdLine.c_str(),newPs.getRtPrio());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  if((newPs.getScheduler()==1 || newPs.getScheduler()==2) &&
     (newPs.getRtPrio()<1 || newPs.getRtPrio()>99))
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Real-time priority %d specified "
            "with the command-line option \"-r RT_PRIORITY\" not allowed for "
            "the %s=%d real-time scheduler specified with the command-line "
            "option \"-s SCHEDULER\"! Only 1..99 real-time priority allowed "
            "for %s real-rime scheduler!",cmdLine.c_str(),newPs.getRtPrio(),
            ss[newPs.getScheduler()],newPs.getScheduler(),
            ss[newPs.getScheduler()]);
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  if(newPs.getNice()<-20 || newPs.getNice()>19)
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Nice level %d, specified as "
            "argument of the command line option \"-p NICE_LEVEL\" is not "
            "allowed! Only values -20..19 are allowed for nice level!",
            cmdLine.c_str(),newPs.getNice());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  if(newPs.getOomScoreAdj()<-999 || newPs.getOomScoreAdj()>1000)
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": OOM Score Adjustment %d not "
            "allowed! Only values -999..1000 allowed for OOM Score "
            "Adjustment!",cmdLine.c_str(),newPs.getOomScoreAdj());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* set process path */
  newPs.setPath(rArgv[optind]);
  /*-------------------------------------------------------------------------*/
  /* set process arguments */
  for(int i=optind+1;i<rArgc;i++)newPs.addArg(rArgv[i]);
  /*-------------------------------------------------------------------------*/
  /* missing UTGID */
  if(!newPs.getUtgid().size())
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": Missing UTGID! The \"-u UTGID\" "
            "command line option is mandatory!",cmdLine.c_str());
    FREE_ADD_CMD_HANDLER_HEAP;
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* free-up heap the memory mallocated by PcAddCmd::commandHandler()        */
  FREE_ADD_CMD_HANDLER_HEAP;
  /*-------------------------------------------------------------------------*/
  /* add process to scheduled process list of farm nodes matching patten */
  addSchPs(newPs,nodePttn,cmdLine,startIt);
  /*-------------------------------------------------------------------------*/
  return;
}
/*****************************************************************************/
/* Remove all the processes which match the UTGID wildcard pattern utgidPttn */
/* from all the cluster nodes whose hostname matches at least one of the     */
/* wildcard pattern nodePttn.                                                */
/* The "cmd" argument is used only for diagnostic messages                   */
void PcCluster::rmSchPs(string utgidPttn,vector<string>nodePttn,
                        unsigned signo,unsigned s9delay,string cmd)
{
  vector<PcNode>::iterator ndIt;
  vector<string>::iterator npIt;
  int i;
  /*-------------------------------------------------------------------------*/
  /* loop over the node wildcard patterns passed to the "rm" command */
  for(npIt=nodePttn.begin();npIt!=nodePttn.end();npIt++)
  {
    /* loop over the nodes of the cluster */
    for(ndIt=nodes.begin(),i=0;ndIt!=nodes.end();ndIt++)
    {
      if(!fnmatch(npIt->c_str(),ndIt->getName().c_str(),0))
      {
        ndIt->rmSchPs(utgidPttn,signo,s9delay,cmd);
        i++;
      }
    }                                  /* loop over the nodes of the cluster */
    if(!i)
    {
      mPrintf(errU,WARN,METHOD,0,"CMD: \"%s\": No node controlled by this "
              "Ctrl-Pc matches the node pattern \"%s\"!",cmd.c_str(),
              npIt->c_str());
    }
  }       /* loop over the node wildcard patterns passed to the "rm" command */
}
/*****************************************************************************/
void PcCluster::setRunPss(string &node, int runPssSz, char *runPssSeq)
{
  vector<PcNode>::iterator ndIt;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  /* loop over the nodes of the cluster */
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    if(ndIt->getName()==node)
    {
      found=true;
      ndIt->setRunPss(runPssSz,runPssSeq);
      break;
    }
  }
  if(!found)
  {
     mPrintf(errU,WARN,METHOD,0,"Node \"%s\" not found!",node.c_str());
  }
}
/*****************************************************************************/
void PcCluster::setTmSubscribed(const string &node,bool t)
{
  vector<PcNode>::iterator ndIt;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  /* loop over the nodes of the cluster */
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    if(ndIt->getName()==node)
    {
      found=true;
      ndIt->setTmSubscribed(t);
      break;
    }
  }
  if(!found)
  {
     mPrintf(errU,WARN,METHOD,0,"Node \"%s\" not found!",node.c_str());
  }
}
/*****************************************************************************/
/* check methods                                                             */
/*****************************************************************************/
void PcCluster::checkRunning(string &node)
{
  vector<PcNode>::iterator ndIt;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  /* loop over the nodes of the cluster */
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    if(ndIt->getName()==node)
    {
      found=true;
      ndIt->checkRunning();
      break;
    }
  }
  if(!found)
  {
     mPrintf(errU,WARN,METHOD,0,"Node \"%s\" not found!",node.c_str());
  }
}
/*****************************************************************************/
/* read methods                                                              */
/*****************************************************************************/
/* Read the list of the nodes from the file confFileName and add them to     */
/* the list of the nodes for this Cluster                                    */
void PcCluster::readNodes(string confFileName)
{
  unsigned i,j;
  string record;
  size_t c,b,e;
  bool allowed;
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,DEBUG,METHOD,0,"Trying to read Configuration file: \"%s\"...",
          confFileName.c_str());
  /* check configuration file accessibility */
  if(access(confFileName.c_str(),R_OK)==-1)
  {
    mPrintf(errU,FATAL,METHOD,0,"Configuration file: \"%s\" not accessible. "
            "access(2) error: %s!",confFileName.c_str(),strerror(errno));
    exit(1);
  }
  /* parse file */
  ifstream confFs(confFileName.c_str(),ifstream::in);
  for(i=0;;i++)                                    /* loop over file records */
  {
    getline(confFs,record);
    if(confFs.eof())break;
    if(confFs.fail())
    {
      mPrintf(errU,FATAL,METHOD,0,"Failure in reading the Initialization "
              "file: \"%s\"!",confFileName.c_str());
      exit(1);
    }
    /* suppress comments */
    c=record.find_first_of("#");                       /* start comment char */
    if(c!=record.npos)record=record.substr(0,c);              /* cut comment */
    /* suppress heading blanks */
    b=record.find_first_not_of(" \t");          /* first non-blank character */
    if(b==record.npos)           /* no chars after blanks and before comment */
    {
      if(deBug&0x20)
      {
        mPrintf(errU,VERB,METHOD,0,"Configuration file record # %d skipped "
                "(blank record or comment record).",i+1);
      }
      continue;
    }
    record=record.substr(b);                           /* cut heading blanks */
    /* suppress trailing blanks */
    e=record.find_first_of(" \t\n");                /* first blank character */
    if(e!=record.npos)record=record.substr(0,e);      /* cut trailing blanks */
    /* skip illegal hostnames */
    for(allowed=true,j=0;j<record.size();j++)    /* loop over hostname chars */
    {
      if(!isupper(record.at(j)) && !islower(record.at(j)) && 
         !isdigit(record.at(j)) && record.at(j)!='-' && record.at(j)!='.')
      {
        allowed=false;
        break;
      }
    }                                            /* loop over hostname chars */
    if(!allowed)                                         /* illegal hostname */
    {
      mPrintf(errU,ERROR,METHOD,0,"Configuration file: \"%s\": record # %d: "
              "\"%s\" discarded (illegal hostname)!",confFileName.c_str(),
              i+1,record.c_str());
      continue;
    }
    /* skip duplicate hostnames */
    if(findNode(record))
    {
      mPrintf(errU,ERROR,METHOD,0,"Configuration file: \"%s\": record # %d: "
              "\"%s\" discarded (duplicated hostname)!",confFileName.c_str(),
              i+1,record.c_str());
    }
    else
    {
      mPrintf(errU,INFO,METHOD,0,"Got hostname: \"%s\".",record.c_str());
      addNode(record);
    }
  }                                                /* loop over file records */
  confFs.close();
  mPrintf(errU,DEBUG,METHOD,0,"Configuration read from file: \"%s\".",
          confFileName.c_str());
}
/*****************************************************************************/
void PcCluster::readInit(string initFileName)
{
  int i;
  string record;
  size_t c,b;
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,DEBUG,METHOD,0,"Trying to read Initialization file: \"%s\"...",
          initFileName.c_str());
  /* check initialization file accessibility */
  if(access(initFileName.c_str(),R_OK)==-1)
  {
    mPrintf(errU,FATAL,METHOD,0,"Initialization file: \"%s\" not accessible. "
            "access(2) error: %s!",initFileName.c_str(),strerror(errno));
    exit(1);
  }
  /* parse file */
  ifstream initFs(initFileName.c_str(),ifstream::in);
  for(i=0;;i++)                                    /* loop over file records */
  {
    getline(initFs,record);
    if(initFs.eof())break;
    if(initFs.fail())
    {
      mPrintf(errU,FATAL,METHOD,0,"Failure in reading the Initialization "
              "file: \"%s\"!",initFileName.c_str());
      exit(1);
    }
    /* suppress comments */
    c=record.find_first_of("#");                       /* start comment char */
    if(c!=record.npos)record=record.substr(0,c);              /* cut comment */
    /* suppress heading blanks */
    b=record.find_first_not_of(" \t");          /* first non-blank character */
    if(b==record.npos)           /* no chars after blanks and before comment */
    {
      if(deBug&0x20)
      {
        mPrintf(errU,VERB,METHOD,0,"Init file record %d skipped (blank "
                "record or comment record).",i+1);
      }
      continue;
    }
    record=record.substr(b);                           /* cut heading blanks */
    record="pcAdd "+record;
    mPrintf(errU,INFO,METHOD,0,"Got init command: \"%s\".", record.c_str());
    addSchPs(record,false);
  }
  initFs.close();
  mPrintf(errU,DEBUG,METHOD,0,"Initialization read from file: \"%s\".",
          initFileName.c_str());
}
/*****************************************************************************/
/* print methods                                                             */
/*****************************************************************************/
/* Print the list of processes scheduled on the nodes of this Cluster        */
void PcCluster::printSchPs(int sev,string head1,string sep1,string trail1,
                           string head2,string sep2,string trail2)
{
  vector<PcNode>::iterator ndIt;
  list<PcProcess> schPss;
  list<PcProcess>::iterator psIt;
  string s;
  /*-------------------------------------------------------------------------*/
  s=head1;
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)/* loop over cluster nodes */
  {
    if(ndIt!=nodes.begin())s+=sep1;
    s+=ndIt->getName();
    s+=head2;
    schPss=ndIt->getSchPss();    /* makes a copy of schPss list out of mutex */
    for(psIt=schPss.begin();psIt!=schPss.end();psIt++)
    {
      if(psIt!=schPss.begin())s+=sep2;
      s+=psIt->getUtgid();
    }
    s+=trail2;
  }
  s+=trail1;
  mPrintf(errU,sev,METHOD,0,"%s",s.c_str());
}
/*****************************************************************************/
/* Print the list of the nodes for this Cluster                              */
void PcCluster::printNodes(int sev,string head,string separator,string trail)
{
  vector<PcNode>::iterator ndIt;
  string s;
  /*-------------------------------------------------------------------------*/
  s=head;
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)/* loop over cluster nodes */
  {
    if(ndIt!=nodes.begin())s+=separator;
    s+=ndIt->getName();
  }
  s+=trail;
  mPrintf(errU,sev,METHOD,0,"%s",s.c_str());
}
/*****************************************************************************/
/* Print the list of the processes running on node node */
void PcCluster::printRunPss(string &node, int sev)
{
  vector<PcNode>::iterator ndIt;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  /* loop over the nodes of the cluster */
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    if(ndIt->getName()==node)
    {
      found=true;
      ndIt->printRunPss(sev);
      break;
    }
  }
  if(!found)
  {
     mPrintf(errU,WARN,METHOD,0,"Node \"%s\" not found!",node.c_str());
  }
}
/*****************************************************************************/
void PcCluster::printUpdateCnt(int sev)
{
  vector<PcNode>::iterator ndIt;
  unsigned long totalUpdateCnt=0;
  string s;
  /*-------------------------------------------------------------------------*/
  /* loop over the nodes of the cluster */
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    totalUpdateCnt+=ndIt->getUpdateCnt();
  }
  s+=FmcUtils::ultos(totalUpdateCnt)+" total updates (";
  for(ndIt=nodes.begin();ndIt!=nodes.end();ndIt++)
  {
    if(ndIt!=nodes.begin())s+="; ";
    s+=ndIt->getName()+": "+FmcUtils::ultos(ndIt->getUpdateCnt());
  }
  s+=").";
  mPrintf(errU,sev,METHOD,0,"%s",s.c_str());
}
/* ######################################################################### */
