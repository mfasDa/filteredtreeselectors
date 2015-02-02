#! /bin/bash

inputdir=$1
rootfilename=$2
treename=$3

sourcedirin=`dirname $0`
sourcedir=`readlink -f $sourcedirin`

files=($(find $inputdir -name root_archive.zip))
for f in ${files[@]}; do
	myfile=$(printf "%s#%s" $f $rootfilename)
	cmd=$(printf "qsub -o %s/filetest.log -e %s/filetesterr.log %s/testFile.sh %s %s %s %s" $inputdir $inputdir $sourcedir $sourcedir $myfile $treename $inputdir)
	eval $cmd
done