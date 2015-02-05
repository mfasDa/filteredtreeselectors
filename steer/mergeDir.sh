#! /bin/bash

. /usr/share/Modules/init/bash
module use /cvmfs/alice.cern.ch/x86_64-2.6-gnu-4.1.2/Modules/modulefiles
module load AliEn/v2-19-217
module load AliRoot/v5-03-Rev-30d

OD=$PWD
FILENAME=$1

listoffiles=($(find $OD -name $FILENAME | grep -v $OD/$FILENAME))
mergecommand="hadd -f $FILENAME"
for myfile in ${listoffiles[@]}; do
	mergecommand=$(printf "%s %s" "$mergecommad" $myfile)
done
eval $mergecommand