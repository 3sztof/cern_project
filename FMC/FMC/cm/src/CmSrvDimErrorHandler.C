/* ######################################################################### */
/*
 * $Log: CmSrvDimErrorHandler.C,v $
 * Revision 1.2  2008/10/10 11:40:30  galli
 * minor changes
 *
 * Revision 1.1  2008/10/01 11:01:02  galli
 * Initial revision
 */
/* ######################################################################### */
#include "CmSrvDimErrorHandler.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
/* ######################################################################### */
void CmSrvDimErrorHandler::errorHandler(int severity,int errCode,char *errMsg)
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
CmSrvDimErrorHandler::CmSrvDimErrorHandler()
{
  DimServer::addErrorHandler(this);
}
/* ######################################################################### */
