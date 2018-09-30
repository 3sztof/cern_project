/* ######################################################################### */
/*
 * $Log: fmcSigUtils.c,v $
 * Revision 1.3  2008/10/23 07:48:27  galli
 * minor changes
 *
 * Revision 1.1  2008/10/09 07:00:49  galli
 * Initial revision
 */
/* ######################################################################### */
static char rcsid[]="$Id: fmcSigUtils.c,v 1.3 2008/10/23 07:48:27 galli Exp $";
/* ######################################################################### */
const char *sig2msg(int signo)
{
  const char *msg;
  static const char *sigName[64]={
    /*  0 */ "SIG 0: Unknown signal 0",
    /*  1 */ "SIGHUP: Hangup",      
    /*  2 */ "SIGINT: Interrupt",
    /*  3 */ "SIGQUIT: Quit",
    /*  4 */ "SIGILL: Illegal instruction",
    /*  5 */ "SIGTRAP: Trace/breakpoint trap",
    /*  6 */ "SIGABRT: Aborted",
    /*  7 */ "SIGBUS: Bus error",
    /*  8 */ "SIGFPE: Floating point exception",
    /*  9 */ "SIGKILL: Killed",
    /* 10 */ "SIGUSR1: User defined signal 1",
    /* 11 */ "SIGSEGV: Segmentation fault",
    /* 12 */ "SIGUSR2: User defined signal 2",
    /* 13 */ "SIGPIPE: Broken pipe",
    /* 14 */ "SIGALRM: Alarm clock",
    /* 15 */ "SIGTERM: Terminated",
    /* 16 */ "SIGSTKFLT: Stack fault",
    /* 17 */ "SIGCHLD: Child exited",
    /* 18 */ "SIGCONT: Continued",
    /* 19 */ "SIGSTOP: Stopped [signal]",
    /* 20 */ "SIGTSTP: Stopped",
    /* 21 */ "SIGTTIN: Stopped [tty input]",
    /* 22 */ "SIGTTOU: Stopped [tty output]",
    /* 23 */ "SIGURG: Urgent I/O condition",
    /* 24 */ "SIGXCPU: CPU time limit exceeded",
    /* 25 */ "SIGXFSZ: File size limit exceeded",
    /* 26 */ "SIGVTALRM: Virtual timer expired",
    /* 27 */ "SIGPROF: Profiling timer expired",
    /* 28 */ "SIGWINCH: Window changed",
    /* 29 */ "SIGIO: I/O possible",
    /* 30 */ "SIGPWR: Power failure",
    /* 31 */ "SIGSYS: Bad system call",
    /* 32 */ "SIG 32: Unknown signal 32",
    /* 33 */ "SIG 33: Unknown signal 33",
    /* 34 */ "SIGRTMIN: Real-time signal 0",
    /* 35 */ "SIGRTMIN+1: Real-time signal 1",
    /* 36 */ "SIGRTMIN+2: Real-time signal 2",
    /* 37 */ "SIGRTMIN+3: Real-time signal 3",
    /* 38 */ "SIGRTMIN+4: Real-time signal 4",
    /* 39 */ "SIGRTMIN+5: Real-time signal 5",
    /* 40 */ "SIGRTMIN+6: Real-time signal 6",
    /* 41 */ "SIGRTMIN+7: Real-time signal 7",
    /* 42 */ "SIGRTMIN+8: Real-time signal 8",
    /* 43 */ "SIGRTMIN+9: Real-time signal 9",
    /* 44 */ "SIGRTMIN+10: Real-time signal 10",
    /* 45 */ "SIGRTMIN+11: Real-time signal 11",
    /* 46 */ "SIGRTMIN+12: Real-time signal 12",
    /* 47 */ "SIGRTMIN+13: Real-time signal 13",
    /* 48 */ "SIGRTMIN+14: Real-time signal 14",
    /* 49 */ "SIGRTMIN+15: Real-time signal 15",
    /* 50 */ "SIGRTMIN+16: Real-time signal 16",
    /* 51 */ "SIGRTMIN+17: Real-time signal 17",
    /* 52 */ "SIGRTMIN+18: Real-time signal 18",
    /* 53 */ "SIGRTMIN+19: Real-time signal 19",
    /* 54 */ "SIGRTMIN+20: Real-time signal 20",
    /* 55 */ "SIGRTMIN+21: Real-time signal 21",
    /* 56 */ "SIGRTMIN+22: Real-time signal 22",
    /* 57 */ "SIGRTMIN+23: Real-time signal 23",
    /* 58 */ "SIGRTMIN+24: Real-time signal 24",
    /* 59 */ "SIGRTMIN+25: Real-time signal 25",
    /* 60 */ "SIGRTMIN+26: Real-time signal 26",
    /* 61 */ "SIGRTMIN+27: Real-time signal 27",
    /* 62 */ "SIGRTMIN+28: Real-time signal 28",
    /* 63 */ "SIGRTMIN+29: Real-time signal 29"
  };
  /*-------------------------------------------------------------------------*/
  if(signo>0 && signo<64)msg=(const char*)sigName[signo];
  else msg=(const char*)"Unknown signal";
  return msg;
}
/*****************************************************************************/
char *getFmcSigUtilsVersion()
{
  return rcsid;
}
/* ######################################################################### */
