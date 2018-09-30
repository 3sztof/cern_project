#include <stdio.h>
#include <unistd.h>  /* sleep() */
#include <stdlib.h>  /* atoi() */
int main(int argc,char **argv)
{
  unsigned long long i;
  if(argc!=2)
  {
    fprintf(stderr,"usage %s number_of_second\n",argv[0]);
    exit(1);
  }
  for(i=0;i<atoi(argv[1]);i++)
  {
    printf("%llu\n",i);
    fflush(stdout);
    sleep(1);
  }
  return 0;
}
