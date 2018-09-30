#!/bin/sh
if [ ! -f /etc/redhat-release ] ; then
  rhRel=0;
else
  rhRel=`cat /etc/redhat-release`
fi
# -----------------------------------------------------------------------------
if   [[ $rhRel =~ "Scientific Linux CERN SLC release 6" ]] ; then
  distr=el6
elif [[ $rhRel =~ "CentOS release 6" ]] ; then
  distr=el6
elif [[ $rhRel =~ "Scientific Linux CERN SLC release 5" ]] ; then
  distr=el5
elif [[ $rhRel =~ "Scientific Linux SL release 5" ]] ; then
  distr=el5
elif [[ $rhRel =~ "Red Hat Enterprise Linux Server release 5" ]] ; then
  distr=el5
elif [[ $rhRel =~ "Scientific Linux CERN SLC release 4" ]] ; then
  distr=el4
elif [[ $rhRel =~ "Red Hat Enterprise Linux AS release 4" ]] ; then
  distr=el4
elif [[ $rhRel =~ "Red Hat Enterprise Linux ES release 3" ]] ; then
  distr=el3
elif [[ $rhRel =~ "CentOS Linux release 7.2.1511 (Core)" ]] ; then
  distr=el7
else
  distr=unknown
fi
# -----------------------------------------------------------------------------
echo $distr
# -----------------------------------------------------------------------------
