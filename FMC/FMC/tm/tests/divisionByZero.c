#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>
int main(int argc,char **argv,char **envp)
{
  int i;
  int invi;
  for(i=-3;;i++)
  {
    invi=1/i;
    printf("%d\t%d\n",i,invi);
    sleep(1);
  }
  return 0;
}
