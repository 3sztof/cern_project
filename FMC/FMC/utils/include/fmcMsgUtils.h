/* ######################################################################### */
/*
 * $Log: fmcMsgUtils.h,v $
 * Revision 2.8  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 2.7  2009/01/29 11:06:37  galli
 * minor changes
 *
 * Revision 2.6  2008/10/24 21:23:54  galli
 * added enum CONG_PROOF, NO_DROP
 *
 * Revision 2.3  2007/08/14 06:44:54  galli
 * message size definition moved to header
 *
 * Revision 2.2  2007/08/09 13:00:29  galli
 * New functions mPrintf(), rPrintf(), mfPrintf(), rfPrintf()
 * Functions printM(), printMF(), printR() and printRF() now outdated
 * and deprecated.
 *
 * Revision 2.1  2006/12/14 10:11:59  galli
 * added VERBOSE severity level
 *
 * Revision 2.0  2006/10/10 10:28:48  galli
 * Suppressed log to a specific DIM service.
 * New functions loggerOpen(), loggerClose(), dfltLoggerOpen(),
 * dfltLoggerClose(), getDfltFifoFD(), printMF(), printRF().
 * Functions msgInit(), msgGetLogFD(), msgTerm(), msgSend() now outdated
 * and deprecated.
 *
 * Revision 1.6  2006/08/24 09:31:31  galli
 * msgInit() looses the first (useless) argument: char **argv
 * added prototype for dimLoggerInit()
 *
 * Revision 1.5  2006/08/22 06:46:29  galli
 * function printR() added
 * bug fixed in gMsgSend()
 *
 * Revision 1.4  2006/08/17 09:27:53  galli
 * function printM() added
 *
 * Revision 1.3  2004/10/21 14:06:05  galli
 * 3rd argument (srvName) added to msgInit()
 * DIMSVC logger added to msgInit()
 *
 * Revision 1.2  2004/10/19 13:37:59  galli
 * Added loggerType argument in msgInit()
 *
 * Revision 1.1  2004/10/12 14:44:43  galli
 * Initial revision
 *
 */
/* ######################################################################### */
#ifndef _FMC_MSG_UTILS_H
#define _FMC_MSG_UTILS_H 1
/* ######################################################################### */
#include <limits.h>                                              /* PATH_MAX */
/* ######################################################################### */
#ifdef __cplusplus
extern "C" {
#endif
/* ######################################################################### */
#define PATH_LEN (PATH_MAX-1)                                    /* now 4095 */
#define FIFO_MSG_SZ 4096  /* PIPE_BUF=4096 # bytes in atomic write to a pipe */
#define SYSLOG_MSG_SZ 990
#define MSG_SZ (FIFO_MSG_SZ*10)
#define MSG_HEAD_SZ 256
enum severity{OK=0,VERB,DEBUG,INFO,WARN,ERROR,FATAL};
enum outType{L_DIM=0x1,L_STD=0x2,L_SYS=0x4,L_STD_C=0x8};
enum loggerPolicy{CONG_PROOF=0,NO_DROP};
#define dfltFifoPath "/tmp/logSrv.fifo"
/*---------------------------------------------------------------------------*/
/* Public functions */
int loggerOpen(char *fifoPath,int noDrop,int errU,int errL);
void loggerClose(int fifoFD);
int dfltLoggerOpen(int openTryN,int noDrop,int errL,int infoL,int color);
int getDfltFifoFD(void);
void dfltLoggerClose(void);
int mPrintf(int out,int severity,const char* fName,int tryN,
            const char *format,...);
int rPrintf(int out,int tryN,const char *format,...);
int mfPrintf(int fifoFD,int severity,const char* fName,int tryN,
             const char *format,...);
int rfPrintf(int fifoFD,int tryN,const char *format,...);
char *getFmcMsgUtilsVersion();
/* Deprecated. Only for back-portability. Will be suppressed. */
#define printM(out,severity,fName,format,...) mPrintf(out,severity,fName,0,format,## __VA_ARGS__)
#define printMF(fifoFD,severity,fName,format, ...) mfPrintf(fifoFD,severity,fName,0,format,## __VA_ARGS__)
#define printR(out,format, ...) rPrintf(out,2,format,## __VA_ARGS__)
#define printRF(fifoFD,format, ...) rfPrintf(fifoFD,2,format,## __VA_ARGS__)
int msgInit(int loggerType,char *srvName);
int msgGetLogFD(void);
void msgTerm();
int msgSend(char *fName,int severity,char *msg);
/* ######################################################################### */
#ifdef __cplusplus
}
#endif                                                        /* __cplusplus */
/* ######################################################################### */
#endif                                                   /* _FMC_MSG_UTILS_H */
/* ######################################################################### */
