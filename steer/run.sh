#! /bin/bash

. /usr/share/Modules/init/bash

SANDBOX=$1
SELECTOR=$2
SOURCEBASE=$3
mytask=$SGE_TASK_ID
let "mytask--"
WD=$SANDBOX/job$mytask
echo Using working dir $WD
cd $WD

modulepaths=()
modules=()
nmodpaths=0
nmodules=0
while read line; do
	paththest=$(echo $line | grep modulepaths:)
	moduletest=$(echo $line | grep modules:)
	if [ "x$pathtest" != "$x" ]; then
		modulepaths[$nmodpaths]=$(echo $line | awk '{split($1, a, ":"); print a[1]}')
		let "nmodpaths++"
	fi
	if [ "x$moduletest" != "$x" ]; then
		modules[$nmodules]=$(echo $line | awk '{split($1, a, ":"); print a[1]}')
		let "nmodules++"
	fi
done < $WD/config

echo "Using modulespaths:"
echo "==========================="
for mymodpath in ${modulepaths[@]}; do
	echo $mymodpath
	cmd=$(printf "module use %s" $mymodpath)
	eval $cmd
done
echo "Using modules:"
echo "==========================="
for mymod in ${modules[@]}; do
	echo $mymod
	cmd=$(printf "module load %s" $mymod)
	eval $cmd
done


SOURCELOCATION=$SOURCEBASE/steer
SELECTORLOCATION=$SOURCEBASE/source
inputfiles=(runAnalysisLocal.C MakeChain.C LoadLibs.C)
selectors=($SELECTOR.C $SELECTOR.h)

for f in ${inputfiles[@]}; do cp $SOURCELOCATION/$f $WD/; done
for s in ${selectors[@]}; do cp $SELECTORLOCATION

runcmd=$(print "root -b -q \'runAnalysisLocal.C(\"files.txt\", \"%s.C\")\'" $SELECTOR)
eval $runcmd

#Remove temporary content
for f in ${inputfiles[@]}; do
  basein=`basename $f` 
  rm $WD/$basein
done
for s in ${selectors[@]}; do
  basein=`basename $s` 
  rm $WD/$basein
done

rm -rf *.d *.so
echo "Done"

