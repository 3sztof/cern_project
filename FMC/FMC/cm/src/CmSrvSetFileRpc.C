/* ######################################################################### */
/*
 * $Log: CmSrvSetFileRpc.C,v $
 * Revision 1.6  2008/10/07 15:11:59  galli
 * bug fixed
 *
 * Revision 1.4  2008/10/05 22:38:17  galli
 * Can set permissions, uid and gid of the written file
 *
 * Revision 1.2  2008/10/02 07:40:55  galli
 * use global CmAcl acl instead of int unSafeMode and char *allowedFiles[]
 *
 * Revision 1.1  2008/10/01 11:01:02  galli
 * Initial revision
 */
/* ######################################################################### */
#include "CmSrvSetFileRpc.IC"
/* ######################################################################### */
#define iMode (*((uint64_t*)iBuf))
#define iUid  (*((uint64_t*)(iBuf+sizeof(uint64_t))))
#define iGid  (*((uint64_t*)(iBuf+2*sizeof(uint64_t))))
#define iStr  (iBuf+3*sizeof(uint64_t))
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
extern CmAcl acl;
extern string confFileName;
extern string aclFileName;
/* ######################################################################### */
/* input data ("X:3;C")                                                      */
/*   (uint64_t) mode_t st_mode  iMode                                        */
/*   (uint64_t) uid_t  st_uid   iUid                                         */
/*   (uint64_t) gid_t  st_gid   iGid                                         */
/*   char* username\thostname filePath\0fileContent\0                        */
/* output data ("C")                                                         */
/*   error message                                                           */
/*****************************************************************************/
void CmSrvSetFileRpc::rpcHandler()
{
  char *iBuf=NULL;
  int oFD=-1;                                      /* output file descriptor */
  mode_t mode;
  uid_t uid;
  gid_t gid;
  string user, host;
  string filePath;
  char *fileContent=NULL;
  ssize_t fileSize=0;
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
  /*-------------------------------------------------------------------------*/
  /* unpack data */
  mode=(mode_t)iMode;
  uid=(uid_t)iUid;
  if(uid==~(uid_t)0)uid=getuid();
  gid=(gid_t)iGid;
  if(gid==~(gid_t)0)gid=getgid();
  filePath=string(iStr);
  /*-------------------------------------------------------------------------*/
  /* check user@host permission */
  /* strips permission out of filePath and return user and host */
  if(!acl.isUserAllowed("write",filePath,user,host))
  {
    string s;
    s="Write access to user \""+user+"\" at host \""+host+"\" REFUSED by the "+
      "FMC Configuration Manager Server (see file \""+aclFileName+"\")!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    setData((char*)s.c_str());
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* unpack data */
  fileContent=iStr+strlen(iStr)+1;
  fileSize=1+strlen(fileContent);
  /*-------------------------------------------------------------------------*/
  /* check file access permission */
  if(!acl.isFileWritable(filePath))
  {
    string s;
    s="Write access to file \""+filePath+"\" by the FMC Configuration Manager "+
      "Server is forbidden (see configuration file \""+confFileName+"\")!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    setData((char*)s.c_str());
    return;
  }
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,DEBUG,METHOD,0,"Going to write file \"%s\" (%d Bytes, mode "
          "%04o, uid %lu, gid %lu)...", filePath.c_str(), fileSize, mode, uid,
          gid);
  if(deBug&0x08)
  {
    mPrintf(errU,VERB,METHOD,0,"File content: \"%s\"\n",fileContent);
  }
  /*-------------------------------------------------------------------------*/
  /* open file */
  oFD=open(filePath.c_str(),O_WRONLY|O_CREAT|O_TRUNC,mode);
  if(oFD==-1)                                                /* open failure */
  {
    sErrno=errno;
    string s;
    s="Can't open file \""+filePath+"\" for writing. open(2) error: "+
      strerror(sErrno)+"!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    setData((char*)s.c_str());
    return;
  }                                                          /* open failure */
  /*-------------------------------------------------------------------------*/
  /* write file */
  ssize_t written=write(oFD,fileContent,fileSize);
  sErrno=errno;
  close(oFD);
  if(written==-1)                                           /* write failure */
  {
    string s;
    s="Can't write file \""+filePath+"\". write(2) error: "+
      strerror(sErrno)+"!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    setData((char*)s.c_str());
    return;
  }                                                         /* write failure */
  /* handle incomplete write */
  else if(written<fileSize)                              /* incomplete write */
  {
    string s;
    s="Problem writing file \""+filePath+"\" for writing: "+
      FmcUtils::ultos(written)+" B written out of "+
      FmcUtils::ultos(fileSize)+" B!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    setData((char*)s.c_str());
    return;
  }                                                      /* incomplete write */
  /*-------------------------------------------------------------------------*/
  string s0;
  s0="File \""+filePath+"\" written ("+FmcUtils::ultos(fileSize)+" Bytes).";
  mPrintf(errU,INFO,METHOD,0,"%s",s0.c_str());
  s0="[INFO] "+s0;
  /*-------------------------------------------------------------------------*/
  /* change the ownership */
  string s1;
  if(chown(filePath.c_str(),uid,gid))
  {                                                         /* chown failure */
    sErrno=errno;
    s1="Can't change ownership of file \""+filePath+"\". chown(2) error: "+
      strerror(sErrno)+"!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s1.c_str());
    s1="\n[ERROR] "+s1;
  }                                                         /* chown failure */
  else
  {                                                       /* chown succeeded */
    s1="Ownership of file \""+filePath+"\" transferred to UID "+
       FmcUtils::ultos(uid)+" and GID "+FmcUtils::ultos(gid)+".";
    mPrintf(errU,INFO,METHOD,0,"%s",s1.c_str());
    s1="\n[INFO] "+s1;
  }                                                       /* chown succeeded */
  /*-------------------------------------------------------------------------*/
  /* change the permissions */
  string s2;
  if(chmod(filePath.c_str(),mode))
  {                                                         /* chmod failure */
    sErrno=errno;
    s2="Can't change permissions of file \""+filePath+"\". chmod(2) error: "+
      strerror(sErrno)+"!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s2.c_str());
    s2="\n[ERROR] "+s2;
  }                                                         /* chmod failure */
  else
  {                                                       /* chmod succeeded */
    s2="Permissions of file \""+filePath+"\" set to "+FmcUtils::ultoso(mode)+
       ".";
    mPrintf(errU,INFO,METHOD,0,"%s",s2.c_str());
    s2="\n[INFO] "+s2;
  }                                                       /* chmod succeeded */
  /*-------------------------------------------------------------------------*/
  string s=s0+s1+s2;
  setData((char*)s.c_str());
  return;
  /*-------------------------------------------------------------------------*/
}
/*****************************************************************************/
/* constructor                                                               */
/*****************************************************************************/
CmSrvSetFileRpc::CmSrvSetFileRpc(string name)
:DimRpc(name.c_str(),"X:3;C","C")
{
}
/* ######################################################################### */
