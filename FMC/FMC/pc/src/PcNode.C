/* ######################################################################### */
/*
 * $Log: PcNode.C,v $
 * Revision 1.15  2012/12/13 10:56:46  galli
 * bug fixed
 *
 * Revision 1.13  2012/12/04 11:23:47  galli
 * Support for oomScoreAdj
 *
 * Revision 1.12  2008/09/26 20:11:03  galli
 * method addSchPs(): added 2nd argument with default (bool startIt=true)
 * addSchPs() calls setNode() and parExpand()
 * startPs(PcProcess&) calls PcProcess::checkRc()
 * checkRunning() increments updateCnt
 * startPs(PcProcess&) do not start ps if !PcProcess::isRcEnabled()
 *
 * Revision 1.11  2008/09/24 09:00:24  galli
 * addSchPs() sets the time of process enabling
 * startPs(PcProcess &) calls PcProcess::addRcStartTime()
 *
 * Revision 1.10  2008/09/23 07:42:07  galli
 * start command to Task Manager is sent only if the Task Manager
 * service list is subscribed
 *
 * Revision 1.9  2008/09/22 07:58:01  galli
 * utils toUpper(), itos(), ltos() and ultosx() moved to FmcUtils.C
 * added methods isTmSubscibed() and setTmSubscibed()
 *
 * Revision 1.8  2008/09/19 21:18:53  galli
 * added methods: getSchUtgids(), getRunUtgids(), checkRunning().
 * changed method: printRunPss()
 * added tmSrvName member variable
 *
 * Revision 1.7  2008/09/17 15:34:31  galli
 * added method int PcNode::startPs(string utgid)
 * added method int PcNode::stopPs(utgid,signo,s9delay)
 * PcNode::rmSchPs() calls PcNode::stopPs()
 * added utility function utos()
 *
 * Revision 1.6  2008/09/17 13:44:11  galli
 * gets acl as global
 * PcNode::setName() set also task manager SVCs/CMDs
 * added method startPs()
 * added utilities toUpper(), itos(), ltos() and ultosx()
 * modified constructors/destructor
 *
 * Revision 1.5  2008/09/16 06:49:16  galli
 * method getSchPss() makes a list duplicate to take the process list
 * out of mutex barrier
 *
 * Revision 1.4  2008/09/15 07:50:39  galli
 * included directives moved to PcNode.IC
 * changed addSchPs() and rmSchPs() methods
 *
 * Revision 1.3  2008/09/12 09:33:26  galli
 * addSchPs() check for duplicate UTGID
 *
 * Revision 1.1  2008/09/08 13:58:33  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcNode.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
extern PcAcl acl;
/* ######################################################################### */
/* get methods */
/*****************************************************************************/
list<PcProcess> &PcNode::getSchPss()
{
  if(pthread_mutex_lock(&schLk))eExit("pthread_mutex_lock()");
  /* make a deep copy of the list, to get it out of the mutex barrier */
  schPssCp=schPss;
  if(pthread_mutex_unlock(&schLk))eExit("pthread_mutex_unlock()");
  return schPssCp;
}
/*****************************************************************************/
vector<string> PcNode::getSchUtgids()
{
  vector<string> utgids;
  list<PcProcess>::iterator psIt;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&schLk))eExit("pthread_mutex_lock()");
  for(psIt=schPss.begin();psIt!=schPss.end();psIt++)
  {
    utgids.push_back(psIt->getUtgid());
  }
  if(pthread_mutex_unlock(&schLk))eExit("pthread_mutex_unlock()");
  return utgids;
}
/*****************************************************************************/
vector<string> PcNode::getRunUtgids()
{
  vector<string> utgids;
  vector<string>::iterator psIt;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&runLk))eExit("pthread_mutex_lock()");
  for(psIt=runPss.begin();psIt!=runPss.end();psIt++)
  {
    utgids.push_back(*psIt);
  }
  if(pthread_mutex_unlock(&runLk))eExit("pthread_mutex_unlock()");
  return utgids;
}
/*****************************************************************************/
bool PcNode::isTmSubscribed()
{
  bool t;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&runLk))eExit("pthread_mutex_lock()");
  t=tmSubscribed;
  if(pthread_mutex_unlock(&runLk))eExit("pthread_mutex_unlock()");
  return t;
}
/*****************************************************************************/
/* set methods */
/*****************************************************************************/
void PcNode::setName(string& name)
{
  this->name=name;
  this->tmSrvName=string(SVC_HEAD)+"/"+FmcUtils::toUpper(name)+"/task_manager";
  this->tmLsSvcName=this->tmSrvName+"/list";
  this->tmStartCmdName=this->tmSrvName+"/start";
  this->tmStopCmdName=this->tmSrvName+"/stop";
  return;
}
/*****************************************************************************/
/* The "cmd" argument is used only for diagnostic messages                   */
void PcNode::addSchPs(PcProcess &ps,string &cmd,bool startIt)
{
  list<PcProcess>::iterator psIt;
  bool found;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&schLk))eExit("pthread_mutex_lock()");
  /* check if the utgid of ps is already in the list */
  for(psIt=schPss.begin(),found=false;psIt!=schPss.end();psIt++)
  {
    if(psIt->getUtgid()==ps.getUtgid())
    {
      found=true;
      break;
    }
  }
  if(found)                            /* utgid of ps is already in the list */
  {
    if(pthread_mutex_unlock(&schLk))eExit("pthread_mutex_unlock()");
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s\": A process having UTGID=\"%s\" "
            "(CMD=\"%s\") was already scheduled on node \"%s\"! No new "
            "process added to node \"%s\"!",cmd.c_str(),
            psIt->getUtgid().c_str(),psIt->getPath().c_str(),getName().c_str(),
            getName().c_str());
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* add the process to the list */
  schPss.push_back(ps);                                  /* make a deep copy */
  schPss.back().setRcEnTime();           /* set the time of process enabling */
  schPss.back().setNode(getName());                     /* set the node name */
  schPss.back().parExpand();   /* perform parameter expansion of ${RUN_NODE} */
  if(pthread_mutex_unlock(&schLk))eExit("pthread_mutex_unlock()");
  mPrintf(errU,INFO,METHOD,0,"Process: UTGID=\"%s\", CMD=\"%s\" added to "
          "node \"%s\".",ps.getUtgid().c_str(),ps.getPath().c_str(),
          getName().c_str());
  /*-------------------------------------------------------------------------*/
  /* start the process */
  if(startIt)startPs(schPss.back().getUtgid());
  /*-------------------------------------------------------------------------*/
  return;
}
/*****************************************************************************/
/* The "cmd" argument is used only for diagnostic messages                   */
void PcNode::rmSchPs(string &utgidPttn,unsigned signo,unsigned s9delay,
                     string &cmd)
{
  list<PcProcess>::iterator psIt;
  int i;
  vector<string>rmUtgidLs;                   /* list of UTGIDs to be stopped */
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&schLk))eExit("pthread_mutex_lock()");
  /* compose the list of the UTGIDs to be stopped */
  for(psIt=schPss.begin(),i=0;psIt!=schPss.end();)
  {
    if(!fnmatch(utgidPttn.c_str(),psIt->getUtgid().c_str(),0))
    {
      mPrintf(errU,INFO,METHOD,0,"Process: UTGID=\"%s\", CMD=\"%s\" removed "
              "from node \"%s\".",psIt->getUtgid().c_str(),
              psIt->getPath().c_str(),getName().c_str());
      rmUtgidLs.push_back(psIt->getUtgid());
      psIt=schPss.erase(psIt);   /* remove list element and advance iterator */
      i++;
    }
    else
    {
      psIt++;                                            /* advance iterator */
    }
  }
  if(pthread_mutex_unlock(&schLk))eExit("pthread_mutex_unlock()");
  /*-------------------------------------------------------------------------*/
  /* stop the processes */
  if(i)                                            /* found at least 1 UTGID */
  {
    vector<string>::iterator utgidIt;
    for(utgidIt=rmUtgidLs.begin(),i=0;utgidIt!=rmUtgidLs.end();utgidIt++)
    {
      stopPs(*utgidIt,signo,s9delay);
    }
  }
  else                                                     /* no UTGID found */
  {
    mPrintf(errU,WARN,METHOD,0,"CMD: \"%s\": No process matching the utgid "
            "pattern \"%s\" found in the list of scheduled process for node "
            "\"%s\"! Nothing to remove!",cmd.c_str(),utgidPttn.c_str(),
            getName().c_str());
  }
  /*-------------------------------------------------------------------------*/
}
/*****************************************************************************/
/* Set the running processes list runPss from a sequence of                  */
/* NULL-terminating strings (as returned by task_manager_list DIM SVC).      */
/* runPssSeq: pointer to the first char of the first string                  */
/* runPssSz:  size (in character) of the whole string sequence               */
/*            (including \0s)                                                */
void PcNode::setRunPss(int runPssSz,char *runPssSeq)
{
  char *p;
  /*-------------------------------------------------------------------------*/
  if(pthread_mutex_lock(&runLk))eExit("pthread_mutex_lock()");
  runPss.clear();
  if(runPssSz && runPssSeq && runPssSeq[0])
  {
    for(p=runPssSeq;;)
    {
      runPss.push_back(p);
      p+=(1+strlen(p));
      if(p>=runPssSeq+runPssSz)break;
    }
  }
  if(pthread_mutex_unlock(&runLk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
void PcNode::setTmSubscribed(bool t)
{
  if(pthread_mutex_lock(&schLk))eExit("pthread_mutex_lock()");
  tmSubscribed=t;
  if(pthread_mutex_unlock(&schLk))eExit("pthread_mutex_unlock()");
}
/*****************************************************************************/
/* start/stop methods                                                        */
/*****************************************************************************/
/* returns 1 if success, 0 if failure */
int PcNode::startPs(PcProcess &ps)
{
  string tmCmd;
  int rv;
  /*-------------------------------------------------------------------------*/
  /* check if the Task Manager on this node is subscribed */
  if(!isTmSubscribed())
  {
    mPrintf(errU,WARN,METHOD,0,"Cannot start process: UTGID=\"%s\", "
            "CMD=\"%s\" on node \"%s\" right now since the Task Manager "
            "Server on node \"%s\" is not (not yet?) reachable!",
            ps.getUtgid().c_str(), ps.getPath().c_str(), name.c_str(),
            name.c_str());
    rv=0;
    return rv;
  }
  /*-------------------------------------------------------------------------*/
  ps.checkRc();
  if(!ps.isRcEnabled())
  {
    if(deBug&0x10)
    {
       mPrintf(errU,VERB,METHOD,0,"Process UTGID=\"%s\" on node \"%s\" "
               "disabled!",ps.getUtgid().c_str(), name.c_str());
    }
    rv=0;
    return rv;
  }
  /*-------------------------------------------------------------------------*/
  /* compose start command string */
  if(acl.isAuthenticated())tmCmd+=acl.getAuthString()+" ";
  if(ps.isRdrStdout())tmCmd+="-o ";
  if(ps.isRdrStderr())tmCmd+="-e ";
  if(ps.getOutFIFO().size())tmCmd+="-O "+ps.getOutFIFO()+" ";
  if(ps.getOutFIFO().size())tmCmd+="-E "+ps.getErrFIFO()+" ";
  if(ps.isNoDrop())tmCmd+="-A ";
  if(ps.isCleanEnv())tmCmd+="-c ";
  if(ps.getEnvp().size())
  {
    list<string>::iterator sIt;
    for(sIt=ps.getEnvp().begin();sIt!=ps.getEnvp().end();sIt++)
    {
      tmCmd+="-D "+*sIt+" ";
    }
  }
  if(ps.isDaemon())tmCmd+="-d ";
  tmCmd+="-s "+FmcUtils::itos(ps.getScheduler())+" ";
  tmCmd+="-p "+FmcUtils::itos(ps.getNice())+" ";
  tmCmd+="-r "+FmcUtils::itos(ps.getRtPrio())+" ";
  if(ps.getOomScoreAdj())
  {
    tmCmd+="-k "+FmcUtils::itos(ps.getOomScoreAdj())+" ";
  }
  if(ps.getAffMask()!=~0UL)
  {
    for(unsigned i=0;i<8*sizeof(unsigned long);i++)
    {
      if(ps.getAffMask() & 1UL<<i)tmCmd+="-a "+FmcUtils::itos(i)+" ";
    }
  }
  if(ps.getUser().size())tmCmd+="-n "+ps.getUser()+" ";
  if(ps.getGroup().size())tmCmd+="-g "+ps.getGroup()+" ";
  tmCmd+="-u "+ps.getUtgid()+" ";
  if(ps.getWd().size())tmCmd+="-w "+ps.getWd()+" ";
  tmCmd+=ps.getPath();
  if(ps.getArgs().size())tmCmd+=" "+ps.getArgs();
  /*-------------------------------------------------------------------------*/
  /* send command to the Task Manager Server */
  rv=DimClient::sendCommand(tmStartCmdName.c_str(),tmCmd.c_str()); 
  /*-------------------------------------------------------------------------*/
  /* process return value */
  if(rv)                                                          /* success */
  {
    ps.addStartTime();
    ps.addRcStartTime();
    if(deBug&0x10)ps.printRcStartTimes(VERB);
    if(ps.isFirstStart())                                     /* first start */
    {
      mPrintf(errU,INFO,METHOD,0,"Start command for process: UTGID=\"%s\", "
              "CMD=\"%s\" sent to node \"%s\".",ps.getUtgid().c_str(),
              ps.getPath().c_str(), name.c_str());
      ps.setFirstStart(false);
    }                                                         /* first start */
    else                                                  /* not first start */
    {
      mPrintf(errU,WARN,METHOD,0,"Start command for process: UTGID=\"%s\", "
              "CMD=\"%s\" RE-sent to node \"%s\" (%d times in the last %ld "
              "seconds)!", ps.getUtgid().c_str(), ps.getPath().c_str(),
              name.c_str(), ps.getRcStartTimeN(),
              MIN(time(NULL)-ps.getRcEnTime(),ps.getCheckPeriod()));
    }                                                     /* not first start */
  }                                                               /* success */
  else                                                            /* failure */
  {
    mPrintf(errU,ERROR,METHOD,0,"Failed to send start command for process: "
            "UTGID=\"%s\", CMD=\"%s\" to node \"%s\"!",ps.getUtgid().c_str(),
            ps.getPath().c_str(), name.c_str());
  }                                                               /* failure */
  /*-------------------------------------------------------------------------*/
  return rv;
}
/*****************************************************************************/
/* Process must be already added to schPss.                                  */
/* Returns 1 if success, 0 if DIM failure, -1 if process not found in list   */
int PcNode::startPs(string utgid)
{
  list<PcProcess>::iterator psIt;
  int rv;
  bool found;
  /*-------------------------------------------------------------------------*/
  /* Check if the utgid is in the list of scheduled processes and get the    */
  /* pointer to the PcProcess object psIt.                                   */
  if(pthread_mutex_lock(&schLk))eExit("pthread_mutex_lock()");
  for(psIt=schPss.begin(),found=false;psIt!=schPss.end();psIt++)
  {
    if(psIt->getUtgid()==utgid)
    {
      found=true;
      break;
    }
  }
  if(pthread_mutex_unlock(&schLk))eExit("pthread_mutex_unlock()");
  /*-------------------------------------------------------------------------*/
  if(!found)
  {
    rv=-1;
    mPrintf(errU,ERROR,METHOD,0,"Process UTGID=\"%s\" is not in the list for "
            "the node \"%s\"!", utgid.c_str(), name.c_str());
    return rv;
  }
  /*-------------------------------------------------------------------------*/
  /* start the process */
  rv=startPs(*psIt);
  /*-------------------------------------------------------------------------*/
  return rv;
}
/*****************************************************************************/
/* returns 1 if success, 0 if DIM failure */
int PcNode::stopPs(string utgid,unsigned signo,unsigned s9delay)
{
  string tmCmd;
  int rv;
  /*-------------------------------------------------------------------------*/
  /* compose stop command string */
  if(acl.isAuthenticated())tmCmd+=acl.getAuthString()+" ";
  tmCmd+="-s "+FmcUtils::utos(signo)+" ";
  tmCmd+="-d "+FmcUtils::utos(s9delay)+" ";
  tmCmd+=utgid;
  rv=DimClient::sendCommand(tmStopCmdName.c_str(),tmCmd.c_str());
  if(rv)                                                          /* success */
  {
    mPrintf(errU,INFO,METHOD,0,"Stop command for process: UTGID=\"%s\" "
            "sent to node \"%s\".", utgid.c_str(), name.c_str());
  }                                                               /* success */
  else                                                            /* failure */
  {
    mPrintf(errU,ERROR,METHOD,0,"Failed to send stop command for process: "
            "UTGID=\"%s\" to node \"%s\"!",utgid.c_str(),name.c_str());
  }                        
  return rv;
}
/*****************************************************************************/
/* check methods                                                             */
/*****************************************************************************/
void PcNode::checkRunning()
{
  vector<string> misUtgids;                        /* missing processes list */
  vector<string>::iterator schIt;
  vector<string>::iterator runIt;
  vector<string>::iterator misIt;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  vector<string>schUtgids=getSchUtgids();         /* get scheduled processes */
  vector<string>runUtgids=getRunUtgids();           /* get running processes */
  /* loop over scheduled processes */
  for(schIt=schUtgids.begin();schIt!=schUtgids.end();schIt++)
  {
    /* loop over running processes */
    for(runIt=runUtgids.begin(),found=false;runIt!=runUtgids.end();runIt++)
    {
      if(*runIt==*schIt)
      {
        found=true;
        break;
      }
    }
    if(!found)misUtgids.push_back(*schIt);
  }
  for(misIt=misUtgids.begin();misIt!=misUtgids.end();misIt++)
  {
    if(deBug&0x10)
    {
      mPrintf(errU,VERB,METHOD,0,"Node: \"%s\": missing utgid \"%s\".",
              getName().c_str(),misIt->c_str());
    }
    startPs(*misIt);
  }
  incUpdateCnt();                                /* increment update counter */
}
/*****************************************************************************/
/* print methods                                                             */
/*****************************************************************************/
void PcNode::printSchPss(string head,string separator,string trail)
{
  list<PcProcess>::iterator pssIt;
  /*-------------------------------------------------------------------------*/
  printf("%s",head.c_str());
  if(pthread_mutex_lock(&runLk))eExit("pthread_mutex_lock()");
  //printf("Scheduled process number: %zu.\n",schPss.size());
  for(pssIt=schPss.begin();pssIt!=schPss.end();pssIt++)
  {
    if(pssIt!=schPss.begin())printf("%s",separator.c_str());
    printf("%s",pssIt->getUtgid().c_str());
  }
  if(pthread_mutex_unlock(&runLk))eExit("pthread_mutex_unlock()");
  printf("%s",trail.c_str());

}
/*****************************************************************************/
void PcNode::printRunPss(int sev)
{
  vector<string>::iterator psIt;
  string s;
  /*-------------------------------------------------------------------------*/
  s+="Process(es) running on node "+name+" ("+FmcUtils::itos(runPss.size())+
     "): ";
  if(pthread_mutex_lock(&runLk))eExit("pthread_mutex_lock()");
  for(psIt=runPss.begin();psIt!=runPss.end();psIt++)
  {
    if(psIt!=runPss.begin())s+=", ";
    s+=*psIt;
  }
  if(pthread_mutex_unlock(&runLk))eExit("pthread_mutex_unlock()");
  s+=".";
  mPrintf(errU,sev,METHOD,0,"%s",s.c_str());
}
/*****************************************************************************/
/* constructors                                                              */
/*****************************************************************************/
PcNode::PcNode()
{
  if(pthread_mutex_init(&schLk,NULL))eExit("pthread_mutex_init()");
  if(pthread_mutex_init(&runLk,NULL))eExit("pthread_mutex_init()");
  this->tmSubscribed=false;
  this->updateCnt=0;
  if(deBug&0x10)
  {
    mPrintf(errU,VERB,__PRETTY_FUNCTION__,0,"PcProcess constructor 1 "
            "invoked.");
  }
}
/*****************************************************************************/
PcNode::PcNode(string name)
{
  if(deBug&0x10)
  {
    mPrintf(errU,VERB,__PRETTY_FUNCTION__,0,"PcProcess constructor 2 "
            "invoked.");
  }
  if(pthread_mutex_init(&schLk,NULL))eExit("pthread_mutex_init()");
  if(pthread_mutex_init(&runLk,NULL))eExit("pthread_mutex_init()");
  this->name=name;
  this->tmSrvName=string(SVC_HEAD)+"/"+FmcUtils::toUpper(name)+"/task_manager";
  this->tmLsSvcName=this->tmSrvName+"/list";
  this->tmStartCmdName=this->tmSrvName+"/start";
  this->tmStopCmdName=this->tmSrvName+"/stop";
  this->tmSubscribed=false;
  this->updateCnt=0;
}
/*****************************************************************************/
/* destructor                                                                */
/*****************************************************************************/
PcNode::~PcNode()
{
  if(pthread_mutex_destroy(&schLk))eExit("pthread_mutex_destroy()");
  if(pthread_mutex_destroy(&runLk))eExit("pthread_mutex_destroy()");
  if(deBug&0x10)
  {
    mPrintf(errU,VERB,__PRETTY_FUNCTION__,0,"PcProcess destructor "
            "invoked.");
  }
}
/* ######################################################################### */
