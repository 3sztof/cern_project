/* ######################################################################### */
/*
 * $Log: LogDimErrorHandler.C,v $
 * Revision 1.1  2009/02/09 16:28:23  galli
 * Initial revision
 */
/* ######################################################################### */
#include "LogDimErrorHandler.IC"
#define BUF_SIZE 4096                             /* size of logger messages */
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
extern int viewOutput;
extern char *pName;
extern char *utgid;
extern FILE *outFP;
extern int lStd;
extern pthread_mutex_t prLock;
/* ######################################################################### */
void LogDimErrorHandler::errorHandler(int severity,int errCode,char *errMsg)
{   
  int u;
  /*.........................................................................*/
    if(severity>3||severity<0)severity=3;
    if(viewOutput)
    {
      char errBuf[BUF_SIZE]="";
      snprintf(errBuf,BUF_SIZE,"%s(%s): %s(): DIM error message: \"%s\"! (DIM "
               "error code = %d).\n",pName,utgid,METHOD,errMsg,errCode);
      LogUtils::headerPrepend(errBuf,severity+3);
      pthread_mutex_lock(&prLock);
      LogUtils::printC(outFP,severity+3,-1,color,errBuf);
      pthread_mutex_unlock(&prLock);
    }
    u=errU&~lStd;
    if(severity==3)u|=L_SYS;
    if(severity==3 && !viewOutput)u|=lStd;
    mPrintf(u,severity+3,METHOD,0,"DIM error message: \"%s\"! (DIM error "
            "code = %d).",errMsg,errCode);
    if(severity==DIM_FATAL)exit(1);
    return;
  /*.........................................................................*/
}
/*****************************************************************************/
/* constructor */
LogDimErrorHandler::LogDimErrorHandler(int color)
{
  this->color=color;
  DimClient::addErrorHandler(this);
}
/* ######################################################################### */
