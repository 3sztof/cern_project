/* ######################################################################### */
/*
 * $Log: PsMoniDimErrorHandler.C,v $
 * Revision 1.1  2009/02/23 14:14:58  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PsMoniDimErrorHandler.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
/* ######################################################################### */
void PsMoniDimErrorHandler::errorHandler(int severity,int errCode,char *errMsg)
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
PsMoniDimErrorHandler::PsMoniDimErrorHandler()
{
  DimClient::addErrorHandler(this);
}
/* ######################################################################### */
