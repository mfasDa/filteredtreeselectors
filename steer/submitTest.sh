#! /bin/bash

inputdir=`readlink -f $1`
rootfilename=$2
treename=$3
outputbase=$4

sourcedirin=`dirname $0`
sourcedir=`readlink -f $sourcedirin`

files=($(find $inputdir -name root_archive.zip))
nfiles=0
ntest=0
if [ ! -d $outputbase ]; then mkdir -p $outputbase; fi
testbase=$(readlink -f $outputbase)
testdir=$testbase/job$ntest
mkdir -p $testdir
for f in ${files[@]}; do
	myfile=$(printf "%s#%s" $f $rootfilename)
	echo $myfile >> $testdir/files.txt
	let "nfiles++"
	if [ $nfiles -ge 100 ]; then 
		qcmd=$(printf "qsub -l projectio=1,gscratchio=1 -o %s/filetest.log -e %s/filetesterr.log %s/testFile.sh %s %s %s" $testbase $testbase $sourcedir $sourcedir $testdir $treename)
		eval $qcmd
		let "ntest++"
		testdir=$testbase/job$ntest
		mkdir -p $testdir
		nfiles=0
	fi
done
if [ $nfiles -gt 0 ]; then
	#submit last bunch
	qcmd=$(printf "qsub -l projectio=1,gscratchio=1 -o %s/filetest.log -e %s/filetesterr.log %s/testFile.sh %s %s %s" $testbase $testbase $sourcedir $sourcedir $testdir $treename)
	eval $qcmd
fi