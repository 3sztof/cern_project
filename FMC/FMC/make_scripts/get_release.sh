#!/bin/sh
if [ ! -f release ] ; then
  echo 0 > release
  echo 0;
else
  echo `cat release`
fi
