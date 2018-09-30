#include <stdio.h>
#include <unistd.h>
int myFunc(int i)
{
  myFunc(i);
  return 0;
}
int main(int argc,char **argv,char **envp)
{
  unsigned long long i;
  for(i=0;;i++)
  {
    printf("%llu\n",i);
    if(i==5)myFunc(i);
    sleep(1);
  }
  return 0;
}
