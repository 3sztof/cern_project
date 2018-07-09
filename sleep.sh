#!/bin/bash

b=$1

for(( a=1; a<=b; a++))
do
    clear
    tput cup 5 5
    date
    tput cup 6 5
    echo "Hostname : $(hostname)"
    sleep 1
done
