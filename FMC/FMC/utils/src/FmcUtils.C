/* ######################################################################### */
/*
 * $Log: FmcUtils.C,v $
 * Revision 1.7  2008/10/14 11:59:11  galli
 * additional parameter (with default value) to printLogServerVersion()
 *
 * Revision 1.6  2008/10/13 11:51:21  galli
 * added function printLogServerVersion()
 *
 * Revision 1.5  2008/10/09 07:52:48  galli
 * added function ultoso()
 *
 * Revision 1.4  2008/09/30 18:06:30  galli
 * bug fixed
 *
 * Revision 1.3  2008/09/30 13:05:14  galli
 * added functions toLower() and printServerVersion()
 *
 * Revision 1.2  2008/09/26 20:35:41  galli
 * added static method ultos()
 *
 * Revision 1.1  2008/09/22 07:45:26  galli
 * Initial revision
 */
/* ######################################################################### */
#include "FmcUtils.IC"
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
/* change each element of the string to upper case                           */
string FmcUtils::toUpper(string &s)
{
  string S(s);
  transform(S.begin(),S.end(),S.begin(),(int(*)(int))toupper);
  return S;
}
/*****************************************************************************/
/* change each element of the string to lower case                           */
string FmcUtils::toLower(string &s)
{
  string S(s);
  transform(S.begin(),S.end(),S.begin(),(int(*)(int))tolower);
  return S;
}
/*****************************************************************************/
/* convert int to string */
string FmcUtils::itos(int i)
{
  stringstream s;
  s << i;
  return s.str();
}
/*****************************************************************************/
/* convert int to string */
string FmcUtils::utos(unsigned i)
{
  stringstream s;
  s << i;
  return s.str();
}
/*****************************************************************************/
/* convert long to string */
string FmcUtils::ltos(long i)
{
  stringstream s;
  s << i;
  return s.str();
}
/*****************************************************************************/
/* convert unsigned long to string */
string FmcUtils::ultos(unsigned long i)
{
  stringstream s;
  s << i;
  return s.str();
}
/*****************************************************************************/
/* convert unsigned long to string in hexadecimal format 0xffffffff */
string FmcUtils::ultosx(unsigned long i)
{
  char s[19];
  if(sizeof(unsigned long)==8)snprintf(s,19,"%#16.16lx",i);
  else if(sizeof(unsigned long)==4)snprintf(s,19,"%#8.8lx",i);
  return string(s);
}
/*****************************************************************************/
/* convert unsigned long to string in octal format 0777 */
string FmcUtils::ultoso(unsigned long i)
{
  char s[8];
  snprintf(s,8,"%04lo",i);
  return string(s);
}
/*****************************************************************************/
void FmcUtils::printServerVersion(int nodeHostNameC, char **nodeHostNameV,
                                  string svcHead, string srvName, int deBug,
                                  int width1, int width2, unsigned slashInName)
{
  /*-------------------------------------------------------------------------*/
  /* counters */
  int i,j;
  unsigned k,sc1,sc2;
  /* pointers */
  char *p;
  /* service browsing */
  string nodePttn;
  string successRawSvcPttn;
  string successSvcPttn;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  string successSvc;
  size_t b,e;
  string nodeFound;
  string fmcVersionSvc;
  string srvVersionSvc;
  char nolinkStr[]="N/F";
  /* retrieved quantities */
  int success;
  string fmcVersion;
  string srvVersion;
  /*-------------------------------------------------------------------------*/
  for(i=0,j=0;i<nodeHostNameC;i++)                /* loop over node patterns */
  {
    nodePttn=nodeHostNameV[i];
    nodePttn=toUpper(nodePttn);
    successRawSvcPttn=svcHead+"/*/"+srvName+"/success";
    successSvcPttn=svcHead+"/"+nodePttn+"/"+srvName+"/success";
    if(deBug)printf("  Service pattern: \"%s\"\n",successSvcPttn.c_str());
    /*.......................................................................*/
    /* count the number of the slash */
    for(k=0,sc1=0;k<successSvcPttn.size();k++)
    {
      if(successSvcPttn.at(k)=='/')sc1++;
    }
    /*.......................................................................*/
    br.getServices(successRawSvcPttn.c_str());
    while((type=br.getNextService(p,format))!=0)          /* loop over nodes */
    {
      if(!fnmatch(successSvcPttn.c_str(),p,0))
      {
        successSvc=p;
        /*...................................................................*/
        /* count the number of the slash */
        for(k=0,sc2=0;k<successSvc.size();k++)
        {
          if(successSvc.at(k)=='/')sc2++;
        }
        if(sc2!=sc1)continue;
        /*...................................................................*/
        /* extract node hostname from "success" service name */
        e=successSvc.find_last_of('/');                        /* last slash */
        e=successSvc.find_last_of('/',e-1);            /* last-but-one slash */
        for(k=0;k<slashInName;k++)e=successSvc.find_last_of('/',e-1);
        b=successSvc.find_last_of('/',e-1)+1;
        nodeFound=successSvc.substr(b,e-b);
        nodeFound=toLower(nodeFound);
        /*...................................................................*/
        /* read Success */
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        successSvc.c_str());
        DimCurrentInfo Success(successSvc.c_str(),-1);
        success=(int)Success.getInt();
        if(success!=1)
        {
          fprintf(stderr,"Service \"%s\" unreachable!\n",successSvc.c_str());
          continue;
        }
        /*...................................................................*/
        /* read fmc version */
        fmcVersionSvc=svcHead+"/"+toUpper(nodeFound)+"/"+srvName+
                      "/fmc_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        fmcVersionSvc.c_str());
        DimCurrentInfo FmcVersion(fmcVersionSvc.c_str(),nolinkStr);
        fmcVersion=(char*)FmcVersion.getString();
        /*...................................................................*/
        /* read server version */
        srvVersionSvc=svcHead+"/"+toUpper(nodeFound)+"/"+srvName+
                      "/server_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        srvVersionSvc.c_str());
        DimCurrentInfo SrvVersion(srvVersionSvc.c_str(),nolinkStr);
        srvVersion=(char*)SrvVersion.getString();
        /*...................................................................*/
        printf("%-*s %-*s %s\n",width1,nodeFound.c_str(),width2,
               fmcVersion.c_str(),srvVersion.c_str());
        /*...................................................................*/
      }                                                    /* if(!fnmatch()) */
    }                                                     /* loop over nodes */
    /*.......................................................................*/
  }                                               /* loop over node patterns */
}
/*****************************************************************************/
void FmcUtils::printServerVersion(vector<string> hostPttn,
                                  string svcHead, string srvName, int deBug,
                                  int width1, int width2, unsigned slashInName)
{
  /*-------------------------------------------------------------------------*/
  /* counters */
  int j=0;
  vector<string>::iterator hIt;
  unsigned k,sc1,sc2;
  /* pointers */
  char *p;
  /* service browsing */
  string nodePttn;
  string successRawSvcPttn;
  string successSvcPttn;
  DimBrowser br;
  int type=0;
  char *format=NULL;
  string successSvc;
  size_t b,e;
  string nodeFound;
  string fmcVersionSvc;
  string srvVersionSvc;
  char nolinkStr[]="N/F";
  /* retrieved quantities */
  int success;
  string fmcVersion;
  string srvVersion;
  /*-------------------------------------------------------------------------*/
  for(hIt=hostPttn.begin();hIt!=hostPttn.end();hIt++) /* loop over node pttn */
  {
    nodePttn=*hIt;
    nodePttn=toUpper(nodePttn);
    successRawSvcPttn=svcHead+"/*/"+srvName+"/success";
    successSvcPttn=svcHead+"/"+nodePttn+"/"+srvName+"/success";
    if(deBug)printf("  Service pattern: \"%s\"\n",successSvcPttn.c_str());
    /*.......................................................................*/
    /* count the number of the slash */
    for(k=0,sc1=0;k<successSvcPttn.size();k++)
    {
      if(successSvcPttn.at(k)=='/')sc1++;
    }
    /*.......................................................................*/
    br.getServices(successRawSvcPttn.c_str());
    while((type=br.getNextService(p,format))!=0)          /* loop over nodes */
    {
      if(!fnmatch(successSvcPttn.c_str(),p,0))
      {
        successSvc=p;
        /*...................................................................*/
        /* count the number of the slash */
        for(k=0,sc2=0;k<successSvc.size();k++)
        {
          if(successSvc.at(k)=='/')sc2++;
        }
        if(sc2!=sc1)continue;
        /*...................................................................*/
        /* extract node hostname from "success" service name */
        e=successSvc.find_last_of('/');                        /* last slash */
        e=successSvc.find_last_of('/',e-1);            /* last-but-one slash */
        for(k=0;k<slashInName;k++)e=successSvc.find_last_of('/',e-1);
        b=successSvc.find_last_of('/',e-1)+1;
        nodeFound=successSvc.substr(b,e-b);
        nodeFound=toLower(nodeFound);
        /*...................................................................*/
        /* read Success */
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        successSvc.c_str());
        DimCurrentInfo Success(successSvc.c_str(),-1);
        success=(int)Success.getInt();
        if(success!=1)
        {
          fprintf(stderr,"Service \"%s\" unreachable!\n",successSvc.c_str());
          continue;
        }
        /*...................................................................*/
        /* read fmc version */
        fmcVersionSvc=svcHead+"/"+toUpper(nodeFound)+"/"+srvName+
                      "/fmc_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        fmcVersionSvc.c_str());
        DimCurrentInfo FmcVersion(fmcVersionSvc.c_str(),nolinkStr);
        fmcVersion=(char*)FmcVersion.getString();
        /*...................................................................*/
        /* read server version */
        srvVersionSvc=svcHead+"/"+toUpper(nodeFound)+"/"+srvName+
                      "/server_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        srvVersionSvc.c_str());
        DimCurrentInfo SrvVersion(srvVersionSvc.c_str(),nolinkStr);
        srvVersion=(char*)SrvVersion.getString();
        /*...................................................................*/
        printf("%-*s %-*s %s\n",width1,nodeFound.c_str(),width2,
               fmcVersion.c_str(),srvVersion.c_str());
        /*...................................................................*/
      }                                                    /* if(!fnmatch()) */
    }                                                     /* loop over nodes */
    /*.......................................................................*/
  }                                               /* loop over node patterns */
}
/*****************************************************************************/
void FmcUtils::printLogServerVersion(vector<string> hostPttn,
                                     vector<string> logPttn,
                                     string svcHead, int deBug,
                                     int width1, int width2)
{
  /* server pattern list */
  string rawSrvPttn;
  vector<string> srvPttn;
  vector<string>::iterator hIt,lIt;
  string s;
  /* service browsing */
  vector<string>::iterator sIt;
  DimBrowser br;
  char *p,*format;
  int type;
  unsigned sc1,sc2,j,k;
  string successRawSvcPttn,successSvcPttn;
  string logRawSvcPttn,logSvcPttn;
  string successSvc,fmcVersionSvc,srvVersionSvc,logSvc;
  size_t b,e;
  string nodeFound,logFound,srvFound;
  char nolinkStr[]="N/F";
  /* retrieved quantities */
  int success;
  string fmcVersion;
  string srvVersion;
  /*-------------------------------------------------------------------------*/
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/logger/<LOGNAME>/log */
  /*-------------------------------------------------------------------------*/
  /* compose rawSrvPttn and srvPttn */
  rawSrvPttn=svcHead+"/*/logger/*";
  srvPttn.clear();
  for(hIt=hostPttn.begin();hIt!=hostPttn.end();hIt++)
  {
    for(lIt=logPttn.begin();lIt!=logPttn.end();lIt++)
    {
      s=svcHead+"/"+toUpper(*hIt)+"/logger/"+*lIt;
      srvPttn.push_back(s);
    }
  }
  /*-------------------------------------------------------------------------*/
  successRawSvcPttn=rawSrvPttn+"/success";
  /*-------------------------------------------------------------------------*/
  j=0;
  for(sIt=srvPttn.begin();sIt!=srvPttn.end();sIt++)/* loop over SVC patterns */
  {
    successSvcPttn=*sIt+"/success";
    if(deBug)printf("  Service pattern: \"%s\".\n",successSvcPttn.c_str());
    /*.......................................................................*/
    /* count the number of the slashes */
    for(k=0,sc1=0;k<successSvcPttn.size();k++)
    {
      if(successSvcPttn.at(k)=='/')sc1++;
    }
    /*.......................................................................*/
    br.getServices(successRawSvcPttn.c_str());
    while((type=br.getNextService(p,format))!=0)           /* loop over SVCs */
    {
      if(!fnmatch(successSvcPttn.c_str(),p,0))     /* if SVC matches pattern */
      {
        successSvc=p;
        /*...................................................................*/
        /* count the number of the slash */
        for(k=0,sc2=0;k<successSvc.size();k++)
        {
          if(successSvc.at(k)=='/')sc2++;
        }
        if(sc2!=sc1)continue;
        /*...................................................................*/
        /* /FMC/<HOSTNAME>/logger/<LOGNAME>/success */
        /* extract HOSTNAME & LOGNAME from "success" service name */
        e=successSvc.find_last_of('/');                        /* last slash */
        b=successSvc.find_last_of('/',e-1)+1; /* 1st char after last-but-one */
        logFound=successSvc.substr(b,e-b);
        e=successSvc.find_last_of('/',e-1);            /* last-but-one slash */
        e=successSvc.find_last_of('/',e-1);            /* last-but-two slash */
        b=successSvc.find_last_of('/',e-1)+1;   /* 1st char after last-but-3 */
        nodeFound=successSvc.substr(b,e-b);
        srvFound=svcHead+"/"+nodeFound+"/logger/"+logFound;
        nodeFound=toLower(nodeFound);
        /*...................................................................*/
        /* read success */
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        successSvc.c_str());
        DimCurrentInfo Success(successSvc.c_str(),-1);
        success=(int)Success.getInt();
        if(success==-1)continue;
        /*...................................................................*/
        /* read fmc version */
        fmcVersionSvc=srvFound+"/fmc_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        fmcVersionSvc.c_str());
        DimCurrentInfo FmcVersion(fmcVersionSvc.c_str(),nolinkStr);
        fmcVersion=(char*)FmcVersion.getString();
        if(fmcVersion=="N/A")continue;
        /*...................................................................*/
        /* read server version */
        srvVersionSvc=srvFound+"/server_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        srvVersionSvc.c_str());
        DimCurrentInfo SrvVersion(srvVersionSvc.c_str(),nolinkStr);
        srvVersion=(char*)SrvVersion.getString();
        /*...................................................................*/
        /* print */
        printf("%-*s %-*s %s\n",width1, (nodeFound+"("+logFound+")").c_str(),
               width2,fmcVersion.c_str(),srvVersion.c_str());
        /*...................................................................*/
      }                                            /* if SVC matches pattern */
    }                                                      /* loop over SVCs */
  }                                                /* loop over SVC patterns */
  /*-------------------------------------------------------------------------*/
  /* end current (>=FMC-3.9) service name rule */
  /*-------------------------------------------------------------------------*/
  /* old service name rule (FMC-3.8) */
  /* /<HOSTNAME>/logger/<LOGNAME>/log */
  /*-------------------------------------------------------------------------*/
  /* compose rawSrvPttn and srvPttn */
  rawSrvPttn="/*/logger/*";
  srvPttn.clear();
  for(hIt=hostPttn.begin();hIt!=hostPttn.end();hIt++)
  {
    for(lIt=logPttn.begin();lIt!=logPttn.end();lIt++)
    {
      s="/"+toUpper(*hIt)+"/logger/"+*lIt;
      srvPttn.push_back(s);
    }
  }
  /*-------------------------------------------------------------------------*/
  successRawSvcPttn=rawSrvPttn+"/success";
  /*-------------------------------------------------------------------------*/
  j=0;
  for(sIt=srvPttn.begin();sIt!=srvPttn.end();sIt++)/* loop over SVC patterns */
  {
    successSvcPttn=*sIt+"/success";
    if(deBug)printf("  Service pattern: \"%s\".\n",successSvcPttn.c_str());
    /*.......................................................................*/
    /* count the number of the slashes */
    for(k=0,sc1=0;k<successSvcPttn.size();k++)
    {
      if(successSvcPttn.at(k)=='/')sc1++;
    }
    /*.......................................................................*/
    br.getServices(successRawSvcPttn.c_str());
    while((type=br.getNextService(p,format))!=0)           /* loop over SVCs */
    {
      if(!fnmatch(successSvcPttn.c_str(),p,0))     /* if SVC matches pattern */
      {
        successSvc=p;
        /*...................................................................*/
        /* count the number of the slash */
        for(k=0,sc2=0;k<successSvc.size();k++)
        {
          if(successSvc.at(k)=='/')sc2++;
        }
        if(sc2!=sc1)continue;
        /*...................................................................*/
        /* /FMC/<HOSTNAME>/logger/<LOGNAME>/success */
        /* extract HOSTNAME & LOGNAME from "success" service name */
        e=successSvc.find_last_of('/');                        /* last slash */
        b=successSvc.find_last_of('/',e-1)+1; /* 1st char after last-but-one */
        logFound=successSvc.substr(b,e-b);
        e=successSvc.find_last_of('/',e-1);            /* last-but-one slash */
        e=successSvc.find_last_of('/',e-1);            /* last-but-two slash */
        b=successSvc.find_last_of('/',e-1)+1;   /* 1st char after last-but-3 */
        nodeFound=successSvc.substr(b,e-b);
        srvFound="/"+nodeFound+"/logger/"+logFound;
        nodeFound=toLower(nodeFound);
        /*...................................................................*/
        /* read success */
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        successSvc.c_str());
        DimCurrentInfo Success(successSvc.c_str(),-1);
        success=(int)Success.getInt();
        if(success==-1)continue;
        /*...................................................................*/
        /* read fmc version */
        fmcVersionSvc=srvFound+"/fmc_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        fmcVersionSvc.c_str());
        DimCurrentInfo FmcVersion(fmcVersionSvc.c_str(),nolinkStr);
        fmcVersion=(char*)FmcVersion.getString();
        /*...................................................................*/
        /* read server version */
        srvVersionSvc=srvFound+"/server_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        srvVersionSvc.c_str());
        DimCurrentInfo SrvVersion(srvVersionSvc.c_str(),nolinkStr);
        srvVersion=(char*)SrvVersion.getString();
        /*...................................................................*/
        printf("%-*s %-*s %s\n",width1, (nodeFound+"("+logFound+")").c_str(),
               width2,fmcVersion.c_str(),srvVersion.c_str());
        /*...................................................................*/
      }                                            /* if SVC matches pattern */
    }                                                      /* loop over SVCs */
  }                                                /* loop over SVC patterns */
  /*-------------------------------------------------------------------------*/
  /* end old service name rule (FMC-3.8) */
  /*-------------------------------------------------------------------------*/
  /* very old service name (<=FMC-3.7) */
  /* /<HOSTNAME>/<LOGNAME>/log */
  /*-------------------------------------------------------------------------*/
  /* compose rawSrvPttn and srvPttn */
  rawSrvPttn="/*/*";
  srvPttn.clear();
  for(hIt=hostPttn.begin();hIt!=hostPttn.end();hIt++)
  {
    for(lIt=logPttn.begin();lIt!=logPttn.end();lIt++)
    {
      s="/"+toUpper(*hIt)+"/"+*lIt;
      srvPttn.push_back(s);
    }
  }
  /*-------------------------------------------------------------------------*/
  logRawSvcPttn=rawSrvPttn+"/log";
  /*-------------------------------------------------------------------------*/
  j=0;
  for(sIt=srvPttn.begin();sIt!=srvPttn.end();sIt++)/* loop over SVC patterns */
  {
    logSvcPttn=*sIt+"/log";
    if(deBug)printf("  Service pattern: \"%s\".\n",logSvcPttn.c_str());
    /*.......................................................................*/
    /* count the number of the slashes */
    for(k=0,sc1=0;k<logSvcPttn.size();k++)
    {
      if(logSvcPttn.at(k)=='/')sc1++;
    }
    /*.......................................................................*/
    br.getServices(logRawSvcPttn.c_str());
    while((type=br.getNextService(p,format))!=0)           /* loop over SVCs */
    {
      if(!fnmatch(logSvcPttn.c_str(),p,0))         /* if SVC matches pattern */
      {
        logSvc=p;
        /*...................................................................*/
        /* count the number of the slash */
        for(k=0,sc2=0;k<logSvc.size();k++)
        {
          if(logSvc.at(k)=='/')sc2++;
        }
        if(sc2!=sc1)continue;
        /*...................................................................*/
        /* /<HOSTNAME>/<LOGNAME>/log */
        /* extract HOSTNAME & LOGNAME from "log" service name */
        e=logSvc.find_last_of('/');                            /* last slash */
        b=logSvc.find_last_of('/',e-1)+1;     /* 1st char after last-but-one */
        logFound=logSvc.substr(b,e-b);
        e=logSvc.find_last_of('/',e-1);                /* last-but-one slash */
        b=logSvc.find_last_of('/',e-1)+1;       /* 1st char after last-but-2 */
        nodeFound=logSvc.substr(b,e-b);
        srvFound="/"+nodeFound+"/"+logFound;
        nodeFound=toLower(nodeFound);
        /*...................................................................*/
        /* read success */
        successSvc=srvFound+"/success";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        successSvc.c_str());
        DimCurrentInfo Success(successSvc.c_str(),-1);
        success=(int)Success.getInt();
        if(success==-1)continue;
        /*...................................................................*/
        /* read fmc version */
        fmcVersionSvc=srvFound+"/fmc_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        fmcVersionSvc.c_str());
        DimCurrentInfo FmcVersion(fmcVersionSvc.c_str(),nolinkStr);
        fmcVersion=(char*)FmcVersion.getString();
        /*...................................................................*/
        /* read server version */
        srvVersionSvc=srvFound+"/server_version";
        if(deBug)printf("  %3d Contacting Service: \"%s\"...\n",j++,
                        srvVersionSvc.c_str());
        DimCurrentInfo SrvVersion(srvVersionSvc.c_str(),nolinkStr);
        srvVersion=(char*)SrvVersion.getString();
        /*...................................................................*/
        printf("%-*s %-*s %s\n",width1, (nodeFound+"("+logFound+")").c_str(),
               width2,fmcVersion.c_str(),srvVersion.c_str());
        /*...................................................................*/
      }                                            /* if SVC matches pattern */
    }                                                      /* loop over SVCs */
  }                                                /* loop over SVC patterns */
  /*-------------------------------------------------------------------------*/
  /* end very old service name (<=FMC-3.7) */
  /*-------------------------------------------------------------------------*/
}
/* ######################################################################### */
