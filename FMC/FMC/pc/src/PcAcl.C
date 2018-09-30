/* ######################################################################### */
/*
 * $Log: PcAcl.C,v $
 * Revision 1.5  2008/09/26 20:41:00  galli
 * close acl file (bug fix)
 *
 * Revision 1.4  2008/09/15 07:44:52  galli
 * included directives moved to PcAcl.IC
 *
 * Revision 1.3  2008/09/12 09:29:51  galli
 * bool isAllowed(string,string&) prefixes output string for getopt() processing
 *
 * Revision 1.2  2008/09/09 14:40:19  galli
 * first working version
 *
 * Revision 1.1  2008/09/09 11:20:48  galli
 * Initial revision
 */
/* ######################################################################### */
#include "PcAcl.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
extern int errU;
extern int deBug;
/* ######################################################################### */
void PcAcl::read(string aclFileName)
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
      if(deBug&0x4)
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
void PcAcl::print(int sev,string head,string separator,string trail)
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
  s=head;
  for(eIt=acl.begin();eIt!=acl.end();eIt++)
  {
    if(eIt!=acl.begin())s+=separator;
    s+=eIt->user+"@"+eIt->host;
  }
  s+=trail;
  mPrintf(errU,sev,METHOD,0,"%s",s.c_str());
}
/*****************************************************************************/
/* cmdArgs is modified (auth strings and leading blanks are stripped out).   */
/* cmd is used only to be included in diagnostic messages                    */
bool PcAcl::isAllowed(string cmd,string& cmdArgs)
{
  size_t len,sep,b,e;
  string authString,authUser,authHost;
  bool allowed;
  vector<entry>::iterator eIt;
  /*-------------------------------------------------------------------------*/
  // TODO: check exception of find
  if(!authenticated)
  {
    sep=cmdArgs.find('\f');
    if(sep!=cmdArgs.npos)                             /* auth string present */
    {
      len=cmdArgs.size();
      b=cmdArgs.find_first_not_of(" \t",0);                   /* skip blanks */
      b=cmdArgs.find_first_of(" \t",b);                  /* skip auth string */
      b=cmdArgs.find_first_not_of(" \t",b);                   /* skip blanks */
      cmdArgs=cmdArgs.substr(b,len-b);     /* strip out auth string & blanks */
    }
    mPrintf(errU,DEBUG,METHOD,0,"Allowed CMD: \" %s %s\".",
            cmd.c_str(),cmdArgs.c_str());
    cmdArgs=cmd+" "+cmdArgs;       /* prefix the cmd for getopt() processing */
    return true;
  }
  len=cmdArgs.size();
  b=cmdArgs.find_first_not_of(" \t",0);                       /* skip blanks */
  e=cmdArgs.find_first_of(" \t",b);                    /* end of auth string */
  authString=cmdArgs.substr(b,e-b);                    /* extract authString */
  b=cmdArgs.find_first_not_of(" \t",e);                       /* skip blanks */
  cmdArgs=cmdArgs.substr(b,len-b);         /* strip out auth string & blanks */
  len=authString.size();
  /* check authString format */
  sep=authString.find('\f');
  if(sep==authString.npos)
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s %s\": MALFORMED "
            "authentication string \"%s\" received!",cmd.c_str(),
            cmdArgs.c_str(),authString.c_str());
    cmdArgs=cmd+" "+cmdArgs;       /* prefix the cmd for getopt() processing */
    return false;
  }
  authUser=authString.substr(0,sep);
  authHost=authString.substr(sep+1,len);
  if(deBug&0x4)
  {
    mPrintf(errU,VERB,METHOD,0,"Received authentication string: "
            "\"%s@%s\".",authUser.c_str(),authHost.c_str());
  }
  if(!authUser.size() || !authHost.size())
  {
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s %s\": MALFORMED "
            "authentication string \"%s@%s\" received!",cmd.c_str(),
            cmdArgs.c_str(),authUser.c_str(),authHost.c_str());
    cmdArgs=cmd+" "+cmdArgs;       /* prefix the cmd for getopt() processing */
    return false;
  }
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
    mPrintf(errU,ERROR,METHOD,0,"CMD: \"%s %s\" REFUSED! User "
            "\"%s\" at host \"%s\" is NOT AUTHORIZED to send commands to the "
            "Process Controller Server running on this node!",cmd.c_str(),
            cmdArgs.c_str(),authUser.c_str(),authHost.c_str());
    cmdArgs=cmd+" "+cmdArgs;       /* prefix the cmd for getopt() processing */
    return false;
  }
  mPrintf(errU,DEBUG,METHOD,0,"Allowed CMD: \"%s %s\" from "
          "user: \"%s\" at host: \"%s\".",cmd.c_str(),cmdArgs.c_str(),
          authUser.c_str(),authHost.c_str());
  cmdArgs=cmd+" "+cmdArgs;         /* prefix the cmd for getopt() processing */
  return allowed;
}
/*****************************************************************************/
/* constructor */
PcAcl::PcAcl()
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
/*****************************************************************************/
/*****************************************************************************/
/* ######################################################################### */
