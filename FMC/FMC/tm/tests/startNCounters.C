/*****************************************************************************/
/*
 * $Log: startNCounters.C,v $
 * Revision 1.4  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.3  2007/11/28 10:20:18  galli
 * *** empty log message ***
 *
 * Revision 1.2  2007/11/27 16:28:39  galli
 * *** empty log message ***
 *
 * Revision 1.1  2007/11/27 15:51:36  galli
 * Initial revision
 *
 */
/*****************************************************************************/
#include <stdio.h>
#include <fnmatch.h>                                            /* fnmatch() */
#include "dic.hxx"                                  /* DimBrowser, DimClient */
#include <sys/timeb.h>
#include <ctype.h> /* toupper(3) */
/*****************************************************************************/
void usage(char *pName);
/*****************************************************************************/
int main(int argc,char **argv)
{
  int i;
  char *p;
  DimBrowser br;
  char *cmdPattern=NULL;
  char *sN=NULL;
  char *format=NULL;
  int type=0;
  char *pName=strdup(basename(argv[0]));
  int processNumber=1;
  int dummyVarNum=0;
  char *envStr=NULL;
  int envStrLen=0;
  char *startCmd=NULL;
  int startCmdLen=0;
  int envSz=0;
  struct timeb now;
  double nowF=0.0;
  ftime(&now);
  const char *dummyVar=
"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
  char *srvPattern=NULL;
  char *startPsPath;
  /*-------------------------------------------------------------------------*/
  nowF=(double)now.time+(double)now.millitm/1000.0;
  printf("StartTime=%f\n",nowF);
  /*-------------------------------------------------------------------------*/
  /* process command-line options */
  if(argc!=5)usage(pName);                                   /* no arguments */
  if(!strcmp(argv[1],"-h")||!strcmp(argv[1],"--help"))usage(pName);
  srvPattern=(char*)realloc(srvPattern,1+strlen(argv[1]));
  strcpy(srvPattern,argv[1]);
  /* convert to upper case */
  for(p=srvPattern;*p;p++)*p=toupper(*p);
  cmdPattern=(char*)realloc(cmdPattern,1+strlen(srvPattern)+
                            strlen("/FMC//task_manager/start"));
  sprintf(cmdPattern,"/FMC/%s/task_manager/start",srvPattern);
  printf("1 - cmdPattern=\"%s\"\n",cmdPattern);
  processNumber=atoi(argv[2]);
  printf("2 - processNumber=%d\n",processNumber);
  dummyVarNum=atoi(argv[3]);
  printf("3 - dummyVarNum=%d\n",dummyVarNum);
  startPsPath=argv[4];
  printf("4 - startPsPath=\"%s\"\n",startPsPath);
  startCmdLen=1+snprintf(NULL,0,"-d -e -c %s",startPsPath);
  envSz=20;
  for(i=0;i<dummyVarNum;i++)
  {
    int len;
    len=snprintf(NULL,0,"-D DUMMY_VAR_%d=%s ",i,dummyVar);
    if(envStrLen<len+1)envStrLen=len+1;
    startCmdLen+=len;
    envSz+=len-4;
  }
  envStr=(char*)malloc(envStrLen);
  startCmd=(char*)malloc(startCmdLen);
  sprintf(startCmd,"-d -e -c ");
  for(i=0;i<dummyVarNum;i++)
  {
    sprintf(envStr,"-D DUMMY_VAR_%d=%s ",i,dummyVar);
    strcat(startCmd,envStr);
  }
  strcat(startCmd,startPsPath);
  printf("startCmd=\"%s\"\n",startCmd);
  printf("environment size >=%d B\n",envSz);
  /*-------------------------------------------------------------------------*/
  br.getServices("/FMC/*/task_manager/start");
  while((type=br.getNextService(sN,format))!=0)
  {
    if(!fnmatch(cmdPattern,sN,0))
    {
      int j=0;
      printf("Contacting task manager Service: \"%s\"...\n",sN);
      for(j=0;j<processNumber;j++)
      {
        //printf("%d\n",j);
        //snprintf(startCmd,256,"-d -e -u counter_%d %s",j,argv[1]);
        DimClient::sendCommand(sN,startCmd);
      }
    }
  }
  /*-------------------------------------------------------------------------*/
  return 0;
}
/*****************************************************************************/
void usage(char *pName)
{
  fprintf(stderr,"\nUsage: %s hostname_pattern process_number "
          "number_of_dummy_env_variable path\n",pName);
  fprintf(stderr,"e.g.: %s farm0101 100 23 /opt/FMC/tests/counter\n",pName);
  exit(1);
}
/*****************************************************************************/
