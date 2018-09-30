/* ######################################################################### */
/*
 * $Log: irqSrv.h,v $
 * Revision 1.3  2009/01/29 08:13:21  galli
 * minor changes
 *
 * Revision 1.1  2009/01/08 08:31:37  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _IRQ_SRV_H
#define _IRQ_SRV_H 1
/* ######################################################################### */
/* common */
/* DIM server basename */
#define SRV_NAME "IRQ"
/*---------------------------------------------------------------------------*/
/* server */
/* maximim length of the name of a IRQ source (e.g.: timer, ide0, ehci_hcd) */
#define DEVICE_LEN 127
#define NUM_LEN 20              /* ULONG_MAX = 18446744073709551615 (64 bit) */
#define RATE_LEN 9                                              /* 0.000e+00 */
/* labels to be published */
/*---------------------------------------------------------------------------*/
/* client */
/* number of fields which have to be set to "N/F" if server unreachable */
#define FIELD_N_ERR 5
/* ######################################################################### */
#endif                                                         /* _IRQ_SRV_H */
/* ######################################################################### */
