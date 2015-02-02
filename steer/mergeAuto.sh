#! /bin/bash

. /usr/share/Modules/init/bash
module use /cvmfs/alice.cern.ch/x86_64-2.6-gnu-4.1.2/Modules/modulefiles
module load AliEn/v2-19-217
module load AliRoot/v5-03-Rev-30d

OD=$1
cd $OD

fls=($(find $PWD -name *.root))
cmd="hadd -f"
counter=0
for f in ${fls[@]}; do
    if [ $counter -eq 0 ]; then
    	FILENAME=`basename $f`
    	cmd=$(printf "%s %s" "cmd" $FILENAME)
    fi
	cmd=$(printf "%s %s" "$cmd" $f)
done
eval $cmd