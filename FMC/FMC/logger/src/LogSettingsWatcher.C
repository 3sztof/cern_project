/* ######################################################################### */
/*
 * $Log: LogSettingsWatcher.C,v $
 * Revision 1.2  2009/02/12 15:01:58  galli
 * added method getSvcPath()
 *
 * Revision 1.1  2009/02/09 16:28:23  galli
 * Initial revision
 */
/* ######################################################################### */
#include "LogSettingsWatcher.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern pthread_mutex_t csLock;                       /* change settings lock */
extern list<string> csList;        /* list of servers which changed settings */
extern pthread_t mainPtid;           /* thread identifier of the main thread */
/* ######################################################################### */
/* called when setting in DIM SVC svcPath are changed */
void LogSettingsWatcher::infoHandler()
{
  char lastLogSvcName[DIM_SVC_LEN+1];
  char *p=NULL;
  list<string>::iterator it;
  snprintf(lastLogSvcName,sizeof(lastLogSvcName),"%s",getName());
  p=strrchr(lastLogSvcName,'/');
  if(p)*p='\0';
  else return;
  strcat(lastLogSvcName,"/last_log");
  pthread_mutex_lock(&csLock);
  int found=0;
  for(it=csList.begin();it!=csList.end();it++)
  {
    if(!it->compare(lastLogSvcName))
    {
      found=1;
      break;
    }
  }
  if(!found)
  {
    csList.push_front(lastLogSvcName);
  }
  pthread_mutex_unlock(&csLock);
  pthread_kill(mainPtid,SIGUSR1);          /* speed-up action of main thread */
}
/*****************************************************************************/
char *LogSettingsWatcher::getSvcPath(void)
{
  return getName();
}
/*****************************************************************************/
/* constructor */
LogSettingsWatcher::LogSettingsWatcher(const char *svcPath)
:DimInfo(svcPath,-1)
{
}
/* ######################################################################### */

