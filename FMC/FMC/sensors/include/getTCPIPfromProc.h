/*****************************************************************************/
/* 
 * $Log: getTCPIPfromProc.h,v $
 * Revision 1.14  2008/10/09 09:50:01  galli
 * minor changes
 *
 * Revision 1.13  2007/08/10 14:07:33  galli
 * added function getTCPIPsensorVersion()
 *
 * Revision 1.12  2006/10/23 21:01:55  galli
 * compatible with libFMCutils v 1.0.
 *
 * Revision 1.1  2005/08/28 09:07:31  galli
 * Initial revision
 *
 * Revision 1.11  2004/11/17 12:39:03  galli
 * time since started added to tcpipData structure
 *
 * Revision 1.10  2004/11/03 22:33:41  galli
 * added version to data structure
 *
 * Revision 1.9  2004/09/13 14:14:34  galli
 * added maximum rates and maximum ratios
 *
 * Revision 1.8  2004/09/05 16:04:58  galli
 * tcpipInfo() function added
 *
 * Revision 1.7  2004/09/05 09:47:44  galli
 * added 64-bit counters
 * modified rates, added ratios, average rates and average ratios
 *
 * Revision 1.1  2004/08/06 07:31:23  galli
 * Initial revision
 *  */
/*****************************************************************************/
/* 32-bit ip system counters */
struct ipCnts
{
  unsigned Forwarding;
  unsigned DefaultTTL;
  unsigned InReceives;
  unsigned InHdrErrors;
  unsigned InAddrErrors;
  unsigned ForwDatagrams;
  unsigned InUnknownProtos;
  unsigned InDiscards;
  unsigned InDelivers;
  unsigned OutRequests;
  unsigned OutDiscards;
  unsigned OutNoRoutes;
  unsigned ReasmTimeout;
  unsigned ReasmReqds;
  unsigned ReasmOKs;
  unsigned ReasmFails;
  unsigned FragOKs;
  unsigned FragFails;
  unsigned FragCreates;
};
/*---------------------------------------------------------------------------*/
/* 32-bit icmp system counters */
struct icmpCnts
{
  unsigned InMsgs;
  unsigned InErrors;
  unsigned InDestUnreachs;
  unsigned InTimeExcds;
  unsigned InParmProbs;
  unsigned InSrcQuenchs;
  unsigned InRedirects;
  unsigned InEchos;
  unsigned InEchoReps;
  unsigned InTimestamps;
  unsigned InTimestampReps;
  unsigned InAddrMasks;
  unsigned InAddrMaskReps;
  unsigned OutMsgs;
  unsigned OutErrors;
  unsigned OutDestUnreachs;
  unsigned OutTimeExcds;
  unsigned OutParmProbs;
  unsigned OutSrcQuenchs;
  unsigned OutRedirects;
  unsigned OutEchos;
  unsigned OutEchoReps;
  unsigned OutTimestamps;
  unsigned OutTimestampReps;
  unsigned OutAddrMasks;
  unsigned OutAddrMaskReps;
};
/*---------------------------------------------------------------------------*/
/* 32-bit tcp system counters */
struct tcpCnts
{
  unsigned RtoAlgorithm;
  unsigned RtoMin;
  unsigned RtoMax;
  unsigned MaxConn;
  unsigned ActiveOpens;
  unsigned PassiveOpens;
  unsigned AttemptFails;
  unsigned EstabResets;
  unsigned CurrEstab;
  unsigned InSegs;
  unsigned OutSegs;
  unsigned RetransSegs;
  unsigned InErrs;
  unsigned OutRsts;
};
/*---------------------------------------------------------------------------*/
/* 32-bit udp system counters */
struct udpCnts
{
  unsigned InDatagrams;
  unsigned NoPorts;
  unsigned InErrors;
  unsigned OutDatagrams;
};
/*---------------------------------------------------------------------------*/
/* 64-bit ip counters, 0 at process start */
struct tIpCnts
{
  unsigned long long Forwarding;
  unsigned long long DefaultTTL;
  unsigned long long InReceives;
  unsigned long long InHdrErrors;
  unsigned long long InAddrErrors;
  unsigned long long ForwDatagrams;
  unsigned long long InUnknownProtos;
  unsigned long long InDiscards;
  unsigned long long InDelivers;
  unsigned long long OutRequests;
  unsigned long long OutDiscards;
  unsigned long long OutNoRoutes;
  unsigned long long ReasmTimeout;
  unsigned long long ReasmReqds;
  unsigned long long ReasmOKs;
  unsigned long long ReasmFails;
  unsigned long long FragOKs;
  unsigned long long FragFails;
  unsigned long long FragCreates;
};
/*---------------------------------------------------------------------------*/
/* 64-bit icmp counters, 0 at process start */
struct tIcmpCnts
{
  unsigned long long InMsgs;
  unsigned long long InErrors;
  unsigned long long InDestUnreachs;
  unsigned long long InTimeExcds;
  unsigned long long InParmProbs;
  unsigned long long InSrcQuenchs;
  unsigned long long InRedirects;
  unsigned long long InEchos;
  unsigned long long InEchoReps;
  unsigned long long InTimestamps;
  unsigned long long InTimestampReps;
  unsigned long long InAddrMasks;
  unsigned long long InAddrMaskReps;
  unsigned long long OutMsgs;
  unsigned long long OutErrors;
  unsigned long long OutDestUnreachs;
  unsigned long long OutTimeExcds;
  unsigned long long OutParmProbs;
  unsigned long long OutSrcQuenchs;
  unsigned long long OutRedirects;
  unsigned long long OutEchos;
  unsigned long long OutEchoReps;
  unsigned long long OutTimestamps;
  unsigned long long OutTimestampReps;
  unsigned long long OutAddrMasks;
  unsigned long long OutAddrMaskReps;
};
/*---------------------------------------------------------------------------*/
/* 64-bit tcp counters, 0 at process start */
struct tTcpCnts
{
  unsigned long long RtoAlgorithm;
  unsigned long long RtoMin;
  unsigned long long RtoMax;
  unsigned long long MaxConn;
  unsigned long long ActiveOpens;
  unsigned long long PassiveOpens;
  unsigned long long AttemptFails;
  unsigned long long EstabResets;
  unsigned long long CurrEstab;
  unsigned long long InSegs;
  unsigned long long OutSegs;
  unsigned long long RetransSegs;
  unsigned long long InErrs;
  unsigned long long OutRsts;
};
/*---------------------------------------------------------------------------*/
/* 64-bit udp counters, 0 at process start */
struct tUdpCnts
{
  unsigned long long InDatagrams;
  unsigned long long NoPorts;
  unsigned long long InErrors;
  unsigned long long OutDatagrams;
};
/*---------------------------------------------------------------------------*/
struct ipRates
{
  float InReceivesRate;
  float InDeliversRate;
  float ForwDatagramsRate;
  float OutRequestsRate;
  float ReasmReqdsRate;
  float FragReqdsRate;
};
/*---------------------------------------------------------------------------*/
struct tcpRates
{
  float InSegsRate;
  float OutSegsRate;
};
/*---------------------------------------------------------------------------*/
struct udpRates
{
  float InDatagramsRate;
  float OutDatagramsRate;
};
/*---------------------------------------------------------------------------*/
struct ipRatios
{
  float InHdrErrorsFrac;
  float InAddrErrorsFrac;
  float InUnknownProtosFrac;
  float InDiscardsFrac;
  float ForwDatagramsFrac;
  float InDeliversFrac;
  float ReasmReqdsFrac;
  float ReasmTimeoutFrac;
  float ReasmFailsFrac;
  float ReasmOKsFrac;
  float OutDiscardsFrac;
  float OutNoRoutesFrac;
  float FragReqdsFrac;
  float FragFailsFrac;
  float FragCreatesFrac;
};
/*---------------------------------------------------------------------------*/
struct tcpRatios
{
  float RetransSegsFrac;
  float OutRstsFrac;
  float InErrsFrac;
};
/*---------------------------------------------------------------------------*/
struct udpRatios
{
  float NoPortsFrac;
  float InErrorsFrac;
};
/*****************************************************************************/
/* 32-bit system counters */
struct counts
{
  struct ipCnts   ip;
  struct icmpCnts icmp;
  struct tcpCnts  tcp;
  struct udpCnts  udp;
};
/*---------------------------------------------------------------------------*/
/* 64-bit counters, 0 at process start */
struct tCounts
{
  struct tIpCnts   ip;
  struct tIcmpCnts icmp;
  struct tTcpCnts  tcp;
  struct tUdpCnts  udp;
};
/*---------------------------------------------------------------------------*/
struct rates
{
  struct ipRates  ip;
  struct tcpRates tcp;
  struct udpRates udp;
};
/*---------------------------------------------------------------------------*/
struct ratios
{
  struct ipRatios  ip;
  struct tcpRatios tcp;
  struct udpRatios udp;
};
/*****************************************************************************/
struct tcpipData
{
  int success;               /* 1 if this struct is meaningfull, 0 otherwise */
  char *version;
  char *stss;                              /* time since start or last reset */
  struct counts cnt;                               /* 32-bit system counters */
  struct tCounts tcnt;              /* 64-bit counters zero at process start */
  struct rates  rate;                                           /* counter/s */
  struct ratios ratio;                                /* counter_a/counter_b */
  struct rates  arate;                                      /* average rates */
  struct ratios aratio;                                    /* average ratios */
  struct rates  mrate;                                      /* average rates */
  struct ratios mratio;                                    /* average ratios */
};
/*---------------------------------------------------------------------------*/
struct tcpipData readTCPIP(int deBug,int errU);
void resetTCPIP();
char *getTCPIPsensorVersion(void);
/*****************************************************************************/
