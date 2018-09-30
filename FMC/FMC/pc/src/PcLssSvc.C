/* ######################################################################### */
/*
 * $Log: PcLssSvc.C,v $
 * Revision 1.3  2008/09/17 13:27:59  galli
 * get cluster as global
 * bug fized in PcLssSvc::serviceHandler()
 *
 * Revision 1.2  2008/09/16 09:20:51  galli
 * minor changes
 *
 * Revision 1.1  2008/09/16 07:21:56  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcLssSvc.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern PcCluster cluster;
/* ######################################################################### */
/* process the "add" DIM command */
void PcLssSvc::serviceHandler()
{
  lssSvcData=cluster.getSchPsLss(":",",",".\n","[","]",",");
  /*-------------------------------------------------------------------------*/
  setData((void*)lssSvcData.c_str(),1+lssSvcData.size());
}
/* ######################################################################### */
