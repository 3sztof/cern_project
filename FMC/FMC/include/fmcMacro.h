/* ######################################################################### */
/*
 * $Log: fmcMacro.H,v $
 * Revision 1.2  2008/10/14 11:56:22  galli
 * first working version
 *
 * Revision 1.1  2008/09/11 09:11:38  galli
 * Initial revision
 */
/* ######################################################################### */
#ifndef _CPP_FMC_MACRO_H
#define _CPP_FMC_MACRO_H 1
/* ######################################################################### */
/* macro to produce string class:function */
#define METHOD ((string(__FILE__).substr(string(__FILE__).rfind('/')+1,\
                 string(__FILE__).rfind('.')-string(__FILE__).rfind('/')-1)+\
                 string("::")+string(__func__)).c_str())
/* ######################################################################### */
#endif                                                   /* _CPP_FMC_MACRO_H */
/* ######################################################################### */
