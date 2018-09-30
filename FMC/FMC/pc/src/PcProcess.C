/* ######################################################################### */
/*
 * $Log: PcProcess.C,v $
 * Revision 1.14  2012/12/04 11:24:10  galli
 * Support for oomScoreAdj
 *
 * Revision 1.13  2008/09/29 14:48:17  galli
 * allows multiple parameter expansions on the same string
 *
 * Revision 1.12  2008/09/26 20:22:13  galli
 * added method cleanRcDisTime()
 * modified method setRcEnabled()
 * modified method setRcDisabled()
 * added method checkRc()
 * added methods void parExpand(string&) and void parExpand()
 *
 * Revision 1.11  2008/09/24 09:05:17  galli
 * setRcEnTime() and setRcDisTime() take no argument
 *
 * Revision 1.10  2008/09/23 15:27:45  galli
 * removed method getStartN() and added method getRcStartTimeN()
 * added methods: addRcStartTime(), cleanRcStartTime() and printRcStartTimes()
 * method addStartTime() requires no arguments
 *
 * Revision 1.9  2008/09/23 07:40:08  galli
 * added mutex to protect variable member access
 * added destructor
 *
 * Revision 1.8  2008/09/19 07:35:29  galli
 * added copy constructor
 *
 * Revision 1.7  2008/09/18 08:06:53  galli
 * fixed format in suseconds_t printf
 *
 * Revision 1.6  2008/09/17 13:49:45  galli
 * startTimes made of struct timeval (instead of time_t)
 * to have better time resolution (1 s -> 0.000001 s)
 *
 * Revision 1.5  2008/09/16 09:24:44  galli
 * renamed list<string>getStartTimesStr() into list<string>getStartTimesStrLst()
 * added method string getStartTimesStr(string sep)
 *
 * Revision 1.4  2008/09/16 06:55:27  galli
 * added method getEnvs()
 * bug fix in method getArgs()
 * added operator= overload (deep copy) to be used in PcNode:getSchPss()
 *
 * Revision 1.3  2008/09/15 07:47:51  galli
 * included directives moved to PcProcess.IC
 *
 * Revision 1.1  2008/09/08 10:22:35  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcProcess.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern size_t startTimeMaxSize;
extern int errU;
extern int deBug;
/* ######################################################################### */
/* get methods                                                               */
/*****************************************************************************/
string PcProcess::getEnv(string envName)
{
  list<string>::iterator sIt;
  string foundEnvName;
  string envVal;
  size_t sep;
  /*-------------------------------------------------------------------------*/
  for(sIt=envp.begin();sIt!=envp.end();sIt++)
  {
    sep=sIt->find('=');
    foundEnvName=sIt->substr(0,sep);
    if(foundEnvName==envName)
    {
      envVal=sIt->substr(sep+1);
      break;
    }
  }
  return envVal;
}
/*****************************************************************************/
string PcProcess::getEnvs(string sep)
{
  list<string>::iterator sIt;
  string s;
  /*-------------------------------------------------------------------------*/
  for(sIt=envp.begin();sIt!=envp.end();sIt++)
  {
    if(sIt!=envp.begin())s+=sep;
    s+=*sIt;
  }
  return s;
}
/*****************************************************************************/
string PcProcess::getStartTimesStr(string sep)
{
  list<struct timeval>::iterator stIt;
  struct timeval t;
  struct tm tl;
  char ts[22];                                    /* Sep17-14:25:18.589805\0 */
  string s;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  for(stIt=startTimes.begin();stIt!=startTimes.end();stIt++)
  {
    if(stIt!=startTimes.begin())s+=sep;
    t=*stIt;
    localtime_r(&t.tv_sec,&tl);
    strftime(ts,15,"%b%d-%H:%M:%S",&tl);                 /* Sep17-14:25:18\0 */
    snprintf(ts+14,8,".%6.6lu",t.tv_usec);                      /* .589805\0 */
    s+=string(ts);
  }
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  return s;
}
/*****************************************************************************/
list<string>PcProcess::getStartTimesStrLst()
{
  list<string>startTimesStr;
  list<struct timeval>::iterator stIt;
  struct timeval t;
  struct tm tl;
  char ts[22];                                    /* Sep17-14:25:18.589805\0 */
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  for(stIt=startTimes.begin();stIt!=startTimes.end();stIt++)
  {
    t=*stIt;
    localtime_r(&t.tv_sec,&tl);
    strftime(ts,15,"%b%d-%H:%M:%S",&tl);                 /* Sep17-14:25:18\0 */
    snprintf(ts+14,8,".%6.6lu",t.tv_usec);                      /* .589805\0 */
    startTimesStr.push_back(ts);
  }
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  return startTimesStr;
}
/*****************************************************************************/
string PcProcess::getArgs()
{
  string args;
  vector<string>::iterator sIt;
  /*-------------------------------------------------------------------------*/
  for(sIt=argv.begin();sIt!=argv.end();sIt++)
  {
    if(sIt>argv.begin())args+=" ";
    /* quote empty arg or arg including blanks */
    if(sIt->find_first_of(" \t")!=sIt->npos || *sIt=="")
      args+=string("\"")+sIt->c_str()+string("\"");
    else
      args+=*sIt;
  }
  return args;
}
/*****************************************************************************/
bool PcProcess::isFirstStart()
{
  bool t;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  t=firstStart;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  return t;
}
/*****************************************************************************/
time_t PcProcess::getRcEnTime()
{
  time_t t;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  t=rcEnTime;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  return t;
}
/*****************************************************************************/
time_t PcProcess::getRcDisTime()
{
  time_t t;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  t=rcDisTime;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  return t;
}
/*****************************************************************************/
bool PcProcess::isRcEnabled()
{
  bool t;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  t=(rcDisTime==0);
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  return t;
}
/*****************************************************************************/
unsigned PcProcess::getStartCnt()
{
  unsigned n;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  n=startCnt;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  return n;
}
/*****************************************************************************/
list<struct timeval> PcProcess::getStartTimes()
{
  list<struct timeval>l;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  l=startTimes;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  return l;
}
/*****************************************************************************/
/* return the number of (re)starts in the last checkPeriod seconds */
int PcProcess::getRcStartTimeN()
{
  
  list<time_t>::iterator tIt;
  time_t startChkTime;
  int sz;
  /*-------------------------------------------------------------------------*/
  /* suppress start times older than checkPeriod seconds */
  startChkTime=time(NULL)-checkPeriod;
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  for(tIt=rcStartTimes.begin();tIt!=rcStartTimes.end();)
  {
    if(*tIt<startChkTime)
    {
       tIt=rcStartTimes.erase(tIt);  /* remove list el. and advance iterator */
    }
    else
    {
      tIt++;                                             /* advance iterator */
    }
  }
  /* count the start time number */
  sz=rcStartTimes.size();
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  return sz;
}
/*****************************************************************************/
/* set methods                                                               */
/*****************************************************************************/
void PcProcess::setArgv(char **argv)
{
  char **p;
  /*-------------------------------------------------------------------------*/
  for(p=argv;*p;p++)
  {
    this->argv.push_back(*p);
  }
}
/*****************************************************************************/
void PcProcess::setArgs(string args)
{
  vector<string>argv;
  size_t b,e;
  string s;
  /*-------------------------------------------------------------------------*/
  for(b=0;;)
  {
    b=args.find_first_not_of(" \n\t",b);              /* skip initial blanks */
    if(b==args.npos)break;                        /* end of arg list reached */
    if(args.at(b)=='\"')                      /* arg start with double quote */
    {
      e=args.find_first_of("\"",b+1); /* arg terminates at next double quote */
      if(e==args.npos)e=args.size();      /* matching double quote not found */
      else e++;                   /* first blank after matching double quote */
      s=args.substr(b+1,e-b-2);     /* extract arg (excluding double quotes) */
      argv.push_back(s);
    }
    else if(args.at(b)=='\'')                 /* arg start with single quote */
    {
      e=args.find_first_of("\'",b+1); /* arg terminates at next single quote */
      if(e==args.npos)e=args.size();      /* matching single quote not found */
      else e++;                   /* first blank after matching single quote */
      s=args.substr(b+1,e-b-2);     /* extract arg (excluding single quotes) */
      argv.push_back(s);
    }
    else                                                     /* unquoted arg */
    {
      e=args.find_first_of(" \n\t",b);  /* first blank after end of argument */
      if(e==args.npos)e=args.size();                   /* end of args string */
      s=args.substr(b,e-b);                                   /* extract arg */
      argv.push_back(s);
    }
    if(e==args.size())break;                           /* end of args string */
    b=e;                       /* move to the first blank after end argument */
  }
  this->argv=argv;
}
/*****************************************************************************/
void PcProcess::setArgs(char *args)
{
  PcProcess::setArgs(string(args));
}
/*****************************************************************************/
void PcProcess::putEnv(string envLine)
{
  string envName,envVal;
  list<string>::iterator sIt;
  string foundEnvName;
  size_t sep;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  /* separate envName and envVal */
  sep=envLine.find('=');
  envName=envLine.substr(0,sep);
  envVal=envLine.substr(sep+1,envLine.size()-sep);
  /* replace old variable if found */
  for(sIt=envp.begin();sIt!=envp.end();sIt++)
  {
    sep=sIt->find('=');
    foundEnvName=sIt->substr(0,sep);
    if(foundEnvName==envName)
    {
      sIt->replace(sep+1,sIt->size()-sep,envVal);
      found=true;
      break;
    }
  }
  /* if old variable not found then set new variable */
  if(!found)envp.push_back(envLine);
}
/*****************************************************************************/
void PcProcess::setEnv(string envName,string envVal)
{
  list<string>::iterator sIt;
  string foundEnvName;
  size_t sep;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  /* replace old variable if found */
  for(sIt=envp.begin();sIt!=envp.end();sIt++)
  {
    sep=sIt->find('=');
    foundEnvName=sIt->substr(0,sep);
    if(foundEnvName==envName)
    {
      sIt->replace(sep+1,sIt->size()-sep,envVal);
      found=true;
      break;
    }
  }
  /* if old variable not found then set new variable */
  if(!found)envp.push_back(envName+string("=")+envVal);
}
/*****************************************************************************/
void PcProcess::unsetEnv(string envName)
{
  list<string>::iterator sIt;
  string foundEnvName;
  size_t sep;
  /*-------------------------------------------------------------------------*/
  for(sIt=envp.begin();sIt!=envp.end();sIt++)
  {
    sep=sIt->find('=');
    foundEnvName=sIt->substr(0,sep);
    if(foundEnvName==envName)
    {
      envp.erase(sIt);
      break;
    }
  }
}
/*****************************************************************************/
void PcProcess::clearEnv()
{
  envp.clear();
}
/*****************************************************************************/
void PcProcess::addCpu(unsigned cpuN)
{
  if(affMask==~0UL)affMask=0UL;
  affMask|=1UL<<cpuN;
}
/*****************************************************************************/
void PcProcess::rmCpu(unsigned cpuN)
{
  affMask&=~(1UL<<cpuN);
  if(affMask==0UL)affMask=~0UL;
}
/*****************************************************************************/
void PcProcess::addStartTime()
{
  struct timeval t;
  gettimeofday(&t,NULL);
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  /* add new start time */
  startTimes.push_back(t);                                 /* add at the end */
  /* increment start counter */
  startCnt++;
  /* if list exceeds maximum size, then trim it */
  while(startTimes.size()>startTimeMaxSize)startTimes.pop_front();
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
void PcProcess::addRcStartTime()
{
  list<time_t>::iterator tIt;
  time_t startChkTime;
  /*-------------------------------------------------------------------------*/
  /* suppress start times older than checkPeriod seconds */
  startChkTime=time(NULL)-checkPeriod;
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  for(tIt=rcStartTimes.begin();tIt!=rcStartTimes.end();)
  {
    if(*tIt<startChkTime)
    {
       tIt=rcStartTimes.erase(tIt);  /* remove list el. and advance iterator */
    }
    else
    {
      tIt++;                                             /* advance iterator */
    }
  }
  /* add new start time */
  rcStartTimes.push_back(time(NULL));                      /* add at the end */
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
void PcProcess::cleanRcStartTimes()
{
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  rcStartTimes.clear();
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
void PcProcess::setFirstStart(bool t)
{
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  firstStart=t;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
void PcProcess::setRcEnTime()
{
  time_t t;
  /*-------------------------------------------------------------------------*/
  t=time(NULL);
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  rcEnTime=t;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
void PcProcess::setRcDisTime()
{
  time_t t;
  /*-------------------------------------------------------------------------*/
  t=time(NULL);
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  rcDisTime=t;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
void PcProcess::cleanRcDisTime()
{
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  rcDisTime=0;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
void PcProcess::setRcEnabled()
{
  mPrintf(errU,WARN,METHOD,0,"Process: UTGID=\"%s\" enabled again on node "
          "\"%s\" after %ld seconds.", utgid.c_str(), node.c_str(), 
          time(NULL)-getRcDisTime());
  cleanRcDisTime();
  setRcEnTime();
}
/*****************************************************************************/
void PcProcess::setRcDisabled()
{
  if(disPeriod==-1)
  {
    mPrintf(errU,FATAL,METHOD,0,"Process UTGID=\"%s\" respawning too fast on "
            "node \"%s\" (more than %d times in %ld seconds). Disabled "
            "forever!", utgid.c_str(), node.c_str(), getRcStartTimeN(),
            MIN(time(NULL)-getRcEnTime(),getCheckPeriod()));
  }
  else
  {
    mPrintf(errU,ERROR,METHOD,0,"Process UTGID=\"%s\" respawning too fast on "
            "node \"%s\" (more than %d times in %ld seconds). Disabled "
            "for %ld seconds!", utgid.c_str(), node.c_str(), getRcStartTimeN(),
            MIN(time(NULL)-getRcEnTime(),checkPeriod),disPeriod);
  }
  setRcDisTime();
  cleanRcStartTimes();
}
/*****************************************************************************/
void PcProcess::setStartCnt(unsigned startCnt)
{
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  this->startCnt=startCnt;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
void PcProcess::setStartTimes(list<struct timeval>startTimes)
{
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  this->startTimes=startTimes;
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
/* check methods                                                             */
/*****************************************************************************/
void PcProcess::checkRc()
{
  if(isRcEnabled())                                       /* respawn enabled */
  {
    if(maxStartN!=-1 &&                      /* respawn control not excluded */
       getRcStartTimeN()>=maxStartN)                     /* too fast respawn */
    {
      setRcDisabled();
    }                                                    /* too fast respawn */
  }                                                       /* respawn enabled */
  else                                                   /* respawn disabled */
  {
    if(disPeriod!=-1 &&                               /* re-enabling allowed */
       time(NULL)-getRcDisTime()>=disPeriod)     /* disabling period expired */
    {
      setRcEnabled();
    }
  }                                                      /* respawn disabled */
}
/*****************************************************************************/
/* expansion methods                                                         */
/*****************************************************************************/
/* Parameter expansion ${RUN_NODE} -> process running hostname in string s   */
void PcProcess::parExpand(string &s)
{
  size_t b,l;
  /*-------------------------------------------------------------------------*/
  l=strlen("${RUN_NODE}");
  for(;;)
  {
    b=s.find("${RUN_NODE}");
    if(b==string::npos)break;
    s=s.replace(b,l,node);
  }
}
/*****************************************************************************/
/* Parameter expansion ${RUN_NODE} -> process running hostname               */
/* in utgid, path, argv, wd, envp, outFIFO, errFIFO                          */
void PcProcess::parExpand()
{
  vector<string>::iterator saIt;
  list<string>::iterator seIt;
  /*-------------------------------------------------------------------------*/
  parExpand(utgid);
  parExpand(path);
  for(saIt=argv.begin();saIt!=argv.end();saIt++)
  {
    parExpand(*saIt);
  }
  parExpand(wd);
  for(seIt=envp.begin();seIt!=envp.end();seIt++)
  {
    parExpand(*seIt);
  }
  parExpand(outFIFO);
  parExpand(errFIFO);
}
/*****************************************************************************/
/* print methods                                                             */
/*****************************************************************************/
void PcProcess::printArg(string head,string separator,string trail)
{
  vector<string>::iterator sIt;
  /*-------------------------------------------------------------------------*/
  printf("%s",head.c_str());
  for(sIt=argv.begin();sIt!=argv.end();sIt++)
  {
    if(sIt>argv.begin())printf("%s",separator.c_str());
    printf("%s",sIt->c_str());
  }
  printf("%s",trail.c_str());
}
/*****************************************************************************/
void PcProcess::printEnv(string head,string separator,string trail)
{
  list<string>::iterator sIt;
  /*-------------------------------------------------------------------------*/
  printf("%s",head.c_str());
  for(sIt=envp.begin();sIt!=envp.end();sIt++)
  {
    if(sIt!=envp.begin())printf("%s",separator.c_str());
    printf("%s",sIt->c_str());
  }
  printf("%s",trail.c_str());
}
/*****************************************************************************/
void PcProcess::printCpu(string head,string separator,string trail)
{
  unsigned i,j;
  /*-------------------------------------------------------------------------*/
  printf("%s",head.c_str());
  for(i=0,j=0;i<8*sizeof(unsigned long);i++)
  {
    if(affMask&(1UL<<i))
    {
      if(j++)printf("%s",separator.c_str());
      printf("%d",i);
    }
  }
  printf("%s",trail.c_str());
}
/*****************************************************************************/
void PcProcess::printStartTimes(string head,string separator,string trail)
{
  list<struct timeval>::iterator stIt;
  struct timeval t;
  struct tm tl;
  char ts[22];                                    /* Sep17-14:25:18.589805\0 */
  /*-------------------------------------------------------------------------*/
  printf("%s",head.c_str());
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  for(stIt=startTimes.begin();stIt!=startTimes.end();stIt++)
  {
    t=*stIt;
    localtime_r(&t.tv_sec,&tl);
    strftime(ts,15,"%b%d-%H:%M:%S",&tl);                 /* Sep17-14:25:18\0 */
    snprintf(ts+14,8,".%6.6lu",t.tv_usec);                      /* .589805\0 */
    if(stIt!=startTimes.begin())printf("%s",separator.c_str());
    printf("%s",ts);
  }
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  printf("%s",trail.c_str());
}
/*****************************************************************************/
void PcProcess::printRcStartTimes(int sev)
{
  list<time_t>::iterator tIt;
  time_t startChkTime;
  time_t t;
  struct tm tl;
  char ts[15];                                           /* Sep17-14:25:18\0 */
  string s;
  /*-------------------------------------------------------------------------*/
  s="Process UTGID=\""+utgid+"\", start times in the last "+
    FmcUtils::ltos(checkPeriod)+" seconds: ";
  /* suppress start times older than checkPeriod seconds */
  startChkTime=time(NULL)-checkPeriod;
  if(pthread_mutex_lock(&lk))eExit("pthread_mutex_lock()");
  for(tIt=rcStartTimes.begin();tIt!=rcStartTimes.end();)
  {
    if(*tIt<startChkTime)
    {
       tIt=rcStartTimes.erase(tIt);  /* remove list el. and advance iterator */
    }
    else
    {
      tIt++;                                             /* advance iterator */
    }
  }
  for(tIt=rcStartTimes.begin();tIt!=rcStartTimes.end();tIt++)
  {
    if(tIt!=rcStartTimes.begin())s+=", ";
    t=*tIt;
    localtime_r(&t,&tl);
    strftime(ts,15,"%b%d-%H:%M:%S",&tl);                 /* Sep17-14:25:18\0 */
    s+=ts;
  }
  if(pthread_mutex_unlock(&lk))eExit("pthread_mutex_unlock()");
  s+=".";
  mPrintf(errU,sev,METHOD,0,"%s",s.c_str());
}
/*****************************************************************************/
/* operators overloads                                                       */
/*****************************************************************************/
PcProcess& PcProcess::operator=(const PcProcess& rhs)
{
  if(this!=&rhs)
  {
    utgid=rhs.utgid;                                               /* string */
    path=rhs.path;                                                 /* string */
    argv=rhs.argv;                                         /* vector<string> */
    wd=rhs.wd;                                                     /* string */
    cleanEnv=rhs.cleanEnv;
    envp=rhs.envp;                                           /* list<string> */
    daemon=rhs.daemon;
    scheduler=rhs.scheduler;
    nice=rhs.nice;
    rtPrio=rhs.rtPrio;
    oomScoreAdj=rhs.oomScoreAdj;
    affMask=rhs.affMask;
    user=rhs.user;                                                 /* string */
    group=rhs.group;                                               /* string */
    rdrStdout=rhs.rdrStdout;
    rdrStderr=rhs.rdrStderr;
    outFIFO=rhs.outFIFO;                                           /* string */
    errFIFO=rhs.errFIFO;                                           /* string */
    noDrop=rhs.noDrop;
    maxStartN=rhs.maxStartN;
    disPeriod=rhs.disPeriod;
    checkPeriod=rhs.checkPeriod;
    node=rhs.node;
    /*.......................................................................*/
    firstStart=rhs.firstStart;
    rcEnTime=rhs.rcEnTime;
    rcDisTime=rhs.rcDisTime;
    startCnt=rhs.startCnt;
    startTimes=rhs.startTimes;                       /* list<struct timeval> */
    rcStartTimes=rhs.rcStartTimes;                           /* list<time_t> */
    if(pthread_mutex_init(&lk,NULL))eExit("pthread_mutex_init()");
  }
  if(deBug&0x08)
  {
    mPrintf(errU,VERB,__PRETTY_FUNCTION__,0,"PcProcess assignment operator "
            "invoked.");
  }
  return *this;
}
/*****************************************************************************/
/* constructors                                                              */
/*****************************************************************************/
/* default constructor */
PcProcess::PcProcess(string utgid,string path,vector<string>argv,string wd,
                     int cleanEnv,list<string>envp,int daemon,int scheduler,
                     int nice,int rtPrio,int oomScoreAdj,unsigned long affMask,
                     string user,string group,int rdrStderr,int rdrStdout,
                     string outFIFO,string errFIFO,int noDrop,int maxStartN,
                     time_t disPeriod,time_t checkPeriod,string node)
{
  this->utgid=utgid;                                               /* string */
  this->path=path;                                                 /* string */
  this->argv=argv;                                         /* vector<string> */
  this->wd=wd;                                                     /* string */
  this->cleanEnv=cleanEnv;
  this->envp=envp;                                           /* list<string> */
  this->daemon=daemon;
  this->scheduler=scheduler;
  this->nice=nice;
  this->rtPrio=rtPrio;
  this->oomScoreAdj=oomScoreAdj;
  this->affMask=affMask;
  this->user=user;                                                 /* string */
  this->group=group;                                               /* string */
  this->rdrStderr=rdrStderr;
  this->rdrStdout=rdrStdout;
  this->outFIFO=outFIFO;                                           /* string */
  this->errFIFO=errFIFO;                                           /* string */
  this->noDrop=noDrop;
  this->maxStartN=maxStartN;
  this->disPeriod=disPeriod;
  this->checkPeriod=checkPeriod;
  this->node=node;
  /*.........................................................................*/
  this->firstStart=true;
  this->rcEnTime=0;
  this->rcDisTime=0;                                         /* i.e. enabled */
  this->startCnt=0;
  /* list<struct timeval> startTimes has default value of empty list */
  /* list<time_t> rcStartTimes has default value of empty list */
  if(pthread_mutex_init(&lk,NULL))eExit("pthread_mutex_init()");
  if(deBug&0x08)
  {
    mPrintf(errU,VERB,__PRETTY_FUNCTION__,0,"PcProcess constructor 1 "
            "invoked.");
  }
}
/*****************************************************************************/
/* constructor with argument string args instead of argument vector argv */
PcProcess::PcProcess(string utgid,string path,string args,string wd,
                     int cleanEnv,list<string>envp,int daemon,int scheduler,
                     int nice,int rtPrio,int oomScoreAdj,unsigned long affMask,
                     string user,string group,int rdrStderr,int rdrStdout,
                     string outFIFO,string errFIFO,int noDrop,int maxStartN,
                     time_t disPeriod,time_t checkPeriod,string node)
{
  vector<string>argv;
  size_t b,e;
  string s;
  /*-------------------------------------------------------------------------*/
  this->utgid=utgid;                                               /* string */
  this->path=path;                                                 /* string */
  this->wd=wd;                                                     /* string */
  this->cleanEnv=cleanEnv;
  this->envp=envp;                                           /* list<string> */
  this->daemon=daemon;
  this->scheduler=scheduler;
  this->nice=nice;
  this->rtPrio=rtPrio;
  this->oomScoreAdj=oomScoreAdj;
  this->affMask=affMask;
  this->user=user;                                                 /* string */
  this->group=group;                                               /* string */
  this->rdrStderr=rdrStderr;
  this->rdrStdout=rdrStdout;
  this->outFIFO=outFIFO;                                           /* string */
  this->errFIFO=errFIFO;                                           /* string */
  this->noDrop=noDrop;
  this->maxStartN=maxStartN;
  this->disPeriod=disPeriod;
  this->checkPeriod=checkPeriod;
  this->node=node;
  /*.........................................................................*/
  this->firstStart=true;
  this->rcEnTime=0;
  this->rcDisTime=0;                                         /* i.e. enabled */
  this->startCnt=0;
  /* list<struct timeval> startTimes has default value of empty list */
  /* list<time_t> rcStartTimes has default value of empty list */
  /*.........................................................................*/
  /* build the argument vector argv from the argument string args */
  for(b=0;;)
  {
    b=args.find_first_not_of(" \n\t",b);              /* skip initial blanks */
    if(b==args.npos)break;                             /* end of args string */
    e=args.find_first_of(" \n\t",b);    /* first blank after end of argument */
    if(e==args.npos)e=args.size();                     /* end of args string */
    s=args.substr(b,e-b);                                /* extract argument */
    argv.push_back(s);
    if(e==args.size())break;                           /* end of args string */
    b=e;                       /* move to the first blank after end argument */
  }
  this->argv=argv;
  /*.........................................................................*/
  if(pthread_mutex_init(&lk,NULL))eExit("pthread_mutex_init()");
  if(deBug&0x08)
  {
    mPrintf(errU,VERB,__PRETTY_FUNCTION__,0,"PcProcess constructor 2 "
            "invoked.");
  }
}
/*****************************************************************************/
/* copy constructor */
PcProcess::PcProcess(const PcProcess& rhs)
{
  *this=rhs;
  if(pthread_mutex_init(&lk,NULL))eExit("pthread_mutex_init()");
  if(deBug&0x08)
  {
    mPrintf(errU,VERB,__PRETTY_FUNCTION__,0,"PcProcess constructor 3 "
            "(copy constructor) invoked.");
  }
}
/*****************************************************************************/
/* destructor                                                                */
/*****************************************************************************/
PcProcess::~PcProcess()
{
  if(pthread_mutex_destroy(&lk))eExit("pthread_mutex_destroy()");
  if(deBug&0x08)
  {
    mPrintf(errU,VERB,__PRETTY_FUNCTION__,0,"PcProcess destructor invoked.");
  }
}
/* ######################################################################### */
