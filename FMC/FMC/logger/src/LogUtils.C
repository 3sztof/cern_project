/* ######################################################################### */
/*
 * $Log: LogUtils.C,v $
 * Revision 1.3  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.2  2009/02/10 13:02:41  galli
 * Added method LogUtils::fifoOpen()
 *
 * Revision 1.1  2009/02/09 16:28:23  galli
 * Initial revision
 */
/* ######################################################################### */
#include "LogUtils.IC"
/* ######################################################################### */
extern regex_t verbRegExp;
extern regex_t debugRegExp;
extern regex_t infoRegExp;
extern regex_t warnRegExp;
extern regex_t errorRegExp;
extern regex_t fatalRegExp;
/* ------------------------------------------------------------------------- */
extern char *filterRegExpPattern;
extern regex_t filterRegExp;
extern char *xFilterRegExpPattern;
extern regex_t xFilterRegExp;
extern char *filterWildCardPattern;
extern char *xFilterWildCardPattern;
/* ------------------------------------------------------------------------- */
extern int deBug;
extern int errU;
extern int lStd;
extern pthread_mutex_t prLock;                                 /* print lock */
extern FILE *outFP;
/* ------------------------------------------------------------------------- */
extern int chkdLineN;
extern int minLd;                            /* minimum Levenshtein distance */
extern int minWd;                                   /* minimum word distance */
extern int ptCmp;                                     /* punctual comparison */
extern size_t cutCol;
extern char *cutStr;
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
int LogUtils::headerPrepend(char *buf,int severity) 
{
  static char tmpBuf[BUF_SIZE];
  static const char *sl[7]={"[OK]","[VERB] ","[DEBUG]","[INFO] ","[WARN] ",
                            "[ERROR]","[FATAL]"};
  time_t now;
  struct tm lNow;
  char sNow[13]="";
  char hostName[80]="";
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  gethostname(hostName,80);
  p=strchr(hostName,'.');
  if(p)*p='\0';
  /*-------------------------------------------------------------------------*/
  now=time(NULL);
  localtime_r(&now,&lNow);
  strftime(sNow,80,"%b%d-%H%M%S",&lNow);
  /*-------------------------------------------------------------------------*/
  if(severity==-1)
  {
    /* try to guess severity from message content */
    if(!regexec(&fatalRegExp,buf,(size_t)0,NULL,0))severity=6;
    else if(!regexec(&errorRegExp,buf,(size_t)0,NULL,0))severity=5;
    else if(!regexec(&warnRegExp,buf,(size_t)0,NULL,0))severity=4;
    else if(!regexec(&infoRegExp,buf,(size_t)0,NULL,0))severity=3;
    else if(!regexec(&debugRegExp,buf,(size_t)0,NULL,0))severity=2;
    else if(!regexec(&verbRegExp,buf,(size_t)0,NULL,0))severity=1;
    else severity=INFO;
  }
  /*-------------------------------------------------------------------------*/
  strncpy(tmpBuf,buf,BUF_SIZE);
  snprintf(buf,BUF_SIZE,"%s%s%s: %s",sNow,sl[severity],hostName,tmpBuf);
  /* NULL-terminate truncated messages */
  if(!memchr(buf,0,BUF_SIZE))buf[BUF_SIZE-1]='\0';
  /* append '\n' if not present */
  if(!strchr(buf,'\n'))
  {
    int bufLen=strlen(buf);
    if(bufLen<BUF_SIZE-1)strcat(buf,"\n");
    else buf[bufLen-1]='\n';
  }
  return 0;
}
/*****************************************************************************/
int LogUtils::printC(FILE *outFP,int severity,int severityThreshold,int color,
                     char *s)
{
  int len=0;
  /*-------------------------------------------------------------------------*/
  /* Evaluate message severity */
  /* Read severity from labels set by mPrintf/mfPrintf/logSrv starting at    */
  /* column 13, if present (cheaper method) */
  len=strlen(s);
  if(severity>-1);
  else if(len>18 && !strncmp(s+12,"[VERB]",6))severity=1;
  else if(len>19 && !strncmp(s+12,"[DEBUG]",7))severity=2;
  else if(len>18 && !strncmp(s+12,"[INFO]",6))severity=3;
  else if(len>18 && !strncmp(s+12,"[WARN]",6))severity=4;
  else if(len>19 && !strncmp(s+12,"[ERROR]",7))severity=5;
  else if(len>19 && !strncmp(s+12,"[FATAL]",7))severity=6;
  /* Else use regex to guess severity (more expensive method) */
  else if(!regexec(&fatalRegExp,s,(size_t)0,NULL,0))severity=6;
  else if(!regexec(&errorRegExp,s,(size_t)0,NULL,0))severity=5;
  else if(!regexec(&warnRegExp,s,(size_t)0,NULL,0))severity=4;
  else if(!regexec(&infoRegExp,s,(size_t)0,NULL,0))severity=3;
  else if(!regexec(&debugRegExp,s,(size_t)0,NULL,0))severity=2;
  else if(!regexec(&verbRegExp,s,(size_t)0,NULL,0))severity=1;
  /* Else set severity to the default severity (INFO) */
  else severity=2;
  /*-------------------------------------------------------------------------*/
  if(severity>=severityThreshold)
  {
    if(!color)fprintf(outFP,"%s",s);
    else
    {
      switch(severity)
      {
        case 0: /* No label - white */
          fprintf(outFP,"%s%s%s","\033[0;37;40m",s,"\033[0m");
          break;
        case 1: /* VERBOSE - blue */
          fprintf(outFP,"%s%s%s","\033[0;34;40m",s,"\033[0m");
          break;
        case 2: /* DEBUG - green */
          fprintf(outFP,"%s%s%s","\033[0;32;40m",s,"\033[0m");
          break;
        case 3: /* INFO - white */
          fprintf(outFP,"%s%s%s","\033[0;37;40m",s,"\033[0m");
          break;
        case 4: /* WARN - bold yellow */
          fprintf(outFP,"%s%s%s","\033[0;1;33;40m",s,"\033[0m");
          break;
        case 5: /* ERROR - bold red */
          fprintf(outFP,"%s%s%s","\033[0;1;31;40m",s,"\033[0m");
          break;
        case 6: /* FATAL - bold magenta */
          fprintf(outFP,"%s%s%s","\033[0;1;35;40m",s,"\033[0m");
          break;
      }
    }
    return 0;
  }
  else return 1;
}
/*****************************************************************************/
int LogUtils::msgCmp(const void *arg1,const void *arg2)
{
  static int month1,month2;
  static struct tm time1_tm,time2_tm;
  static char *p1,*p2,*pb1,*pb2,*ph1,*ph2,*pc1,*pc2;
  static srt_msg_t *srtMsg1,*srtMsg2;
  static int cmp;
  static int uf1,uf2;
  /*-------------------------------------------------------------------------*/
  srtMsg1=(srt_msg_t*)arg1;
  srtMsg2=(srt_msg_t*)arg2;
  /* get month, if any */
  p1=strptime(srtMsg1->msg,"%b",&time1_tm);
  p2=strptime(srtMsg2->msg,"%b",&time2_tm);
  /* standard header: "May16-160228[DEBUG]lhcbcn2:" */
  /*              p1/p2 + 012345678901234567        */
  /* identify messages without standard header */
  if(unlikely(!p1))uf1=1;                             /* no month in message */
  else if(unlikely(strlen(p1)<17))uf1=1;                /* too short message */
  else if(unlikely(!isdigit(*p1)))uf1=1;
  else if(unlikely(!isdigit(*(p1+1))))uf1=1;
  else if(unlikely(*(p1+2)!='-'))uf1=1;
  else if(unlikely(!isdigit(*(p1+3))))uf1=1;
  else if(unlikely(!isdigit(*(p1+4))))uf1=1;
  else if(unlikely(!isdigit(*(p1+5))))uf1=1;
  else if(unlikely(!isdigit(*(p1+6))))uf1=1;
  else if(unlikely(!isdigit(*(p1+7))))uf1=1;
  else if(unlikely(!isdigit(*(p1+8))))uf1=1;
  else if(unlikely(*(p1+9)!='['))uf1=1;
  else if(unlikely((pb1=strchr(p1+10,']'))==NULL))uf1=1;
  else if(unlikely((pc1=strchr(pb1,':'))==NULL))uf1=1;
  else uf1=0;
  if(unlikely(!p2))uf2=1;                             /* no month in message */
  else if(unlikely(strlen(p2)<17))uf2=1;                /* too short message */
  else if(unlikely(!isdigit(*p2)))uf2=1;
  else if(unlikely(!isdigit(*(p2+1))))uf2=1;
  else if(unlikely(*(p2+2)!='-'))uf2=1;
  else if(unlikely(!isdigit(*(p2+3))))uf2=1;
  else if(unlikely(!isdigit(*(p2+4))))uf2=1;
  else if(unlikely(!isdigit(*(p2+5))))uf2=1;
  else if(unlikely(!isdigit(*(p2+6))))uf2=1;
  else if(unlikely(!isdigit(*(p2+7))))uf2=1;
  else if(unlikely(!isdigit(*(p2+8))))uf2=1;
  else if(unlikely(*(p2+9)!='['))uf2=1;
  else if(unlikely((pb2=strchr(p2+10,']'))==NULL))uf2=1;
  else if(unlikely((pc2=strchr(pb2,':'))==NULL))uf2=1;
  else uf2=0;
  /* message without header before message with header */
  if(unlikely(unlikely(uf1) && likely(!uf2)))return -1;  /* no head in msg 1 */
  if(unlikely(likely(!uf1) && unlikely(uf2)))return 1;   /* no head in msg 2 */
  /* if no header in both messages then compare strings */
  if(unlikely(unlikely(uf1) && unlikely(uf2)))
  {
    return strcasecmp(srtMsg1->msg,srtMsg2->msg);
  }
  /* compare month */
  month1=time1_tm.tm_mon;
  month2=time2_tm.tm_mon;
  if(unlikely(month1==0 && month2==11))return 1;
  if(unlikely(month1==11 && month2==0))return -1;
  if(month1<month2)return -1;
  if(month1>month2)return 1;
  /* same month: compare day and time, e.g.: 16-160228 */
  cmp=strncmp(p1,p2,9);
  if(likely(cmp))return cmp;
  /* same month, same day and time: compare hostname */
  ph1=pb1+1+strspn(pb1+1," \t");
  ph2=pb2+1+strspn(pb2+1," \t");
  cmp=strncasecmp(ph1,ph2,MIN(pc1-ph1,pc2-ph2));
  if(likely(cmp))return cmp;
  /* same month, same day and time, same hostname: compare ordinal number */
  if(srtMsg1->ord<srtMsg2->ord)return -1;
  if(srtMsg1->ord>srtMsg2->ord)return 1;
  /* compare strings */
  return strcasecmp(p1,p2);
}
/*****************************************************************************/
/* open a file for writing */
FILE *LogUtils::fileOpen(char *filePath)
{
  struct stat statBuf;
  FILE *fileFP=NULL;
  /*-------------------------------------------------------------------------*/
  if(!access(filePath,F_OK)==-1)                           /* if file exists */
  {
    if(access(filePath,W_OK)==-1)                     /* write access denied */
    {
      mPrintf(errU,ERROR,METHOD,0,"access(\"%s\"): %s!",filePath,
              strerror(errno));
      return NULL;
    }
    /* check if filePath is a regular file */
    if(stat(filePath,&statBuf)==-1)
    {
      mPrintf(errU,ERROR,METHOD,0,"stat(\"%s\"): %s!",filePath,
              strerror(errno));
      return NULL;
    }
    if(!S_ISREG(statBuf.st_mode))
    {
      mPrintf(errU,ERROR,METHOD,0,"I-node: \"%s\" is not a regular file!",
              filePath);
      return NULL;
    }
    mPrintf(errU,INFO,METHOD,0,"File \"%s\" exists. Try to open it to "
            "appending.",filePath);
    fileFP=fopen(filePath,"a");
  }                                                        /* if file exists */
  else fileFP=fopen(filePath,"w");
  if(!fileFP)
  {
    mPrintf(errU|lStd|L_SYS,FATAL,METHOD,0,"fopen(): %s! Exiting!",
           strerror(errno));
  }
  /*-------------------------------------------------------------------------*/
  return fileFP;
}
/*****************************************************************************/
FILE *LogUtils::fifoOpen(char *fifoPath,int noDrop)
{
  int fifoFD=-1;
  FILE *fifoFP=NULL;
  /*-------------------------------------------------------------------------*/
  for(;;)
  {
    fifoFD=-1;
    fifoFP=NULL;
    fifoFD=loggerOpen(fifoPath,noDrop,errU|lStd|L_SYS,ERROR);
    if(fifoFD==-1)
    {
      mPrintf(errU|lStd|L_SYS,ERROR,__func__,0,"Failed to open output logger "
              "FIFO: \"%s\"! Waiting 1 second before retrying...",fifoPath);
    }
    else                                               /* output FIFO opened */
    {
      if(noDrop)fifoFP=fdopen(fifoFD,"w");
      else fifoFP=fdopen(fifoFD,"w+");
      if(!fifoFP)
      {
        mPrintf(errU|lStd|L_SYS,ERROR,__func__,0,"Failed to open a stream for "
                "the output logger FIFO: \"%s\"! fdopen() error: %s! Waiting "
                "1 second before retrying...",fifoPath,strerror(errno));
        loggerClose(fifoFD);
      }
      else break;
    }                                                  /* output FIFO opened */
    dtq_sleep(1);
  }
  /*-------------------------------------------------------------------------*/
  return fifoFP;
}
/*****************************************************************************/
void LogUtils::printLastLog(char *lastLogSvcName,int color,
                            int severityThreshold)
{
  char *lastLog=NULL;
  int lastLogSize;
  int j;
  const int sepLen=79;
  char openSep[sepLen+1]="------ begin old messages from ";
  char intSep[sepLen+1]="... old messages from ";
  char closeSep[sepLen+1]="------ end old messages from ";
  char nameBuf[DIM_SVC_LEN+1];
  char lastLogSvcShortName[DIM_SVC_LEN+1];
  char *p;
  char *hp,*sp;
  /*-------------------------------------------------------------------------*/
  /* compose old message separators */
  snprintf(nameBuf,sizeof(nameBuf),"%s",lastLogSvcName);
  p=strrchr(nameBuf,'/');
  if(!p)return;
  *p='\0';
  p=strrchr(nameBuf,'/');
  if(!p)return;
  *p='\0';
  sp=p+1;
  p=strrchr(nameBuf,'/');
  if(!p)return;
  *p='\0';
  p=strrchr(nameBuf,'/');
  if(!p)return;
  hp=p+1;
  for(p=hp;*p;p++)*p=tolower(*p);
  snprintf(lastLogSvcShortName,sizeof(lastLogSvcShortName),"%s@%s",sp,hp);
  strncat(openSep,lastLogSvcShortName,sepLen-strlen(openSep));
  strncat(intSep,lastLogSvcShortName,sepLen-strlen(intSep));
  strncat(closeSep,lastLogSvcShortName,sepLen-strlen(closeSep));
  if((int)strlen(openSep)<sepLen)strcat(openSep," ");
  if((int)strlen(closeSep)<sepLen)strcat(closeSep," ");
  for(j=strlen(openSep);j<sepLen;j++)strcat(openSep,"-");
  for(j=strlen(closeSep);j<sepLen;j++)strcat(closeSep,"-");
  /*-------------------------------------------------------------------------*/
  /* get old messages */
  if(deBug)mPrintf(errU,VERB,METHOD,0,"lastLogSvcName=%s",lastLogSvcName);
  DimCurrentInfo LastLog(lastLogSvcName,-1);
  lastLogSize=LastLog.getSize();
  lastLog=(char*)LastLog.getString();
  if(deBug)mPrintf(errU,VERB,METHOD,0,"lastLogSize=%d",lastLogSize);
  /*-------------------------------------------------------------------------*/
  if(lastLogSize!=0 &&                          /* last_log not NULL pointer */
     lastLog[0]!=-1 &&                             /* last_log service found */
     lastLog[0]!=0)                              /* last_log not NULL string */
  {
    int i2=0;
    char *p=NULL;
    int msgLen=0;
    char **mBuf2=NULL;                /* buffer containing the last messages */
    char *cBuf2=NULL;                                   /* alias of mBuf2[0] */
    int chkdLineN2=chkdLineN;
    int isRepeated2=0;
    int minLd2=minLd;
    int minWd2=minWd;
    int ptCmp2=ptCmp;
    int filteredMsgN2=0;
    int suppressedMsgN2=0;
    int k=0;
    /*.......................................................................*/
    /* allocate memory for mBuf2 */
    mBuf2=(char**)malloc(chkdLineN2*sizeof(char*));
    for(i2=0;i2<chkdLineN2;i2++)
      mBuf2[i2]=(char*)malloc(BUF_SIZE*sizeof(char));
    cBuf2=mBuf2[0];
    /* clear the chkdLineN2 message buffers */
    for(i2=0;i2<chkdLineN2;i2++)memset(mBuf2[i2],0,BUF_SIZE);
    /*.......................................................................*/
    pthread_mutex_lock(&prLock);
    if(color)fprintf(outFP,"%s%s\n%s","\033[0;1;34;40m",openSep,"\033[0m");
    else fprintf(outFP,"%s\n",openSep);
    /*.......................................................................*/
    for(p=lastLog;;)                       /* loop over messages in last_log */
    {
      char msg[BUF_SIZE];
      msgLen=strcspn(p,"\n");
      if(!msgLen)break;
      msgLen++;                                   /* include '\n' in message */
      strncpy(msg,p,msgLen);
      msg[msgLen]='\0';
      /*.....................................................................*/
      /* filter messages */
      if(filterRegExpPattern && 
         regexec(&filterRegExp,msg,(size_t)0,NULL,0))
      {
        filteredMsgN2++;
        p+=msgLen;
        continue;
      }
      if(filterWildCardPattern && fnmatch(filterWildCardPattern,msg,0))
      {
        filteredMsgN2++;
        p+=msgLen;
        continue;
      }
      if(xFilterRegExpPattern && 
         !regexec(&xFilterRegExp,msg,(size_t)0,NULL,0))
      {
        filteredMsgN2++;
        p+=msgLen;
        continue;
      }
      if(xFilterWildCardPattern && !fnmatch(xFilterWildCardPattern,msg,0))
      {
        filteredMsgN2++;
        p+=msgLen;
        continue;
      }
      /*.....................................................................*/
      /* suppres duplicates */
      memset(cBuf2,0,BUF_SIZE);
      strncpy(cBuf2,msg,BUF_SIZE);
      /* NULL-terminate truncated messages */
      if(!memchr(cBuf2,0,BUF_SIZE))cBuf2[BUF_SIZE-1]='\0';
      /* suppress repeated messages if required */
      isRepeated2=0;
      if(minLd2>=-1||minWd2>-1||ptCmp2)
      {
        for(j=1;j<chkdLineN2;j++)
        {
          char *cStr=cBuf2,*oStr=mBuf2[j];
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
          if(ptCmp2)                                  /* punctual comparison */
          {
            if(!strcmp(cStr,oStr))
            {
              isRepeated2=1;
              break;
            }
          }
          if(minLd2>-1)                            /* Levenshtein comparison */
          {
            int d;
            d=levDist(cStr,oStr);
            if(deBug & 0x2)rPrintf(errU,2,"curr=\"%s\", old%d=\"%s\", "
                                   "levDist=%d\n",cStr,j,oStr,d);
            if(d>=0 && d<=minLd2)
            {
              isRepeated2=1;
              break;
            }
          }
          if(minWd2>-1)                          /* punctual word comparison */
          {
            int res;
            res=wordCmp(cStr,oStr,minWd2);
            if(deBug & 0x2)rPrintf(errU,2,"curr=\"%s\", old%d=\"%s\", "
                                   "wordCmp=%d\n",cStr,j,oStr,res);
            if(res)
            {
              isRepeated2=1;
              break;
            }
          }
          /* end strings comparison */
        }
      }
      /* rotate the chkdLineN2 buffers */
      cBuf2=mBuf2[chkdLineN2-1];
      for(j=chkdLineN2-1;j>0;j--)mBuf2[j]=mBuf2[j-1];
      mBuf2[0]=cBuf2;
      /* suppress repeated message */
      if(isRepeated2)
      {
        suppressedMsgN2++;
        p+=msgLen;
        continue;
      }
      /*.....................................................................*/
      if(LogUtils::printC(outFP,-1,severityThreshold,color,msg))filteredMsgN2++;
      else k++;
      if(k%24==23)
      {
        if(color)fprintf(outFP,"%s%s (%d)...\n%s","\033[0;1;34;40m",intSep,k,
                         "\033[0m");
        else fprintf(outFP,"%s (%d)...\n",intSep,k);
      }
      p+=msgLen;
    }                                      /* loop over messages in last_log */
    /*.......................................................................*/
    if(color)
    {
      fprintf(outFP,"%s------ %d old messages filtered out by client.\n%s",
              "\033[0;32;40m",filteredMsgN2,"\033[0m");
      fprintf(outFP,"%s------ %d duplicated old messages suppressed by "
              "client.\n%s","\033[0;32;40m",suppressedMsgN2,"\033[0m");
    }
    else
    {
      fprintf(outFP,"------ %d old messages filtered out by client.\n",
              filteredMsgN2);
      fprintf(outFP,"------ %d duplicated old messages suppressed by "
              "client.\n",suppressedMsgN2);
    }
    /*.......................................................................*/
    if(color)fprintf(outFP,"%s%s\n%s","\033[0;1;34;40m",closeSep,"\033[0m");
    else fprintf(outFP,"%s\n",closeSep);
    pthread_mutex_unlock(&prLock);
    /* free memory used for duplicate suppression */
    for(i2=0;i2<chkdLineN2;i2++)
    {
      if(mBuf2[i2])
      {
        free(mBuf2[i2]);
        mBuf2[i2]=NULL;
      }
    }
    if(mBuf2)
    {
      free(mBuf2);
      mBuf2=NULL;
    }
    cBuf2=NULL;
  }
  /*-------------------------------------------------------------------------*/
  return;
}
/* ######################################################################### */

