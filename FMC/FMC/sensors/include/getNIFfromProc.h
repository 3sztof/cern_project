/*****************************************************************************/
/*
 * $Log: getNIFfromProc.h,v $
 * Revision 1.12  2009/01/29 09:56:23  galli
 * minor changes
 *
 * Revision 1.9  2007/12/20 13:22:53  galli
 * compatible with getNIFfromProc.c 1.20
 *
 * Revision 1.8  2007/08/10 13:45:39  galli
 * added function getNIFsensorVersion()
 *
 * Revision 1.7  2004/11/26 09:20:13  galli
 * added total ethernet rates
 *
 * Revision 1.6  2004/11/17 00:05:23  galli
 * time since started added to nifsData structure
 *
 * Revision 1.5  2004/11/03 22:35:33  galli
 * added version to data structure
 *
 * Revision 1.4  2004/09/12 23:37:58  galli
 * evaluate also max rates and ratios
 * info function
 *
 * Revision 1.3  2004/09/11 23:38:43  galli
 * added 64-bit counters
 * modified rates, added ratios, average rates and average ratios
 *
 * Revision 1.1  2004/08/08 17:29:17  galli
 * Initial revision
 */
/*****************************************************************************/
#define IF_NAME_LEN 63                    /* max length of an interface name */
#define PROC_NET_DEV_SIZE 2048      /* max size of /proc/net/dev pseudo-file */
/*****************************************************************************/
/* i386   ULONG_MAX  = 4294967295 UL                                         */
/* x86_64 ULONG_MAX  = 18446744073709551615 UL                               */
/* all    ULLONG_MAX = 18446744073709551615 ULL                              */
/*****************************************************************************/
/* 16 unsigned long system counters (4 bytes on i386, 8 bytes on x86_64) */
struct counts
{
  unsigned long rx_bytes;
  unsigned long rx_packets;
  unsigned long rx_errors;
  unsigned long rx_dropped;
  unsigned long rx_fifo_errors;
  unsigned long rx_frame_errors;
  unsigned long rx_compressed;
  unsigned long rx_multicast;
  unsigned long tx_bytes;
  unsigned long tx_packets;
  unsigned long tx_errors;
  unsigned long tx_dropped;
  unsigned long tx_fifo_errors;
  unsigned long collisions;
  unsigned long tx_carrier_errors;
  unsigned long tx_compressed;
};
/*---------------------------------------------------------------------------*/
struct counts64
{
  unsigned long long rx_bytes;
  unsigned long long rx_packets;
  unsigned long long rx_errors;
  unsigned long long rx_dropped;
  unsigned long long rx_fifo_errors;
  unsigned long long rx_frame_errors;
  unsigned long long rx_compressed;
  unsigned long long rx_multicast;
  unsigned long long tx_bytes;
  unsigned long long tx_packets;
  unsigned long long tx_errors;
  unsigned long long tx_dropped;
  unsigned long long tx_fifo_errors;
  unsigned long long collisions;
  unsigned long long tx_carrier_errors;
  unsigned long long tx_compressed;
};
/*---------------------------------------------------------------------------*/
struct rates
{
  float rxBitRate;
  float rxFrameRate;
  float rxMulticastRate;
  float rxCompressedRate;
  float txBitRate;
  float txFrameRate;
  float txCompressedRate;
};
/*---------------------------------------------------------------------------*/
struct ratios
{
  float rxBytePerFrame;                               /* rx_bytes/rx_packets */
  float rxErrorFrac;                                 /* rx_errors/rx_packets */
  float rxDroppedFrac;                              /* rx_dropped/rx_packets */
  float rxFifoErrorFrac;                        /* rx_fifo_errors/rx_packets */
  float rxFrameErrorFrac;                      /* rx_frame_errors/rx_packets */
  float txBytePerFrame;                               /* tx_bytes/tx_packets */
  float txErrorFrac;                                 /* tx_errors/tx_packets */
  float txDroppedFrac;                              /* tx_dropped/tx_packets */
  float txFifoErrorFrac;                        /* tx_fifo_errors/tx_packets */
  float collisionsFrac;                             /* collisions/tx_packets */
  float txCarrierErrorFrac;                  /* tx_carrier_errors/tx_packets */
};
/*---------------------------------------------------------------------------*/
/* data of only one NIF */
struct nifData
{
  char *name;          /* interface name (e.g.: lo, eth0, eth1, bond0, etc.) */
  char *ipAddress;        /* interface ip address in dotted decimal notation */
  char *hwAddress;                                       /* hardware address */
  char *busAddress;                                     /* bus (PCI) address */
  struct counts cnt;                        /* unsigned long system counters */
  struct rates  rate;                                           /* counter/s */
  struct ratios ratio;                                /* counter_a/counter_b */
  struct counts64 tcnt;           /* 64-bit counters zeroed at process start */
  struct rates  arate;                                      /* average rates */
  struct ratios aratio;                                    /* average ratios */
  struct rates  mrate;                                      /* average rates */
  struct ratios mratio;                                    /* average ratios */
};
/*---------------------------------------------------------------------------*/
/* structure returned by readNIF(): data of all the NIFs */
typedef struct nifsData
{
  int success;             /* 0 if readNIF() failure, 1 if readNIF() success */
  char *version;                              /* version of getNIFfromProc.c */
  char *stss;                              /* time since start or last reset */
  int nifN;                                                /* number of NIFs */
  struct nifData *data;                                   /* data of one NIF */
  float totalEthRx_bitRate;
  float totalEthRx_packetsRate;
  float totalEthTx_bitRate;
  float totalEthTx_packetsRate;
}nifsData_t;
/*****************************************************************************/
nifsData_t readNIF(int deBug,int errU);
void resetNIF();
char *nifInfo(char *what);
char *getNIFsensorVersion();
/*****************************************************************************/
