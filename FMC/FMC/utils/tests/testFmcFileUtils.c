#include <stdio.h>
#include "fmcMsgUtils.h"
#include "fmcFileUtils.h"
int main()
{
  char *buf=NULL;
  size_t bufSz=0;
  size_t bufFil=0;
bufFil=fmcFile2str(&buf,&bufSz,4096,"/proc/cpuinfo",L_STD,4);
printf("------------------------------------------------------------\n");
if(bufFil)printf("\"%s\"\n",buf);
printf("------------------------------------------------------------\n");
bufFil=fmcFile2str(&buf,&bufSz,4096,"/proc/cpuinfo",L_STD,4);
printf("------------------------------------------------------------\n");
if(bufFil)printf("\"%s\"\n",buf);
printf("------------------------------------------------------------\n");
bufFil=fmcFile2str(&buf,&bufSz,4096,"/proc/bus/pci/devices",L_STD,4);
printf("------------------------------------------------------------\n");
if(bufFil)printf("\"%s\"\n",buf);
printf("------------------------------------------------------------\n");
bufFil=fmcFile2str(&buf,&bufSz,4096,"/proc/cpuinfo",L_STD,4);
printf("------------------------------------------------------------\n");
if(bufFil)printf("\"%s\"\n",buf);
printf("------------------------------------------------------------\n");
bufFil=fmcFile2str(&buf,&bufSz,4096,"/proc/bus/pci/devices",L_STD,4);
printf("------------------------------------------------------------\n");
if(bufFil)printf("\"%s\"\n",buf);
printf("------------------------------------------------------------\n");
  return 0;
}
