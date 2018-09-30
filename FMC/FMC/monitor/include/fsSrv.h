/* ######################################################################### */
/*
 * $Log: fsSrv.h,v $
 * Revision 2.2  2011/11/07 11:30:38  galli
 * minor changes
 *
 * Revision 2.1  2011/11/03 09:11:20  galli
 * Rewritten from scratch.
 * New data structure fs_list_t.
 *
 * Revision 1.2  2009/01/29 08:04:13  galli
 * minor changes
 *
 * Revision 1.1  2009/01/07 15:46:55  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _FS_SRV_H
#define _FS_SRV_H 1
/* ######################################################################### */
/* Common to SERVER and CLIENT */
/* DIM server basename */
#define SRV_NAME "filesystems"
/* maximim length of the name of a filesystem label (e.g.: fs_01) */
#define DEVICE_LEN 15
#define TIME_LEN 12                                          /* Oct12-134923 */
/* ######################################################################### */
#ifdef _CLIENT
/* ************************************************************************* */
/* number of fields which have to be set to "N/F" if server unreachable */
#define DEV_FIELD_N_ERR 1
/* ************************************************************************* */
#endif                                                            /* _CLIENT */
/* ######################################################################### */
#ifdef _SERVER
/* ************************************************************************* */
/* labels to be published */
char *sumLabelV[4]={"TotalRoot","UseU%","Filesystem","Mounted on"};
char *sumUnitsV[4]={"KiB","%","",""};
char *detLabelV[13]={"Minfree","TotalRoot","TotalUser","Used","AvailRoot",
                     "AvailUser","UseR%","UseU%","Filesystem","Type",
                     "Mounted on","Time","Status"};
char *detUnitsV[13]={"KiB","KiB","KiB","KiB","KiB","KiB","%","%","","","",
                     "mmmdd-HHMMSS",""};
/* ************************************************************************* */
/* Data Structures */
/* ------------------------------------------------------------------------- */
/* double-linked list of file systems */
typedef struct fs_list
{
  struct fs_data *head;
  struct fs_data *tail;
  int success;
}fs_list_t;
/* ************************************************************************* */
/* Function Prototype */
/* ------------------------------------------------------------------------- */
/* Usage */
void usage(int mode);
void shortUsage(void);
/* ------------------------------------------------------------------------- */
/* Signal Handler */
static void signalHandler(int signo);
/* ------------------------------------------------------------------------- */
/* DIM Error Handler */
void dimErrorHandler(int severity,int errorCode,char *message);
/* ------------------------------------------------------------------------- */
/* DIM Update Handlers */
void devicesHandler(long *tag,int **address,int *size);
void fsSumHandler(long *tag,int **address,int *size);
void fsDetHandler(long *tag,int **address,int *size);
/* ------------------------------------------------------------------------- */
/* Data access functions */
int updateFsList(int doGetDummyFs,int doGetRemoteFs);
int updateSuccessFlag(void);
int startGetFsUsage(fs_data_t *fs);
/* ------------------------------------------------------------------------- */
/* list management */
int fslAdd(char *name,char *mp,char *type);
fs_data_t fslRm(char *mp);
void fslPrint(void);
int fslNewId(void);
/* ************************************************************************* */
#endif                                                            /* _SERVER */
/* ######################################################################### */
#endif                                                          /* _FS_SRV_H */
/* ######################################################################### */
