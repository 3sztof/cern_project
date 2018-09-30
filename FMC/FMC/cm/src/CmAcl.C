/* ######################################################################### */
/*
 * $Log: CmAcl.C,v $
 * Revision 1.5  2008/10/07 08:44:39  galli
 * minor changes
 *
 * Revision 1.3  2008/10/03 22:08:32  galli
 * isUserAllowed() returns also user and host
 *
 * Revision 1.2  2008/10/03 06:52:49  galli
 * if authentication off, then isFileReadable() and isFileWritable()
 * always return true
 * Increased control and diagnostics in isUserAllowed()
 *
 * Revision 1.1  2008/10/02 07:10:57  galli
 * Initial revision
 */
/* ######################################################################### */
#include "CmAcl.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
/* ######################################################################### */
/* get methods                                                               */
/*****************************************************************************/
bool CmAcl::isFileReadable(const string &file)
{
  vector<string>::iterator it;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  if(!authenticated)return true;
  for(it=readAllwdFiles.begin();it!=readAllwdFiles.end();it++)
  {
    if(*it==file || !fnmatch(it->c_str(),file.c_str(),0))
    {
      found=true;
      break;
    }
  }
  return found;
}
/*****************************************************************************/
bool CmAcl::isFileWritable(const string &file)
{
  vector<string>::iterator it;
  bool found=false;
  /*-------------------------------------------------------------------------*/
  if(!authenticated)return true;
  for(it=writeAllwdFiles.begin();it!=writeAllwdFiles.end();it++)
  {
    if(*it==file || !fnmatch(it->c_str(),file.c_str(),0))
    {
      found=true;
      break;
    }
  }
  return found;
}
/*****************************************************************************/
/* rpcArgs is modified (auth strings and leading blanks are stripped out).   */
/* rpc is used only to be included in diagnostic messages                    */
bool CmAcl::isUserAllowed(const string &rpc, string &rpcArgs,
                          string &authUser, string &authHost)
{
  size_t len,sep,b,e;
  //string authString,authUser,authHost;
  string authString;
  bool allowed;
  vector<entry>::iterator eIt;
  /*-------------------------------------------------------------------------*/
  authUser="";
  authHost="";
  /*-------------------------------------------------------------------------*/
  /* if authentication off just strip off authentication string */
  if(!authenticated)                                    /* no authentication */
  {
    sep=rpcArgs.find('\f');
    /*.......................................................................*/
    if(sep!=string::npos)                             /* auth string present */
    {
      rpcArgs.at(sep)='@';                          /* replace '\f' with '@' */
      len=rpcArgs.size();
      b=rpcArgs.find_first_not_of(" \t",0);               /* first non-blank */
      if(b==string::npos)
      {
        mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\": Malformed command string "
                "received: no non-blank characters!", rpc.c_str(),
                rpcArgs.c_str());
        return false;
      }
      e=rpcArgs.find_first_of(" \t",b);     /* first blank after auth string */
      if(e==string::npos)
      {
        mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\": Malformed command string "
                "received: no blank characters after authentication string!",
                rpc.c_str(), rpcArgs.c_str());
        return false;
      }
      authString=rpcArgs.substr(b,e-b);                /* extract authString */
      len=authString.size();
      sep=authString.find('@');
      if(sep!=string::npos && sep!=0 && sep<len-1)
      {
        authUser=authString.substr(0,sep);
        authHost=authString.substr(sep+1,len);
      }
      else
      {
        authUser="unknown";
        authHost="unknown";
      }
      b=rpcArgs.find_first_not_of(" \t",e);/* first non-blank after auth str */
      if(b==string::npos)
      {
        mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\": Malformed command string "
                "received: no non-blank characters after authentication "
                "string!", rpc.c_str(), rpcArgs.c_str());
        return false;
      }
      rpcArgs=rpcArgs.substr(b,len-b);     /* strip out auth string & blanks */
      mPrintf(errU,DEBUG,METHOD,0,"Allowed RPC: \"%s %s\" (without "
              "authentication) from user: \"%s\" at host: \"%s\".",
              rpc.c_str(), rpcArgs.c_str(), authUser.c_str(),
              authHost.c_str());
    }                                                 /* auth string present */
    /*.......................................................................*/
    else                                          /* auth string not present */
    {
      mPrintf(errU,DEBUG,METHOD,0,"Allowed RPC: \"%s %s\" (without "
              "authentication).", rpc.c_str(), rpcArgs.c_str());
    }                                             /* auth string not present */
    /*.......................................................................*/
    return true;
  }                                                     /* no authentication */
  /*-------------------------------------------------------------------------*/
  /* check if user/host separator '\f' does exist and replace it with '@' */
  sep=rpcArgs.find('\f');
  if(sep==string::npos)
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\": Missing authentication "
            "string user@hostname!", rpc.c_str(), rpcArgs.c_str());
    return false;
  }
  else
  {
    rpcArgs.at(sep)='@';                            /* replace '\f' with '@' */
  }
  /*-------------------------------------------------------------------------*/
  /* extract authString */
  len=rpcArgs.size();
  b=rpcArgs.find_first_not_of(" \t",0);                   /* first non-blank */
  if(b==string::npos)
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\": Malformed command string "
            "received: no non-blank characters!",rpc.c_str(),rpcArgs.c_str());
    return false;
  }
  e=rpcArgs.find_first_of(" \t",b);         /* first blank after auth string */
  if(e==string::npos)
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\": Malformed command string "
            "received: no blank characters after authentication string!",
            rpc.c_str(), rpcArgs.c_str());
    return false;
  }
  authString=rpcArgs.substr(b,e-b);                    /* extract authString */
  /*-------------------------------------------------------------------------*/
  /* extract bare rpc arguments */
  b=rpcArgs.find_first_not_of(" \t",e); /* first non-blank after auth string */
  if(b==string::npos || b>=len)
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\": Malformed command string "
            "received: no non-blank characters after authentication string!",
            rpc.c_str(), rpcArgs.c_str());
    return false;
  }
  rpcArgs=rpcArgs.substr(b,len-b);         /* strip out auth string & blanks */
  /*-------------------------------------------------------------------------*/
  /* check authString format */
  len=authString.size();
  sep=authString.find('@');
  if(sep==string::npos)
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\": Malformed authentication "
            "string \"%s\" received: missing user/hostname separator (must be "
            "user@hostname)!",rpc.c_str(), rpcArgs.c_str(),
            authString.c_str());
    return false;
  }
  if(sep==0)
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\". Malformed authentication "
            "string \"%s\" received: empty user field (must be "
            "user@hostname)!", rpc.c_str(), rpcArgs.c_str(),
            authString.c_str());
    return false;
  }
  if(sep>=len-1)
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\". Malformed authentication "
            "string \"%s\" received: empty hostname field (must be "
            "user@hostname)!", rpc.c_str(), rpcArgs.c_str(),
            authString.c_str());
    return false;
  }
  authUser=authString.substr(0,sep);
  authHost=authString.substr(sep+1,len);
  if(!authUser.size())
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\". Malformed authentication "
            "string \"%s@%s\" received empty user field (must be "
            "user@hostname)!", rpc.c_str(), rpcArgs.c_str(), authUser.c_str(),
            authHost.c_str());
    return false;
  }
  if(!authHost.size())
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\". Malformed authentication "
            "string \"%s@%s\" received empty hostname field (must be "
            "user@hostname)!", rpc.c_str(), rpcArgs.c_str(), authUser.c_str(),
            authHost.c_str());
    return false;
  }
  if(deBug&0x2)
  {
    mPrintf(errU,VERB,METHOD,0,"RPC: \"%s %s\". Received formally correct "
            "authentication string: \"%s@%s\".", rpc.c_str(), rpcArgs.c_str(),
            authUser.c_str(), authHost.c_str());
  }
  /*-------------------------------------------------------------------------*/
  /* check permission */
  for(eIt=acl.begin(),allowed=false;eIt!=acl.end();eIt++)
  {
    if(!fnmatch(eIt->user.c_str(),authUser.c_str(),0) &&
       !fnmatch(eIt->host.c_str(),authHost.c_str(),0))
    {
      allowed=true;
      break;
    }
  }
  if(!allowed)
  {
    mPrintf(errU,ERROR,METHOD,0,"RPC: \"%s %s\" REFUSED! User \"%s\" at host "
            "\"%s\" is NOT AUTHORIZED to send commands to the Configuration "
            "Manager Server running on this node!", rpc.c_str(),
            rpcArgs.c_str(), authUser.c_str(), authHost.c_str());
    return false;
  }
  mPrintf(errU,DEBUG,METHOD,0,"Allowed RPC: \"%s %s\" from user: \"%s\" at "
          "host: \"%s\".", rpc.c_str(), rpcArgs.c_str(), authUser.c_str(),
          authHost.c_str());
  return allowed;
}
/*****************************************************************************/
/* read methods                                                              */
/*****************************************************************************/
void CmAcl::readAcl(string &aclFileName)
{
  int i;
  string record;
  size_t sep,c,b,e,len;
  entry pttn;
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,DEBUG,METHOD,0,"Trying to read Access Control "
          "List file: \"%s\"...",aclFileName.c_str());
  /* check acl file accessibility */
  if(access(aclFileName.c_str(),R_OK)==-1)
  {
    mPrintf(errU,FATAL,METHOD,0,"Access Control List file: "
            "\"%s\" not accessible. access(2) error: %s!",aclFileName.c_str(),
            strerror(errno));
    exit(1);
  }
  ifstream aclFs(aclFileName.c_str(),ifstream::in);
  for(i=0;;i++)
  {
    getline(aclFs,record);
    if(aclFs.eof())break;
    if(aclFs.fail())
    {
      mPrintf(errU,FATAL,METHOD,0,"Failure in reading the Access "
              "Control List file: \"%s\"!",aclFileName.c_str());
      exit(1);
    }
    c=record.find_first_of("#");                       /* start comment char */
    if(c!=record.npos)record=record.substr(0,c);        /* suppress comments */
    len=record.size();
    b=record.find_first_not_of(" \t");                        /* skip blanks */
    if(b==record.npos)           /* no chars after blanks and before comment */
    {
      if(deBug&0x2)
      {
        mPrintf(errU,VERB,METHOD,0,"ACL record %d skipped (blank "
                "record or comment record).",i+1);
      }
      continue;
    }
    sep=record.find('@');
    if(sep==record.npos)
    {
      mPrintf(errU,ERROR,METHOD,0,"Malformed ACL record %d: "
              "\"%s\" skipped (has not the foreseen format user_pattern@"
              "host_pattern)!",i+1,record.c_str());
      continue;
    }
    pttn.user=record.substr(0,sep);
    pttn.host=record.substr(sep+1,len);
    b=pttn.user.find_first_not_of(" \t");            /* first non-blank char */
    if(b==pttn.user.npos)                 /* no non-blank chars in pttn.user */
    {
      mPrintf(errU,ERROR,METHOD,0,"Malformed ACL record %d: "
              "\"%s\" skipped (has not the foreseen format user_pattern@"
              "host_pattern)!",i+1,record.c_str());
      continue;
    }
    e=pttn.user.find_first_of(" \t",b+1);      /* fist blank after pttn.user */
    if(e==pttn.user.npos)e=pttn.user.size();
    pttn.user=pttn.user.substr(b,e-b);   /* suppress heading/trailing blanks */
    b=pttn.host.find_first_not_of(" \t");            /* first non-blank char */
    if(b==pttn.host.npos)                 /* no non-blank chars in pttn.host */
    {
      mPrintf(errU,ERROR,METHOD,0,"Malformed ACL record %d: "
              "\"%s\" skipped (has not the foreseen format user_pattern@"
              "host_pattern)!",i+1,record.c_str());
      continue;
    }
    e=pttn.host.find_first_of(" \t",b+1);      /* fist blank after pttn.host */
    if(e==pttn.host.npos)e=pttn.host.size();
    pttn.host=pttn.host.substr(b,e-b);   /* suppress heading/trailing blanks */
    acl.push_back(pttn);
  }
  aclFs.close();
  mPrintf(errU,DEBUG,METHOD,0,"Access Control List read from "
          "file: \"%s\".",aclFileName.c_str());
}
/*****************************************************************************/
void CmAcl::readConf(string &confFileName)
{
  unsigned i;
  string record;
  string permission;
  string fileName;
  size_t c,b,e;
  /*-------------------------------------------------------------------------*/
  mPrintf(errU,DEBUG,METHOD,0,"Trying to read Configuration file: \"%s\"...",
          confFileName.c_str());
  /* check configuration file accessibility */
  if(access(confFileName.c_str(),R_OK)==-1)
  {
    mPrintf(errU,FATAL,METHOD,0,"Configuration file: \"%s\" not accessible. "
            "access(2) error: %s!",confFileName.c_str(),strerror(errno));
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* parse file */
  ifstream confFs(confFileName.c_str(),ifstream::in);
  for(i=0;;i++)                                    /* loop over file records */
  {
    getline(confFs,record);
    if(confFs.eof())break;
    if(confFs.fail())
    {
      mPrintf(errU,FATAL,METHOD,0,"Failure in reading the Initialization "
              "file: \"%s\"!",confFileName.c_str());
      exit(1);
    }
    /*.......................................................................*/
    /* suppress comments */
    c=record.find_first_of("#");                       /* start comment char */
    if(c!=record.npos)record=record.substr(0,c);              /* cut comment */
    /* suppress heading blanks */
    b=record.find_first_not_of(" \t");          /* first non-blank character */
    if(b==record.npos)           /* no chars after blanks and before comment */
    {
      if(deBug&0x02)
      {
        mPrintf(errU,VERB,METHOD,0,"Configuration file record # %d skipped "
                "(blank record or comment record).",i+1);
      }
      continue;
    }
    record=record.substr(b);                           /* cut heading blanks */
    /*.......................................................................*/
    /* extract permission */
    b=record.find_first_of("(");
    if(b==string::npos)
    {
      mPrintf(errU,WARN,METHOD,0,"Configuration file record # %d: \"%s\" "
              "skipped (malformed record: missing permission)!",i+1,
              record.c_str());
      continue;
    }
    b++;
    e=record.find_first_of(")",b);
    if(e==string::npos || e-b!=2)
    {
      mPrintf(errU,WARN,METHOD,0,"Configuration file record # %d: \"%s\" "
              "skipped (malformed record: missing or bad permission)!",i+1,
              record.c_str());
      continue;
    }
    permission=record.substr(b,e-b);
    if(permission!="ro" && permission!="rw")
    {
      mPrintf(errU,WARN,METHOD,0,"Configuration file record # %d: \"%s\" "
              "skipped (malformed record: bad permission)!",i+1,
              record.c_str());
      continue;
    }
    /*.......................................................................*/
    /* extract fileName */
    b=record.find_first_of(" \t",e);         /* first blank after permission */
    if(b==string::npos)
    {
      mPrintf(errU,WARN,METHOD,0,"Configuration file record # %d: \"%s\" "
              "skipped (malformed record: nothing after permission)!",i+1,
              record.c_str());
      continue;
    }
    b=record.find_first_not_of(" \t",b); /* first non-blank after permission */
    if(b==string::npos)
    {
      mPrintf(errU,WARN,METHOD,0,"Configuration file record # %d: \"%s\" "
              "skipped (malformed record: nothing after permission)!",i+1,
              record.c_str());
      continue;
    }
    fileName=record.substr(b);
    e=fileName.find_first_of(" \t",b);               /* first trailing blank */
    if(e!=string::npos)fileName=fileName.substr(0,e); /* cut trailing blanks */
    /*.......................................................................*/
    /* add the file to the list(s) */
    if(permission=="ro")
    {
      if(isFileReadable(fileName))         /* already in readAllwdFiles list */
      {
        mPrintf(errU,WARN,METHOD,0,"Configuration file record # %d: \"%s\" "
                "skipped (duplicated record)!",i+1,record.c_str());
      }
      else                                     /* not in readAllwdFiles list */
      {
        readAllwdFiles.push_back(fileName);
      }
    }
    else if(permission=="rw")
    {
      if(isFileWritable(fileName))        /* already in writeAllwdFiles list */
      {
        mPrintf(errU,WARN,METHOD,0,"Configuration file record # %d: \"%s\" "
                "skipped (duplicated record)!",i+1,record.c_str());
      }
      else           /* not in writeAllwdFiles; but can be in readAllwdFiles */
      {
        if(!isFileReadable(fileName))          /* not in readAllwdFiles list */
        {
          readAllwdFiles.push_back(fileName);
        }
        writeAllwdFiles.push_back(fileName);
      }
    }
    /*.......................................................................*/
  }                                                /* loop over file records */
  /*-------------------------------------------------------------------------*/
  confFs.close();
  mPrintf(errU,DEBUG,METHOD,0,"Configuration read from file: \"%s\".",
          confFileName.c_str());
}
/*****************************************************************************/
/* print methods                                                             */
/*****************************************************************************/
void CmAcl::printAcl(int severity)
{
  vector<entry>::iterator eIt;
  string s;
  /*-------------------------------------------------------------------------*/
  if(!authenticated)
  {
     mPrintf(errU,WARN,METHOD,0,"Client AUTHENTICATION NOT "
             "required! ALL the users from ALL the hosts are allowed to "
             "send command to the Process Controller Server running on "
             "this node! Start pcSrv without the \"--no-authentication\" "
             "command-line switch and use an authenticated client to "
             "avoid this!");
    return;
  }
  s="Allowed user@nodes pairs: ";
  for(eIt=acl.begin();eIt!=acl.end();eIt++)
  {
    if(eIt!=acl.begin())s+=", ";
    s+=eIt->user+"@"+eIt->host;
  }
  s+=".";
  mPrintf(errU,severity,METHOD,0,"%s",s.c_str());
}
/*****************************************************************************/
void CmAcl::printConf(int severity)
{
  vector<string>::iterator it;
  string sr,sw;
  /*-------------------------------------------------------------------------*/
  sr="Readable files: ";
  for(it=readAllwdFiles.begin();it!=readAllwdFiles.end();it++)
  {
    if(it!=readAllwdFiles.begin())sr+=", ";
    sr+="\""+*it+"\"";
  }
  sr+=".";
  sw="Writable files: ";
  for(it=writeAllwdFiles.begin();it!=writeAllwdFiles.end();it++)
  {
    if(it!=writeAllwdFiles.begin())sw+=", ";
    sw+="\""+*it+"\"";
  }
  sw+=".";
  mPrintf(errU,severity,METHOD,0,"%s",sr.c_str());
  mPrintf(errU,severity,METHOD,0,"%s",sw.c_str());
}
/*****************************************************************************/
/* constructor                                                               */
/*****************************************************************************/
CmAcl::CmAcl()
{
  char hostName[HOST_NAME_MAX]="";
  struct passwd *pw;
  char *userName=NULL;
  /*-------------------------------------------------------------------------*/
  gethostname(hostName,HOST_NAME_MAX);
  //userName=getlogin();
  pw=getpwuid(getuid());
  userName=pw->pw_name;
  authString=string(userName)+string("\f")+string(hostName);
  authenticated=true;
}
/* ######################################################################### */
