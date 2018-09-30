/* ######################################################################### */
/*
 * $Log: nifSrv.h,v $
 * Revision 1.3  2011/06/13 13:43:43  galli
 * Now publishes total counters (64bit) for the nifSrv (additional to rates) - David.G.Svantesson@cern.ch
 *
 * Revision 1.2  2009/01/29 09:04:38  galli
 * minor changes
 *
 * Revision 1.1  2009/01/08 13:08:19  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _NIF_SRV_H
#define _NIF_SRV_H 1
/* ######################################################################### */
/* common */
/* DIM server basename */
#define SRV_NAME "net/ifs"
/*---------------------------------------------------------------------------*/
/* server */
/* maximim length of the name of a NIF (e.g.: net_00, net_01, ...) */
#define DEVICE_LEN 15
/* default ethtool path */
#define DFLT_ETHTOOL_PATH "/usr/sbin/ethtool"
/* labels to be published */
char *sumLabelV[8]={"rx bit rate","tx bit rate","rx total error frac",
                    "tx total error frac","IF name","BUS address","MAC address",
                    "IP address"};
char *sumUnitsV[8]={"b/s","b/s","","","","","",""};
char *detLabelV[27]={"rx bit rate","rx frame rate","rx multicast rate",
                     "rx compressed rate","tx bit rate","tx frame rate",
                     "tx compressed rate","rx frame size",
                     "rx error frac","rx dropped frac","rx fifo error frac",
                     "rx frame error frac","tx frame size",
                     "tx error frac","tx dropped frac","tx fifo error frac",
                     "collision frac","tx carrier error frac","IF name",
                     "BUS address","MAC address","IP address","Speed","Duplex",
                     "Port type","Auto-negotiation","Link detected"};
char *detUnitsV[27]={"b/s","frames/s","frames/s","frames/s","b/s","frames/s",
                     "frames/s","B","","","","","B","","","","",
                     "","","","","","Mb/s","","","",""};
char *dettotLabelV[25]={"rx bytes","rx packets","rx errors",
                     "rx dropped","rx fifo errors","rx frame errors",
                     "rx compressed","rx multicast",
                     "tx bytes","tx packets","tx errors",
                     "tx dropped","tx fifo errors",
                     "collisions","tx carrier errors","tx compressed",
                     "IF name","BUS address","MAC address","IP address","Speed","Duplex",
                     "Port type","Auto-negotiation","Link detected"};
char *dettotUnitsV[25]={"B","frames","frames","frames","","frames",
                     "frames","frames","B","frames","frames","frames","","","","frames",
                     "","","","","Mb/s","","","",""};
char *ethLabelV[4]={"rx bit rate","rx frame rate","tx bit rate",
                    "tx frame rate"};
char *ethUnitsV[4]={"b/s","frames/s","b/s","frames/s"};
/*---------------------------------------------------------------------------*/
/* client */
/* number of fields which have to be set to "N/F" if server unreachable */
#define DEV_FIELD_N_ERR 1
#define DET_FLOAT_FIELD_N_ERR 18
#define DET_STR_FIELD_N_ERR 9
#define ETH_FIELD_N_ERR 4
/* ######################################################################### */
#endif                                                         /* _NIF_SRV_H */
/* ######################################################################### */
