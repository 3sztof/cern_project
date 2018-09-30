/* ######################################################################### */
/*
 * $Log: getFS.c,v $
 * Revision 2.4  2011/11/08 15:26:22  galli
 * Bug fixed (file size overrun on 32-bit machines)
 *
 * Revision 2.3  2011/11/07 09:25:50  galli
 * minor changes
 *
 * Revision 2.2  2011/11/03 08:49:37  galli
 * Rewritten from scratch.
 * 2 functions: readFs() and getFsUsage()
 *
 * Revision 1.11  2011/09/27 11:49:38  galli
 * Exclude a list of filesystems when onlyLocal flag is set
 *
 * Revision 1.9  2009/01/29 10:06:06  galli
 * minor changes
 *
 * Revision 1.7  2008/04/16 14:17:16  galli
 * bug fixed in popen stderr redirection
 *
 * Revision 1.5  2007/08/10 09:11:18  galli
 * added function getFSsensorVersion()
 *
 * Revision 1.4  2007/08/10 08:23:28  galli
 * compatible with libFMCutils v 2
 *
 * Revision 1.3  2006/10/23 21:37:58  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.2  2006/08/03 09:08:02  galli
 * store version number in the returned structure
 *
 * Revision 1.1  2006/08/02 14:23:59  galli
 * Initial revision
 * */
/* ######################################################################### */
/*
 * Linux File system types [see fs(5) fstab(5)]:
 * adfs, affs, autofs, cifs, coda, coherent, cramfs, debugfs, devpts, efs, ext,
 * ext2, ext3, hfs, hpfs, iso9660, jfs, minix, msdos, ncpfs, nfs, nfs4, ntfs,
 * proc, qnx4, ramfs, reiserfs, romfs, smbfs, sysv, tmpfs, udf, ufs, umsdos,
 * usbfs, vfat, xenix, xfs, xiafs
 * */
