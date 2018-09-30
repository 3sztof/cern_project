#!/bin/bash
#CGIURL=https://lhcbweb.bo.infn.it/FMC/htmlMan/man\$section/\$title.\$section\$subsection.html
CGIURL=../../htmlMan/man\$section/\$title.\$section\$subsection.html
MAN2HTML="/opt/man2html3.0.1/man2html -compress -headmap scripts/man2html.map"
for i in `ls man/man*/*.?.gz` ; do
  if [ -L $i ] ; then
    continue
  fi
  coreName=`echo $i | cut -d '/' -f 2- | cut -d '.' -f -2`
  j=htmlMan/$coreName.html
  echo $i "=>" $j
  gunzip -c $i |\
  groff -t -e -mandoc -Tascii |\
  $MAN2HTML -pgsize 1300 -topm 0 -cgiurl $CGIURL > $j
done
