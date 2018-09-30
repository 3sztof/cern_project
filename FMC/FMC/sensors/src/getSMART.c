/*****************************************************************************/
/*
 * $Log: getSMART.c,v $
 * Revision 1.10  2009/01/29 10:17:16  galli
 * minor changes
 *
 * Revision 1.9  2009/01/08 15:37:01  galli
 * bug fixed in disk size
 *
 * Revision 1.7  2008/04/16 14:17:56  galli
 * bug fixed in popen stderr redirection
 *
 * Revision 1.6  2007/09/20 14:50:05  galli
 * getPhysDisks() uses /proc/partitions instead of fdisk -l
 *
 * Revision 1.5  2007/08/10 14:02:02  galli
 * added function getSMARTsensorVersion()
 * compatible with libFMCutils v 2
 *
 * Revision 1.4  2007/08/07 13:15:22  galli
 * compatible with smartctl version 5.33
 *
 * Revision 1.2  2006/10/23 21:32:18  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.1  2006/04/07 06:53:18  galli
 * Initial revision
 * */
/*****************************************************************************/
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <sys/wait.h>                                       /* WEXITSTATUS() */
#include <errno.h>                                                  /* errno */
#include <string.h>                                  /* strerror(), strcmp() */
#include <stdlib.h>                                             /* strtoul() */
/*****************************************************************************/
#include "getSMART.h"                                /* struct smartAtaAttrs */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
/*****************************************************************************/
/* globals */
char *smartctlCmd;
static char rcsid[]="$Id: getSMART.c,v 1.10 2009/01/29 10:17:16 galli Exp $";
/*****************************************************************************/
physDiskData_t getPhysDisks(int deBug,int errU)
{
  physDiskData_t disk;
  char listCmdLine[1024]="";
  FILE *listOutErrFP=NULL;
  char listOutErrStr[SENSORS_BUFFER_SIZE]="";
  char *p=NULL;
  char *lp=NULL;
  char *line=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,VERB,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  memset(&disk,0,sizeof(disk));
  disk.version=rcsid;
  /*-------------------------------------------------------------------------*/
  /* execute list command */
  //sprintf(listCmdLine,"fdisk -l 2>/dev/null|grep Disk|egrep 'hd|sd'");
  sprintf(listCmdLine,"(/bin/cat /proc/partitions|/bin/egrep 'hd|sd'|"
          "/bin/egrep -v '[a-zA-Z]{3}[0-9]{1,2}'|"
          "/usr/bin/awk '{print $4 \" \" $3}') 2>&1");
  if(deBug>1)mPrintf(errU,VERB,__func__,0,"Execute: \"%s\".",listCmdLine);
  listOutErrFP=popen(listCmdLine,"r");
  fread(listOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,listOutErrFP);
  disk.exitStatus=WEXITSTATUS(pclose(listOutErrFP));
  /* force null-termination in output */
  listOutErrStr[sizeof(listOutErrStr)-1]='\0';
  /*-------------------------------------------------------------------------*/
  /* copy output */
  strncpy(disk.errorString,listOutErrStr,sizeof(disk.errorString));
  /* force null-termination in output copy */
  disk.errorString[sizeof(disk.errorString)-1]='\0';
  /* substitutes newlines with semicolumns */
  for(p=disk.errorString;*p;p++)if(*p=='\n'||*p=='\r')*p=';';
  /* debug print */
  if(deBug>1)mPrintf(errU,VERB,__func__,0,"Output of fdisk command: \"%s\"",
                     disk.errorString);
  if(disk.exitStatus)                                             /* failure */
  {
    disk.success=0;
    return disk;
  }
  disk.success=1;
  disk.errorString[0]='\0';
  /*-------------------------------------------------------------------------*/
  for(p=listOutErrStr,disk.N=0;;)
  {
    int fl;
    line=p;
    p=strchr(p,'\n');
    if(!p)break;
    *p='\0';
    if(!strlen(line))break;
    /*.......................................................................*/
    lp=line;
    fl=strcspn(lp," \t");
    snprintf(disk.item[disk.N].name,strlen("/dev/")+fl+1,"/dev/%s",lp);
    lp+=fl;
    lp+=strspn(lp," \t");                                     /* skip blanks */
    disk.item[disk.N].size=1024*strtoull(lp,NULL,10);
    /*.......................................................................*/
    p++;
    disk.N++;
  }
  /*-------------------------------------------------------------------------*/
  return disk;
}
/*****************************************************************************/
diskType_t getDiskType(char *dskDev,int deBug,int errU)
{
  char smartctlCmdLine[1024]="";
  FILE *smartctlOutErrFP=NULL;
  char smartctlOutErrStr[SENSORS_BUFFER_SIZE]="";
  char smartctlOutErrStr2[SENSORS_BUFFER_SIZE]="";
  char* sataMsg[]=
  {
    "SATA disks accessed via libata are not currently supported by",
    "SATA devices are not supported via the SCSI device type"
  };
  diskType_t dt;
  char *p=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,VERB,__func__,0,"Entered...");
  if(deBug>1)mPrintf(errU,VERB,__func__,0,"Device: %s",dskDev);
  /*-------------------------------------------------------------------------*/
  memset(&dt,0,sizeof(dt));
  dt.type=UNKNOWN;
  /*-------------------------------------------------------------------------*/
  /* execute smartctl command */
  sprintf(smartctlCmdLine,"%s -i %s 2>&1",smartctlCmd,dskDev);
  if(deBug>1)mPrintf(errU,VERB,__func__,0,"Execute: \"%s\".",smartctlCmdLine);
  smartctlOutErrFP=popen(smartctlCmdLine,"r");
  fread(smartctlOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,smartctlOutErrFP);
  dt.exitStatus=WEXITSTATUS(pclose(smartctlOutErrFP));
  /* force null-termination in output */
  smartctlOutErrStr[sizeof(smartctlOutErrStr)-1]='\0';
  /*-------------------------------------------------------------------------*/
  /* copy output */
  strncpy(dt.errorString1,smartctlOutErrStr,sizeof(dt.errorString1));
  /* force null-termination in output copy */
  dt.errorString1[sizeof(dt.errorString1)-1]='\0';
  /* substitutes newlines with semicolumns */
  for(p=dt.errorString1;*p;p++)if(*p=='\n'||*p=='\r')*p=';';
  /* debug print */
  if(deBug>1)mPrintf(errU,VERB,__func__,0,"Output of \"smartctl -i\" command: "
                     "\"%s\".",dt.errorString1);
  if(dt.exitStatus)                                               /* failure */
  {
    dt.success=0;
    return dt;
  }
  dt.success=1;
  dt.errorString1[0]='\0';
  /*-------------------------------------------------------------------------*/
  /* ATA */
  if(strstr(smartctlOutErrStr,"ATA Version is"))
  {
    dt.type=ATA;
    if(deBug>1)mPrintf(errU,VERB,__func__,0,"Type ATA.");
  }
  /* SATA? */
  else if(strstr(smartctlOutErrStr,sataMsg[0])||
          strstr(smartctlOutErrStr,sataMsg[1])||
          (strstr(smartctlOutErrStr,"ATA")&&
          (strstr(smartctlOutErrStr,"does not support SMART"))))
  {
    if(deBug>1)mPrintf(errU,VERB,__func__,0,"Maybe type SATA.");
    /*-----------------------------------------------------------------------*/
    /* execute smartctl command */
    sprintf(smartctlCmdLine,"%s -d ata -i %s 2>&1",smartctlCmd,dskDev);
    if(deBug>1)mPrintf(errU,VERB,__func__,0,"Execute2: \"%s\".",
                       smartctlCmdLine);
    smartctlOutErrFP=popen(smartctlCmdLine,"r");
    fread(smartctlOutErrStr2,sizeof(char),SENSORS_BUFFER_SIZE,smartctlOutErrFP);
    dt.exitStatus=WEXITSTATUS(pclose(smartctlOutErrFP));
    /* force null-termination in output */
    smartctlOutErrStr2[sizeof(smartctlOutErrStr2)-1]='\0';
    /*-----------------------------------------------------------------------*/
    /* copy output */
    strncpy(dt.errorString2,smartctlOutErrStr2,sizeof(dt.errorString2));
    /* force null-termination in output copy */
    dt.errorString2[sizeof(dt.errorString2)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=dt.errorString2;*p;p++)if(*p=='\n'||*p=='\r')*p=';';
    /* debug print */
    if(deBug>1)mPrintf(errU,VERB,__func__,0,"Output of \"smartctl -d ata -i\" "
                       "command: \"%s\".",dt.errorString2);
    if(dt.exitStatus)
    {
      dt.success=0;
      return dt;
    }
    dt.success=1;
    dt.errorString1[0]='\0';
    dt.errorString2[0]='\0';
    /* SATA */
    if(strstr(smartctlOutErrStr2,"ATA Version is"))
    {
      dt.type=SATA;
      if(deBug>1)mPrintf(errU,VERB,__func__,0,"Type SATA.");
    }
  }
  /* SCSI */
  else if(strstr(smartctlOutErrStr,"Transport protocol:"))
  {
    dt.type=SCSI;
    if(deBug>1)mPrintf(errU,VERB,__func__,0,"Type SCSI.");
  }
  /*-------------------------------------------------------------------------*/
  return dt;
}
/*****************************************************************************/
ataDiskInfo_t getAtaInfo(char *dskDev,int deBug,int errU)
{
  ataDiskInfo_t ataInfo;
  char smartctlCmdLine[1024]="";
  FILE *smartctlOutErrFP=NULL;
  char smartctlOutErrStr[SENSORS_BUFFER_SIZE]="";
  char *p=NULL;
  char *vp=NULL;
  char *line=NULL;
  char label[32]="";
  char value[64]="";
  int labelLen=0;
  int valueLen=0;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,VERB,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  memset(&ataInfo,0,sizeof(ataInfo));
  /*-------------------------------------------------------------------------*/
  /* execute smartctl command */
  sprintf(smartctlCmdLine,"%s -i -d ata %s 2>&1",smartctlCmd,dskDev);
  smartctlOutErrFP=popen(smartctlCmdLine,"r");
  fread(smartctlOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,smartctlOutErrFP);
  ataInfo.exitStatus=WEXITSTATUS(pclose(smartctlOutErrFP));
  /*-------------------------------------------------------------------------*/
  if(!ataInfo.exitStatus)ataInfo.success=1;
  else
  {
    strncpy(ataInfo.errorString,smartctlOutErrStr,sizeof(ataInfo.errorString));
    ataInfo.errorString[sizeof(ataInfo.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=ataInfo.errorString;*p;p++)if(*p=='\n')*p=';';
    return ataInfo;
  }
  /*-------------------------------------------------------------------------*/
  p=strstr(smartctlOutErrStr,"=== START OF INFORMATION SECTION ===");
  if(!p)
  {
    ataInfo.success=0;
    strncpy(ataInfo.errorString,smartctlOutErrStr,sizeof(ataInfo.errorString));
    ataInfo.errorString[sizeof(ataInfo.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=ataInfo.errorString;*p;p++)if(*p=='\n')*p=';';
    return ataInfo;
  }
  p=strchr(p,'\n');
  p++;
  /*-------------------------------------------------------------------------*/
  for(;;)
  {
    line=p;
    p=strchr(p,'\n');
    if(!p)break;
    *p='\0';
    if(!strlen(line))break;
    /*.......................................................................*/
    labelLen=strcspn(line,":");
    if(labelLen>31)labelLen=31;
    strncpy(label,line,labelLen);
    label[labelLen]='\0';
    vp=1+strchr(line,':');
    vp+=strspn(vp," \t");
    valueLen=strlen(vp);
    if(valueLen>63)valueLen=63;
    strncpy(value,vp,valueLen);
    value[valueLen]='\0';
    /*.......................................................................*/
    if(strstr(label,"Device Model"))strcpy(ataInfo.deviceModel,value);
    else if(strstr(label,"Serial Number"))strcpy(ataInfo.serialNumber,value);
    else if(strstr(label,"Firmware Version"))strcpy(ataInfo.firmwareVers,value);
    else if(strstr(label,"ATA Version is"))ataInfo.ataVersion=atoi(value);
    else if(strstr(label,"ATA Standard is"))strcpy(ataInfo.ataStandard,value);
    /*.......................................................................*/
    p++;
  }
  /*-------------------------------------------------------------------------*/
  return ataInfo;
}
/*****************************************************************************/
scsiDiskInfo_t getScsiInfo(char *dskDev,int deBug,int errU)
{
  scsiDiskInfo_t scsiInfo;
  char smartctlCmdLine[1024]="";
  FILE *smartctlOutErrFP=NULL;
  char smartctlOutErrStr[SENSORS_BUFFER_SIZE]="";
  char *p=NULL;
  char *vp=NULL;
  char *line=NULL;
  char label[32]="";
  char value[64]="";
  int labelLen=0;
  int valueLen=0;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,VERB,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  memset(&scsiInfo,0,sizeof(scsiInfo));
  /*-------------------------------------------------------------------------*/
  /* execute smartctl command */
  sprintf(smartctlCmdLine,"%s -i -d scsi %s 2>&1",smartctlCmd,dskDev);
  smartctlOutErrFP=popen(smartctlCmdLine,"r");
  fread(smartctlOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,smartctlOutErrFP);
  scsiInfo.exitStatus=WEXITSTATUS(pclose(smartctlOutErrFP));
  /*-------------------------------------------------------------------------*/
  if(!scsiInfo.exitStatus)scsiInfo.success=1;
  else
  {
    strncpy(scsiInfo.errorString,smartctlOutErrStr,
            sizeof(scsiInfo.errorString));
    scsiInfo.errorString[sizeof(scsiInfo.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=scsiInfo.errorString;*p;p++)if(*p=='\n')*p=';';
    return scsiInfo;
  }
  /*-------------------------------------------------------------------------*/
  p=strstr(smartctlOutErrStr,"Device:");
  if(!p)
  {
    scsiInfo.success=0;
    strncpy(scsiInfo.errorString,smartctlOutErrStr,sizeof(scsiInfo.errorString));
    scsiInfo.errorString[sizeof(scsiInfo.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=scsiInfo.errorString;*p;p++)if(*p=='\n')*p=';';
    return scsiInfo;
  }
  /*-------------------------------------------------------------------------*/
  for(;;)
  {
    line=p;
    p=strchr(p,'\n');
    if(!p)break;
    *p='\0';
    p++;
    if(!strlen(line))continue;
    if(!strchr(line,':'))continue;
    /*.......................................................................*/
    labelLen=strcspn(line,":");
    if(labelLen>31)labelLen=31;
    strncpy(label,line,labelLen);
    label[labelLen]='\0';
    vp=1+strchr(line,':');
    vp+=strspn(vp," \t");
    valueLen=strlen(vp);
    if(valueLen>63)valueLen=63;
    strncpy(value,vp,valueLen);
    value[valueLen]='\0';
    /*.......................................................................*/
    if(!strcmp(label,"Device"))strcpy(scsiInfo.deviceModel,value);
    else if(strstr(label,"Serial number"))strcpy(scsiInfo.serialNumber,value);
    else if(strstr(label,"Transport protocol"))
      strcpy(scsiInfo.transpProto,value);
    /*.......................................................................*/
  }
  /*-------------------------------------------------------------------------*/
  return scsiInfo;
}
/*****************************************************************************/
smartAtaAttrs_t readAtaSMARTattrs(char *dskDev,int deBug,int errU)
{
  smartAtaAttrs_t ataAttrs;
  char smartctlCmdLine[1024]="";
  FILE *smartctlOutErrFP=NULL;
  char smartctlOutErrStr[SENSORS_BUFFER_SIZE]="";
  char *p=NULL;
  char *hp=NULL,*lp=NULL;
  char *line=NULL;
  char header[128]="";
  char hf[64]="";
  char lf[64]="";
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,VERB,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  memset(&ataAttrs,0,sizeof(ataAttrs));
  /*-------------------------------------------------------------------------*/
  /* execute smartctl command */
  sprintf(smartctlCmdLine,"%s -A -d ata %s 2>&1",smartctlCmd,dskDev);
  smartctlOutErrFP=popen(smartctlCmdLine,"r");
  fread(smartctlOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,smartctlOutErrFP);
  ataAttrs.exitStatus=WEXITSTATUS(pclose(smartctlOutErrFP));
  /*-------------------------------------------------------------------------*/
  if(!ataAttrs.exitStatus)ataAttrs.success=1;
  else
  {
    strncpy(ataAttrs.errorString,smartctlOutErrStr,
            sizeof(ataAttrs.errorString));
    ataAttrs.errorString[sizeof(ataAttrs.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=ataAttrs.errorString;*p;p++)if(*p=='\n')*p=';';
    return ataAttrs;
  }
  /*-------------------------------------------------------------------------*/
  /* read the header line */
  hp=strstr(smartctlOutErrStr,"ID# ATTRIBUTE_NAME");
  if(!hp)
  {
    ataAttrs.success=0;
    strncpy(ataAttrs.errorString,smartctlOutErrStr,
            sizeof(ataAttrs.errorString));
    ataAttrs.errorString[sizeof(ataAttrs.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=ataAttrs.errorString;*p;p++)if(*p=='\n')*p=';';
    return ataAttrs;
  }
  p=strchr(hp,'\n');
  if(p)*p='\0';
  strncpy(header,hp,sizeof(header));
  header[sizeof(header)-1]='\0';
  p++;
  /*-------------------------------------------------------------------------*/
  /* read Vendor Specific SMART Attributes with Thresholds */
  for(ataAttrs.N=0;;)
  {
    line=p;
    p=strchr(p,'\n');
    if(!p)break;
    *p='\0';
    if(!strlen(line))break;
    /*.......................................................................*/
    for(hp=header,lp=line;;)
    {
      hp+=strspn(hp," \t");
      lp+=strspn(lp," \t");
      if(hp>=header+strlen(header)||lp>=line+strlen(line))break;
      strncpy(hf,hp,strcspn(hp," \t"));
      hf[strcspn(hp," \t")]='\0';
      strncpy(lf,lp,strcspn(lp," \t"));
      lf[strcspn(lp," \t")]='\0';
      if(!strcmp(hf,"ID#"))
      {
        ataAttrs.item[ataAttrs.N].id=strtoul(lf,NULL,0);
      }
      else if(!strcmp(hf,"ATTRIBUTE_NAME"))
      {
        strncpy(ataAttrs.item[ataAttrs.N].name,lf,27);
        ataAttrs.item[ataAttrs.N].name[27]='\0';
      }
      else if(!strcmp(hf,"FLAG"))
      {
        ataAttrs.item[ataAttrs.N].flag=strtoul(lf,NULL,0);
      }
      else if(!strcmp(hf,"VALUE"))
      {
        ataAttrs.item[ataAttrs.N].value=strtoul(lf,NULL,10);
      }
      else if(!strcmp(hf,"WORST"))
      {
        ataAttrs.item[ataAttrs.N].worst=strtoul(lf,NULL,10);
      }
      else if(!strcmp(hf,"THRESH"))
      {
        ataAttrs.item[ataAttrs.N].threshold=strtoul(lf,NULL,10);
      }
      else if(!strcmp(hf,"TYPE"))
      {
        strncpy(ataAttrs.item[ataAttrs.N].type,lf,8);
        ataAttrs.item[ataAttrs.N].type[8]='\0';
      }
      else if(!strcmp(hf,"UPDATED"))
      {
        strncpy(ataAttrs.item[ataAttrs.N].updated,lf,7);
        ataAttrs.item[ataAttrs.N].updated[7]='\0';
      }
      else if(!strcmp(hf,"WHEN_FAILED"))
      {
        strncpy(ataAttrs.item[ataAttrs.N].whenFailed,lf,11);
        ataAttrs.item[ataAttrs.N].whenFailed[11]='\0';
      }
      else if(!strcmp(hf,"RAW_VALUE"))
      {
        ataAttrs.item[ataAttrs.N].rawValue=strtoull(lf,NULL,10);
      }
      hp+=strcspn(hp," \t");
      lp+=strcspn(lp," \t");
      if(hp>=header+strlen(header)||lp>=line+strlen(line))break;
    }
    /*.......................................................................*/
    p++;
    ataAttrs.N++;
  }
  /*-------------------------------------------------------------------------*/
  return ataAttrs;
}
/*****************************************************************************/
smartScsiAttrs_t readScsiSMARTattrs(char *dskDev,int deBug,int errU)
{
  smartScsiAttrs_t scsiAttrs;
  char smartctlCmdLine[1024]="";
  FILE *smartctlOutErrFP=NULL;
  char smartctlOutErrStr[SENSORS_BUFFER_SIZE]="";
  char *p=NULL;
  char *np=NULL,*vp=NULL,*up=NULL,*cp=NULL;
  char *line=NULL;
  char name[SCSI_ATTR_NAME_LEN+1]="";
  char units[SCSI_ATTR_UNITS_LEN+1]="";
  int nameLen=0;
  int unitsLen=0;
  uint64_t value=0;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,VERB,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  memset(&scsiAttrs,0,sizeof(scsiAttrs));
  /*-------------------------------------------------------------------------*/
  /* execute smartctl command */
  sprintf(smartctlCmdLine,"%s -A -d scsi %s 2>&1",smartctlCmd,dskDev);
  smartctlOutErrFP=popen(smartctlCmdLine,"r");
  fread(smartctlOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,smartctlOutErrFP);
  scsiAttrs.exitStatus=WEXITSTATUS(pclose(smartctlOutErrFP));
  /*-------------------------------------------------------------------------*/
  if(!scsiAttrs.exitStatus)scsiAttrs.success=1;
  else
  {
    strncpy(scsiAttrs.errorString,smartctlOutErrStr,
            sizeof(scsiAttrs.errorString));
    scsiAttrs.errorString[sizeof(scsiAttrs.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=scsiAttrs.errorString;*p;p++)if(*p=='\n')*p=';';
    return scsiAttrs;
  }
  /*-------------------------------------------------------------------------*/
  /* skip the version lines */
  p=strstr(smartctlOutErrStr,"Home page is");
  if(!p)
  {
    scsiAttrs.success=0;
    strncpy(scsiAttrs.errorString,smartctlOutErrStr,
            sizeof(scsiAttrs.errorString));
    scsiAttrs.errorString[sizeof(scsiAttrs.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=scsiAttrs.errorString;*p;p++)if(*p=='\n')*p=';';
    return scsiAttrs;
  }
  scsiAttrs.N=0; 
  p=strchr(p,'\n');
  p+=2;                                                       /* skip "\n\n" */
  /*-------------------------------------------------------------------------*/
  /* read SMART Attributes */
  for(;;)                                       /* loop over attribute lines */
  {
    line=p;
    p=strchr(p,'\n');
    if(!p)break;
    *p='\0';
    p++;
    /* empty line */
    if(!strlen(line))continue;
    /* line has no attributes */
    if(!strstr(line,": ")&&!strstr(line," = "))continue;
    /*.......................................................................*/
    /* extract name, value and units from line */
    /* skip initial blanks */
    np=line+strspn(line," \t");                         /* np = name pointer */
    if(np>=line+strlen(line))continue;
    /* copy to "name" the line until ": " or " = " */
    vp=strstr(np,": ");                                /* vp = value pointer */
    if(!vp)vp=strstr(np," = ");
    nameLen=vp-np;
    vp+=2;
    if(nameLen>SCSI_ATTR_NAME_LEN)nameLen=SCSI_ATTR_NAME_LEN;
    strncpy(name,np,nameLen);
    *(name+nameLen)='\0';
    /* chop name */
    for(cp=name+nameLen-1;cp>=name;cp--)
    {
      if(*cp==' '||*cp=='\t')cp='\0';
      else break;
    }
    value=(uint64_t)strtoull(vp,&up,0);                /* up = units pointer */
    if(*up=='.')value=(uint64_t)strtod(vp,&up);  /* because of a float value */
    if(*up!='\0')
    {
      up=up+strspn(up," \t");
      if(up>=line+strlen(line))break;
      unitsLen=strlen(up);
      if(unitsLen>SCSI_ATTR_UNITS_LEN)unitsLen=SCSI_ATTR_UNITS_LEN;
      strncpy(units,up,unitsLen);
      *(units+unitsLen)='\0';
      /* chop units */
      for(cp=units+unitsLen-1;cp>=units;cp--)
      {
        if(*cp==' '||*cp=='\t')cp='\0';
        else break;
      }
    }
    else strcpy(units,"");
    strncpy(scsiAttrs.item[scsiAttrs.N].name,name,SCSI_ATTR_NAME_LEN);
    scsiAttrs.item[scsiAttrs.N].name[SCSI_ATTR_NAME_LEN]='\0';
    scsiAttrs.item[scsiAttrs.N].value=value;
    strncpy(scsiAttrs.item[scsiAttrs.N].units,units,SCSI_ATTR_UNITS_LEN);
    scsiAttrs.item[scsiAttrs.N].units[SCSI_ATTR_UNITS_LEN]='\0';
    scsiAttrs.N++;
  }                                             /* loop over attribute lines */
  /*-------------------------------------------------------------------------*/
  return scsiAttrs;
}
/*****************************************************************************/
smartScsiErrs_t readScsiSMARTerrs(char *dskDev,int deBug,int errU)
{
  smartScsiErrs_t scsiErrs;
  char smartctlCmdLine[1024]="";
  FILE *smartctlOutErrFP=NULL;
  char smartctlOutErrStr[SENSORS_BUFFER_SIZE]="";
  char *p=NULL;
  char *line=NULL;
  char *tail=NULL;
  /*-------------------------------------------------------------------------*/
  if(deBug)mPrintf(errU,VERB,__func__,0,"Entered...");
  /*-------------------------------------------------------------------------*/
  memset(&scsiErrs,0,sizeof(scsiErrs));
  /*-------------------------------------------------------------------------*/
  /* execute smartctl command */
  sprintf(smartctlCmdLine,"%s -l error -d scsi %s 2>&1",smartctlCmd,dskDev);
  smartctlOutErrFP=popen(smartctlCmdLine,"r");
  fread(smartctlOutErrStr,sizeof(char),SENSORS_BUFFER_SIZE,smartctlOutErrFP);
  scsiErrs.exitStatus=WEXITSTATUS(pclose(smartctlOutErrFP));
  /*-------------------------------------------------------------------------*/
  if(!scsiErrs.exitStatus)scsiErrs.success=1;
  else
  {
    strncpy(scsiErrs.errorString,smartctlOutErrStr,
            sizeof(scsiErrs.errorString));
    scsiErrs.errorString[sizeof(scsiErrs.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=scsiErrs.errorString;*p;p++)if(*p=='\n')*p=';';
    return scsiErrs;
  }
  /*-------------------------------------------------------------------------*/
  /* skip header lines */
  p=strstr(smartctlOutErrStr,"[10^9 bytes]  errors");
  if(!p)
  {
    scsiErrs.success=0;
    strncpy(scsiErrs.errorString,smartctlOutErrStr,
            sizeof(scsiErrs.errorString));
    scsiErrs.errorString[sizeof(scsiErrs.errorString)-1]='\0';
    /* substitutes newlines with semicolumns */
    for(p=scsiErrs.errorString;*p;p++)if(*p=='\n')*p=';';
    return scsiErrs;
  }
  p=strchr(p,'\n');
  p++;                                                          /* skip "\n" */
  /*-------------------------------------------------------------------------*/
  /* read SMART Errors */
  for(;;)                                       /* loop over attribute lines */
  {
    line=p;
    p=strchr(p,'\n');
    if(!p)break;
    *p='\0';
    p++;
    /* empty line */
    if(!strlen(line))continue;
    /*.......................................................................*/
    if(!strncmp(line,"read:",5))
    {
      tail=line+5;
      scsiErrs.eecFastR=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.eecDelR=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.reR=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.corrR=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.algInvR=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.procGBr=(double)strtod(tail,&tail);
      scsiErrs.unCorrR=(uint64_t)strtoull(tail,&tail,0);
    }
    else if(!strncmp(line,"write:",6))
    {
      tail=line+6;
      scsiErrs.eecFastW=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.eecDelW=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.reW=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.corrW=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.algInvW=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.procGBw=(double)strtod(tail,&tail);
      scsiErrs.unCorrW=(uint64_t)strtoull(tail,&tail,0);
    }
    else if(!strncmp(line,"verify:",7))
    {
      tail=line+7;
      scsiErrs.eecFastV=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.eecDelV=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.reV=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.corrV=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.algInvV=(uint64_t)strtoull(tail,&tail,0);
      scsiErrs.procGBv=(double)strtod(tail,&tail);
      scsiErrs.unCorrV=(uint64_t)strtoull(tail,&tail,0);
    }
    else if(!strncmp(line,"Non-medium error count:",23))
    {
      tail=line+23;
      scsiErrs.nonMedium=(uint64_t)strtoull(tail,&tail,0);
    }
    /*.......................................................................*/
  }                                             /* loop over attribute lines */
  /*-------------------------------------------------------------------------*/
  return scsiErrs;
}
/*****************************************************************************/
void setSmartctlCmd(char *cmd)
{
  smartctlCmd=cmd;
  return;
}
/*****************************************************************************/
/* getSMARTsensorVersion() - Returns the RCS identifier of this file.        */
/*****************************************************************************/
char *getSMARTsensorVersion()
{
  return rcsid;
}
/*****************************************************************************/