/* ######################################################################### */
/* headers */
#include <stdio.h>                                                   /* FILE */
#include <sys/wait.h>                                       /* WEXITSTATUS() */
#include <errno.h>                                                  /* errno */
#include <string.h>                                /* strerror(3), strcmp(3) */
#include <stdlib.h>                                   /* exit(3), strtoul(3) */
#define _GNU_SOURCE                                        /* getmntent_r(3) */
#include <mntent.h>                                        /* getmntent_r(3) */
#include <sys/vfs.h>                                            /* statfs(2) */
#include <pthread.h>       /* pthread_mutex_lock(3), pthread_mutex_unlock(3) */
/* ------------------------------------------------------------------------- */
/* in sensors/include */
#include "getFS.h"     /* fs_usage_t, fs_data_t, fs_arr_t, DUMMY(), REMOTE() */
/* ------------------------------------------------------------------------- */
/* in utils/include */
#include "fmcMsgUtils.h"                                        /* mPrintf() */
/* ------------------------------------------------------------------------- */
/* in include */
#include "fmcMacro.h"     /* eExit(), likely(), unlikely(), printOutFuncOK() */
/* ------------------------------------------------------------------------- */
/* Size 1025 is recommended by HP. Do not change it! */
#define MNT_BUF_LEN 1025            /* size of buffer used by getmntent_r(3) */
/* ------------------------------------------------------------------------- */
/* Test flags to simulate anomalous conditions. Must be commented off! */
//#define TEST_HUNG
//#define TEST_MOUNT
/* ######################################################################### */
/* global variables */
static char rcsid[]="$Id: getFS.c,v 2.4 2011/11/08 15:26:22 galli Exp galli $";
/* ------------------------------------------------------------------------- */
/* external global variables */
extern int deBug;                                              /* debug mask */
extern int errU;                                               /* error unit */
/* ######################################################################### */
/* readFs()                                                                  */
/*   Reads the list of mounted file systems from file /etc/mtab              */
/*     and store information in the array fsArr->fs.                         */
/*   The number of mounted file systems is stored in fsArr->fsN.             */
/*   doGetDummyFs:                                                           */
/*     0: exclude "dummy" file systems (proc, sysfs, devpts, etc.)           */
/*     1: include "dummy" file systems (proc, sysfs, devpts, etc.)           */
/*   doGetRemoteFs:                                                          */
/*     0: exclude remote file systems (nfs, afs, gpfs, etc.)                 */
/*     1: include remote file systems (nfs, afs, gpfs, etc.)                 */
/*   RETURN VALUE:                                                           */
/*     0: OK                                                                 */
/*    -1: Can't open file /etc/mtab                                          */
/*    -2: Can't open file /etc/mtab                                          */
/* ************************************************************************* */
int readFs(int doGetDummyFs,int doGetRemoteFs,fs_arr_t *fsArr)
{
  fs_data_t *fsdp;
  FILE *mnttabFp;
  struct mntent mntBuf;
  char buf[MNT_BUF_LEN];
  struct mntent *rv=NULL;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* ----------------------------------------------------------------------- */
  /* reset array */
  fsArr->fsN=0;
  fsArr->fs=(fs_data_t*)realloc(fsArr->fs,fsArr->fsN*sizeof(fs_data_t));
  /* ----------------------------------------------------------------------- */
  /* open file /etc/mtab */
  mnttabFp=setmntent(_PATH_MOUNTED,"r");
  if(!mnttabFp)
  {
    mPrintf(errU,FATAL,__func__,0,"setmntent(3): Can't open file: \"%s\"!",
            _PATH_MOUNTED);
    printOutFuncNOK;
    return -1;
  }
  /* ----------------------------------------------------------------------- */
  /* read file /etc/mtab */
  for(;(rv=getmntent_r(mnttabFp,&mntBuf,buf,MNT_BUF_LEN));)
  {
    if(!rv)
    {
      mPrintf(errU,FATAL,__func__,0,"getmntent_r(3): Can't read file: \"%s\"!",
              _PATH_MOUNTED);
      printOutFuncNOK;
      return -2;
    }
    if(!doGetDummyFs && DUMMY(mntBuf.mnt_type))continue;
    if(!doGetRemoteFs && REMOTE(mntBuf.mnt_type))continue;
    /* allocate space for new filesystem */
    fsArr->fsN++;
    fsArr->fs=(fs_data_t*)realloc(fsArr->fs,fsArr->fsN*sizeof(fs_data_t));
    fsdp=&fsArr->fs[fsArr->fsN-1];
    memset(fsdp,0,sizeof(fs_data_t));
    /* fill in filesystem attrs */
    fsdp->id=fsArr->fsN-1;
    snprintf(fsdp->name,NAME_LEN+1,"%s",mntBuf.mnt_fsname);
    snprintf(fsdp->mp,MP_LEN+1,"%s",mntBuf.mnt_dir);
    snprintf(fsdp->type,TYPE_LEN+1,"%s",mntBuf.mnt_type);
  }
#ifdef TEST_MOUNT
  {
    static int i=0;
    i++;
    if(i%10==5)
    {
      fsArr->fsN=1;
      fsArr->fs=(fs_data_t*)realloc(fsArr->fs,fsArr->fsN*sizeof(fs_data_t));
    }
    else if(i%10==9)
    {
      fsArr->fsN=0;
      fsArr->fs=(fs_data_t*)realloc(fsArr->fs,fsArr->fsN*sizeof(fs_data_t));
      fsArr->fs=NULL;
    }
  }
#endif
  /* ----------------------------------------------------------------------- */
  /* close file /etc/mtab */
  endmntent(mnttabFp);
  printOutFuncOK;
  return 0;
}
/* ************************************************************************* */
/* getFsUsage() runs in a short-living thread                                */
/* Gets the filesystem usage of filesystem fsv->name from the OS             */
/* and store it in fsv->usage                                                */
/* ************************************************************************* */
void *getFsUsage(void *fsv)
{
  struct statfs sbuf;
  uint64_t bs;
  int busy;
  int rv;
  fs_data_t *fs=(fs_data_t*)fsv;
  /* ----------------------------------------------------------------------- */
  printInFunc;
  /* synchronize busy flag */
  pthread_mutex_lock(fs->usageLock);
  busy=fs->usage.busy;
  pthread_mutex_unlock(fs->usageLock);
  /* do not continue if another istance of statfs is running on this fs */
  if(busy)
  {
    time_t now;
    /* set stalled flag */
    now=time(NULL);
    pthread_mutex_lock(fs->usageLock);
    fs->usage.stalled=now-fs->usage.lastUpdate;
    pthread_mutex_unlock(fs->usageLock);
    mPrintf(errU,WARN,__func__,0,"File system \"%s\" mounted on \"%s\" "
            "stalled (statfs(2) call not yet returned after %ld seconds)!",
            fs->name,fs->mp,fs->usage.stalled);
    printOutFuncNOK;
    pthread_exit(NULL);
  }
  else
  {
    /* unset stalled flag */
    pthread_mutex_lock(fs->usageLock);
    fs->usage.stalled=0;
    pthread_mutex_unlock(fs->usageLock);
  }
  /* ----------------------------------------------------------------------- */
  pthread_mutex_lock(fs->usageLock);
  fs->usage.busy=1;
  pthread_mutex_unlock(fs->usageLock);
  /* system call */
  rv=statfs(fs->mp,&sbuf);                                 /* may hangs here */
#ifdef TEST_HUNG
  /* simulate hung */
  if(!strcmp(fs->mp,"/"))
  {
    struct timespec delay;
    delay.tv_sec=187;
    delay.tv_nsec=0;
    nanosleep(&delay,NULL);
  }
#endif
  if(!rv)
  {
    pthread_mutex_lock(fs->usageLock);
    bs=sbuf.f_bsize/1024;
    fs->usage.totalRoot=bs*sbuf.f_blocks;
    fs->usage.availRoot=bs*sbuf.f_bfree;
    fs->usage.availUser=bs*sbuf.f_bavail;
    fs->usage.minfree=fs->usage.availRoot-fs->usage.availUser;
    fs->usage.used=fs->usage.totalRoot-fs->usage.availRoot;
    fs->usage.totalUser=fs->usage.availUser+fs->usage.used;
    fs->usage.lastUpdate=time(NULL);
    fs->usage.busy=0;
    pthread_mutex_unlock(fs->usageLock);
  }
  else                                                   /* statfs(2) failed */
  {
    pthread_mutex_lock(fs->usageLock);
    fs->usage.rv=1;
    fs->usage.err=errno;
    fs->usage.busy=0;
    pthread_mutex_unlock(fs->usageLock);
    mPrintf(errU,ERROR,__func__,0,"Can't get usage of file system \"%s\" "
            "mounted on \"%s\"! statfs(2) error: %s!",fs->name,fs->mp,
            strerror(errno));
  }
  printOutFuncOK;
  pthread_exit(NULL);
}
/* ************************************************************************* */
/* getFSsensorVersion() - Returns the RCS identifier of this file.           */
/* ************************************************************************* */
char *getFSsensorVersion()
{
  return rcsid;
}
/* ######################################################################### */
