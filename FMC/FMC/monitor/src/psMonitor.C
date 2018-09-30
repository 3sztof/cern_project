/* ######################################################################### */
/*
 * $Log: psMonitor.C,v $
 * Revision 1.23  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.22  2009/07/21 15:19:09  galli
 * Improved man page
 *
 * Revision 1.20  2009/07/21 09:21:31  galli
 * added (complete) function rmPss()
 *
 * Revision 1.15  2009/04/03 12:42:30  galli
 * added (complete) function addPss()
 *
 * Revision 1.14  2009/04/03 08:17:23  galli
 * added form to add a process filter
 *
 * Revision 1.13  2009/03/19 08:01:36  galli
 * bug fixed
 * beep on forbidden movement
 *
 * Revision 1.12  2009/03/17 14:53:25  galli
 * colored fields
 *
 * Revision 1.11  2009/02/26 21:51:03  galli
 * cleanup
 *
 * Revision 1.10  2009/02/26 20:07:06  galli
 * first working version
 *
 * Revision 1.1  2009/02/23 13:22:05  galli
 * Initial revision
 */
/* ######################################################################### */
#include <string>                                             /* std::string */
#include <list>                                                 /* std::list */
#include <vector>                                               /* std::list */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <fnmatch.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>                                              /* tolower() */
#include <getopt.h>                                         /* getopt_long() */
#include <sys/wait.h>                                             /* wait(2) */
/*---------------------------------------------------------------------------*/
#include <dic.hxx>
/*---------------------------------------------------------------------------*/
/* in utils/include */
#include "fmcMsgUtils.h"           /* dfltLoggerOpen(), mPrintf(), rPrintf() */
#include "fmcCmpUtils.h"                                        /* levDist() */
#include "fmcSrvNameUtils.h"                               /* getSrvPrefix() */
#include "fmcDimUtils.h"                                  /* getDimDnsNode() */
#include "FmcUtils.IC"                            /* printLogServerVersion() */
/*---------------------------------------------------------------------------*/
/* in include */
#include "fmcPar.h"                                    /* DIM_CONF_FILE_NAME */
#include "fmcDate.h"                                     /* compilation date */
#include "fmcMacro.h"     /* eExit(), likely(), unlikely(), printOutFuncOK() */
#include "fmcVersion.h"                             /* FMC_VERSION, SVC_HEAD */
#include "fmcGroffMacro.h"                             /* macro for man page */
/*---------------------------------------------------------------------------*/
/* in logger/include */
#include "PsMoniDimErrorHandler.IC"           /* class PsMoniDimErrorHandler */
#include "PsMoniSrvSeeker.IC"                       /* class PsMoniSrvSeeker */
#include "psSrv.h"                                     /* BUF_SIZE, SRV_NAME */
/*---------------------------------------------------------------------------*/
/* curses header */
#include <ncurses.h>
#include <form.h>
#include <menu.h>
/* ######################################################################### */
using namespace std;
/* ######################################################################### */
void usage(int mode);
void shortUsage(void);
void getServerVersion(vector<string> hostPttnLs);
void drawWin(int raws, int cols, int shiftY, int shiftX,
             int contLines, int contCols);
