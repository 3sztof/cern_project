/* ######################################################################### */
/*
 * Revision 1.1  2009/02/09 16:28:23  galli
 * Initial revision
 */
/* ######################################################################### */
#include "LogWriter.IC"
/* ######################################################################### */
extern char *filterRegExpPattern;
extern regex_t filterRegExp;
extern char *filterWildCardPattern;
extern char *xFilterRegExpPattern;
extern regex_t xFilterRegExp;
extern char *xFilterWildCardPattern;
extern char **mBuf;         /* buffer containing the last chkdLineN messages */
extern char *cBuf;                                       /* alias of mBuf[0] */
extern int minLd;                            /* minimum Levenshtein distance */
extern int minWd;                                   /* minimum word distance */
extern int ptCmp;                                     /* punctual comparison */
extern int chkdLineN;    
extern size_t cutCol;
extern char *cutStr;
extern int deBug;
extern int errU;
extern unsigned long long suppressedMsgN;
extern char *pName;
extern char *utgid;
extern pthread_mutex_t prLock;
extern FILE *outFP;
extern int lStd;
/* ######################################################################### */
void LogWriter::infoHandler()
{
  int isRepeated=0;
  int j=0;
  s=getString();
  /*.........................................................................*/
  if(s[0]==-1 && s[1]==-1 && s[2]==-1 && s[3]==-1)return; /* server exited */
  len=strlen(s);
  if(len<2)return;                                         /* "\0", "\n\0" */
  /*.........................................................................*/
  if(filterRegExpPattern && regexec(&filterRegExp,s,(size_t)0,NULL,0))
    return;
  if(filterWildCardPattern && fnmatch(filterWildCardPattern,s,0))
    return;
  if(xFilterRegExpPattern && !regexec(&xFilterRegExp,s,(size_t)0,NULL,0))
    return;
  if(xFilterWildCardPattern && !fnmatch(xFilterWildCardPattern,s,0))
    return;
  /*.........................................................................*/
  memset(cBuf,0,BUF_SIZE);
  strncpy(cBuf,s,BUF_SIZE);
  /* NULL-terminate truncated messages */
  if(!memchr(cBuf,0,BUF_SIZE))cBuf[BUF_SIZE-1]='\0';
  /* suppress repeated messages if required */
  isRepeated=0;
  if(minLd>=-1||minWd>-1||ptCmp)
  {
    for(j=1;j<chkdLineN;j++)
    {
      char *cStr=cBuf,*oStr=mBuf[j];
      int cutStrLen=0;
      int k=0;
      /* shorten string to be compared using -c */
      if(cutCol)
      {
        if(strlen(cStr)>cutCol)cStr+=cutCol;
        else cStr=(char*)"";
        if(strlen(oStr)>cutCol)oStr+=cutCol;
        else oStr=(char*)"";
      }
      /* shorten string to be compared using -C */
      if((cutStrLen=strlen(cutStr)))
      {
        for(k=0;k<cutStrLen;k++)
        {
          char *p=NULL;
          if((p=strchr(cStr,cutStr[k]))!=NULL)cStr=p;
          else{cStr=(char*)"";break;}
          if((p=strchr(oStr,cutStr[k]))!=NULL)oStr=p;
          else{oStr=(char*)"";break;}
        }
      }
      /* compare strings */
      if(ptCmp)                                     /* punctual comparison */
      {
        if(!strcmp(cStr,oStr))
        {
          isRepeated=1;
          break;
        }
      }
      if(minLd>-1)                               /* Levenshtein comparison */
      {
        int d;
        d=levDist(cStr,oStr);
        if(deBug & 0x2)rPrintf(errU,2,"curr=\"%s\", old%d=\"%s\", "
                               "levDist=%d\n",cStr,j,oStr,d);
        if(d>=0 && d<=minLd)
        {
          isRepeated=1;
          break;
        }
      }
      if(minWd>-1)                             /* punctual word comparison */
      {
        int res;
        res=wordCmp(cStr,oStr,minWd);
        if(deBug & 0x2)rPrintf(errU,2,"curr=\"%s\", old%d=\"%s\", "
                               "wordCmp=%d\n",cStr,j,oStr,res);
        if(res)
        {
          isRepeated=1;
          break;
        }
      }
      /* end strings comparison */
    }
  }
  /* rotate the chkdLineN buffers */
  cBuf=mBuf[chkdLineN-1];
  for(j=chkdLineN-1;j>0;j--)mBuf[j]=mBuf[j-1];
  mBuf[0]=cBuf;
  /* suppress repeated message */
  if(isRepeated)
  {
    suppressedMsgN++;
    if(!(suppressedMsgN%1000))
    {
      memset(cBuf,0,BUF_SIZE);
      snprintf(cBuf,BUF_SIZE,"%s(%s): %s: (%llu repeated messages "
               "suppressed).",pName,utgid,METHOD,
               suppressedMsgN);
      LogUtils::headerPrepend(cBuf,DEBUG);
      pthread_mutex_lock(&prLock);
      LogUtils::printC(outFP,-1,-1,color,cBuf);
      pthread_mutex_unlock(&prLock);
      mPrintf(errU & ~lStd,DEBUG,METHOD,0,"(%llu repeated "
              "messages suppressed).",suppressedMsgN);
    }
    return;
  }
  /*.........................................................................*/
  pthread_mutex_lock(&prLock);
  LogUtils::printC(outFP,-1,severityThreshold,color,s);
  fflush(outFP);
  pthread_mutex_unlock(&prLock);
}                                           /* void LogWriter::infoHandler() */
/*****************************************************************************/
char *LogWriter::getSvcPath(void)
{
  return getName();
}
/*****************************************************************************/
/* constructor */
LogWriter::LogWriter(const char *svcPath,int severityThreshold,int color)
 : DimInfo(svcPath,-1)
{
  this->severityThreshold=severityThreshold;
  this->color=color;
}
/* ######################################################################### */
