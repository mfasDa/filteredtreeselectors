#! /bin/bash

sourcedir=$1
workdir=$2
treetotest=$3

. /usr/share/Modules/init/bash
module use /cvmfs/alice.cern.ch/x86_64-2.6-gnu-4.1.2/Modules/modulefiles
module load AliPhysics/vAN-20150128
module load cgal/v4.4
module load fastjet/v3.0.6_1.012
module load AliEn/v2-19-217


while read line; do
	cmd=$(printf "root -b -q %s/LoadLibs.C \'%s/testfile.C(\"%s\",\"%s\")\' &> %s/test.log" $sourcedir $sourcedir $line $treetotest $workdir) 
	eval $cmd
	status=$?

	# analyse logfile
	logbad=0
	while read line; do
		badalloctest=$(echo $line | grep "std::bad_alloc")
		if [ "x$badalloctest" != "x" ]; then 
			# bad alloc thrown
			logbad=1
			break
		fi
	done < $workdir/test.log

	if [ $status -gt 1 -o $logbad -ne 0 ]; then
		echo $filetotest >> $workdir/badfiles.txt
	else
		echo $filetotest >> $workdir/goodfiles.txt
	fi
done < $workdir/files.txt