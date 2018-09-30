# /etc/profile.d/fmc
# #############################################################################
if ! echo ${PATH} | /bin/egrep -q "(^|:)/opt/FMC/bin($|:)" ; then
  PATH=${PATH}:/opt/FMC/bin
fi
if ! echo ${MANPATH} | grep -q "(^|:)/opt/FMC/man($|:)" ; then
  MANPATH=${MANPATH}:/opt/FMC/man
fi
# #############################################################################
