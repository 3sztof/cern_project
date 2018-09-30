/* ######################################################################### */
/*
 * $Log: LogSrvSeeker.C,v $
 * Revision 1.7  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.6  2009/02/26 21:03:19  galli
 * bug fixed
 *
 * Revision 1.5  2009/02/26 20:51:28  galli
 * Handles server error condition
 *
 * Revision 1.4  2009/02/12 15:00:05  galli
 * Un-subscrive Log SVCs when they become unavailable
 *
 * Revision 1.3  2009/02/11 14:15:59  galli
 * Use <vector> for hostPttnLs, logPttnLs and svcPttnLs
 *
 * Revision 1.1  2009/02/09 16:28:23  galli
 * Initial revision
 */
/* ######################################################################### */
#include "LogSrvSeeker.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int severityThreshold;
extern int color;
extern int lStd;
extern int doPrintLast;
extern int deBug;
extern int errU;
/* ######################################################################### */
/* called when a server is added or removed to/from DIM DNS */
void LogSrvSeeker::infoHandler()
{
  char *msg;
  char status;
  string svc,node,logger;
  list<LogWriter*>::iterator it;
  /* ----------------------------------------------------------------------- */
  msg=getString();
  switch(*msg)
  {
    /* ..................................................................... */
    case '+':                                     /* server added to DIM DNS */
      getService(++msg,svc,node);
      /* select DIM services which match the given logger SVC pattern */
      if(!matchSvcPattern(svc))
      {
        if(unlikely(deBug & 0x4))
        {
          mPrintf(lStd,DEBUG,METHOD,9,"+ Node: \"%s\"; SVC: \"%s\" matches.",
                  node.c_str(),svc.c_str());
        }
        addLogSrv(svc,node);
      }                                         /* if(!matchSvcPattern(svc)) */
      break;
    /* ..................................................................... */
    case '-':                                 /* server removed from DIM DNS */
    case '!':                                             /* server in error */
      status=*msg;
      getService(++msg,svc,node);
      /* select DIM services which match the given logger SVC pattern */
      if(!matchSvcPattern(svc))
      {
        if(unlikely(deBug & 0x4))
        {
          mPrintf(lStd,DEBUG,METHOD,9,"- Node: \"%s\"; SVC: \"%s\" matches.",
                  node.c_str(),svc.c_str());
        }
        rmLogSrv(svc,node,status);
      }
      break;
    /* ..................................................................... */
    default:    /* server list at "DIS_DNS/SERVER_LIST" service subscription */
      if(msg!=(char*)-1)            /* service DIS_DNS/SERVER_LIST reachable */
      {
        char *at,*p=msg,*last=msg;
        while(last!=0 && (at=strchr(p,'@'))!=0)     /* loop over SVC records */
        {
          last=strchr(at,'|');
          if(last)*last=0;
          getService(p,svc,node);
          /* select DIM services which match the given logger SVC pattern */
          if(!matchSvcPattern(svc))
          {
            if(unlikely(deBug & 0x4))
            {
              mPrintf(lStd,DEBUG,METHOD,9,"I Node: \"%s\", SVC: \"%s\" "
                      "matches.",node.c_str(),svc.c_str());
            }
            addLogSrv(svc,node);
          }                                     /* if(!matchSvcPattern(svc)) */
          p=last+1;
        }                       /* loop over DIS_DNS/SERVER_LIST SVC records */
      }                          /* if service DIS_DNS/SERVER_LIST reachable */
      else
      {
        mPrintf(lStd,ERROR,METHOD,0,"DIM DNS unreachable");
      }
      break;
    /* ..................................................................... */
  }                                                          /* switch(*msg) */
  if(unlikely(deBug & 0x4))
  {
    for(it=loggerList.begin();it!=loggerList.end();it++)
    {
      printf("=>%s\n",(*it)->getSvcPath());
    }
    mPrintf(lStd,INFO,METHOD,0,"==>\"%s\"\n",getString());
  }
}
/*****************************************************************************/
void LogSrvSeeker::addLogSrv(const string &svc, const string &node)
{
  string logSvcPath=svc+"/log";
  string propsSvcPath=svc+"/get_properties";
  list<LogWriter*>::iterator lIt;
  list<LogSettingsWatcher*>::iterator wIt;
  vector<string>::iterator knIt;
  bool logFound=false;
  bool watchFound=false;
  bool known=false;
  /* ----------------------------------------------------------------------- */
  /* chech if SVC is known */
  for(knIt=knwnLogSrvNames.begin();knIt!=knwnLogSrvNames.end();knIt++)
  {
    if(svc==*knIt)
    {
      known=true;
      break;
    }
  }
  if(!known)knwnLogSrvNames.push_back(svc);
  /* ----------------------------------------------------------------------- */
  /* check if the SVC logSvcPath is already subscribed */
  for(lIt=loggerList.begin();lIt!=loggerList.end();lIt++)
  {
    if((*lIt)->getSvcPath()==logSvcPath)
    {
      logFound=true;
      break;
    }
  }
  if(doPrintLast)
  {
    /* check if the SVC propsSvcPath is already subscribed */
    for(wIt=watcherList.begin();wIt!=watcherList.end();wIt++)
    {
      if((*wIt)->getSvcPath()==propsSvcPath)
      {
        watchFound=true;
        break;
      }
    }
  }
  /* ----------------------------------------------------------------------- */
  /* if not, subscribe to DIM SVC /FMC/<HOST>/logger/<LOGGER>/log */
  if(!logFound)
  {
    loggerList.push_front(new LogWriter(logSvcPath.c_str(),severityThreshold,
                                        color));
  }
  if(doPrintLast)                             /* get the last message buffer */
  {
    if(!watchFound)
    {
      /* if not, subscribe to get_properties DIM SVC in order to be notified */
      /* if server settings change and in this case print last_log           */
      watcherList.push_front(new LogSettingsWatcher(propsSvcPath.c_str()));
    }
  }                                                       /* if(doPrintLast) */
  /* ----------------------------------------------------------------------- */
  if(!logFound)
  {
    string logger=getLoggerName(svc);
    if(!known)
    {
      mPrintf(lStd,INFO,METHOD,0,"Start getting messages from logger \"%s\" "
              "on node \"%s\".",logger.c_str(),node.c_str());
    }
    else
    {
      mPrintf(lStd,WARN,METHOD,0,"RE-start getting messages from logger "
              "\"%s\" on node \"%s\".",logger.c_str(),node.c_str());
    }
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
void LogSrvSeeker::rmLogSrv(const string &svc, const string &node, char status)
{
  string logSvcPath=svc+"/log";
  string propsSvcPath=svc+"/get_properties";
  list<LogWriter*>::iterator lIt;
  list<LogSettingsWatcher*>::iterator wIt;
  bool logFound=false;
  bool watchFound=false;
  /* ----------------------------------------------------------------------- */
  /* if the SVC logSvcPath is subscribed then unsubscribe */
  for(lIt=loggerList.begin();lIt!=loggerList.end();lIt++)
  {
    if((*lIt)->getSvcPath()==logSvcPath)
    {
      logFound=true;
      delete *lIt;                              /* remove LogWriter object */
      loggerList.erase(lIt);  /* remove pointer to LogWriter obj from list */
      break;
    }
  }
  if(doPrintLast)
  {
    /* if the SVC propsSvcPath is subscribed then unsubscribe */
    for(wIt=watcherList.begin();wIt!=watcherList.end();wIt++)
    {
      if((*wIt)->getSvcPath()==propsSvcPath)
      {
        watchFound=true;
        delete *wIt;                     /* remove LogSettingsWatcher object */
        watcherList.erase(wIt); /* rm pt to LogSettingsWatcher obj from list */
        break;
      }
    }
  }
  /* ----------------------------------------------------------------------- */
  string logger=getLoggerName(svc);
  if(status=='!')
  {
    mPrintf(lStd,WARN,METHOD,0,"Stop getting messages from logger \"%s\" "
            "on node \"%s\" (in error).",logger.c_str(),node.c_str());
  }
  else if(status=='-')
  {
    mPrintf(lStd,WARN,METHOD,0,"Stop getting messages from logger \"%s\" "
            "on node \"%s\" (removed).",logger.c_str(),node.c_str());
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
/* svc@node ==> svc, node */
/* E.g.: input:   s = "/FMC/MYHOST/logger/fmc@myhost.mydomain"               */
/*       output:  svc = "/FMC/MYHOST/logger/fmc"   node = "myhost.mydomain"  */
void LogSrvSeeker::getService(char* s,string& svc,string& node)
{
  char* at=strchr(s,'@');
  *at=0;
  svc=s;
  node=at+1;
  if(unlikely(deBug & 0x4))
  {
    mPrintf(lStd,VERB,METHOD,0,"Node: \"%s\"; SVC: \"%s\".",node.c_str(),
            svc.c_str());
  }
  return;
}
/*****************************************************************************/
string LogSrvSeeker::getLoggerName(const string& svc)
{
  string logger=svc.substr(1+svc.rfind('/'));
  return logger;
}
/*****************************************************************************/
int LogSrvSeeker::matchSvcPattern(string svc)
{
  vector<string>::iterator svIt;          /* service pattern vector iterator */
  string ssvc(svc);
  /* ----------------------------------------------------------------------- */
  ssvc+="/log";
  /* compare against a generic logger pattern */
  if(fnmatch(SVC_HEAD"/*/logger/*/log",ssvc.c_str(),0)==FNM_NOMATCH)
  {
    return FNM_NOMATCH;
  }
  /* compare against logger pattern selected using cmd-line */
  for(svIt=svcPttnLs.begin();svIt!=svcPttnLs.end();svIt++)
  {
    if(!fnmatch(svIt->c_str(),ssvc.c_str(),0))return 0;
  }
  return FNM_NOMATCH;
} 
/*****************************************************************************/
/* constructor */
LogSrvSeeker::LogSrvSeeker(const char* dnsSrvLsSvcPath,
                           vector<string>& svcPttnLs)
:DimInfo(dnsSrvLsSvcPath,-1), svcPttnLs(svcPttnLs)
{
}
/* ######################################################################### */
