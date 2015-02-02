#! /bin/bash

. /usr/share/Modules/init/bash
module use /cvmfs/alice.cern.ch/x86_64-2.6-gnu-4.1.2/Modules/modulefiles
module load AliEn/v2-19-217
module load AliRoot/v5-03-Rev-30d

OD=$PWD
FILENAME=$1

pthardbins=($(ls -1 $OD))
for ihard in ${pthardbins[@]}; do
  bindir=$OD/$ihard
  if [ -d $bindir ]; then
    cd $bindir
    fls=($(find $PWD -name $FILENAME))
    cmd="hadd -f $FILENAME"
    for f in ${fls[@]}; do
      cmd=$(printf "%s %s" "$cmd" $f)
    done
    eval $cmd
    cd $OD
  fi
done

