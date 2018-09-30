/* ######################################################################### */
/*
 * $Log: PcAddCmd.C,v $
 * Revision 1.10  2008/09/26 20:38:17  galli
 * argument processing moved to PcCluster::addSchPs(string,bool)
 *
 * Revision 1.9  2008/09/24 09:06:44  galli
 * minor changes
 *
 * Revision 1.8  2008/09/17 13:11:03  galli
 * get cluster, acl, lsSvc, llSvc and lssSvc from globals
 * if out FIFO do not set then set rdrStdout to true
 * if err FIFO do not set then set rdrStderr to true
 *
 * Revision 1.7  2008/09/16 09:31:43  galli
 * if out FIFO set then set rdrStdout to true
 * if err FIFO set then set rdrStderr to true
 * PcAddCmd::commandHandler() calls llSvc::updateService()
 * PcAddCmd::commandHandler() calls lssSvc::updateService()
 *
 * Revision 1.6  2008/09/15 08:02:36  galli
 * included directives moved to PcAddCmd.IC
 * PcAddCmd::commandHandler() calls lsSvc.updateService()
 *
 * Revision 1.5  2008/09/12 09:35:37  galli
 * avoid a copy of the received DIM command
 *
 * Revision 1.1  2008/09/10 09:02:38  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcAddCmd.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
extern PcCluster cluster;
extern PcAcl acl;
extern PcLsSvc *lsSvc;
extern PcLlSvc *llSvc;
extern PcLssSvc *lssSvc;
/* ######################################################################### */
/* process the "add" DIM command */
void PcAddCmd::commandHandler()
{
  /* vector of node wildcard patterns */
  vector<string>nodePttn;
  /* iterator on nodePttn */
  vector<string>::iterator nIt;
  /*-------------------------------------------------------------------------*/
  /* get the received command string rcvCmd */
  string rcvCmd(getString());         /* getString() inherited by DimCommand */
  /* chech authentication */
  if(!acl.isAllowed("pcAdd",rcvCmd))return;
  cluster.addSchPs(rcvCmd);
  cluster.printSchPs(DEBUG,"Process list: ",";  ",".","(",", ",")");
  /*-------------------------------------------------------------------------*/
  lsSvc->updateService();
  llSvc->updateService();
  lssSvc->updateService();
  return;
}
/* ######################################################################### */
