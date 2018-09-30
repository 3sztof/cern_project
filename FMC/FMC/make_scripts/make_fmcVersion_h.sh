#!/bin/bash
# #############################################################################
# provide preprocessor include directives depending on FMC version
# #############################################################################
# strVersCmp()
# compare numerically 2 strings of the form a.b.c
# a - most significant
# c - least significant
# RETURN VALUE:
#   1  if  v1 > v2
#   0  if  v1 = v2
# 255  if  v1 < v2
# EXAMPLES:
#   strVersCmp 2.1.12 3.15.1 ($? = 255)
#   strVersCmp 2.1.12 2.1.12 ($? = 0)
#   strVersCmp 2.1.10 2.1.2 ($? = 1)
#   strVersCmp 2.3.4 2.4.3 ($? = 255)
# #############################################################################
strVersCmp()
{
  v1_1=`echo $1|cut -d "." -f 1`
  v1_2=`echo $1|cut -d "." -f 2`
  v1_3=`echo $1|cut -d "." -f 3`
  v2_1=`echo $2|cut -d "." -f 1`
  v2_2=`echo $2|cut -d "." -f 2`
  v2_3=`echo $2|cut -d "." -f 3`
  # compare first number
  if [[ $v1_1 -gt $v2_1 ]] ; then
    return 1
  elif [[ $v1_1 -lt $v2_1 ]] ; then
    return 255
  else
    # first number equal. Compare second number
    if [[ $v1_2 -gt $v2_2 ]] ; then
      return 1
    elif [[ $v1_2 -lt $v2_2 ]] ; then
      return 255
    else
      # second number equal. Compare third number
      if [[ $v1_3 -gt $v2_3 ]] ; then
        return 1
      elif [[ $v1_3 -lt $v2_3 ]] ; then
        return 255
      else
        return 0
      fi
    fi
  fi
}
# #############################################################################
if [ ! -f ./include/fmcVersion.h ] ; then
  touch ./include/fmcVersion.h
fi
grep -q $1 ./include/fmcVersion.h
if [ $? -ne 0 ] ; then               # different version found in fmcVersion.h
  echo "/* File generated automatically by make_fmcVersion_h.sh */" \
       > ./include/fmcVersion.h
  echo "#ifndef _FMC_VERSION_H" >> ./include/fmcVersion.h
  echo "#define _FMC_VERSION_H 1" >> ./include/fmcVersion.h
  echo "#define FMC_VERSION \"$1\"" >> ./include/fmcVersion.h 
  strVersCmp $1 3.9.0
  if [[ $? -eq 255 ]]; then                                        # $1 < 3.9.0
    echo "#define SVC_HEAD \"\"" >> ./include/fmcVersion.h 
  else                                                            # $1 >= 3.9.0
    echo "#define SVC_HEAD \"/FMC\"" >> ./include/fmcVersion.h 
  fi
  echo "#endif /* _FMC_VERSION_H */" >> ./include/fmcVersion.h
fi
# #############################################################################
