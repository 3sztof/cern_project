/* ######################################################################### */
/*
 * $Log: getFS.h,v $
 * Revision 2.3  2011/11/08 15:00:55  galli
 * Disk sizes in uint64_t instead of long
 * to fix a bug with 32-bit machines
 *
 * Revision 2.2  2011/11/07 09:26:30  galli
 * minor changes
 *
 * Revision 2.1  2011/11/03 08:52:30  galli
 * Rewritten from scratch.
 * 2 functions: readFs() and getFsUsage().
 * New data structures: fs_usage_t, fs_data_t and fs_arr_t.
 *
 * Revision 1.6  2009/01/29 10:06:06  galli
 * minor changes
 *
 * Revision 1.4  2007/08/10 09:11:42  galli
 * added function getFSsensorVersion()
 *
 * Revision 1.3  2006/10/23 21:38:19  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.2  2006/08/03 09:06:19  galli
 * added sensor version to fsAttrs structure
 *
 * Revision 1.1  2006/08/02 14:23:24  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _GET_FS_H
#define _GET_FS_H 1
/* ######################################################################### */
#include <inttypes.h>                                              /* PRIu64 */
#include <sys/types.h>
/* ------------------------------------------------------------------------- */
#define NAME_LEN 127    /* max length of a filesystem name (/dev/hda0, etc.) */
#define MP_LEN 127    /* max length of a filesystem mountpoint (/boot, etc.) */
#define TYPE_LEN 15          /* max length of a filesystem type (ext3, etc.) */
#define SENSORS_BUFFER_SIZE (10*128)
/* ------------------------------------------------------------------------- */
/* Macro to recognize dummy file systems */
#define DUMMY(fsType)                   \
    (strcmp(fsType,"autofs"     )==0 || \
     strcmp(fsType,"none"       )==0 || \
     strcmp(fsType,"proc"       )==0 || \
     strcmp(fsType,"sysfs"      )==0 || \
     strcmp(fsType,"devpts"     )==0 || \
     strcmp(fsType,"tmpfs"      )==0 || \
     strcmp(fsType,"usbfs"      )==0 || \
     strcmp(fsType,"binfmt_misc")==0 || \
     strcmp(fsType,"rpc_pipefs" )==0 || \
     strcmp(fsType,"nfsd"       )==0 || \
     strcmp(fsType,"subfs"      )==0 || \
     strcmp(fsType,"kernfs"     )==0 || \
     strcmp(fsType,"ignore"     )==0)
/* Macro to recognize remote file systems */
#define REMOTE(fsType)            \
    (strcmp(fsType,"nfs"  )==0 || \
     strcmp(fsType,"nfs4" )==0 || \
     strcmp(fsType,"afs"  )==0 || \
     strcmp(fsType,"gpfs" )==0 || \
     strcmp(fsType,"cifs" )==0 || \
     strcmp(fsType,"davfs")==0 || \
     strcmp(fsType,"smbfs")==0 || \
     strcmp(fsType,"cxfs" )==0 || \
     strcmp(fsType,"coda" )==0 || \
     strcmp(fsType,"sshfs")==0)
/* ************************************************************************* */
/* Data Structures */
/* ------------------------------------------------------------------------- */
/* Usage data for a single file system */
/* long size => max 2 TiB on 32-bit machines, 8 ZiB on 64-bit machines       */
typedef struct fs_usage
{
  int rv;
  int err;
  uint64_t minfree;
  uint64_t totalRoot;
  uint64_t totalUser;
  uint64_t used;
  uint64_t availRoot;
  uint64_t availUser;
  long busy;
  long stalled;
  time_t lastUpdate;
}fs_usage_t;
/* ------------------------------------------------------------------------- */
/* Complete data for a single file system */
typedef struct fs_data
{
  int id;
  char name[NAME_LEN+1];
  char mp[MP_LEN+1];
  char type[TYPE_LEN+1];
  fs_usage_t usage;
  struct fs_data *next;
  struct fs_data *prev;
  unsigned sumSvcId;
  unsigned detSvcId;
  pthread_mutex_t *usageLock;
}fs_data_t;
/* ------------------------------------------------------------------------- */
/* Array of file system data read from /etc/mtab */
typedef struct fs_arr
{
  int fsN;
  fs_data_t *fs;
}fs_arr_t;
/* ************************************************************************* */
/* Function prototype */
int readFs(int doGetDummyFs,int doGetRemoteFs,fs_arr_t *fsArr);
void *getFsUsage(void *fsv);
char *getFSsensorVersion(void);
/* ######################################################################### */
#endif
/* ######################################################################### */
