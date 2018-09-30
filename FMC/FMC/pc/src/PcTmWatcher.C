/* ######################################################################### */
/*
 * $Log: PcTmWatcher.C,v $
 * Revision 1.1  2008/09/19 21:33:06  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcTmWatcher.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern PcCluster cluster;
extern int errU;
extern int deBug;
/* ######################################################################### */
/* called when the service /FMC/task_manager/list is updated */
void PcTmWatcher::infoHandler()
{
  cluster.setRunPss(node,getSize(),(char*)getData());
  if(deBug&0x02)
  {
    cluster.printRunPss(node,INFO);
    mPrintf(errU,VERB,METHOD,0,"node: \"%s\": \"%s\" updated.", node.c_str(),
            tmLsSvcPath.c_str());
  }
  cluster.checkRunning(node);
}
/*****************************************************************************/
/* constructor                                                               */
/*****************************************************************************/
PcTmWatcher::PcTmWatcher(string tmLsSvcPath, string node)
:DimInfo(tmLsSvcPath.c_str(), PC_TM_WATCHER_TIMEOUT, -1),
 node(node),
 tmLsSvcPath(tmLsSvcPath)
{
}
/* ######################################################################### */

