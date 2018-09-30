/* ######################################################################### */
/*
 * $Log: PcDimErrorHandler.C,v $
 * Revision 1.4  2008/10/10 11:33:17  galli
 * minor changes
 *
 * Revision 1.1  2008/09/18 08:05:20  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcDimErrorHandler.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
/* ######################################################################### */
void PcDimErrorHandler::errorHandler(int severity,int errCode,char *errMsg)
{   
  /*.........................................................................*/
  mPrintf(errU|L_SYS,severity+3,__func__,0,"DIM error message: "
          "\"%s\". (DIM error code = %d)!",errMsg,errCode);
  if(severity==DIM_FATAL)exit(1);
  return;
  /*.........................................................................*/
}
/*****************************************************************************/
/* constructor */
PcDimErrorHandler::PcDimErrorHandler()
{
  DimClient::addErrorHandler(this);
  DimServer::addErrorHandler(this);
}
/* ######################################################################### */
