#!/bin/bash
now=`date +"%Y-%m-%d"`
echo "#ifndef _FMC_DATE_H" > ./include/fmcDate.h
echo "#define _FMC_DATE_H 1" >> ./include/fmcDate.h
echo "#define FMC_DATE \"$now\"" >> ./include/fmcDate.h
echo "#endif                          /* _FMC_DATE_H */" >> ./include/fmcDate.h