void getFieldSz(void);
void drawLine(WINDOW *win, string line,int y,int x,int cols,int shiftX);
void drawEmptyWin(int raws, int cols);
void drawHead(int cols,int shiftX);
void drawFoot(int cols,int raws);
void checkShiftYrange(int *shiftY, int pShiftX, int maxShiftY);
void checkShiftXrange(int *shiftX, int pShiftY, int maxShiftX);
static int file2str(const char *path,char *buf,int bufSz);
int getMaxPid(void);
void addPss(void);
void rmPss(void);
/* ######################################################################### */
/* globals */
/* counters */
unsigned long long suppressedMsgN=0;
/* Running Parameters */
char *pName=NULL;
char *utgid=NULL;
int deBug=0;
FILE *outFP=NULL;
int errU=L_STD;
pthread_t mainPtid;                  /* thread identifier of the main thread */
/* variables */
list<string> csList;               /* list of servers which changed settings */
/* start-up messages */
string propertiesMsg;
string lvFunction;
/* Version IDs */
static char rcsid[]="$Id: psMonitor.C,v 1.23 2012/11/29 15:52:27 galli Exp galli $";
char *rcsidP;
/* ------------------------------------------------------------------------- */
/* output table */
list<string> outTbl;
pthread_mutex_t outTblLock=PTHREAD_MUTEX_INITIALIZER;
int headerSz=1;
int footerSz=3;
size_t fieldLen[25];
size_t lineLen;
unsigned psSrvN;
/* ######################################################################### */
int main(int argc,char **argv)
{
  /* counters */
  int i=0;
  /* pointers */
  char *p;
  char *deBugS=NULL;
  /* service browsing */
  vector<string> hostPttnLs;                          /* host pattern vector */
  vector<string> cmdPttnLs;                    /* process CMD pattern vector */
  vector<string> utgidPttnLs;                /* process UTGID pattern vector */
  vector<string> srvPttnLs;                         /* server pattern vector */
  vector<string>::iterator hostIt;           /* host pattern vector iterator */
  vector<string>::iterator cmdIt;     /* process CMD pattern vector iterator */
  vector<string>::iterator utgidIt; /* process UTGID pattern vector iterator */
  vector<string>::iterator srvIt;          /* server pattern vector iterator */
  DimBrowser br;
  char *dimDnsNode=NULL;
  /* command line switch */
  int doPrintServerVersion=0;
  /* getopt */
  char flag;
  const char *shortOptions="vN:m:C:U:Vh::";
  static struct option longOptions[]=
  {
    {"debug",no_argument,NULL,'v'},
    {"dim-dns",required_argument,NULL,'N'},
    {"hostname",required_argument,NULL,'m'},
    {"select-cmd",required_argument,NULL,'C'},
    {"select-utgid",required_argument,NULL,'U'},
    {"version",required_argument,NULL,'V'},
    {"help",optional_argument,NULL,'h'},
    {NULL,0,NULL,0}
  };
  /* messages */ 
  string startUpMsg;
  /* curses variables */
  int maxWx,maxWy;
  int shiftX,shiftY;
  int pShiftX,pShiftY;
  int maxShiftX,maxShiftY;
  /*-------------------------------------------------------------------------*/
  /* Strip rcsid */
  rcsidP=strchr(rcsid,':')+2;
  for(i=0,p=rcsidP;i<5;i++)p=1+strchr(p,' ');
  *(p-1)='\0';
  /*-------------------------------------------------------------------------*/
  pName=strdup(basename(argv[0]));
  utgid=getenv("UTGID");
  if(!utgid)utgid=(char*)"no UTGID";
  /*-------------------------------------------------------------------------*/
  /* process command line options */
  opterr=0;                  /* do not print default error message to stderr */
  while((flag=getopt_long(argc,argv,shortOptions,longOptions,NULL))!=EOF)
  {
    switch(flag)
    {
      case 'v': /* -v, --debug */
        deBug=0xffff;
        break;
      case 'N': /* -N, --dim-dns */
        dimDnsNode=optarg;
        break;
      case 'm': /* -m, --hostname */
        hostPttnLs.push_back(optarg);
        break;
      case 'C': /* -C, --select-cmd */
        cmdPttnLs.push_back(optarg);
        break;
      case 'U': /* -U, --select-utgid */
        utgidPttnLs.push_back(optarg);
        break;
      case 'V': /* -V, --version */
        doPrintServerVersion=1;
        break;
      case 'h': /* -h, --help */
        if(!optarg)usage(0);                   /* -h: open manual using less */
        else usage(1);                     /* -hh: print raw groff to stdout */
      default:
        fprintf(stderr,"\ngetopt_long(): invalid option \"%s\"!\n",
                argv[optind-1]);
        shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  if(dfltLoggerOpen(1,0,WARN,DEBUG,1)!=-1)errU=L_DIM;
  else
  {
    mPrintf(errU,INFO,__func__,0,"Can't initialize standard FMC Message "
            "Logger. Using syslog instead.");
    errU=L_SYS;
  }
  /*-------------------------------------------------------------------------*/
  /* set the DIM DNS node */
  if(dimDnsNode)
  {
    mPrintf(errU|L_STD,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from -N "
            "command-line option).",dimDnsNode);
  }
  else
  {
    dimDnsNode=getenv("DIM_DNS_NODE");
    if(dimDnsNode)
    {
      mPrintf(errU|L_STD,DEBUG,__func__,0,"DIM_DNS_NODE: \"%s\" (from "
              "\""DIM_CONF_FILE_NAME"\" file).",dimDnsNode);
    }
    else
    {
      dimDnsNode=getDimDnsNode(DIM_CONF_FILE_NAME,0,1);
      if(dimDnsNode)
      {
        printf("DIM_DNS_NODE: \"%s\" (from \""DIM_CONF_FILE_NAME" file).\n",
               dimDnsNode);
      }
      else
      {
        mPrintf(errU|L_STD,FATAL,__func__,0,"DIM Name Server (DIM_DNS_NODE) "
                "not defined!\nDIM Name Server can be defined (in decreasing "
                "order of priority):\n"
                "  1. Specifying the -N DIM_DNS_NODE command-line option;\n"
                "  2. Specifying the DIM_DNS_NODE environment variable;\n"
                "  3. Defining the DIM_DNS_NODE in the file "
                "\""DIM_CONF_FILE_NAME"\".\n");
        exit(1);
      }
    }
  }
  if(setenv("DIM_DNS_NODE",dimDnsNode,1))
  {
    mPrintf(errU|L_STD,FATAL,__func__,0,"setenv(): %s!",strerror(errno));
    exit(1);
  }
  /*-------------------------------------------------------------------------*/
  /* read environment variables */
  deBugS=getenv("deBug");
  if(deBugS)deBug=(int)strtol(deBugS,(char**)NULL,0);
  /*-------------------------------------------------------------------------*/
  /* default wildcard for hostPttnLs cmdPttnLs and utgidPttnLs */
  if(hostPttnLs.empty())hostPttnLs.push_back("*");
  if(cmdPttnLs.empty())cmdPttnLs.push_back("*");
  if(utgidPttnLs.empty())utgidPttnLs.push_back("*");
  /*-------------------------------------------------------------------------*/
  /* check hostPttnLs */
  for(hostIt=hostPttnLs.begin();hostIt!=hostPttnLs.end();hostIt++)
  {
    if(hostIt->find_first_of('/')!=string::npos)
    {
      mPrintf(errU|L_STD,FATAL,__func__,0,"Illegal NODE_PATTERN "
              "\"%s\"! A NODE_PATTERN specified with the command-line option "
              "\"-m NODE_PATTERN\" or \"--hostname NODE_PATTERN\" must not "
              "contain the '/' character!",hostIt->c_str());
      shortUsage();
    }
  }
  /*-------------------------------------------------------------------------*/
  /* compose srvPttnLs */
  for(hostIt=hostPttnLs.begin();hostIt!=hostPttnLs.end();hostIt++)
  {
    srvPttnLs.push_back(string(SVC_HEAD)+"/"+FmcUtils::toUpper(*hostIt)+
                        "/"+SRV_NAME);
  }
  /*-------------------------------------------------------------------------*/
  /* handling of -V option */
  if(doPrintServerVersion)
  {
    getServerVersion(hostPttnLs);
  }
  /*-------------------------------------------------------------------------*/
  /* define DIM error handler */
  new PsMoniDimErrorHandler();
  /*-------------------------------------------------------------------------*/
  /* compose properties message propertiesMsg */
  propertiesMsg+=string("TGID = ")+FmcUtils::utos(getpid())+". ";
  propertiesMsg+=string("Software version: \"");
  propertiesMsg+=string(rcsidP)+"\", \"FMC-"+FMC_VERSION+"\". ";
  propertiesMsg+="DIM SRV pattern list = (";
  for(srvIt=srvPttnLs.begin();srvIt!=srvPttnLs.end();srvIt++)
  {
    if(srvIt!=srvPttnLs.begin())propertiesMsg+=", ";
    propertiesMsg+=*srvIt;
  }
  propertiesMsg+="); ";
  propertiesMsg+="Process CMD pattern list = (";
  for(cmdIt=cmdPttnLs.begin();cmdIt!=cmdPttnLs.end();cmdIt++)
  {
    if(cmdIt!=cmdPttnLs.begin())propertiesMsg+=", ";
    propertiesMsg+=*cmdIt;
  }
  propertiesMsg+="); ";
  propertiesMsg+="Process UTGID pattern list = (";
  for(utgidIt=utgidPttnLs.begin();utgidIt!=utgidPttnLs.end();utgidIt++)
  {
    if(utgidIt!=utgidPttnLs.begin())propertiesMsg+=", ";
    propertiesMsg+=*utgidIt;
  }
  propertiesMsg+="); ";
  propertiesMsg.erase(propertiesMsg.length()-2,2);       /* esase final "; " */
  /*-------------------------------------------------------------------------*/
  new PsMoniSrvSeeker("DIS_DNS/SERVER_LIST",srvPttnLs,cmdPttnLs,utgidPttnLs);
  /*-------------------------------------------------------------------------*/
  /* print start-up message */
  startUpMsg=string("FMC Process Monitor Client started. ")+
             propertiesMsg+".";
  mPrintf(errU|L_STD,INFO,__func__,0,"%s",startUpMsg.c_str());
  /*-------------------------------------------------------------------------*/
  dtq_sleep(1);            /* leave time to seekers for initial subscription */
  /*-------------------------------------------------------------------------*/
  /* curses settings */
  initscr();                                            /* Start curses mode */
  if(has_colors()==FALSE)
  {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  start_color();
  init_pair(1,COLOR_WHITE,   COLOR_BLACK);
  init_pair(2,COLOR_RED,     COLOR_BLACK);
  init_pair(3,COLOR_GREEN,   COLOR_BLACK);
  init_pair(4,COLOR_YELLOW,  COLOR_BLACK);
  init_pair(5,COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6,COLOR_BLACK,   COLOR_WHITE);
  init_pair(7,COLOR_GREEN,   COLOR_BLACK);
  init_pair(8,COLOR_WHITE,   COLOR_BLUE);
  noecho();
  cbreak();
  wtimeout(stdscr,1000);
  keypad(stdscr,TRUE);                         /* undestand KEY_UP, KEY_DOWN */
  curs_set(0);                                       /* set cursor invisible */
  getmaxyx(stdscr,maxWy,maxWx);                       /* get maxWy and maxWx */
  /*-------------------------------------------------------------------------*/
  pthread_mutex_lock(&outTblLock);
  getFieldSz();                      /* evaluate max size of fields and line */
  maxShiftY=MAX(outTbl.size()-maxWy+headerSz+footerSz,0);
  maxShiftX=lineLen-maxWx;
  if(maxShiftX<0)maxShiftX=0;
  pthread_mutex_unlock(&outTblLock);
  /* ----------------------------------------------------------------------- */
  shiftY=0;
  shiftX=0;
  pthread_mutex_lock(&outTblLock);
  getFieldSz();                      /* evaluate max size of fields and line */
  drawWin(maxWy,maxWx,shiftY,shiftX,outTbl.size(),lineLen);
  pthread_mutex_unlock(&outTblLock);
  /* ----------------------------------------------------------------------- */
  /* main loop */
  while(1)
  {
    pShiftX=shiftX;
    pShiftY=shiftY;
    switch(getch())
    {
      /* ................................................................... */
      /* quit */
      case 'q':                                                  /* vi-style */
      case 'Q':                                                  /* vi-style */
      case KEY_EXIT:                                             /* exit key */
        endwin();                                         /* End curses mode */
        return 0;
      /* ................................................................... */
      /* backward one line */
      case 'k':                                                  /* vi-style */ 
      case 'K':                                                  /* vi-style */ 
      case 'p':                                                /* pine-style */
      case 'P':                                                /* pine-style */
      case KEY_UP:                                           /* up-arrow key */
        shiftY--;
        break;
      /* ................................................................... */
      /* forward one line */
      case 'j':                                                  /* vi-style */
      case 'J':                                                  /* vi-style */
      case 'n':                                                /* pine-style */
      case 'N':                                                /* pine-style */
      case '\n':                                                   /* return */
      case KEY_DOWN:                                       /* down-arrow key */
      case KEY_ENTER:                                      /* enter/send key */
        shiftY++;
        break;
      /* ................................................................... */
      /* backward half page */
      case 'u':                                                  /* vi-style */
      case 'U':                                                  /* vi-style */
      case KEY_SR:                                    /* scroll-backward key */
        shiftY-=((maxWy-headerSz-footerSz)/2);
        break;
      /* ................................................................... */
      /* forward half page */
      case 'd':                                                  /* vi-style */
      case 'D':                                                  /* vi-style */
      case KEY_SF:                                     /* scroll-forward key */
        shiftY+=((maxWy-headerSz-footerSz)/2);
        break;
      /* ................................................................... */
      /* backward one page */
      case 'b':                                                  /* vi-style */
      case 'B':                                                  /* vi-style */
      case '-':                                                /* pine-style */
      case KEY_PPAGE:                                   /* previous-page key */
        shiftY-=(maxWy-headerSz-footerSz);
        break;
      /* ................................................................... */
      /* forward one page */
      case 'f':                                                  /* vi-style */
      case 'F':                                                  /* vi-style */
      case ' ':                                                /* less-style */
      case KEY_NPAGE:                                       /* next-page key */
        shiftY+=(maxWy-headerSz-footerSz);
        break;
      /* ................................................................... */
      /* beginning */
      case 'g':                                                /* less-style */
      case KEY_HOME:                                             /* home key */
        shiftY=0;
        break;
      /* ................................................................... */
      /* end */
      case 'G':                                             /* vi/less-style */
      case KEY_END:                                               /* end key */
        shiftY=maxShiftY;
        break;
      /* ................................................................... */
      /* left one column */
      case 'h':                                                  /* vi-style */ 
      case KEY_LEFT:                                       /* left-arrow key */
        shiftX--;
        break;
      /* ................................................................... */
      /* right one column */
      case 'l':                                                  /* vi-style */
      case KEY_RIGHT:                                     /* right-arrow key */
        shiftX++;
        break;
      /* ................................................................... */
      /* left one page */
      case 'H':                                                  /* vi-style */ 
      case KEY_BTAB:                                        /* shift-tab key */
        shiftX-=maxWx;
        break;
      /* ................................................................... */
      /* right one page */
      case 'L':                                                  /* vi-style */
      case '\t':                                                  /* tab key */
        shiftX+=maxWx;
        break;
      /* ................................................................... */
      /* terminal resize (SIGWINCH received) */
      case KEY_RESIZE:                              /* terminal resize event */
        getmaxyx(stdscr,maxWy,maxWx);                 /* get maxWy and maxWx */
        break;
      /* ................................................................... */
      case 'a':
        addPss();
        break;
      /* ................................................................... */
      case 'r':
        rmPss();
        break;
      /* ................................................................... */
    }                                                     /* switch(getch()) */
    pthread_mutex_lock(&outTblLock);
    maxShiftY=outTbl.size()-maxWy+headerSz+footerSz;
    if(maxShiftY<0)maxShiftY=0;
    getFieldSz();                    /* evaluate max size of fields and line */
    maxShiftX=lineLen-maxWx;
    if(maxShiftX<0)maxShiftX=0;
    checkShiftYrange(&shiftY,pShiftY,maxShiftY);
    checkShiftXrange(&shiftX,pShiftX,maxShiftX);
    drawWin(maxWy,maxWx,shiftY,shiftX,outTbl.size(),lineLen);
    pthread_mutex_unlock(&outTblLock);
  }                                                             /* main loop */
  /*-------------------------------------------------------------------------*/
  endwin();                                               /* End curses mode */
  return 0;
}
/*****************************************************************************/
void checkShiftYrange(int *shiftY, int pShiftY, int maxShiftY)
{
  if(*shiftY>maxShiftY)
  {
    *shiftY=maxShiftY;
    if(pShiftY>=maxShiftY)beep();
  }
  if(*shiftY<0)
  {
    *shiftY=0;
    if(pShiftY<=0)beep();
  }
}
/*****************************************************************************/
void checkShiftXrange(int *shiftX, int pShiftX, int maxShiftX)
{
  if(*shiftX>maxShiftX)
  {
    *shiftX=maxShiftX;
    if(pShiftX>=maxShiftX)beep();
  }
  if(*shiftX<0)
  {
    *shiftX=0;
    if(pShiftX<=0)beep();
  }
}
/*****************************************************************************/
/* accesses globals: headerSz, footerSz, header, footer */
/* raws = terminal lines */
void drawWin(int raws, int cols, int shiftY, int shiftX,
             int contLines, int contCols)
{
  int i;
  list<string>::iterator otIt;           /* iterator over output table lines */
  /* ----------------------------------------------------------------------- */
  if(!contLines)
  {
    drawEmptyWin(raws,cols);
    return;
  }
  werase(stdscr);
  getFieldSz();                      /* evaluate max size of fields and line */
  drawHead(cols,shiftX);                                      /* draw header */
  /* skip shiftY lines */
  otIt=outTbl.begin();
  for(i=0;i<shiftY && otIt!=outTbl.end();i++,otIt++);
  /* draw content */
  for(i=0;i<raws-headerSz-footerSz && otIt!=outTbl.end() ;i++,otIt++)
  {
    if(otIt!=outTbl.end())
    {
      drawLine(stdscr,*otIt,headerSz+i,0,cols,shiftX);
    }
  }
  drawFoot(cols,raws);                                        /* draw footer */
  refresh();                               /* Print it on to the real screen */
  return;
}
/*****************************************************************************/
void drawLine(WINDOW *win, string rawLine,int y,int x,int cols,int shiftX)
{
  string textField;
  string colorField;
  string fillField;
  string textLine;
  string colorLine;
  string cutTextLine;
  string cutColorLine;
  string lineSegment;
  char curCol;
  int i;
  char color;                                  /* W: white, R: red, G: green */
  int fillLen;
  size_t b=0,e=0;
  /* ----------------------------------------------------------------------- */
  for(i=0;i<25;i++)
  {
    if(i>0)
    {
      textLine+=" ";                                 /* space between fields */
      colorLine+="W";                      /* space between fields are white */
    }
    color=rawLine.at(b+2);
    b+=4;                                                     /* begin field */
    e=rawLine.find("%%",b);                                     /* end field */
    textField=rawLine.substr(b,e-b);
    fillLen=fieldLen[i]-textField.length();/* length to be filled to justify */
    if(i==0||i==2||i==3||i==22||i==23||i==24)     /* left justified (string) */
      fillField=textField+string(fillLen,' ');
    else                                         /* right justified (number) */
      fillField=string(fillLen,' ')+textField;
    colorField=string(fieldLen[i],color);
    textLine+=fillField;
    colorLine+=colorField;
    b=e;
  }
  textLine.resize(lineLen,' ');
  colorLine.resize(lineLen,'W');
  /* ----------------------------------------------------------------------- */
  cutTextLine=textLine.substr(shiftX);
  cutColorLine=colorLine.substr(shiftX);
  cutTextLine.resize(cols,' ');
  cutColorLine.resize(cols,'W');
  /* ----------------------------------------------------------------------- */
  for(b=0,i=0;;i++)
  {
    curCol=cutColorLine[b];
    e=cutColorLine.find_first_not_of(curCol,b);
    if(e==string::npos)e=cutColorLine.length();
    lineSegment=cutTextLine.substr(b,e-b);
    if(curCol=='W')wattron(stdscr,COLOR_PAIR(1));
    else
    {
      wattron(stdscr,A_BOLD);
      if(curCol=='R')wattron(stdscr,COLOR_PAIR(2));
      else if(curCol=='G')wattron(stdscr,COLOR_PAIR(3));
      else if(curCol=='Y')wattron(stdscr,COLOR_PAIR(4));
      else if(curCol=='M')wattron(stdscr,COLOR_PAIR(5));
    }
    mvwprintw(win,y,x+b,lineSegment.c_str());
    if(curCol=='W')wattroff(stdscr,COLOR_PAIR(1));
    else
    {
      if(curCol=='R')wattroff(stdscr,COLOR_PAIR(2));
      else if(curCol=='G')wattroff(stdscr,COLOR_PAIR(3));
      else if(curCol=='Y')wattroff(stdscr,COLOR_PAIR(4));
      else if(curCol=='M')wattroff(stdscr,COLOR_PAIR(5));
      wattroff(stdscr,A_BOLD);
    }

    if(e==cutColorLine.length())break;
    b=e;
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
/* access globals: outTbl(r), fieldLen[25](w), lineLen (w) */
void getFieldSz(void)
{
  list<string>::iterator otIt;           /* iterator over output table lines */
  int i;
  /* ----------------------------------------------------------------------- */
  /* inizialize to label length */
  fieldLen[0]=strlen("HOST");
  fieldLen[1]=strlen("TGID");
  fieldLen[2]=strlen("CMD");
  fieldLen[3]=strlen("UTGID");
  fieldLen[4]=strlen("S");
  fieldLen[5]=strlen("NLWP");
  fieldLen[6]=strlen("P");
  if(fieldLen[7]<strlen("%CPU"))fieldLen[7]=strlen("%CPU");
  if(fieldLen[8]<strlen("%MEM"))fieldLen[8]=strlen("%MEM");
  fieldLen[9]=strlen("MIN_FLT");
  fieldLen[10]=strlen("MAJ_FLT");
  fieldLen[11]=strlen("VSIZE");
  fieldLen[12]=strlen("LOCK");
  fieldLen[13]=strlen("RSS");
  fieldLen[14]=strlen("DATA");
  fieldLen[15]=strlen("STACK");
  fieldLen[16]=strlen("EXE");
  fieldLen[17]=strlen("LIB");
  fieldLen[18]=strlen("SHARE");
  fieldLen[19]=strlen("RT");
  fieldLen[20]=strlen("NI");
  fieldLen[21]=strlen("PR");
  fieldLen[22]=strlen("LAST_UPDATE");
  fieldLen[23]=strlen("USER");
  fieldLen[24]=strlen("CMDLINE");
  /* ----------------------------------------------------------------------- */
  for(otIt=outTbl.begin();otIt!=outTbl.end();otIt++)
  {
    size_t b=4;
    size_t e=4;
    for(i=0;i<25;i++)
    {
      e=otIt->find("%%",b);
      if(fieldLen[i]<e-b)fieldLen[i]=e-b;
      b=e+4;
    }
  }
  /* ----------------------------------------------------------------------- */
  lineLen=0;
  for(i=0;i<25;i++)
  {
    if(i>0)lineLen++;                                   /* 1-space separator */
    lineLen+=fieldLen[i];
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
void drawEmptyWin(int raws, int cols)
{
  const char *s1="NO PROCESS MONITORED!";
  const char *s2="Press <a> to monitor process(es)!";
  const char *s3="NO NODES CONNECTED!";
  const char *s4="Please, start the FMC Process Monitor Server (psSrv) at all "
                 "the farm nodes";
  const char *s5="or choose a different DIM Name Server with the -N "
                 "command-line option!";
  /* ----------------------------------------------------------------------- */
  werase(stdscr);
  if(psSrvN)
  {
    mvwprintw(stdscr,raws/2-1,(cols-strlen(s1))/2,s1);
    mvwprintw(stdscr,raws/2+1,(cols-strlen(s2))/2,s2);
  }
  else
  {
    mvwprintw(stdscr,raws/2-1,(cols-strlen(s3))/2,s3);
    mvwprintw(stdscr,raws/2+1,(cols-strlen(s4))/2,s4);
    mvwprintw(stdscr,raws/2+2,(cols-strlen(s5))/2,s5);
  }
  drawFoot(cols,raws);                                        /* draw footer */
  refresh();                               /* Print it on to the real screen */
  return;
}
/*****************************************************************************/
void drawHead(int cols,int shiftX)
{
  char *headLine;
  string cutHeadLine;
  size_t p;
  /* ----------------------------------------------------------------------- */
  /* assebly header line */
  headLine=(char*)alloca(1+lineLen);
  snprintf(headLine,1+lineLen,
           "%-*s %*s %-*s %-*s %*c %*s %*s %*s %*s %*s %*s %*s %*s %*s "
           "%*s %*s %*s %*s %*s %*s %*s %*s %-*s %-*s %-*s",
           (int)fieldLen[0],  "HOST",
           (int)fieldLen[1],  "TGID",
           (int)fieldLen[2],  "CMD",
           (int)fieldLen[3],  "UTGID",
           (int)fieldLen[4],  'S',
           (int)fieldLen[5],  "NLWP",
           (int)fieldLen[6],  "P",
           (int)fieldLen[7],  "PCPU",
           (int)fieldLen[8],  "PMEM",
           (int)fieldLen[9],  "MIN_FLT",
           (int)fieldLen[10], "MAJ_FLT",
           (int)fieldLen[11], "VSIZE",
           (int)fieldLen[12], "LOCK",
           (int)fieldLen[13], "RSS",
           (int)fieldLen[14], "DATA",
           (int)fieldLen[15], "STACK",
           (int)fieldLen[16], "EXE",
           (int)fieldLen[17], "LIB",
           (int)fieldLen[18], "SHARE",
           (int)fieldLen[19], "RT",
           (int)fieldLen[20], "NI",
           (int)fieldLen[21], "PR",
           (int)fieldLen[22], "LAST_UPDATE",
           (int)fieldLen[23], "USER",
           (int)fieldLen[24], "CMDLINE");
  cutHeadLine=headLine;
  cutHeadLine.resize(lineLen,' ');
  cutHeadLine=cutHeadLine.substr(shiftX);
  cutHeadLine.resize(cols,' ');
  p=cutHeadLine.find("PCPU");
  if(p!=string::npos)cutHeadLine=cutHeadLine.replace(p,4,"%%CPU");
  p=cutHeadLine.find("PMEM");
  if(p!=string::npos)cutHeadLine=cutHeadLine.replace(p,4,"%%MEM");
  /* ----------------------------------------------------------------------- */
  /* draw header line */
  wattron(stdscr,A_REVERSE|A_BOLD);
  mvwprintw(stdscr,0,0,cutHeadLine.c_str());
  wattroff(stdscr,A_REVERSE|A_BOLD);
  return;
}
/*****************************************************************************/
void drawFoot(int cols,int raws)
{
  string footLine[3];
  /* ----------------------------------------------------------------------- */
  /* assebly footer line */
  footLine[0]="q Quit  ";
  footLine[1]="        ";
  footLine[0]+="a Add ps     ";
  footLine[1]+="r Remove ps  ";
  footLine[0]+="k Up-1-line    ";
  footLine[1]+="j Down-1-line  ";
  footLine[0]+="l Right-1-col  ";
  footLine[1]+="h Left-1-col   ";
  footLine[0]+="b Up-1-page    ";
  footLine[1]+="f Down-1-page  ";
  footLine[0]+="L Right-1-page  ";
  footLine[1]+="H Left-1-page   ";
  footLine[0]+="g 1st-line ";
  footLine[1]+="G Last-line";
  footLine[0].resize(cols,' ');
  footLine[1].resize(cols,' ');
  footLine[2]=FmcUtils::ultos(psSrvN)+" node(s) connected.";
  footLine[2]=string("psMonitor ")+string(cols-footLine[2].length()-10,' ')
              +footLine[2];
  /* ----------------------------------------------------------------------- */
  /* draw footer line */
  wattron(stdscr,A_REVERSE|A_BOLD);
  mvwprintw(stdscr,raws-3,0,footLine[0].c_str());
  mvwprintw(stdscr,raws-2,0,footLine[1].c_str());
  wattroff(stdscr,A_REVERSE|A_BOLD);
  wattron(stdscr,COLOR_PAIR(8));
  mvwprintw(stdscr,raws-1,0,"%s",footLine[2].c_str());
  wattroff(stdscr,COLOR_PAIR(8));
  return;
}
/*****************************************************************************/
void addPss(void)
{
  FIELD *field[7];
  FORM  *psAddForm;
  WINDOW *psAddWin;
  int ch, rows, cols;
  string hostPttn="N/A";
  string ucHostPttn="N/A";
  string utgid="N/A";
  string cmd="N/A";
  pid_t tgid=-2;
  char *tgidS=NULL;
  int curField=0;
  int confirmed=0;
  int cancelled=0;
  int escPressed=0;
  int maxPid;
  int maxPidSize;
  string args;
  string fineCmdPttn;
  string coarseCmdPttn;
  DimBrowser br;
  int type=0;
  char *startCmd=NULL;
  char *format=NULL;
  /* ----------------------------------------------------------------------- */
  maxPid=getMaxPid();
  maxPidSize=snprintf(NULL,0,"%d",maxPid);
  tgidS=(char*)alloca(1+maxPidSize);
  /* ----------------------------------------------------------------------- */
  /* Initialize the fields */
  field[0]=new_field(1,64,1,8,0,0);                                  /* HOST */
  field[1]=new_field(1,64,3,8,0,0);                                 /* UTGID */
  field[2]=new_field(1,CMD_LEN,5,8,0,0);                              /* CMD */
  field[3]=new_field(1,maxPidSize,7,8,0,0);                          /* TGID */
  field[4]=new_field(1,6,9,57,0,0);                                /* Cancel */
  field[5]=new_field(1,2,9,67,0,0);                                    /* OK */
  field[6]=NULL;
  /* Set field options */
  set_field_fore(field[0], COLOR_PAIR(6));                           /* HOST */
  set_field_back(field[0], COLOR_PAIR(8));                           /* HOST */
  field_opts_off(field[0], O_AUTOSKIP);                              /* HOST */
  set_field_fore(field[1], COLOR_PAIR(6));                          /* UTGID */
  set_field_back(field[1], COLOR_PAIR(8));                          /* UTGID */
  field_opts_off(field[1], O_AUTOSKIP);                             /* UTGID */
  set_field_fore(field[2], COLOR_PAIR(6));                            /* CMD */
  set_field_back(field[2], COLOR_PAIR(8));                            /* CMD */
  field_opts_off(field[2], O_AUTOSKIP);                               /* CMD */
  set_field_fore(field[3], COLOR_PAIR(6));                           /* TGID */
  set_field_back(field[3], COLOR_PAIR(8));                           /* TGID */
  field_opts_off(field[3], O_AUTOSKIP);                              /* TGID */
  set_field_type(field[3], TYPE_INTEGER,0,0,maxPid);                 /* TGID */
  set_field_back(field[4], COLOR_PAIR(2)|A_BOLD);                  /* Cancel */
  field_opts_off(field[4], O_EDIT);                                /* Cancel */
  set_field_buffer(field[4],0,"Cancel");                           /* Cancel */
  set_field_back(field[5], COLOR_PAIR(7)|A_BOLD);                      /* OK */
  field_opts_off(field[5], O_EDIT);                                    /* OK */
  set_field_buffer(field[5],0,"OK");                                   /* OK */
  /* Create the form and post it */
  psAddForm=new_form(field);
  /* Calculate the area required for the form */
  scale_form(psAddForm,&rows,&cols);
  /* Create the window to be associated with the form */
  psAddWin=newwin(rows+7,cols+4,1,2);
  keypad(psAddWin,TRUE);
  /* Set main window and sub window */
  set_form_win(psAddForm,psAddWin);
  set_form_sub(psAddForm,derwin(psAddWin,rows,cols,3,2));
  /* Print a border around the main window and print a title */
  box(psAddWin,0,0);
  wattron(psAddWin,COLOR_PAIR(4)|A_BOLD);
  mvwprintw(psAddWin,1,2,"%s","Set-up a new process selection filter:");
  wattroff(psAddWin,COLOR_PAIR(4)|A_BOLD);
  mvwprintw(psAddWin,14,2,"    Confirm     TAB Next-field       "
            "Mov-inside-field");
  mvwprintw(psAddWin,15,2,"ESC Cancel   SH-TAB Prev-field       "
            "Del-prev-char     DEL Del-curr-char");
  /* print "Return" symbol */
  mvwaddch(psAddWin,14,2,ACS_LARROW);
  mvwaddch(psAddWin,14,3,ACS_HLINE);
  mvwaddch(psAddWin,14,4,ACS_LRCORNER);
  /* print "Arrow" symbols */
  mvwaddch(psAddWin,14,34,ACS_HLINE);
  mvwaddch(psAddWin,14,35,ACS_RARROW);
  mvwaddch(psAddWin,14,36,ACS_LARROW);
  mvwaddch(psAddWin,14,37,ACS_HLINE);
  /* print "Backspace" symbol */
  mvwaddch(psAddWin,15,34,ACS_LARROW);
  mvwaddch(psAddWin,15,35,ACS_HLINE);
  mvwaddch(psAddWin,15,36,ACS_HLINE);
  mvwaddch(psAddWin,15,37,ACS_HLINE);
  /* print 2 separator lines */
  mvwaddch(psAddWin,2,0,ACS_LTEE);
  mvwhline(psAddWin,2,1,ACS_HLINE,74);
  mvwaddch(psAddWin,2,75,ACS_RTEE);
  mvwaddch(psAddWin,13,0,ACS_LTEE);
  mvwhline(psAddWin,13,1,ACS_HLINE,74);
  mvwaddch(psAddWin,13,75,ACS_RTEE);
  wrefresh(stdscr);
  post_form(psAddForm);
  /* field labels */
  mvwprintw(psAddWin,4,2,"%s","HOST:");
  mvwprintw(psAddWin,6,2,"%s","UTGID:");
  mvwprintw(psAddWin,8,2,"%s","CMD:");
  mvwprintw(psAddWin,10,2,"%s","TGID:");
  set_current_field(psAddForm,field[0]);
  wrefresh(psAddWin);
  pos_form_cursor(psAddForm);
  wrefresh(stdscr);
  curs_set(2);                                         /* set cursor visible */
  /* ----------------------------------------------------------------------- */
  /* Loop through to get user requests */
  for(confirmed=0,cancelled=0;;)                            /* wgetch() loop */
  {
    ch=wgetch(psAddWin);
    curField=field_index(current_field(psAddForm));
    if(ch=='\n' && (confirmed || cancelled))break;
    switch(ch)
    {
      /* ................................................................... */
      case KEY_DOWN:
      case '\t':
      case '\n':
        form_driver(psAddForm,REQ_NEXT_FIELD);
        form_driver(psAddForm,REQ_BEG_LINE);
        curField=field_index(current_field(psAddForm));
        break;
      case KEY_UP:
      case KEY_BTAB:                                        /* shift-tab key */
        form_driver(psAddForm,REQ_PREV_FIELD);
        form_driver(psAddForm,REQ_BEG_LINE);
        curField=field_index(current_field(psAddForm));
        break;
      case KEY_PPAGE:                                   /* previous-page key */
        form_driver(psAddForm,REQ_FIRST_FIELD);
        form_driver(psAddForm,REQ_BEG_LINE);
        curField=field_index(current_field(psAddForm));
        break;
      case KEY_NPAGE:                                       /* next-page key */
        form_driver(psAddForm,REQ_LAST_FIELD);
        form_driver(psAddForm,REQ_BEG_LINE);
        curField=field_index(current_field(psAddForm));
        break;
      /* ................................................................... */
      case KEY_LEFT:                                       /* left-arrow key */
        form_driver(psAddForm,REQ_LEFT_CHAR);
        break;
      case KEY_RIGHT:                                     /* right-arrow key */
        form_driver(psAddForm,REQ_RIGHT_CHAR);
        break;
      case KEY_HOME:                                             /* home key */
        form_driver(psAddForm,REQ_BEG_FIELD);
        break;
      case KEY_END:                                               /* end key */
        form_driver(psAddForm,REQ_END_FIELD);
        break;
      /* ................................................................... */
      case KEY_DC:                                             /* delete key */
        form_driver(psAddForm,REQ_DEL_CHAR);
        break;
      case KEY_BACKSPACE:                                          /* Ctrl-H */
      case 0x7f:                                                /* ASCII DEL */
        form_driver(psAddForm,REQ_DEL_PREV);
        break;
      /* ................................................................... */
      case 0x1b:                                                   /* escape */
        escPressed=1;
        break;
      /* ................................................................... */
      default:
        /* If this is a normal character, it gets */
        /* Printed				  */	
        form_driver(psAddForm,ch);
        break;
      /* ................................................................... */
    }                                                          /* switch(ch) */
    if(escPressed)
    {
      cancelled=1;
      break;
    }
    if(curField==4)cancelled=1;
    else cancelled=0;
    if(curField==5)confirmed=1;
    else confirmed=0;
  }                                                         /* wgetch() loop */
  /* ----------------------------------------------------------------------- */
  if(confirmed)
  {
    size_t bl;
    /* get HOST */
    hostPttn=field_buffer(field[0],0);
    bl=hostPttn.find_first_of(" \t");
    if(bl!=string::npos)hostPttn=hostPttn.substr(0,bl); /* cut @ first blank */
    if(hostPttn=="")hostPttn="*";                             /* default "*" */
    /* get UTGID */
    utgid=field_buffer(field[1],0);
    bl=utgid.find_first_of(" \t");
    if(bl!=string::npos)utgid=utgid.substr(0,bl); /* cut field @ first blank */
    if(utgid=="")utgid="*";                                   /* default "*" */
    /* get CMD */
    cmd=field_buffer(field[2],0);
    bl=cmd.find_first_of(" \t");
    if(bl!=string::npos)cmd=cmd.substr(0,bl);     /* cut field @ first blank */
    if(cmd=="")cmd="*";                                       /* default "*" */
    /* get TID */
    tgid=(pid_t)strtoul(field_buffer(field[3],0),(char**)NULL,0);
    snprintf(tgidS,1+maxPidSize,"%d",tgid);
    if(!tgid)
    {
      tgid=-1;
      snprintf(tgidS,1+maxPidSize,"(any)");
    }
  }
  /* ----------------------------------------------------------------------- */
  /* Un post form and free the memory */
  unpost_form(psAddForm);
  free_form(psAddForm);
  free_field(field[0]);
  free_field(field[1]); 
  free_field(field[2]); 
  free_field(field[4]); 
  free_field(field[5]); 
  /* ----------------------------------------------------------------------- */
  curs_set(0);                                       /* set cursor invisible */
  /* ----------------------------------------------------------------------- */
  if(cancelled)
  {
    mPrintf(errU,WARN,__func__,0,"Set-up of a new selection filter "
            "cancelled!");
    return;
  }
  /* ----------------------------------------------------------------------- */
  if(unlikely(deBug&0x20))
  {
    mPrintf(errU,INFO,__func__,0,"Adding process monitor filter: HOST=\"%s\", "
            "UTGID=\"%s\", CMD=\"%s\", TGID=%s...",hostPttn.c_str(),
            utgid.c_str(),cmd.c_str(),tgidS);
  }
  /* ----------------------------------------------------------------------- */
  /* set-up command argument */
  /* [-u UTGID] [-c CMD] [-t TGID] */
  args+="-u "+utgid+" -c "+cmd;
  if(tgid!=-1)args+=" -t "+string(tgidS);
  /* set-up fine CMD pattern "/FMC/<HOST>/ps/startMonitoring" */
  ucHostPttn=FmcUtils::toUpper(hostPttn);
  fineCmdPttn=string(SVC_HEAD)+"/"+ucHostPttn+"/"+SRV_NAME+"/startMonitoring";
  /* set-up coarse CMD pattern "/FMC/ * /ps/startMonitoring" */
  coarseCmdPttn=string(SVC_HEAD)+"/*/"+SRV_NAME+"/startMonitoring";
  if(unlikely(deBug&0x20))
  {
    mPrintf(errU,DEBUG,__func__,0,"Coarse CMD pattern: \"%s\"",
            coarseCmdPttn.c_str());
    mPrintf(errU,DEBUG,__func__,0,"Fine CMD pattern: \"%s\"",
            fineCmdPttn.c_str());
    mPrintf(errU,DEBUG,__func__,0,"Arguments: \"%s\"",args.c_str());
  }
  /* ----------------------------------------------------------------------- */
  /* look for "/FMC/<HOST>/ps/startMonitoring" */
  br.getServices(coarseCmdPttn.c_str());         /* coarse pattern selection */
  while((type=br.getNextService(startCmd,format))!=0)     /* loop over nodes */
  {
    if(!fnmatch(fineCmdPttn.c_str(),startCmd,0))   /* fine pattern selection */
    {
      string startCmdStr=startCmd;
      size_t b,e;
      string nodeFound;
      /* ................................................................... */
      if(unlikely(deBug&0x20))
      {
        mPrintf(errU,DEBUG,__func__,0,"Found DIM CMD: \"%s\"",startCmd);
      }
      DimClient::sendCommand(startCmd,args.c_str());
      /* ................................................................... */
      b=startCmdStr.find_first_of("/",1)+1;
      e=startCmdStr.find_first_of("/",b);
      nodeFound=startCmdStr.substr(b,e-b);
      nodeFound=FmcUtils::toLower(nodeFound);
      mPrintf(errU,INFO,__func__,0,"Added process monitor filter "
              "UTGID=\"%s\", CMD=\"%s\", TGID=%s to the node \"%s\".",
              utgid.c_str(),cmd.c_str(),tgidS,nodeFound.c_str());
      /* ................................................................... */
    }                                              /* fine pattern selection */
  }                                                       /* loop over nodes */
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
void rmPss(void)
{
  string filterSvcPttn;
  DimBrowser br;
  int type=0;
  char *filterSvc=NULL;
  char *format=NULL;
  string filter;
  string filterEl;
  string filterListEl;
  list<string>filterList;
  list<string>::iterator flIt;
  unsigned maxFilterLen=73;
  char **filterArray=NULL;
  WINDOW *psRmWin;
  ITEM **psRmItem;
  MENU *psRmMenu;
  int ch;				
  int psRmItemN,i;
  int chosen,cancelled;
  string curItem;
  /* ----------------------------------------------------------------------- */
  /* filter = (filterEl1) || (filterEl2) || ... || (filterElN)               */
  /* filterEl = filterElFld1 && filterElFld2 && ... && filterElFldM          */
  /* filter example:                                                         */
  /* "(CMD="psSrv") || (CMD="eat*") || (CMD="fsSrv" && UTGID="fsSrv_u")"     */
  /* ----------------------------------------------------------------------- */
  /* get filters from "/FMC/ * /ps/properties" */
  filterList.clear();
  filterSvcPttn=string(SVC_HEAD)+"/*/"+SRV_NAME+"/monitored/properties";
  br.getServices(filterSvcPttn.c_str());                /* pattern selection */
  while((type=br.getNextService(filterSvc,format))!=0)    /* loop over nodes */
  {
    string filterSvcStr=filterSvc;
    size_t b,e;
    string nodeFound;
    /* ..................................................................... */
    b=filterSvcStr.find_first_of("/",1)+1;
    e=filterSvcStr.find_first_of("/",b);
    nodeFound=filterSvcStr.substr(b,e-b);
    nodeFound=FmcUtils::toLower(nodeFound);
    /* ..................................................................... */
    DimCurrentInfo Filter(filterSvc,(char*)"");
    filter=string((char*)Filter.getString());
    /* ..................................................................... */
    if(unlikely(deBug&0x40))
    {
      mPrintf(errU,DEBUG,__func__,0,"Node: \"%s\", Current Filter: \"%s\".",
              nodeFound.c_str(),filter.c_str());
    }
    /* ..................................................................... */
    for(b=0;;)                                  /* loop over filter elements */
    {
      b=filter.find_first_of("(",b);
      if(b==string::npos)break;
      b++;
      e=filter.find_first_of(")",b);
      if(e==string::npos)break;
      filterEl=filter.substr(b,e-b);
      if(filterEl!="")                           /* filter element not empty */
      {
        filterListEl.clear();
        filterListEl+="HOST="+nodeFound+" && "+filterEl+" ";
        filterList.push_back(filterListEl);
      }                                          /* filter element not empty */
      e=filter.find("||",e);
      if(e==string::npos)break;
      e=e+1;
    }                                           /* loop over filter elements */
    /* ..................................................................... */
  }                                                       /* loop over nodes */
  /* ----------------------------------------------------------------------- */
  /* sort the filter list */
  filterList.sort();
  /* ----------------------------------------------------------------------- */
  if(unlikely(deBug&0x40))
  {
    for(flIt=filterList.begin();flIt!=filterList.end();flIt++)
    {
      mPrintf(errU,DEBUG,__func__,0,"Filter '%s'",flIt->c_str());
    }
  }
  /* ----------------------------------------------------------------------- */
  /* create NULL-terminated array filterArray */
  psRmItemN=filterList.size();
  filterArray=(char**)alloca((psRmItemN+1)*sizeof(char*));
  for(flIt=filterList.begin(),i=0;flIt!=filterList.end();i++,flIt++)
  {
    string s=*flIt;
    s.resize(maxFilterLen,' ');         /* all items resized to maxFilterLen */
    filterArray[i]=strdupa(s.c_str());
  }
  filterArray[psRmItemN]=(char*)NULL;                    /* NULL-termination */
  /* ----------------------------------------------------------------------- */
  /* Create items */
  psRmItem=(ITEM**)alloca((psRmItemN+1)*sizeof(ITEM*));
  for(i=0;i<psRmItemN+1;i++)
  {
    psRmItem[i]=new_item(filterArray[i],filterArray[i]);
  }
  /* ----------------------------------------------------------------------- */
  /* Crate menu */
  psRmMenu=new_menu((ITEM**)psRmItem);
  /* ----------------------------------------------------------------------- */
  /* Create the window to be associated with the menu */
  psRmWin=newwin(17,76,1,2);
  keypad(psRmWin,TRUE);
  /* ----------------------------------------------------------------------- */
  /* Set main window and sub window */
  set_menu_win(psRmMenu,psRmWin);
  set_menu_sub(psRmMenu,derwin(psRmWin,10,maxFilterLen,3,1));
  set_menu_format(psRmMenu,10,1);
  /* ----------------------------------------------------------------------- */
  /* Set menu mark to the string " * " */
  set_menu_mark(psRmMenu," * ");
  /* ----------------------------------------------------------------------- */
  /* Print a border around the main window */
  box(psRmWin,0,0);
  /* print title */
  wattron(psRmWin,COLOR_PAIR(4)|A_BOLD);
  mvwprintw(psRmWin,1,2,"Choose a process selecton filter to remove:");
  wattroff(psRmWin,COLOR_PAIR(4)|A_BOLD);
  /* print help footer */
  mvwprintw(psRmWin,14,2,"    Choose       k Up-1-line         b Up-1-page    "
            "     g 1st-line");
  mvwprintw(psRmWin,15,2,"  q Cancel       j Down-1-line       f Down-1-page  "
            "     G Last-line");
  /* print "Return" symbol */
  mvwaddch(psRmWin,14,2,ACS_LARROW);
  mvwaddch(psRmWin,14,3,ACS_HLINE);
  mvwaddch(psRmWin,14,4,ACS_LRCORNER);
  /* print 2 separator lines */
  mvwaddch(psRmWin,2,0,ACS_LTEE);
  mvwhline(psRmWin,2,1,ACS_HLINE,74);
  mvwaddch(psRmWin,2,75,ACS_RTEE);
  mvwaddch(psRmWin,13,0,ACS_LTEE);
  mvwhline(psRmWin,13,1,ACS_HLINE,74);
  mvwaddch(psRmWin,13,75,ACS_RTEE);
  wrefresh(stdscr);
  /* ----------------------------------------------------------------------- */
  /* Post the menu */
  post_menu(psRmMenu);
  wrefresh(psRmWin);
  /* ----------------------------------------------------------------------- */
  for(chosen=0,cancelled=0;!chosen && !cancelled;)
  {
    switch(ch=wgetch(psRmWin))
    {
      case '\n':
        chosen=1;
        break;
      case 'q':                                                  /* vi-style */
      case 0x1b:                                                   /* escape */
        cancelled=1;
        break;
      case KEY_DOWN:
      case 'j':                                                  /* vi-style */
      case 'J':                                                  /* vi-style */
      case 'n':                                                /* pine-style */
      case 'N':                                                /* pine-style */
        menu_driver(psRmMenu,REQ_DOWN_ITEM);
        break;
      case KEY_UP:
      case 'k':                                                  /* vi-style */
      case 'K':                                                  /* vi-style */
      case 'p':                                                /* pine-style */
      case 'P':                                                /* pine-style */
        menu_driver(psRmMenu,REQ_UP_ITEM);
        break;
      case KEY_NPAGE:
      case 'f':                                                  /* vi-style */
      case 'F':                                                  /* vi-style */
      case ' ':                                                /* less-style */
        menu_driver(psRmMenu,REQ_SCR_DPAGE);
        break;
      case KEY_PPAGE:
      case 'b':                                                  /* vi-style */
      case 'B':                                                  /* vi-style */
      case '-':                                                /* pine-style */
        menu_driver(psRmMenu,REQ_SCR_UPAGE);
        break;
      case 'g':                                                /* less-style */
      case KEY_HOME:                                             /* home key */
        menu_driver(psRmMenu,REQ_FIRST_ITEM);
        break;
      case 'G':                                             /* vi/less-style */
      case KEY_END:                                               /* end key */
        menu_driver(psRmMenu,REQ_LAST_ITEM);
        break;
      case KEY_RESIZE:                              /* terminal resize event */
        getmaxyx(stdscr,LINES,COLS);                 /* get maxWy and maxWx */
        touchwin(psRmWin);
        break;
    }
    wrefresh(psRmWin);
  }	
  /* ----------------------------------------------------------------------- */
  if(chosen)
  {
    /* save name of current item */
    curItem=item_name(current_item(psRmMenu));
    size_t mcl;                                   /* minimum common length */
    size_t b,e;
    string hostName;
    string ucHostName;
    string utgidPttn;
    string cmdPttn;
    string cmdlinePttn;
    pid_t tgid=-2;
    string arg[10];
    string args;
    string fineCmdPttn;
    string coarseCmdPttn;
    char *stopCmd=NULL;
    /* ..................................................................... */
    for(flIt=filterList.begin();flIt!=filterList.end();flIt++)
    {
      mcl=maxFilterLen;
      if(mcl>flIt->length())mcl=flIt->length();
      if(!(flIt->compare(0,mcl,curItem,0,mcl)))
      {
        curItem=*flIt;
        break;
      }
    }
    /* ..................................................................... */
    /* look for hostname */
    b=curItem.find("HOST=");
    if(b!=string::npos)
    {
      b+=strlen("HOST=");
      e=curItem.find(" ",b);
      hostName=curItem.substr(b,e-b);
    }
    /* ..................................................................... */
    /* look for UTGID pattern */
    b=curItem.find("UTGID=\"");
    if(b!=string::npos)
    {
      b+=strlen("UTGID=\"");
      e=curItem.find("\"",b);
      utgidPttn=curItem.substr(b,e-b);
    }
    /* ..................................................................... */
    /* look for CMD pattern */
    b=curItem.find("CMD=\"");
    if(b!=string::npos)
    {
      b+=strlen("CMD=\"");
      e=curItem.find("\"",b);
      cmdPttn=curItem.substr(b,e-b);
    }
    /* ..................................................................... */
    /* look for CMDLINE pattern */
    b=curItem.find("CMDLINE=\"");
    if(b!=string::npos)
    {
      b+=strlen("CMDLINE=\"");
      e=curItem.find("\"",b);
      cmdlinePttn=curItem.substr(b,e-b);
    }
    /* ..................................................................... */
    /* look for PID/TID/TGID */
    b=curItem.find("TID=");
    if(b!=string::npos)
    {
      b+=strlen("TID=");
      e=curItem.find(" ",b);
      tgid=(pid_t)strtoul(curItem.substr(b,e-b).c_str(),(char**)NULL,0);
    }
    /* ..................................................................... */
    /* look for ARGV[i] pattern */
    for(i=1;i<10;i++)
    {
      string s="ARGV["+FmcUtils::itos(i)+"]=\"";
      b=curItem.find(s);
      if(b!=string::npos)
      {
        b+=s.length();
        e=curItem.find("\"",b);
        arg[i]=curItem.substr(b,e-b);
      }
    }
    /* ..................................................................... */
    if(unlikely(deBug&0x40))
    {
      mPrintf(errU,DEBUG,__func__,0,"Remove filter='%s'",curItem.c_str());
      mPrintf(errU,DEBUG,__func__,0,"HOST=\"%s\"",hostName.c_str());
      mPrintf(errU,DEBUG,__func__,0,"UTGID=\"%s\"",utgidPttn.c_str());
      mPrintf(errU,DEBUG,__func__,0,"CMD=\"%s\"",cmdPttn.c_str());
      mPrintf(errU,DEBUG,__func__,0,"CMDLINE=\"%s\"",cmdlinePttn.c_str());
      mPrintf(errU,DEBUG,__func__,0,"TID=%d",tgid);
      for(i=1;i<10;i++)
      {
        mPrintf(errU,DEBUG,__func__,0,"ARGV[%d]=\"%s\"",i,arg[i].c_str());
      }
    }
    /* ..................................................................... */
    /* set-up command argument */
    /* [-t | --tid TID] [-u | --utgid UTGID_PATTERN]                         */
    /* [-c | --cmd CMD_PATTERN] [-C | --cmdline CMDLINE_PATTERN]             */
    /* [--a1 ARG1_PATTERN] [--a2 ARG2_PATTERN] [--a3 ARG3_PATTERN]           */
    /* [--a4 ARG4_PATTERN] [--a5 ARG5_PATTERN] [--a6 ARG6_PATTERN]           */
    /* [--a7 ARG7_PATTERN] [--a8 ARG8_PATTERN] [--a9 ARG9_PATTERN]           */
    if(tgid!=-2) args+=" -t "+FmcUtils::utos(tgid);
    if(utgidPttn!="") args+=" -u "+utgidPttn;
    if(cmdPttn!="") args+=" -c "+cmdPttn;
    if(cmdlinePttn!="") args+=" -C "+cmdlinePttn;
    for(i=1;i<10;i++)
    {
      if(arg[i]!="") args+=" --a"+FmcUtils::utos(i)+" "+arg[i];
    }
    mPrintf(errU,ERROR,__func__,0,"args=\'%s\'",args.c_str());
    /* ..................................................................... */
    /* set-up fine CMD pattern "/FMC/<HOST>/ps/stopMonitoring" */
    ucHostName=FmcUtils::toUpper(hostName);
    fineCmdPttn=string(SVC_HEAD)+"/"+ucHostName+"/"+SRV_NAME+"/stopMonitoring";
    /* set-up coarse CMD pattern "/FMC/ * /ps/stopMonitoring" */
    coarseCmdPttn=string(SVC_HEAD)+"/*/"+SRV_NAME+"/stopMonitoring";
    if(unlikely(deBug&0x20))
    {
      mPrintf(errU,DEBUG,__func__,0,"Coarse CMD pattern: \"%s\"",
              coarseCmdPttn.c_str());
      mPrintf(errU,DEBUG,__func__,0,"Fine CMD pattern: \"%s\"",
              fineCmdPttn.c_str());
      mPrintf(errU,DEBUG,__func__,0,"Arguments: \"%s\"",args.c_str());
    }
    /* ..................................................................... */
    /* look for "/FMC/<HOST>/ps/stopMonitoring" */
    br.getServices(coarseCmdPttn.c_str());       /* coarse pattern selection */
    while((type=br.getNextService(stopCmd,format))!=0)    /* loop over nodes */
    {
      if(!fnmatch(fineCmdPttn.c_str(),stopCmd,0))  /* fine pattern selection */
      {
        string stopCmdStr=stopCmd;
        size_t b,e;
        string nodeFound;
        /* ................................................................. */
        if(unlikely(deBug&0x20))
        {
          mPrintf(errU,DEBUG,__func__,0,"Found DIM CMD: \"%s\"",stopCmd);
        }
        DimClient::sendCommand(stopCmd,args.c_str());
        /* ................................................................. */
        b=stopCmdStr.find_first_of("/",1)+1;
        e=stopCmdStr.find_first_of("/",b);
        nodeFound=stopCmdStr.substr(b,e-b);
        nodeFound=FmcUtils::toLower(nodeFound);
        if(unlikely(deBug & 0x1))
        {
          mPrintf(errU,INFO,__func__,0,"Removed process monitor filter '%s' "
                  "from the node \"%s\".",curItem.c_str(),nodeFound.c_str());
        }
        /* ................................................................. */
      }                                            /* fine pattern selection */
    }                                                     /* loop over nodes */
    /* ..................................................................... */
  }
  else if(cancelled)
  {
    curItem="";
    if(unlikely(deBug & 0x1))
    {
      mPrintf(errU,INFO,__func__,0,"Cancelled.");
    }
  }
  /* ----------------------------------------------------------------------- */
  /* Unpost and free all the memory taken up */
  unpost_menu(psRmMenu);
  free_menu(psRmMenu);
  for(i=0;i<psRmItemN+1;i++)
  {
    free_item(psRmItem[i]);
  }
  /* ----------------------------------------------------------------------- */
  return;
}
/*****************************************************************************/
int getMaxPid(void)
{
  char sbuf[1024];
  int pm=131072;         /* this must be larger then any possible real value */
  /*-------------------------------------------------------------------------*/
  if(file2str(PID_MAX_FILE,sbuf,sizeof(sbuf))!=-1)
  {
    pm=(int)strtol(sbuf,NULL,0);
  }
  return pm;
}
/*****************************************************************************/
static int file2str(const char *path,char *buf,int bufSz)
{
  int fd;
  ssize_t readChars;
  int savedErrno;
  /*-------------------------------------------------------------------------*/
  printInFunc;
  /*-------------------------------------------------------------------------*/
  fd=open(path,O_RDONLY,0);
  if(unlikely(fd==-1))
  {
    if(unlikely(deBug&0x20))
    {
      mPrintf(errU,ERROR,__func__,0,"Cannot open file \"%s\": %s!",path,
              strerror(errno));
    }
    printOutFuncNOK;
    return -1;
  }
  readChars=read(fd,buf,bufSz-1);
  savedErrno=errno;
  close(fd);
  if(unlikely(readChars<=0))
  {
    if(unlikely(deBug&0x20))
    {
      mPrintf(errU,ERROR,__func__,0,"Error reading file \"%s\": read(2) "
              "return value: %d, error: %d (%s)!",path,readChars,savedErrno,
              strerror(savedErrno));
    }
    printOutFuncNOK;
    return -1;
  }
  buf[readChars]='\0';
  printOutFuncOK;
  return readChars;
}
/*****************************************************************************/
void getServerVersion(vector<string>hostPttnLs)
{
  printf("HOSTNAME     FMC      SERVER\n");
  /* current (>=FMC-3.9) service name rule */
  /* /FMC/<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnLs,SVC_HEAD,SRV_NAME,deBug,12,8);
  /* old (<FMC-3.9) service name rule */
  /* /<HOSTNAME>/<SRV_NAME>/success */
  FmcUtils::printServerVersion(hostPttnLs,"",SRV_NAME,deBug,12,8);
  /* older service name rule */
  FmcUtils::printServerVersion(hostPttnLs,"","procs",deBug,12,8);
  printf("HOSTNAME     FMC      CLIENT\n");
  printf("%-12s %-8s %s\n","localhost",FMC_VERSION,rcsidP);
  exit(0);
}
/*****************************************************************************/
void shortUsage(void)
{
  const char *shortUsageStr=
"SYNOPSIS\n"
"psMonitor [-v | --debug] [-N | --dim-dns DIM_DNS_NODE]\n"
"          [-m | --hostname NODE_PATTERN...]\n"
"psMonitor { -V | --version } [-N | --dim-dns DIM_DNS_NODE]\n"
"          [-m | --hostname NODE_PATTERN...]\n"
"psMonitor { -h | --help }\n"
"\n"
"Try \"psMonitor -h\" for more information.\n";
  /*-------------------------------------------------------------------------*/
  fprintf(stderr,"\n%s\n",shortUsageStr);
  exit(1);
}
/*****************************************************************************/
void usage(int mode)
{
  FILE *fpOut;
  int status;
  const char *formatter;
  /*-------------------------------------------------------------------------*/
  if(!mode)formatter="/usr/bin/groff -man -Tascii | /usr/bin/less -iRs";
  else formatter="/bin/cat";
  fpOut=popen(formatter,"w");
  fprintf(fpOut,
".ig\n"
"psMonitor.man\n"
"\n"
"..\n"
"%s"
".hw dim\\[hy]dns hostname select\\[hy]cmd select\\[hy]utgid\n"
".hw version help\n"
".hw DIM_DNS_NODE NODE_PATTERN\n"
".hw WILDCARD_PATTERN\n"
".\n"
".TH psMonitor 1  %s \"FMC-%s\" \"FMC User Commands\"\n"
".\n"
".\n"
".SH NAME\n"
".\n"
".Synopsis psMonitor\\ \\-\n"
"Show the statistics of the processes monitored by the FMC Process Monitor "
"Server\n"
".EndSynopsis\n"
".\n"
".\n"
".SH SYNOPSIS\n"
".ad l\n"
".Synopsis psMonitor\n"
".DoubleOpt[] v debug\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis psMonitor\n"
"{\n.ShortOpt V\n|\n.LongOpt version\n}\n"
".DoubleOpt[] N dim\\[hy]dns DIM_DNS_NODE\n"
".DoubleOpt[] m hostname NODE_PATTERN...\n"
".EndSynopsis\n"
".sp\n"
".Synopsis psMonitor\n"
"{\n.ShortOpt h\n|\n.LongOpt help\n}\n"
".EndSynopsis\n"
".\n"
".\n"
".SH DESCRIPTION\n"
".\n"
"The \\fBpsMonitor\\fP is an application which can continuously shows the "
"parameters of a list of processes running on the nodes of a computer farm "
"on which the server \\fBpsSrv\\fP(8) is running. A generic process running "
"on a farm node can be added or removed to/from the list of the monitored "
"processes by means of the menues opened by pressing the \\fBa\\fP (add) and "
"the \\fBr\\fP (remove) keys.\n"
".PP\n"
"The \\fBpsMonitor\\fP application terminates on the command \\fBq\\fP.\n"
".PP\n"
"Displayed parameters are:\n"
".TP\n"
"\\fBHOST\\fP\n"
"The hostname of the PC the process is running on.\n"
".TP\n"
"\\fBTGID\\fP (alias PID)\n"
"Thread Group Identifier (the former PID, Process Identifier). The term "
"\\fBprocess\\fP is synonymous of \\fBthread group\\fP.\n"
".TP\n"
"\\fBCMD\\fP (alias COMM, COMMAND, UCMD, UCOMM)\n"
"Command name, i.e. the basename (w/o path) of the executable image file, "
"without arguments, eventually truncated at 15 characters.\n"
".TP\n"
"\\fBUTGID\\fP\n"
"FMC User-assigned unique Thread Group Identifier, if available (e.g. if the "
"process has been started by the FMC Task Manager or has the variable UTGID "
"defined in the process environment). Otherwise \"N/A\" is printed.\n"
".TP\n"
"\\fBS\\fP\n"
"Status. Can be \\fBR\\fP (in execution), \\fBN\\fP (new process in "
"execution) or \\fBX\\fP (terminated).\n"
".TP\n"
"\\fBNLWP\\fP (alias THCNT)\n"
"Number of Lightweight Processes (Threads) in the process (thread group). The "
"number is printed in \\fIred\\fP if the number has increased since the last "
"refresh, in \\fIgreen\\fP if it has decreased.\n"
".TP\n"
"\\fBP\\fP (alias CPUID, PSR)\n"
"The ID of the processor that process is currently assigned to. It is useful "
"to check the operation of process-to-CPU affinity setting by the FMC Task "
"Manager. The processor ID is printed in \\fIred\\fP if it has changed since "
"the last refresh.\n"
".TP\n"
"\\fB%%CPU\\fP (alias PCPU, CP, C)\n"
"The task's share of the CPU time since the last update, expressed as a "
"percentage of the total CPU time per processor. \\fBMaximum\\fP: 100%% x "
"CPU_core_number.  The value is printed in \\fIred\\fP if it has increased "
"by at least 1%% since the last refresh, in \\fIyellow\\fP if it is greater "
"than 99.0%%, in \\fIgreen\\fP if it has decreased by at least 1%% since the "
"last refresh.\n"
".TP\n"
"\\fB%%MEM\\fP (alias PMEM)\n"
"The ratio of the process's resident set size to the physical memory on the "
"machine, expressed as a percentage. \\fBMaximum\\fP: 100%%. The value is "
"printed in \\fIred\\fP if it has increased by at least 0.01%% since the last "
"refresh, in \\fIyellow\\fP if it is greater than 90.0%%, in \\fIgreen\\fP if "
"it has decreased by at least 0.01%% since the last refresh.\n"
".TP\n"
"\\fBMIN_FLT\\fP\n"
"The rate (page faults per second) of page faults the process has made which "
"have not required loading a memory page from disk. The value is printed in "
"\\fIred\\fP if it has increased by at least a factor 100 since the last "
"refresh, in \\fIgreen\\fP if it has decreased by at least a factor 100 "
"since the last refresh.\n"
".TP\n"
"\\fBMAJ_FLT\\fP\n"
"The rate (page faults per second) of page faults the process has made which "
"have required loading a memory page from disk. The value is printed in "
"\\fIred\\fP if it has increased by at least a factor 10 since the last "
"refresh, in \\fIyellow\\fP if it is greater than 10 page faults per second, "
"in \\fIgreen\\fP if it has decreased by at least a factor 10 since the last "
"refresh.\n"
".TP\n"
"\\fBVSIZE\\fP (alias VSZ, VIRT)\n"
"The total size of the process's memory footprint. This includes the text "
"segment, stack, static variables, data segment, and pages which are shared "
"with other processes. The value is printed in \\fIred\\fP if it has "
"increased since the last refresh, in \\fIgreen\\fP if it has decreased.\n"
".TP\n"
"\\fBLOCK\\fP\n"
"The amount of the process's memory which is currently locked by the kernel. "
"Locked memory cannot be swapped out. The value is printed in \\fIred\\fP if "
"it has increased since the last refresh, in \\fIgreen\\fP if it has "
"decreased.\n"
".TP\n"
"\\fBRSS\\fP (alias RSZ, RES, RESIDENT)\n"
"The kernel's estimate of the resident set size for this process. The value "
"is printed in \\fIred\\fP if it has increased since the last refresh, in "
"\\fIgreen\\fP if it has decreased.\n"
".TP\n"
"\\fBDATA\\fP\n"
"The amount of memory used for data by the process. It includes static "
"variables and the data segment, but excludes the stack. The value is "
"printed in \\fIred\\fP if it has increased since the last refresh, in "
"\\fIgreen\\fP if it has decreased.\n"
".TP\n"
"\\fBSTACK\\fP\n"
"The amount of memory consumed by the process's stack. The value is printed "
"in \\fIred\\fP if it has increased since the last refresh, in \\fIgreen\\fP "
"if it has decreased.\n"
".TP\n"
"\\fBEXE\\fP\n"
"The size of the process's executable pages, excluding shared pages.\n"
".TP\n"
"\\fBLIB\\fP\n"
"The size of the shared memory pages mapped into the process's address space. "
"This excludes pages shared using System V style IPC. The value is printed in "
"\\fIred\\fP if it has increased since the last refresh, in \\fIgreen\\fP if "
"it has decreased.\n"
".TP\n"
"\\fBSHARE\\fP (alias SHRD, SHR)\n"
"The size of the pages of shared (mmap-ed) memory. The value is printed in "
"\\fIred\\fP if it has increased since the last refresh, in \\fIgreen\\fP if "
"it has decreased.\n"
".TP\n"
"\\fBRT\\fP (alias RTPRIO)\n"
"Static (real-time) priority of the process. Only value 0 is allowed for TS "
"processes. For FF and RR real-time processes, allowed values are in the "
"range 1...99 (0 is the least favorable priority, 99 is the most favorable\n"
"priority).  The value is printed in \\fIred\\fP if it has increased since "
"the last refresh, in \\fIgreen\\fP if it has decreased.\n"
".TP\n"
"\\fBNI\\fP (alias NICE)\n"
"The nice level of the process, used by the TS Linux scheduler to compute the "
"dynamic priority. Allowed values for nice level are in the range -20...19 "
"(-20 corresponds to the most favorable scheduling; 19 corresponds to the "
"least favorable scheduling). The value is printed in \\fIred\\fP if it has "
"decreased since the last refresh, in \\fIgreen\\fP if it has increased.\n"
".TP\n"
"\\fBPR\\fP (alias PRI, PRIO)\n"
"Kernel scheduling priority. Possible values are in the range -100...39 (-100 "
"is the most favorable priority, 39 is the least favorable priority). TS "
"processes have PRIO in the range 0...39, FF and RR processes have PRIO in "
"the range -100...-1. The value is printed in \\fIred\\fP if it has "
"decreased since the last refresh, in \\fIgreen\\fP if it has increased.\n"
".TP\n"
"\\fBLAST_UPDATE\\fP\n"
"Time of the last update of the process data.\n"
".TP\n"
"\\fBUSER\\fP (alias EUSER, UNAME)\n"
"\\fBEffective\\fP user name. Used for all the security checks. The only user "
"name of the process that normally has any effect.\n"
".TP\n"
"\\fBCMDLINE\\fP (alias ARGS, CMD, COMMAND)\n"
"The complete command line with its arguments (up to PATH_MAX = 4096 "
"characters). The output in this column may contain spaces.\n"
".PP\n"
"The DIM Name Server, looked up to seek for Process Monitor Servers, can be\n"
"chosen (in decreasing order of priority) by:\n"
".RS\n"
".TP\n"
"1. Specifying the \\fB-N\\fP \\fIDIM_DNS_NODE\\fP command-line option;\n"
".TP\n"
"2. Specifying the \\fIDIM_DNS_NODE\\fP \\fBenvironment variable\\fP;\n"
".TP\n"
"3. Defining the \\fIDIM_DNS_NODE\\fP in the file "
"\"\\fB"DIM_CONF_FILE_NAME"\\fP\".\n"
".RE\n"
".\n"
".\n"
".SH COMMANDS\n"
".\n"
".SS Main window\n"
".\n"
".TP\n"
"\\fBq\\fP\n"
"Quit the application.\n"
".\n"
".TP\n"
"\\fBa\\fP\n"
"Open the Add-Filter Menu.\n"
".\n"
".TP\n"
"\\fBr\\fP\n"
"Open the Remove-Filter Menu.\n"
".\n"
".TP\n"
"\\fBk\\fP, \\fBp\\fP, UP_ARROW\n"
"Scroll backward one line the process list, if it exceeds the terminal "
"height.\n"
".\n"
".TP\n"
"\\fBj\\fP, \\fBn\\fP, DOWN_ARROW, RETURN, ENTER\n"
"Scroll forward one line the process list, if it exceeds the terminal "
"height.\n"
".\n"
".TP\n"
"\\fBu\\fP\n"
"Scroll backward half page the process list, if it exceeds the terminal "
"height.\n"
".\n"
".TP\n"
"\\fBd\\fP\n"
"Scroll forward half page the process list, if it exceeds the terminal "
"height.\n"
".\n"
".TP\n"
"\\fBb\\fP, \\fB-\\fP, PAGE_UP\n"
"Scroll backward one page the process list, if it exceeds the terminal "
"height.\n"
".\n"
".TP\n"
"\\fBf\\fP, SPACE_BAR, PAGE_DOWN\n"
"Scroll forward one page the process list, if it exceeds the terminal "
"height.\n"
".\n"
".TP\n"
"\\fBg\\fP, HOME\n"
"Go to the beginning of the process list, if it exceeds the terminal "
"height.\n"
".\n"
".TP\n"
"\\fBG\\fP, END\n"
"Go to the end of the process list, if it exceeds the terminal "
"height.\n"
".\n"
".TP\n"
"\\fBh\\fP, LEFT_ARROW\n"
"Scroll leftward one character the process list, if it exceeds the terminal "
"width.\n"
".\n"
".TP\n"
"\\fBl\\fP, RIGHT_ARROW\n"
"Scroll rightward one character the process list, if it exceeds the terminal "
"width.\n"
".\n"
".TP\n"
"\\fBH\\fP, SHIFT-TAB\n"
"Scroll leftward one page the process list, if it exceeds the terminal "
"width.\n"
".\n"
".TP\n"
"\\fBL\\fP, TAB\n"
"Scroll rightward one page the process list, if it exceeds the terminal "
"width.\n"
".\n"
".\n"
".PP\n"
".SS Add-Filter Menu\n"
".\n"
".TP\n"
"TAB, DOWN_ARROW\n"
"Go to the next field in the menu.\n"
".\n"
".TP\n"
"SHIFT-TAB, UP_ARROW\n"
"Go to the previous field in the menu.\n"
".\n"
".TP\n"
"PAGE_UP\n"
"Go to the first field in the menu.\n"
".\n"
".TP\n"
"PAGE_DOWN\n"
"Go to the last field in the menu.\n"
".\n"
".TP\n"
"LEFT_ARROW, RIGHT_ARROW\n"
"Move leftward and rightward one character inside a field.\n"
".\n"
".TP\n"
"HOME, END\n"
"Go to the beginning or the end of a field.\n"
".\n"
".TP\n"
"BACKSPACE\n"
"Delete previous character in a field.\n"
".\n"
".TP\n"
"DELETE\n"
"Delete current character in a field.\n"
".\n"
".TP\n"
"RETURN\n"
"Confirm a field (including \"Cancel\" and \"OK\" fields). Confirming the "
"\"Cancel\" field the Add-Filter action is cancelled and the Add-Filter Menu "
"is closed. Confirming the \"OK\" field a new process selection filter is "
"added to the list and the Add-Filter Menu is closed.\n"
".\n"
".TP\n"
"ESC\n"
"Cancel and exit from the Add-Filter Menu (equivalent to confirm the "
"\"Cancel\" field).\n"
".\n"
".\n"
".PP\n"
".SS Remove-Filter Menu\n"
".\n"
".TP\n"
"RETURN\n"
"Remove the currently selected filter and exit the Remove-Filter Menu.\n"
".\n"
".TP\n"
"\\fBq\\fP, ESC\n"
"Cancel and exit the Remove-Filter Menu.\n"
".\n"
".TP\n"
"\\fBk\\fP, \\fBp\\fP, UP_ARROW\n"
"Move the selected filter backward one line in the filter list.\n"
".\n"
".TP\n"
"\\fBj\\fP, \\fBn\\fP, DOWN_ARROW, RETURN, ENTER\n"
"Move the selected filter forward one line in the filter list.\n"
".\n"
".TP\n"
"\\fBb\\fP, \\fB-\\fP, PAGE_UP\n"
"Move the selected filter backward one page in the filter list.\n"
".\n"
".TP\n"
"\\fBf\\fP, SPACE_BAR, PAGE_DOWN\n"
"Move the selected filter forward one page in the filter list.\n"
".\n"
".TP\n"
"\\fBg\\fP, HOME\n"
"Go to the first filter in the filter list.\n"
".\n"
".TP\n"
"\\fBG\\fP, END\n"
"Go to the last filter in the filter list.\n"
".\n"
".\n"
".SH OPTIONS\n"
".\n"
".OptDef h help\n"
"Get basic usage help from the command line.\n"
".\n"
".OptDef v debug\n"
"Increase verbosity for debugging.\n"
".\n"
".OptDef N dim-dns DIM_DNS_NODE (string)\n"
"Use \\fIDIM_DNS_NODE\\fP as DIM Name Server node.\n"
".\n"
".OptDef m hostname NODE_PATTERN (string, repeatable)\n"
"Contact FMC Process Monitor Servers only at the nodes whose hostname\n"
"matches at least one of the wildcard patterns \\fINODE_PATTERN\\fP "
"(\\fBdefault\\fP: contact the FMC Process Monitor at all the nodes).\n"
".\n"
".OptDef V version\n"
"For each selected node print the FMC Process Monitor Server version and the "
"FMC version, than exit.\n"
".\n"
".\n"
".SH ENVIRONMENT\n"
".TP\n"
".EnvVar DIM_DNS_NODE \\ (string,\\ mandatory\\ if\\ not\\ defined\\ "
"otherwise,\\ see\\ above)\n"
"Host name of the node which is running the DIM DNS.\n"
".\n"
".TP\n"
".EnvVar LD_LIBRARY_PATH \\ (string,\\ mandatory\\ if\\ not\\ set\\ using\\ "
"ldconfig)\n"
"Must include the path to the libraries \"libdim\"\n"
"and \"libFMCutils\".\n"
".\n"
".TP\n"
".EnvVar UTGID \\ (string,\\ optional)\n"
"Printed in diagnostic messages, used to identify the \\fBpsMonitor\\fP\n"
"instance.\n"
".\n"
".TP\n"
".EnvVar deBug \\ (int,\\ optional)\n"
"Debug mask. Can be set to 0..0x10.\n"
".\n"
".\n"
".SH EXAMPLES\n"
".\n"
"Get process data from the monitored processes on all the nodes:\n"
".\n"
".PP\n"
".ShellCommand psMonitor\n"
".ShellCommand psMonitor -m \\[dq]*\\[dq]\n"
".ShellCommand psMonitor -m \\[rs]*\n"
".\n"
".PP\n"
"Get process data only of the monitored processes running at the nodes "
"\"farm0101\" and \"farm0102\":\n"
".\n"
".PP\n"
".ShellCommand psMonitor -m farm0101 -m farm0102\n"
".\n"
".PP\n"
".\n"
".\n"
".SH AVAILABILITY\n"
".\n"
"Information on how to get the FMC (Farm Monitoring and Control System)\n"
"package and related information is available at the web sites:\n"
".PP\n"
".URL https://\\:lhcbweb.bo.infn.it/\\:twiki/\\:bin/\\:view.cgi/\\:"
"LHCbBologna/\\:FmcLinux \"FMC Linux Development Homepage\".\n"
".PP\n"
".URL http://\\:itcobe.web.cern.ch/\\:itcobe/\\:Projects/\\:Framework/\\:"
"Download/\\:Components/\\:SystemOverview/\\:fwFMC/\\:welcome.html "
"\"CERN ITCOBE: FW Farm Monitor and Control\".\n"
".PP\n"
".URL http://\\:lhcb-daq.web.cern.ch/\\:lhcb-daq/\\:online-rpm-repo/\\:"
"index.html \"LHCb Online RPMs\".\n"
".PP\n"
".\n"
".\n"
".SH AUTHORS\n"
".\n"
".MTO domenico.galli@bo.infn.it \"Domenico Galli\"\n"
".\n"
".\n"
".SH VERSION\n"
".\n"
"Source:  %s\n"
".br\n"
"Package: FMC-%s\n"
".\n"
".\n"
".SH SEE ALSO\n"
".ad l\n"
".BR \\%%psSrv (8),\n"
".BR \\%%psSetProperties (1),\n"
".BR \\%%psViewer (1).\n"
".br\n"
".BR \\%%tmSrv (8),\n"
".BR \\%%tmStart (1).\n"
".br\n"
".BR \\%%top (1),\n"
".BR \\%%ps (1),\n"
".BR \\%%proc (5).\n"
".br\n"
".BR \\%%/usr/share/doc/kernel-doc-2.6.9/Documentation/filesystems/proc.txt.\n"
".\n"
".\n"
".\\\" Local Variables:\n"
".\\\" mode: nroff\n"
".\\\" End:\n"
,groffMacro,FMC_DATE,FMC_VERSION
,rcsidP,FMC_VERSION
         );
  fflush(fpOut);
  pclose(fpOut);
  wait(&status);
  exit(1);
}
/* ######################################################################### */
