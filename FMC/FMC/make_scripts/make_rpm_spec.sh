#!/bin/sh
# -----------------------------------------------------------------------------
REV=$1
REL=$2
RH_REL=$3
ARCH=$4
# -----------------------------------------------------------------------------
specFileName=FMC-${REV}-${REL}.${RH_REL}.${ARCH}.spec
tarballName=FMC-${REV}-${REL}.${RH_REL}.${ARCH}.tar.gz
rm -f ${specFileName} || /bin/true
# -----------------------------------------------------------------------------
# generate spec files
cat "FMC.spec.in" | \
sed "s/Version:\ ___version___/Version:\ ${REV}/g" | \
sed "s/Release:\ ___release___/Release:\ ${REL}.${RH_REL}/g" | \
sed "s/Source0:\ ___source0___/Source0:\ ${tarballName}/g" | \
sed "s/BuildArch:\ ___buildarch___/BuildArch:\ ${ARCH}/g" | \
sed "s/Distribution:\ ___distribution___/Distribution:\ ${RH_REL}/g" \
> $specFileName
# -----------------------------------------------------------------------------
# append CHANGELOG.spec to spec files
ci -l -m\".\" -t-\".\" CHANGELOG.spec
echo "%changelog" >> $specFileName
cat CHANGELOG.spec >> $specFileName
echo Spec file name ${specFileName} written.
#let REL=REL+1
#echo $REL>release
# -----------------------------------------------------------------------------
