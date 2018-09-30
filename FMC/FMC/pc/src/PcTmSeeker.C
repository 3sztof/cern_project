/* ######################################################################### */
/*
 * $Log: PcTmSeeker.C,v $
 * Revision 1.6  2012/12/04 11:24:37  galli
 * minor changes
 *
 * Revision 1.5  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.4  2009/02/26 21:30:00  galli
 * Handles server error condition (! from DNS)
 *
 * Revision 1.3  2008/09/22 07:56:04  galli
 * addTmWatcher() and rmTmWatcher() set tmSubscribed bool in PcNode
 *
 * Revision 1.1  2008/09/19 11:46:05  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcTmSeeker.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern PcCluster cluster;
extern int errU;
extern int deBug;
/* ######################################################################### */
/* s = "/FMC/LHCBSRV/task_manager@lhcbsrv.bo.infn.it" */
/* srv = /FMC/LHCBSRV/task_manager */
/* node = lhcbsrv */
void PcTmSeeker::getServer(char *s, string &srv, string &node)
{
  char *at;
  char *dot;
  /*-------------------------------------------------------------------------*/
  at=strchr(s,'@');
  *at=0;
  srv=s;
  dot=strchr(at+1,'.');
  if(dot)*dot=0;
  node=at+1;
}
/*****************************************************************************/
/* add a Task Manager watcher */
void PcTmSeeker::addTmWatcher(const string &srv, const string &node)
{
  PcTmWatcher *tmWatcher;
  list<PcTmWatcher*>::iterator tmIt;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  string tmLsSvcPath=srv+"/list";
  /* check if the service tmLsSvcPath is already subscribed */
  for(tmIt=tmWatchers.begin();tmIt!=tmWatchers.end();tmIt++)
  {
    if((*tmIt)->getTmLsSvcPath()==tmLsSvcPath)
    {
      found=true;
      break;
    }
  }
  /* if not, subscribe to DIM service /FMC/<HOSTNAME>/task_manager/list */
  if(!found)
  {
    tmWatcher=new PcTmWatcher(tmLsSvcPath,node);
    /* save pointer to list */
    tmWatchers.push_back(tmWatcher);
    cluster.setTmSubscribed(node,true);
  }
  if(deBug&0x01)
  {
    if(!found)
    {
      mPrintf(errU,DEBUG,METHOD,0,"Task Manager SVC \"%s\" subscribed.",
              tmLsSvcPath.c_str());
    }
    else
    {
      mPrintf(errU,DEBUG,METHOD,0,"Task Manager SVC \"%s\" RE-subscribed!",
              tmLsSvcPath.c_str());
    }
  }
}
/*****************************************************************************/
/* remove a Task Manager watcher */
void PcTmSeeker::rmTmWatcher(const string &srv, const string &node, char status)
{
  list<PcTmWatcher*>::iterator tmIt;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  string tmLsSvcPath=srv+"/list";
  /* cancel subscribtion to DIM service /FMC/<HOSTNAME>/task_manager/list */
  for(tmIt=tmWatchers.begin();tmIt!=tmWatchers.end();tmIt++)
  {
    if((*tmIt)->getTmLsSvcPath()==tmLsSvcPath)
    {
      found=true;
      delete *tmIt;                             /* remove PcTmWatcher object */
      tmWatchers.erase(tmIt); /* remove pointer to PcTmWatcher obj from list */
      break;
    }
  }
  if(found)cluster.setTmSubscribed(node,false);
  if(deBug&0x01)
  {
    if(found)
    {
      mPrintf(errU,DEBUG,METHOD,0,"Task Manager SVC \"%s\": subsciption "
              "cancelled.",tmLsSvcPath.c_str());
    }
    else
    {
      mPrintf(errU,WARN,METHOD,0,"Task Manager SVC \"%s\": was not "
              "subscribed. Cannot cancel subsciption!",tmLsSvcPath.c_str());
    }
  }
}
/*****************************************************************************/
void PcTmSeeker::printTmWatchers(int sev)
{
  list<PcTmWatcher*>::iterator tmIt;
  string s;
  /*-------------------------------------------------------------------------*/
  s="Active contolled nodes: ";
  for(tmIt=tmWatchers.begin();tmIt!=tmWatchers.end();tmIt++)
  {
    if(tmIt!=tmWatchers.begin())s+=", ";
    s+=(*tmIt)->getTmLsSvcNode();
  }
  s+=".";
  mPrintf(errU,sev,METHOD,0,"%s",s.c_str());
}
/*****************************************************************************/
/* called when a Task Manager Server is added or removed */
void PcTmSeeker::infoHandler()
{
  char c;
  char *msg;
  char status;
  string srv,node;
  /*-------------------------------------------------------------------------*/
  msg=getString();
  if(deBug&0x01)mPrintf(errU,VERB,METHOD,0,"got string: \"%s\".",msg);
  switch(c=*msg)
  {
    /*.......................................................................*/
    case '+':                                     /* server added to DIM DNS */
      getServer(++msg,srv,node);
      /* select the Task Manager servers of the controlled nodes */
      if(isCtrldTmSrv(srv))
      {
        bool found=false;
        vector<string>::iterator knIt;
        for(knIt=knwnTmSrvNames.begin();knIt!=knwnTmSrvNames.end();knIt++)
        {
          if(srv==*knIt)
          {
            found=true;
            break;
          }
        }
        if(!found)
        {
          mPrintf(errU,INFO,METHOD,0,"New Task Manager server: \"%s\" found "
                  "on node \"%s\".", srv.c_str(), node.c_str());
          knwnTmSrvNames.push_back(srv);
        }
        else
        {
          mPrintf(errU,WARN,METHOD,0,"Task Manager server: \"%s\" available "
                  "again on node \"%s\".", srv.c_str(), node.c_str());
        }
        addTmWatcher(srv,node);
      }
      break;
    /*.......................................................................*/
    case '-':                                 /* server removed from DIM DNS */
    case '!':                                             /* server in error */
      status=*msg;
      getServer(++msg,srv,node);
      /* select the Task Manager servers of the controlled nodes */
      if(isCtrldTmSrv(srv))
      {
        if(status=='!')
        {
          mPrintf(errU,WARN,METHOD,0,"Task Manager server: \"%s\" no more "
                  "available on node \"%s\" (in error)!", srv.c_str(),
                  node.c_str());
        }
        else if(status=='-')
        {
          mPrintf(errU,WARN,METHOD,0,"Task Manager server: \"%s\" no more "
                  "available on node \"%s\" (removed)!", srv.c_str(),
                  node.c_str());
        }
        rmTmWatcher(srv,node,status);
      }
      break;
    /*.......................................................................*/
    default:    /* server list at "DIS_DNS/SERVER_LIST" service subscription */
      if(msg!=(char*)-1)            /* service DIS_DNS/SERVER_LIST reachable */
      {
        char *at,*p=msg,*last=msg;
        while(last!=0 && (at=strchr(p,'@'))!=0)     /* loop over SVC records */
        {
          last=strchr(at,'|');
          if(last)*last=0;
          getServer(p,srv,node);
          /* select the Task Manager servers of the controlled nodes */
          if(isCtrldTmSrv(srv))
          {
            bool found=false;
            vector<string>::iterator knIt;
            for(knIt=knwnTmSrvNames.begin();knIt!=knwnTmSrvNames.end();knIt++)
            {
              if(srv==*knIt)
              {
                found=true;
                break;
              }
            }
            if(!found)
            {
              mPrintf(errU,INFO,METHOD,0,"New Task Manager server: \"%s\" "
                      "found on node \"%s\".", srv.c_str(), node.c_str());
              knwnTmSrvNames.push_back(srv);
            }
            else
            {
              mPrintf(errU,WARN,METHOD,0,"Task Manager server: \"%s\" "
                      "available again on node \"%s\".", srv.c_str(),
                      node.c_str());
            }
            addTmWatcher(srv,node);
          }
          p=last+1;
        }                       /* loop over DIS_DNS/SERVER_LIST SVC records */
      }                       /* if service DIS_DNS/SERVER_LIST is reachable */
      else                      /* service DIS_DNS/SERVER_LIST not reachable */
      {
        mPrintf(errU,ERROR,METHOD,0,"DIM DNS unreachable!");
      }
      break;
    /*.......................................................................*/
  }
  if(deBug&0x01)printTmWatchers(DEBUG);
}
/*****************************************************************************/
bool PcTmSeeker::isCtrldTmSrv(string srv)
{
  vector<string>::iterator sIt;
  bool isIt;
  /*-------------------------------------------------------------------------*/
  for(isIt=false,sIt=tmSrvNames.begin();sIt!=tmSrvNames.end();sIt++)
  {
    if(*sIt==srv)
    {
      isIt=true;
      break;
    }
  }
  return isIt;
}
/*****************************************************************************/
/* constructor                                                               */
/*****************************************************************************/
PcTmSeeker::PcTmSeeker(const char *dnsSrvLsSvcPath)
:DimInfo(dnsSrvLsSvcPath,-1)
{
  tmSrvNames=cluster.getTmSrvNames();
}
/* ######################################################################### */
