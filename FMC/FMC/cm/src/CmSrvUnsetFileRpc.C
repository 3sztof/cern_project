/* ######################################################################### */
/*
 * $Log: CmSrvUnsetFileRpc.C,v $
 * Revision 1.4  2008/10/06 14:20:04  galli
 * working version
 *
 * Revision 1.2  2008/10/02 07:38:47  galli
 * use global CmAcl acl instead of int unSafeMode and char *allowedFiles[]
 *
 * Revision 1.1  2008/10/01 11:01:02  galli
 * Initial revision
 */
/* ######################################################################### */
#include "CmSrvUnsetFileRpc.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
extern CmAcl acl;
extern string confFileName;
extern string aclFileName;
/* ######################################################################### */
/* input data ("C")                                                          */
/*   char* username\thostname filePath\0                                     */
/* output data ("C")                                                         */
/*   error message                                                           */
/* ######################################################################### */
void CmSrvUnsetFileRpc::rpcHandler()
{
  char *iBuf=NULL;
  string user, host;
  string filePath;
  int sErrno;                                                 /* saved errno */
  /*-------------------------------------------------------------------------*/
  /* check data existence */
  iBuf=(char*)getData();
  if(!iBuf)
  {
    string s("No rpc arguments!");
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    setData((char*)s.c_str());
    return;
  }
  filePath=string(iBuf);
  /*-------------------------------------------------------------------------*/
  /* check user@host permission */
  /* strips permission out of filePath and return user and host */
  if(!acl.isUserAllowed("remove",filePath,user,host))
  {
    string s;
    s="Remove access to user \""+user+"\" at host \""+host+"\" REFUSED by "+
      "the FMC Configuration Manager Server (see file \""+aclFileName+"\")!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    setData((char*)s.c_str());
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* check file access permission */
  if(!acl.isFileWritable(filePath))
  {
    string s;
    s="Remove access to file \""+filePath+"\" by the FMC Configuration "+
      "Manager "+ "Server is forbidden (see configuration file \""+
      confFileName+"\")!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    setData((char*)s.c_str());
    return;
  }
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,DEBUG,METHOD,0,"Going to remove file \"%s\"...",
          filePath.c_str());
  /*-------------------------------------------------------------------------*/
  /* remove file */
  if(unlink(filePath.c_str())==-1)                          /* remove i-node */
  {                                                        /* remove failure */
    sErrno=errno;
    string s;
    s="Can't remove file \""+filePath+"\". unlink(2) error: "+
      strerror(sErrno)+"!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    setData((char*)s.c_str());
    return;
  }                                                        /* remove failure */
  /*-------------------------------------------------------------------------*/
  string s;
  s="File \""+filePath+"\" removed.";
  mPrintf(errU,INFO,METHOD,0,"%s",s.c_str());
  s="[INFO] "+s;
  setData((char*)s.c_str());
  return;
}
/*****************************************************************************/
/* constructor                                                               */
/*****************************************************************************/
CmSrvUnsetFileRpc::CmSrvUnsetFileRpc(string name)
:DimRpc(name.c_str(),"C","C")
{
}
/* ######################################################################### */
