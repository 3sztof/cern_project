/* ######################################################################### */
/*
 * $Log: PsMoniSvcSeeker.C,v $
 * Revision 1.5  2009/07/21 15:20:05  galli
 * less messages to the logger id deBug disabled
 *
 * Revision 1.4  2009/03/19 07:56:42  galli
 * when SERVICE_LIST return the complete list do not unsubscribe active SVCs
 *
 * Revision 1.3  2009/03/12 09:29:03  galli
 * bug fixed
 *
 * Revision 1.2  2009/02/26 20:16:16  galli
 * first working version
 *
 * Revision 1.1  2009/02/24 10:46:45  galli
 * Initial revision
 */
/* ######################################################################### */
/* Class which inherits from DimInfo and subscribes to a DIM service         */
/* "/FMC/<HOST>/ps/SERVICE_LIST". Instantiated once per each running psSrv.  */
/* ######################################################################### */
#include "PsMoniSvcSeeker.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int deBug;
extern int errU;
/* ######################################################################### */
/* called when a process SVC is added or removed to/from psSrv */
void PsMoniSvcSeeker::infoHandler()
{
  char *msg;
  string srv,node;
  list<PsMoniSvcWatcher*>::iterator it;
  char *vb,*p,*last;
  /* ----------------------------------------------------------------------- */
  if(unlikely(deBug & 0x8))
  {  
    mPrintf(errU,WARN,METHOD,0,"\"%s\": infoHandler called.",
            psSrvSvcLsSvcPath.c_str());
  }
  msg=getString();
  /* ----------------------------------------------------------------------- */
  switch(*msg)
  {
    /* ..................................................................... */
    case '+':                                   /* SVC added to SERVICE_LIST */
      p=last=msg+1;                                         /* skip char '+' */
      while(last && (vb=strchr(p,'|')))
      {
        last=strchr(p,'\n');
        if(last)*last=0;
        *vb=0;
        /* select DIM services which match the given SVC pattern */
        if(!matchSvcPattern(p))
        {
          if(unlikely(deBug & 0x8))
          {
            mPrintf(errU,DEBUG,METHOD,9,"+ SVC: \"%s\" matches.",p);
          }
          addPsSvc(p);
        }
        p=last+1;
      }
      break;
    /* ..................................................................... */
    case '-':                               /* SVC removed from SERVICE_LIST */
      p=last=msg+1;                                         /* skip char '-' */
      while(last && (vb=strchr(p,'|')))
      {
        last=strchr(p,'\n');
        if(last)*last=0;
        *vb=0;
        /* select DIM services which match the given SVC pattern */
        if(!matchSvcPattern(p))
        {
          if(unlikely(deBug & 0x8))
          {
            mPrintf(errU,DEBUG,METHOD,9,"- SVC: \"%s\" matches.",p);
          }
          rmPsSvc(p);
        }
        p=last+1;
      }
      break;
    /* ..................................................................... */
    default:       /* server list at ".../SERVICE_LIST" service subscription */
      if(msg!=(char*)-1 && *msg)       /* service .../SERVICE_LIST reachable */
      {
        setAllPsSvcInactive();
        p=last=msg;
        while(last && (vb=strchr(p,'|')))
        {
          last=strchr(p,'\n');
          if(last)*last=0;
          *vb=0;
          /* select DIM services which match the given SVC pattern */
          if(!matchSvcPattern(p))
          {
            addPsSvc(p);
            if(unlikely(deBug & 0x8))
            {
              mPrintf(errU,DEBUG,METHOD,9,"I SVC: \"%s\" matches.",p);
            }
          }
          p=last+1;
        }
        rmInactivePsSvc();
      }
      break;
    /* ..................................................................... */
  }                                                          /* switch(*msg) */
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
void PsMoniSvcSeeker::addPsSvc(const string svc)
{
  list<PsMoniSvcWatcher*>::iterator wIt;
  vector<string>::iterator knIt;
  bool svcFound=false;
  bool svcKnown=false;
  /* ----------------------------------------------------------------------- */
  /* chech if SVC is known */
  for(knIt=knwnPsSvcNames.begin();knIt!=knwnPsSvcNames.end();knIt++)
  {
    if(svc==*knIt)
    {
      svcKnown=true;
      break;
    }
  }
  if(!svcKnown)knwnPsSvcNames.push_back(svc);     /* from now on it is known */
  /* ----------------------------------------------------------------------- */
  /* check if the SVC svc is already subscribed */
  for(wIt=psSvcLs.begin();wIt!=psSvcLs.end();wIt++)
  {
    if((*wIt)->getPsSvcPath()==svc)
    {
      svcFound=true;
      (*wIt)->setActive();
      break;
    }
  }
  /* ----------------------------------------------------------------------- */
  /* if not subscibed, then subscribe to DIM SVC:                            */
  /* /FMC/ps/details/monitored/processes/CMD{UTGID}TID/data                  */
  if(!svcFound)
  {
    psSvcLs.push_back(new PsMoniSvcWatcher(svc.c_str()));
    /* --------------------------------------------------------------------- */
    if(unlikely(deBug & 0x1))
    {
      if(!svcKnown)
      {
        mPrintf(errU,INFO,METHOD,0,"Process Monitor SVC \"%s\" available.",
                svc.c_str());
      }
      else
      {
        mPrintf(errU,WARN,METHOD,0,"Process Monitor SVC \"%s\" available "
                "again. ",svc.c_str());
      }
    }
    /* --------------------------------------------------------------------- */
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
void PsMoniSvcSeeker::rmPsSvc(const string svc)
{
  list<PsMoniSvcWatcher*>::iterator wIt;
  //bool svcFound=false;
  /* ----------------------------------------------------------------------- */
  /* if the SVC is subscribed then unsubscribe */
  for(wIt=psSvcLs.begin();wIt!=psSvcLs.end();wIt++)
  {
    if((*wIt)->getPsSvcPath()==svc)
    {
      //svcFound=true;
      delete *wIt;                                /* remove LogWriter object */
      psSvcLs.erase(wIt);       /* remove pointer to LogWriter obj from list */
      break;
    }
  }
  /* ----------------------------------------------------------------------- */
  if(unlikely(deBug & 0x1))
  {
    mPrintf(errU,WARN,METHOD,0,"Process Monitor SVC \"%s\" no more "
            "available. ",svc.c_str());
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
void PsMoniSvcSeeker::rmInactivePsSvc(void)
{
  list<PsMoniSvcWatcher*>::iterator wIt;
  /* ----------------------------------------------------------------------- */
  /* unsubscribe the inactive SVC */
  for(wIt=psSvcLs.begin();wIt!=psSvcLs.end();)
  {
    if(!(*wIt)->isActive())
    {
      if(unlikely(deBug & 0x1))
      {
        mPrintf(errU,WARN,METHOD,0,"Process Monitor SVC \"%s\" no more "
                "available. ",(*wIt)->getPsSvcPath().c_str());
      }
      delete *wIt;                                /* remove LogWriter object */
      wIt=psSvcLs.erase(wIt);   /* remove pointer to LogWriter obj from list */
    }
    else wIt++;
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
void PsMoniSvcSeeker::setAllPsSvcInactive(void)
{
  list<PsMoniSvcWatcher*>::iterator wIt;
  /* ----------------------------------------------------------------------- */
  /* set all subscribed SVC inactive */
  for(wIt=psSvcLs.begin();wIt!=psSvcLs.end();wIt++)
  {
    if(unlikely(deBug & 0x8))
    {
      mPrintf(errU,DEBUG,METHOD,0,"Process Monitor SVC \"%s\" set inactive. ",
              (*wIt)->getPsSvcPath().c_str());
    }
    (*wIt)->setInactive();                        /* remove LogWriter object */
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
int PsMoniSvcSeeker::matchSvcPattern(string svc)
{
  vector<string>::iterator cmdIt;             /* process CMD vector iterator */
  vector<string>::iterator utgidIt;         /* process UTGID vector iterator */
  string svcPttn;
  /* ----------------------------------------------------------------------- */
  /* compare against a generic psSrv process SVC pattern */
  if(fnmatch(SVC_HEAD"/*/ps/details/monitored/processes/*{*}*/data",
             svc.c_str(),0)==FNM_NOMATCH)
  {
    return FNM_NOMATCH;
  }
  /* compare against psSrv proc CMD & UTGID pattern selected using cmd-line */
  for(cmdIt=cmdPttnLs.begin();cmdIt!=cmdPttnLs.end();cmdIt++)
  {
    for(utgidIt=utgidPttnLs.begin();utgidIt!=utgidPttnLs.end();utgidIt++)
    {
      svcPttn=srvName+"/details/monitored/processes/"+*cmdIt+"{"+*utgidIt+"}"+
              "*/data";
      if(!fnmatch(svcPttn.c_str(),svc.c_str(),0))return 0;
    }
  }
  return FNM_NOMATCH;
} 
/*****************************************************************************/
/* constructor */
PsMoniSvcSeeker::PsMoniSvcSeeker(const char* psSrvSvcLsSvcPath,
                                 vector<string>& cmdPttnLs,
                                 vector<string>& utgidPttnLs)
: DimInfo(psSrvSvcLsSvcPath,-1), psSrvSvcLsSvcPath(psSrvSvcLsSvcPath),
  cmdPttnLs(cmdPttnLs), utgidPttnLs(utgidPttnLs)
{
  this->srvName=string(psSrvSvcLsSvcPath);
  this->srvName=this->srvName.erase(this->srvName.find_last_of('/'));
  if(unlikely(deBug & 0x8))
  {
    mPrintf(errU,WARN,METHOD,0,"Class PsMoniSvcSeeker instantiated for "
            "server: \"%s\".",this->srvName.c_str());
  }
}
/*****************************************************************************/
/* destructor */
PsMoniSvcSeeker::~PsMoniSvcSeeker()
{
  if(unlikely(deBug & 0x8))
  {
    mPrintf(errU,WARN,METHOD,0,"Class PsMoniSvcSeeker deallocated for "
            "server: \"%s\".",this->srvName.c_str());
  }
}
/* ######################################################################### */
