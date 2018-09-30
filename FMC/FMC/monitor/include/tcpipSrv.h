/* ######################################################################### */
/*
 * $Log: tcpipSrv.h,v $
 * Revision 1.2  2009/02/26 20:20:29  galli
 * minor changes
 *
 * Revision 1.1  2009/01/09 11:40:01  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _TCPIP_SRV_H
#define _TCPIP_SRV_H 1
/* ######################################################################### */
/* common */
/* DIM server basename */
#define SRV_NAME "tcpip"
/*---------------------------------------------------------------------------*/
/* server */
/* labels to be published */
char *rateNames[]={"InReceivesRate","InDeliversRate","ForwDatagramsRate",
                   "OutRequestsRate","ReasmReqdsRate","FragReqdsRate",
                   "InSegsRate","OutSegsRate","InDatagramsRate",
                   "OutDatagramsRate"};
char *ratioNames[]={"InHdrErrorsFrac","InAddrErrorsFrac",
                    "InUnknownProtosFrac","InDiscardsFrac",
                    "ForwDatagramsFrac","InDeliversFrac","ReasmReqdsFrac",
                    "ReasmTimeoutFrac","ReasmFailsFrac","ReasmOKsFrac",
                    "OutDiscardsFrac","OutNoRoutesFrac","FragReqdsFrac",
                    "FragFailsFrac","FragCreatesFrac","RetransSegsFrac",
                    "OutRstsFrac","InErrsFrac","NoPortsFrac","InErrorsFrac"};
/*---------------------------------------------------------------------------*/
/* client */
/* number of fields which have to be set to "N/F" if server unreachable */
/* ######################################################################### */
#endif                                                       /* _TCPIP_SRV_H */
/* ######################################################################### */
