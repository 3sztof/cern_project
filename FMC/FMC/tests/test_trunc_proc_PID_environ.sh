#!/bin/sh
for i in $(/usr/bin/seq 4096) ; do
  export VAR$i=$i$i
done
envsize=$(/bin/env|/usr/bin/wc -c)
procEnvSize=$(/bin/cat /proc/self/environ|/usr/bin/wc -c)
krnRel=$(uname -r)
PATCH_NAME="\"Don-t-truncate-proc-PID-environ-at-4096-characters.patch\""
logger -t fmc -p local7.info "Testing /proc/<PID>/environ truncation in currently running Linux Kernel: \"${krnRel}\"..."
/opt/FMC/bin/mPrint -s INFO "Testing /proc/<PID>/environ truncation in currently running Linux Kernel: \"${krnRel}\"..."
logger -t fmc -p local7.info "Real environment size:    ${envsize}."
/opt/FMC/bin/mPrint -s INFO "Real environment size:    ${envsize}."
logger -t fmc -p local7.info "/proc/<PID>/environ size: ${procEnvSize}."
/opt/FMC/bin/mPrint -s INFO "/proc/<PID>/environ size: ${procEnvSize}."
if [ ${procEnvSize} -eq ${envsize} ] ; then
  logger -t fmc -p local7.info "No /proc/<PID>/environ truncation."
  /opt/FMC/bin/mPrint -s INFO "No /proc/<PID>/environ truncation."
  logger -t fmc -p local7.info "Patch ${PATCH_NAME} applied or inlined in currently running Linux Kernel: ${krnRel}."
  /opt/FMC/bin/mPrint -s INFO "Patch ${PATCH_NAME} applied or inlined in currently running Linux Kernel: ${krnRel}."
  exit 1
else
  logger -t fmc -p local7.info "/proc/<PID>/environ truncated at ${procEnvSize} Bytes!"
  /opt/FMC/bin/mPrint -s INFO "/proc/<PID>/environ truncated at ${procEnvSize} Bytes!"
  logger -t fmc -p local7.info "Patch ${PATCH_NAME} NEITHER applied NOR inlined in currently running Linux Kernel: ${krnRel}!"
  /opt/FMC/bin/mPrint -s WARN "Patch ${PATCH_NAME} NEITHER applied NOR inlined in currently running Linux Kernel: ${krnRel}!"
  exit 0
fi
