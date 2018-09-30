#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
int main(int argc,char **argv,char **envp)
{
  unsigned long long i;
  int j;
  printf("\n");
  signal(SIGINT,SIG_IGN);
  signal(SIGTERM,SIG_IGN);
  for(j=0;envp[j];j++)
  {
    printf("%s: envp[%d] %s\n",argv[0],j,envp[j]);
  }
  fflush(stdout);
  for(i=0;;i++)
  {
    printf("%llu\n",i);
    sleep(1);
  }
  return 0;
}
