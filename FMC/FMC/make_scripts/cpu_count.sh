#!/bin/sh
/bin/ls /dev/cpu/ |/bin/egrep "^[0-9]+"|/usr/bin/wc -l
