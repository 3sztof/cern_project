/* ######################################################################### */
/*
 * $Log: PsMoniSvcWatcher.C,v $
 * Revision 1.15  2009/07/22 21:18:58  galli
 * bug fixed
 *
 * Revision 1.14  2009/07/21 15:17:35  galli
 * bug fixed
 *
 * Revision 1.13  2009/04/03 08:18:17  galli
 * added status N (new process)
 *
 * Revision 1.12  2009/03/19 07:57:56  galli
 * call setActive() in constructor
 *
 * Revision 1.11  2009/03/17 16:44:45  galli
 * cancel colors on the first refresh
 * more colors
 *
 * Revision 1.10  2009/03/17 14:53:58  galli
 * colored fields
 *
 * Revision 1.9  2009/03/12 09:44:05  galli
 * bug fixed
 *
 * Revision 1.7  2009/02/26 20:15:11  galli
 * first working version
 *
 * Revision 1.1  2009/02/25 09:37:14  galli
 * Initial revision
 */
/* ######################################################################### */
/* Class which inherits from DimInfo and subscribes to a DIM service         */
/* "/FMC/ps/details/monitored/processes/CMD{UTGID}TID/data". Instantiated    */
/* once per each published process-dedicated psSrv SVC.                      */
/* ######################################################################### */
/* in monitor/include */
#include "PsMoniSvcWatcher.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int deBug;
extern int errU;
extern list<string> outTbl;
extern pthread_mutex_t outTblLock;
/* ######################################################################### */
void PsMoniSvcWatcher::infoHandler()
{
  //int rawDataSz;
  char *rawData;
  char *cmdline;
  static char field[16];
  char status;
  list<string>::iterator otIt;           /* iterator over output table lines */
  /* ----------------------------------------------------------------------- */
  //rawDataSz=getSize();
  rawData=(char*)getData();
  data=(psData_t*)rawData;
  cmdline=rawData+17*sizeof(int32_t)+4*sizeof(float)+UPDATETIME_LEN+1+CMD_LEN+
          1+UTGID_LEN+1+USER_LEN+1;
  //status=data->status==1 ? 'R' : 'X';
  ihCallCount++;
  if(!data->status)status='X';
  else if(ihCallCount<6)status='N';
  else status='R';
  /* ----------------------------------------------------------------------- */
  pthread_mutex_lock(&outTblLock);
  psLine.clear();
  /* 0. HOST */
  psLine.append("%%W%");
  psLine.append(hostName);
  /* 1. TGID */
  psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->tid));
  /* 2. CMD */
  psLine.append("%%W%");
  psLine.append(data->cmd);
  /* 3. UTGID */
  psLine.append("%%W%");
  psLine.append(data->utgid);
  /* 4. S */
  //if(firstInfoHandlerCall) psLine.append("%%W%");
  //else if(status=='R') psLine.append("%%W%");
  if(status=='R') psLine.append("%%W%");
  else if(status=='N') psLine.append("%%G%");
  else psLine.append("%%R%");
  psLine.push_back(status);
  /* 5. NLWP */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->nlwp>oldData.nlwp) psLine.append("%%R%");
  else if(data->nlwp<oldData.nlwp) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->nlwp));
  /* 6. P */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->processor!=oldData.processor) psLine.append("%%R%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->processor));
  /* 7. %CPU */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->pcpu>oldData.pcpu+1.00) psLine.append("%%R%");
  else if(data->pcpu>99.0) psLine.append("%%Y%");
  else if(data->pcpu<oldData.pcpu-1.00) psLine.append("%%G%");
  else psLine.append("%%W%");
  snprintf(field,16,"%.2f",data->pcpu);
  psLine.append(field);
  /* 8. %MEM */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->pmem>oldData.pmem+0.01) psLine.append("%%R%");
  else if(data->pmem>90.0) psLine.append("%%Y%");
  else if(data->pmem<oldData.pmem-0.01) psLine.append("%%G%");
  else psLine.append("%%W%");
  snprintf(field,16,"%.2f",data->pmem);
  psLine.append(field);
  /* 9. MIN_FLT */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->minFltRate>oldData.minFltRate*100.0) psLine.append("%%R%");
  else if(data->minFltRate<oldData.minFltRate/100.0) psLine.append("%%G%");
  else psLine.append("%%W%");
  snprintf(field,16,"%.2e",data->minFltRate);
  psLine.append(field);
  /* 10. MAJ_FLT */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->majFltRate>oldData.majFltRate*10.0) psLine.append("%%R%");
  else if(data->majFltRate>10.0) psLine.append("%%Y%");
  else if(data->majFltRate<oldData.majFltRate/10.0) psLine.append("%%G%");
  else psLine.append("%%W%");
  snprintf(field,16,"%.2e",data->majFltRate);
  psLine.append(field);
  /* 11. VSIZE */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->vm_size>oldData.vm_size) psLine.append("%%R%");
  else if(data->vm_size<oldData.vm_size) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->vm_size));
  /* 12. LOCK */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->vm_lock>oldData.vm_lock) psLine.append("%%R%");
  else if(data->vm_lock<oldData.vm_lock) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->vm_lock));
  /* 13. RSS */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->vm_rss>oldData.vm_rss) psLine.append("%%R%");
  else if(data->vm_rss<oldData.vm_rss) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->vm_rss));
  /* 14. DATA */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->vm_data>oldData.vm_data) psLine.append("%%R%");
  else if(data->vm_data<oldData.vm_data) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->vm_data));
  /* 15. STACK */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->vm_stack>oldData.vm_stack) psLine.append("%%R%");
  else if(data->vm_stack<oldData.vm_stack) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->vm_stack));
  /* 16. EXE */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->vm_exe>oldData.vm_exe) psLine.append("%%R%");
  else if(data->vm_exe<oldData.vm_exe) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->vm_exe));
  /* 17. LIB */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->vm_lib>oldData.vm_lib) psLine.append("%%R%");
  else if(data->vm_lib<oldData.vm_lib) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->vm_lib));
  /* 18. SHARE */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->share>oldData.share) psLine.append("%%R%");
  else if(data->share<oldData.share) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->share));
  /* 19. RT */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->rtprio>oldData.rtprio) psLine.append("%%R%");
  else if(data->rtprio<oldData.rtprio) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(data->rtprio));
  /* 20. NI */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->nice<oldData.nice) psLine.append("%%R%");
  else if(data->nice>oldData.nice) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ltos(data->nice));
  /* 21. PR */
  if(firstInfoHandlerCall) psLine.append("%%W%");
  else if(data->prio<oldData.prio) psLine.append("%%R%");
  else if(data->prio>oldData.prio) psLine.append("%%G%");
  else psLine.append("%%W%");
  psLine.append(FmcUtils::ltos(data->prio));
  /* 22. LAST_UPDATE */
  psLine.append("%%W%");
  psLine.append(data->lastUpdateTimeStr);
  /* 23. USER */
  psLine.append("%%W%");
  psLine.append(data->euser);
  /* 24. CMDLINE */
  psLine.append("%%W%");
  psLine.append(cmdline);
  psLine.append("%%");
  /* ----------------------------------------------------------------------- */
  if(tid==(pid_t)data->tid)
  {
    /* substitute line in output table outTbl */
    for(otIt=outTbl.begin();otIt!=outTbl.end();otIt++)
    {
      if(getHost(*otIt)==hostName && getTid(*otIt)==tid)
      {
        *otIt=psLine;
        break;
      }
    }
  }
  else   /* should not happen but in fact it can happen at the first refresh */
  {
    if(unlikely(deBug & 0x10))
    {
      mPrintf(errU,ERROR,METHOD,0,"Service \"%s\" contains data for tid %d!",
              psSvcPath.c_str(),data->tid);
    }
  }
  /* ----------------------------------------------------------------------- */
  pthread_mutex_unlock(&outTblLock);
  /* ----------------------------------------------------------------------- */
  /* save old data in oldData */
  if(data)memcpy(&oldData,data,sizeof(psData_t));
  /* ----------------------------------------------------------------------- */
  if(unlikely(firstInfoHandlerCall))firstInfoHandlerCall=false;
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
string PsMoniSvcWatcher::getPsSvcPath(void)
{
  return psSvcPath;
}
/*****************************************************************************/
string PsMoniSvcWatcher::getHost(string psLine)
{
  size_t b=4,e=4;
  /* ----------------------------------------------------------------------- */
  e=psLine.find("%%",b);                                  /* end first field */
  return psLine.substr(b,e-b);
}
/*****************************************************************************/
pid_t PsMoniSvcWatcher::getTid(string psLine)
{
  size_t b=4,e=4;
  /* ----------------------------------------------------------------------- */
  e=psLine.find("%%",b);                                  /* end first field */
  b=e+4;                                               /* begin second field */
  e=psLine.find("%%",b);                                 /* end second field */
  return (pid_t)strtoul(psLine.substr(b,e-b).c_str(),(char**)NULL,0);
}
/*****************************************************************************/
/* constructor */
PsMoniSvcWatcher::PsMoniSvcWatcher(const char *psSvcPath)
: DimInfo(psSvcPath,-1), psSvcPath(psSvcPath)
{
  size_t b,e;
  string path;
  list<string>::iterator otIt;           /* iterator over output table lines */
  /* ----------------------------------------------------------------------- */
  setActive();
  ihCallCount=0;
  /* ----------------------------------------------------------------------- */
  /* clean oldData */
  data=NULL;
  memset(&oldData,0,sizeof(psData_t));
  /* ----------------------------------------------------------------------- */
  /* get hostName and tid from psSvcPath */
  path=this->psSvcPath;
  b=1+path.find_first_of('/');
  b=1+path.find_first_of('/',b);
  e=path.find_first_of('/',b);
  hostName=path.substr(b,e-b);
  hostName=FmcUtils::toLower(hostName);
  b=1+path.find_first_of('}',e);
  e=path.find_first_of('/',b);
  tid=(pid_t)strtoul(path.substr(b,e-b).c_str(),(char**)NULL,0);
  /* ----------------------------------------------------------------------- */
  /* prepare output line */
  psLine.clear();
  /* 0. HOST */
  psLine.append("%%W%");
  psLine.append(hostName);
  /* 1. TGID */
  psLine.append("%%W%");
  psLine.append(FmcUtils::ultos(tid));
  /* 2-24 */
  for(int i=2;i<25;i++) psLine.append("%%W% ");
  psLine.append("%%");
  /* ----------------------------------------------------------------------- */
  firstInfoHandlerCall=true;
  /* ----------------------------------------------------------------------- */
  /* add line to output table outTbl */
  pthread_mutex_lock(&outTblLock);
  /* skip lines preceding in order */
  for(otIt=outTbl.begin();otIt!=outTbl.end();otIt++)
  {
    if(getHost(*otIt)>hostName || 
       (getHost(*otIt)==hostName && getTid(*otIt)>tid))
    {
      break;
    }
  }
  /* insert line */
  outTbl.insert(otIt,string(psLine));
  pthread_mutex_unlock(&outTblLock);
  /* ----------------------------------------------------------------------- */
  if(unlikely(deBug & 0x10))
  {
    mPrintf(errU,DEBUG,METHOD,0,"Class PsMoniSvcWatcher instantiated for "
            "SVC: \"%s\".",this->psSvcPath.c_str());
  }
}
/*****************************************************************************/
/* destructor */
PsMoniSvcWatcher::~PsMoniSvcWatcher()
{
  list<string>::iterator otIt;           /* iterator over output table lines */
  /* ----------------------------------------------------------------------- */
  /* remove line from output table outTbl */
  pthread_mutex_lock(&outTblLock);
  for(otIt=outTbl.begin();otIt!=outTbl.end();otIt++)
  {
    if(getHost(*otIt)==hostName && getTid(*otIt)==tid)
    {
      outTbl.erase(otIt);
      break;
    }
  }
  pthread_mutex_unlock(&outTblLock);
  /* ----------------------------------------------------------------------- */
  if(unlikely(deBug & 0x10))
  {
    mPrintf(errU,DEBUG,METHOD,0,"Class PsMoniSvcWatcher deallocated for "
            "SVC: \"%s\".",this->psSvcPath.c_str());
  }
}
/* ######################################################################### */
