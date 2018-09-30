/* ######################################################################### */
/*
 * $Log: CmSrvGetFileRpc.C,v $
 * Revision 1.4  2008/10/05 22:39:53  galli
 * minor changes
 *
 * Revision 1.3  2008/10/03 22:37:50  galli
 * bug fixed
 *
 * Revision 1.2  2008/10/03 12:52:42  galli
 * working version
 *
 * Revision 1.1  2008/10/01 11:01:02  galli
 * Initial revision
 */
/* ######################################################################### */
#include "CmSrvGetFileRpc.IC"
/* ######################################################################### */
#define oMtime (*((uint64_t*)oBuf))
#define oSize  (*((uint64_t*)(oBuf+sizeof(uint64_t))))
#define oMode  (*((uint64_t*)(oBuf+2*sizeof(uint64_t))))
#define oUid   (*((uint64_t*)(oBuf+3*sizeof(uint64_t))))
#define oGid   (*((uint64_t*)(oBuf+4*sizeof(uint64_t))))
#define oStr   (oBuf+5*sizeof(uint64_t))
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
extern CmAcl acl;
extern string confFileName;
extern string aclFileName;
/* ######################################################################### */
/* input data  ("C"):                                                        */
/*   char* authString+" "+fileName                                           */
/* output data ("X:5;C"):                                                    */
/*   (uint64_t) time_t st_mtime oMtime (dflt: 0)                             */
/*   (uint64_t) off_t  st_size  oSize  (dflt: 0)                             */
/*   (uint64_t) mode_t st_mode  oMode  (dflt: 0)                             */
/*   (uint64_t) uid_t  st_uid   oUid   (dflt: 99)                            */
/*   (uint64_t) gid_t  st_gid   oGid   (dflt: 99)                            */
/*   char*      fileContent     oStr                                         */
/*****************************************************************************/
void CmSrvGetFileRpc::rpcHandler()
{
  char *data=NULL;
  string rpcArgs;
  string fileName;
  string user, host;
  struct stat fileStat;
  ssize_t readFileLen=0;
  int iFD=-1;                                       /* input file descriptor */
  int sErrno;                                                 /* saved errno */
  /*-------------------------------------------------------------------------*/
  /* check data existence */
  data=(char*)getData();
  if(!data)
  {
    string s("No rpc arguments!");
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    oBufLen=(size_t)(5*sizeof(uint64_t)+s.size()+1);
    oBuf=(char*)realloc(oBuf,oBufLen);
    oMtime=oSize=oMode=0;
    oUid=oGid=99;
    strcpy(oStr,s.c_str());
    setData(oBuf,oBufLen);
    return;
  }
  rpcArgs=string(data);                            /* "data" freed by DimRpc */
  /*-------------------------------------------------------------------------*/
  /* check user@host permission */
  /* strips permission out of rpcArgs and return user and host */
  if(!acl.isUserAllowed("read",rpcArgs,user,host))
  {
    string s;
    s="Read access to user \""+user+"\" at host \""+host+"\" REFUSED by the "+
      "FMC Configuration Manager Server (see file \""+aclFileName+"\")!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    oBufLen=(size_t)(5*sizeof(uint64_t)+s.size()+1);
    oBuf=(char*)realloc(oBuf,oBufLen);
    oMtime=oSize=oMode=0;
    oUid=oGid=99;
    strcpy(oStr,s.c_str());
    setData(oBuf,oBufLen);
    return;
  }
  fileName=rpcArgs;
  /*-------------------------------------------------------------------------*/
  /* check file access permission */
  if(!acl.isFileReadable(fileName))
  {
    string s;
    s="Read access to file \""+fileName+"\" by the FMC Configuration Manager "+
      "Server is forbidden (see configuration file \""+confFileName+"\")!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    oBufLen=(size_t)(5*sizeof(uint64_t)+s.size()+1);
    oBuf=(char*)realloc(oBuf,oBufLen);
    oMtime=oSize=oMode=0;
    oUid=oGid=99;
    strcpy(oStr,s.c_str());
    setData(oBuf,oBufLen);
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* check file accessibility */
  if(access(fileName.c_str(),R_OK)==-1)
  {
    sErrno=errno;
    string s;
    s="Can't access file \""+fileName+"\". access(2) error: "+
      strerror(sErrno)+"!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    oBufLen=(size_t)(5*sizeof(uint64_t)+s.size()+1);
    oBuf=(char*)realloc(oBuf,oBufLen);
    oMtime=oSize=oMode=0;
    oUid=oGid=99;
    strcpy(oStr,s.c_str());
    setData(oBuf,oBufLen);
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* get file size */
  if(lstat(fileName.c_str(),&fileStat)<0)
  {
    sErrno=errno;
    string s;
    s="Can't get info about file \""+fileName+"\". lstat(2) error: "+
      strerror(sErrno)+"!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    oBufLen=(size_t)(5*sizeof(uint64_t)+s.size()+1);
    oBuf=(char*)realloc(oBuf,oBufLen);
    oMtime=oSize=oMode=0;
    oUid=oGid=99;
    strcpy(oStr,s.c_str());
    setData(oBuf,oBufLen);
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* empty file */
  if(!fileStat.st_size)
  {
    string s;
    s="Empty file \""+fileName+"\"!";
    mPrintf(errU,WARN,METHOD,0,"%s",s.c_str());
    s="[WARN] "+s;
    oBufLen=(size_t)(5*sizeof(uint64_t)+s.size()+1);
    oBuf=(char*)realloc(oBuf,oBufLen);
    oMtime=(uint64_t)fileStat.st_mtime;
    oSize=(uint64_t)fileStat.st_size;
    oMode=(uint64_t)fileStat.st_mode;
    oUid=(uint64_t)fileStat.st_uid;
    oGid=(uint64_t)fileStat.st_gid;
    strcpy(oStr,s.c_str());
    setData(oBuf,oBufLen);
    return;
  }
  /*-------------------------------------------------------------------------*/
  /* allocate buffer to store confFile */
  oBufLen=(size_t)(5*sizeof(uint64_t)+(float)fileStat.st_size*1.2+1);
  oBuf=(char*)realloc(oBuf,oBufLen);
  /*-------------------------------------------------------------------------*/
  /* set file stat */
  oBuf=(char*)realloc(oBuf,oBufLen);
  oMtime=(uint64_t)fileStat.st_mtime;
  oSize=(uint64_t)fileStat.st_size;
  oMode=(uint64_t)fileStat.st_mode;
  oUid=(uint64_t)fileStat.st_uid;
  oGid=(uint64_t)fileStat.st_gid;
  /*-------------------------------------------------------------------------*/
  /* open file */
  iFD=open(fileName.c_str(),O_RDONLY);
  if(iFD==-1)                                                /* open failure */
  {
    sErrno=errno;
    string s;
    s="Can't open file \""+fileName+"\" for reading. open(2) error: "+
      strerror(sErrno)+"!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    oBufLen=(size_t)(5*sizeof(uint64_t)+s.size()+1);
    oBuf=(char*)realloc(oBuf,oBufLen);
    strcpy(oStr,s.c_str());
    setData(oBuf,oBufLen);
    return;
  }                                                          /* open failure */
  /*-------------------------------------------------------------------------*/
  /* read file */
  readFileLen=read(iFD,oStr,oBufLen-5*sizeof(uint64_t));
  sErrno=errno;
  close(iFD);
  if(readFileLen==-1)                                        /* read failure */
  {
    string s;
    s="Can't read file \""+fileName+"\". read(2) error: "+
      strerror(sErrno)+"!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    oBufLen=(size_t)(5*sizeof(uint64_t)+s.size()+1);
    oBuf=(char*)realloc(oBuf,oBufLen);
    strcpy(oStr,s.c_str());
    setData(oBuf,oBufLen);
    return;
  }                                                          /* read failure */
  oStr[readFileLen]='\0';
  /*-------------------------------------------------------------------------*/
  /* handle incomplete read */
  if(readFileLen<fileStat.st_size)                        /* incomplete read */
  {
    string s;
    s="Problem reading file \""+fileName+"\" for reading: "+
      FmcUtils::ultos(readFileLen)+" B read out of "+
      FmcUtils::ultos(fileStat.st_size)+" B!";
    mPrintf(errU,ERROR,METHOD,0,"%s",s.c_str());
    s="[ERROR] "+s;
    s=string(oStr)+"\n"+s;
    oBufLen=(size_t)(5*sizeof(uint64_t)+s.size()+1);
    oBuf=(char*)realloc(oBuf,oBufLen);
    strcpy(oStr,s.c_str());
    setData(oBuf,oBufLen);
    return;
  }                                                       /* incomplete read */
  /*-------------------------------------------------------------------------*/
  /* send data */
  setData(oBuf,5*sizeof(uint64_t)+1+readFileLen);
  mPrintf(errU,INFO,METHOD,0,"File \"%s\" read (%lu Bytes).",
          fileName.c_str(), readFileLen);
  /*-------------------------------------------------------------------------*/
  return;
}
/*****************************************************************************/
/* constructor                                                               */
/*****************************************************************************/
CmSrvGetFileRpc::CmSrvGetFileRpc(string name):DimRpc(name.c_str(),"C","X:5;C")
{
  oBufLen=5*sizeof(uint64_t)+1;
  oBuf=(char*)malloc(oBufLen);
}
/* ######################################################################### */
