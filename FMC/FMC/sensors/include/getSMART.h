/*****************************************************************************/
/*
 * $Log: getSMART.h,v $
 * Revision 1.6  2009/01/29 10:17:16  galli
 * minor changes
 *
 * Revision 1.4  2007/08/10 14:01:57  galli
 * added function getSMARTsensorVersion()
 *
 * Revision 1.3  2007/08/07 13:14:52  galli
 * compatible with smartctl version 5.33
 *
 * Revision 1.2  2006/10/23 21:31:59  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.1  2006/04/07 06:54:11  galli
 * Initial revision
 */
/*****************************************************************************/
#include <inttypes.h>
#include <sys/types.h>
#define NUMBER_ATA_SMART_ATTRIBUTES 30
#define NUMBER_SCSI_SMART_ATTRIBUTES 30
#define SCSI_ATTR_NAME_LEN 63
#define SCSI_ATTR_UNITS_LEN 15
#define MAX_PHYS_DISK 30
#define SENSORS_BUFFER_SIZE ((NUMBER_ATA_SMART_ATTRIBUTES+10)*132)
/*****************************************************************************/
/* one (S)ATA attribute */
typedef struct smartAtaAttr
{
  uint8_t id;
  char name[28];                            /* "Off-line_Scan_UNC_Sector_Ct" */
  uint16_t flag;
  uint8_t value;
  uint8_t worst;
  uint8_t threshold;
  char type[9];                                      /* "Pre-fail"/"Old_age" */
  char updated[8];                                     /* "Always"/"Offline" */
  char whenFailed[12];                /* "FAILING_NOW"/"In_the_past"/"    -" */
  uint64_t rawValue;                                                  /* 6 B */
}smartAtaAttr_t;
/*---------------------------------------------------------------------------*/
/* all the (S)ATA attributes of one disk */
typedef struct smartAtaAttrs
{
  int success;
  int N;
  int exitStatus;
  char errorString[132*10];
  smartAtaAttr_t item[NUMBER_ATA_SMART_ATTRIBUTES];
}smartAtaAttrs_t;
/*---------------------------------------------------------------------------*/
/* one SCSI attribute */
typedef struct smartScsiAttr
{
  char name[SCSI_ATTR_NAME_LEN+1];
  uint64_t value;
  char units[SCSI_ATTR_UNITS_LEN+1];
}smartScsiAttr_t;
/*---------------------------------------------------------------------------*/
/* all the SCSI attributes of one disk */
typedef struct smartScsiAttrs
{
  int success;
  int N;
  int exitStatus;
  char errorString[132*10];
  smartScsiAttr_t item[NUMBER_SCSI_SMART_ATTRIBUTES];
}smartScsiAttrs_t;
/*---------------------------------------------------------------------------*/
typedef struct smartScsiErrs
{
  int success;
  int N;
  int exitStatus;
  char errorString[132*10];
  uint64_t eecFastR;
  uint64_t eecFastW;
  uint64_t eecFastV;
  uint64_t eecDelR;
  uint64_t eecDelW;
  uint64_t eecDelV;
  uint64_t reR;
  uint64_t reW;
  uint64_t reV;
  uint64_t corrR;
  uint64_t corrW;
  uint64_t corrV;
  uint64_t algInvR;
  uint64_t algInvW;
  uint64_t algInvV;
  double procGBr;
  double procGBw;
  double procGBv;
  uint64_t unCorrR;
  uint64_t unCorrW;
  uint64_t unCorrV;
  uint64_t nonMedium;
}smartScsiErrs_t;
/*---------------------------------------------------------------------------*/
/* one physical disk */
typedef struct physDiskItem
{
  char name[64];                                         /* disk device name */
  uint64_t size;                                       /* disk size in Bytes */
}physDiskItem_t;
/*---------------------------------------------------------------------------*/
/* all the physical disks of a PC */
typedef struct physDiskData
{
  char *version;
  int success;
  int N;
  int exitStatus;
  char errorString[132*10];
  physDiskItem_t item[MAX_PHYS_DISK];
}physDiskData_t;
/*---------------------------------------------------------------------------*/
enum type{UNKNOWN=0,ATA,SATA,SCSI};
/*---------------------------------------------------------------------------*/
typedef struct diskType
{
  int success;
  int exitStatus;
  char errorString1[132*10];
  char errorString2[132*10];
  int type;                                               /* ATA, SATA, SCSI */
}diskType_t;
/*---------------------------------------------------------------------------*/
typedef struct ataDiskInfo
{
  int success;
  int exitStatus;
  char errorString[132*10];
  char deviceModel[64];
  char serialNumber[64];
  char firmwareVers[64];
  int ataVersion;
  char ataStandard[64];
}ataDiskInfo_t;
/*---------------------------------------------------------------------------*/
typedef struct scsiDiskInfo
{
  int success;
  int exitStatus;
  char errorString[132*10];
  char deviceModel[64];
  char serialNumber[64];
  char transpProto[64];
}scsiDiskInfo_t;
/*---------------------------------------------------------------------------*/
void setSmartctlCmd(char *cmd);
physDiskData_t getPhysDisks(int deBug,int errU);
ataDiskInfo_t getAtaInfo(char *dskDev,int deBug,int errU);
scsiDiskInfo_t getScsiInfo(char *dskDev,int deBug,int errU);
diskType_t getDiskType(char *dskDev,int deBug,int errU);
smartAtaAttrs_t readAtaSMARTattrs(char *dskDev,int deBug,int errU);
smartScsiAttrs_t readScsiSMARTattrs(char *dskDev,int deBug,int errU);
smartScsiErrs_t readScsiSMARTerrs(char *dskDev,int deBug,int errU);
char *getSMARTsensorVersion(void);
/*****************************************************************************/
