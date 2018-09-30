/* ######################################################################### */
/*
 * $Log: PsMoniSrvSeeker.C,v $
 * Revision 1.5  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.4  2009/07/22 21:20:31  galli
 * added server counter
 *
 * Revision 1.3  2009/02/26 20:17:06  galli
 * first working version
 *
 * Revision 1.1  2009/02/23 14:14:05  galli
 * Initial revision
 */
/* ######################################################################### */
/* Class which inherits from DimInfo and subscribes to the DIM service       */
/* "DIS_DNS/SERVER_LIST". Instantiated only once.                            */
/* ######################################################################### */
#include "PsMoniSrvSeeker.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int deBug;
extern int errU;
extern unsigned psSrvN;
/* ######################################################################### */
/* called when a server is added or removed to/from DIM DNS */
void PsMoniSrvSeeker::infoHandler()
{
  char *msg;
  string srv,node;
  list<PsMoniSvcSeeker*>::iterator it;
  /* ----------------------------------------------------------------------- */
  if(unlikely(deBug & 0x4))
  {
    mPrintf(errU,WARN,METHOD,0,"\"%s\": infoHandler called.",
            dnsSrvLsSvcPath.c_str());
  }
  msg=getString();
  /* ----------------------------------------------------------------------- */
  switch(*msg)
  {
    /* ..................................................................... */
    case '+':                                     /* server added to DIM DNS */
      getServer(++msg,srv,node);
      /* select DIM services which matches the given SVC pattern */
      if(!matchSrvPattern(srv))
      {
        if(unlikely(deBug & 0x4))
        {
          mPrintf(errU,DEBUG,METHOD,9,"+ Node: \"%s\"; SRV: \"%s\" matches.",
                  node.c_str(),srv.c_str());
        }
        addPsSrv(srv,node);
      }                                         /* if(!matchSrvPattern(srv)) */
      break;
    /* ..................................................................... */
    case '-':                                 /* server removed from DIM DNS */
    case '!':                                             /* server in error */
      getServer(++msg,srv,node);
      /* select DIM services which matches the given SVC pattern */
      if(!matchSrvPattern(srv))
      {
        if(unlikely(deBug & 0x4))
        {
          mPrintf(errU,DEBUG,METHOD,9,"- Node: \"%s\"; SRV: \"%s\" matches.",
                  node.c_str(),srv.c_str());
        }
        rmPsSrv(srv,node,*msg);
      }
      break;
    /* ..................................................................... */
    default:    /* server list at "DIS_DNS/SERVER_LIST" service subscription */
      if(msg!=(char*)-1)            /* service DIS_DNS/SERVER_LIST reachable */
      {
        char *at,*p=msg,*last=msg;
        while(last && (at=strchr(p,'@')))           /* loop over SVC records */
        {
          last=strchr(at,'|');
          if(last)*last=0;
          getServer(p,srv,node);
          /* select DIM services which matches the given SVC pattern */
          if(!matchSrvPattern(srv))
          {
            if(unlikely(deBug & 0x4))
            {
              mPrintf(errU,DEBUG,METHOD,9,"I Node: \"%s\", SRV: \"%s\" "
                      "matches.",node.c_str(),srv.c_str());
            }
            addPsSrv(srv,node);
          }                                     /* if(!matchSrvPattern(srv)) */
          p=last+1;
        }                       /* loop over DIS_DNS/SERVER_LIST SVC records */
      }                          /* if service DIS_DNS/SERVER_LIST reachable */
      else
      {
        mPrintf(errU,ERROR,METHOD,0,"DIM DNS unreachable");
      }
      break;
    /* ..................................................................... */
  }                                                          /* switch(*msg) */
  /* ----------------------------------------------------------------------- */
  if(unlikely(deBug & 0x4))
  {
    for(it=psSrvLs.begin();it!=psSrvLs.end();it++)
    {
      printf("=>%s\n",((*it)->getSrvPath()).c_str());
    }
    mPrintf(errU,INFO,METHOD,0,"==>\"%s\"\n",getString());
  }
}
/*****************************************************************************/
void PsMoniSrvSeeker::addPsSrv(const string &srv, const string &node)
{
  string svcListSvcPath=srv+"/SERVICE_LIST";
  list<PsMoniSvcSeeker*>::iterator slIt;
  vector<string>::iterator knIt;
  bool psSrvFound=false;
  bool psSrvKnown=false;
  /* ----------------------------------------------------------------------- */
  /* chech if SVC is known */
  for(knIt=knwnPsSrvNames.begin();knIt!=knwnPsSrvNames.end();knIt++)
  {
    if(srv==*knIt)
    {
      psSrvKnown=true;
      break;
    }
  }
  if(!psSrvKnown)knwnPsSrvNames.push_back(srv);
  /* ----------------------------------------------------------------------- */
  /* check if the SVC svcListSvcPath is already subscribed */
  for(slIt=psSrvLs.begin();slIt!=psSrvLs.end();slIt++)
  {
    if((*slIt)->getSrvPath()==svcListSvcPath)
    {
      psSrvFound=true;
      break;
    }
  }
  /* ----------------------------------------------------------------------- */
  /* if not subscibed, then subscribe to DIM SVC:                            */
  /* /FMC/<HOST>/ps/SERVICE_LIST                                             */
  if(!psSrvFound)
  {
    psSrvLs.push_front(new PsMoniSvcSeeker(svcListSvcPath.c_str(),cmdPttnLs,
                                           utgidPttnLs));
    psSrvN++;
    /* --------------------------------------------------------------------- */
    if(!psSrvKnown)
    {
      mPrintf(errU,INFO,METHOD,0,"Process Monitor Server \"%s\" available on "
              "node \"%s\".",srv.c_str(),node.c_str());
    }
    else
    {
      mPrintf(errU,WARN,METHOD,0,"Process Monitor Server \"%s\" available "
              "again on node \"%s\".",srv.c_str(),node.c_str());
    }
    /* --------------------------------------------------------------------- */
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
void PsMoniSrvSeeker::rmPsSrv(const string &srv,const string &node,char status)
{
  string svcListSvcPath=srv+"/SERVICE_LIST";
  list<PsMoniSvcSeeker*>::iterator slIt;
  //bool psSrvFound=false;
  /* ----------------------------------------------------------------------- */
  /* if the SVC svcListSvcPath is subscribed then unsubscribe */
  for(slIt=psSrvLs.begin();slIt!=psSrvLs.end();slIt++)
  {
    if((*slIt)->getSrvPath()==svcListSvcPath)
    {
      //psSrvFound=true;
      delete *slIt;                             /* remove PsSvcSeeker object */
      psSrvLs.erase(slIt);    /* remove pointer to PsSvcSeeker obj from list */
      psSrvN--;
      break;
    }
  }
  /* ----------------------------------------------------------------------- */
  if(status=='!')
  {
    mPrintf(errU,ERROR,METHOD,0,"Process Monitor Server \"%s\" no more "
            "available (in error) on node \"%s\".",srv.c_str(),node.c_str());
  }
  else if(status=='-')
  {
    mPrintf(errU,WARN,METHOD,0,"Process Monitor Server \"%s\" no more "
            "available (removed) on node \"%s\".",srv.c_str(),node.c_str());
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
/* srv@node ==> srv, node */
/* E.g.: input:   s = "/FMC/MYHOST/ps/fmc@myhost.mydomain"                   */
/*       output:  srv = "/FMC/MYHOST/ps"   node = "myhost.mydomain"          */
void PsMoniSrvSeeker::getServer(char* s,string& srv,string& node)
{
  char* at=strchr(s,'@');
  *at=0;
  srv=s;
  node=at+1;
  if(unlikely(deBug & 0x2))
  {
    mPrintf(errU,VERB,METHOD,0,"Node: \"%s\"; SVC: \"%s\".",node.c_str(),
            srv.c_str());
  }
  return;
}
/*****************************************************************************/
int PsMoniSrvSeeker::matchSrvPattern(string srv)
{
  vector<string>::iterator srvIt;         /* service pattern vector iterator */
  /* ----------------------------------------------------------------------- */
  /* compare against a generic psSrv pattern */
  if(fnmatch(SVC_HEAD"/*/ps",srv.c_str(),0)==FNM_NOMATCH)
  {
    return FNM_NOMATCH;
  }
  /* compare against pattern selected using cmd-line */
  for(srvIt=srvPttnLs.begin();srvIt!=srvPttnLs.end();srvIt++)
  {
    if(!fnmatch(srvIt->c_str(),srv.c_str(),0))return 0;
  }
  return FNM_NOMATCH;
} 
/*****************************************************************************/
/* constructor */
PsMoniSrvSeeker::PsMoniSrvSeeker(const char* dnsSrvLsSvcPath,
                                 vector<string>& srvPttnLs,
                                 vector<string>& cmdPttnLs,
                                 vector<string>& utgidPttnLs)
: DimInfo(dnsSrvLsSvcPath,-1), dnsSrvLsSvcPath(dnsSrvLsSvcPath),
  srvPttnLs(srvPttnLs), cmdPttnLs(cmdPttnLs),
  utgidPttnLs(utgidPttnLs)
{
  psSrvN=0;
}
/* ######################################################################### */
