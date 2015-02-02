#! /bin/bash

sourcedir=$1
filetotest=$2
treetotest=$3
outputdir=$4

module use /cvmfs/alice.cern.ch/x86_64-2.6-gnu-4.1.2/Modules/modulefiles
module load AliPhysics/vAN-20150128
module load cgal/v4.4
module load fastjet/v3.0.6_1.012
module load AliEn/v2-19-217

cmd=$(printf "root -l %s/LoadLibs.C \'%s/testfile.C(\"%s\","%s\")\'" $sourcedir $sourcedir $filetotest $treetotest) 
eval $cmd
status=$?

while [ -f $outputdir/lock ]; do
	sleep 5;
done
# set lock
touch $outputdir/lock
if [ $status -ne 0 ]; then
	echo $filetotest >> $outputdir/badfiles.txt
else
echo $filetotest >> $outputdir/goodfiles.txt
fi
# release lock
rm $outputdir/lock