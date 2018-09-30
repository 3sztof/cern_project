#!/bin/sh
if [ -f FILES.versions ] ; then
  rm -f FILES.versions
fi
echo > FILES.versions
echo "Alphabetical sorted list " >> FILES.versions
find . -name '*.[cC]'|xargs grep 'rcsid\[\]='|cut -d"=" -f 2|\
cut -d" " -f 2-6 | sort -df >> FILES.versions
echo >> FILES.versions
echo "Time sorted list " >> FILES.versions
find . -name '*.[cC]'|xargs grep 'rcsid\[\]='|cut -d"=" -f 2|\
cut -d" " -f 2-6 | sort -k3,4 >> FILES.versions

