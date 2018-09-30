#!/bin/sh
MAIN_DIR=/home/galli/prog/FMC-3.6
${MAIN_DIR}/sbin32/logSrv -p /tmp/logger_1.fifo -s logger_1 &
${MAIN_DIR}/sbin32/logSrv -p /tmp/logger_2.fifo -s logger_2 &
${MAIN_DIR}/bin32/logViewer -b -s /*/logger_1/log -o /tmp/logger_2.fifo &
