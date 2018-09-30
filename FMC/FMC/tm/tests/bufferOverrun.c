#include <stdio.h>
#include <unistd.h>
int main(int argc,char **argv,char **envp)
{
  unsigned long long i;
  int a[3]={0,0,0};
  for(i=0;;i++)
  {
    printf("%llu %d\n",i,a[i]);
    usleep(1000);
  }
  return 0;
}
