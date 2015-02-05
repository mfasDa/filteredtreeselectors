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
    		# exdlude files from merging when they are marked as bad
			mydirname=`dirname $f`
			if [ -f $mydirname/output_bad ]; then
				continue
			fi
    		cmd=$(printf "%s %s" "$cmd" $f)
    	done
    	eval $cmd
    	cd $OD
	fi
done

	