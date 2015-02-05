#! /bin/bash

. /usr/share/Modules/init/bash

SANDBOX=$1
SELECTOR=$2
SOURCEBASE=$3
mytask=$SGE_TASK_ID
WD=$SANDBOX/job$mytask
echo Using working dir $WD
cd $WD

modulepaths=()
modules=()
nmodpaths=0
nmodules=0
while read line; do
	pathtest=$(echo $line | grep modulepaths:)
	moduletest=$(echo $line | grep modules:)
	if [ "x$pathtest" != "x" ]; then
		modulepaths[$nmodpaths]=$(echo $line | awk '{split($1, a, ":"); print a[2]}')
		let "nmodpaths++"
	fi
	if [ "x$moduletest" != "x" ]; then
		modules[$nmodules]=$(echo $line | awk '{split($1, a, ":"); print a[2]}')
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
for s in ${selectors[@]}; do cp $SELECTORLOCATION/$s $WD/; done

runcmd=$(printf "root -b -q \'runAnalysisLocal.C(\"files.txt\", \"%s.C\")\' &> analysis.log" $SELECTOR)
echo "Running root command: $runcmd"
eval $runcmd

# print log output
cat analysis.log

# validate result
rootstatus=1		# keeps the validation result
while read line; do
	testSegViol=$(echo $line | grep "Segmentation violation")
	testSegFault=$(echo $line | grep "Segmentation fault")
	testFloating=$(echo $line | grep "Floating point exception")
	testBadAlloc=$(echo $line | grep  "bad_alloc")
	
	if [ "x$testSegViol" != "x" ]; then
		rootstatus=0
		break
	fi
	
	if [ "x$testSegFault" != "x" ]; then
		rootstatus=0
		break
	fi
	
	if [ "x$testFloating" != "x" ]; then
		rootstatus=0
		break
	fi
	
	if [ "x$testBadAlloc" != "x" ]; then
		rootatatus=0
		break
	fi
done < analysis.log

if [ $rootstatus -eq 1 ]; then 
	echo "Result good"
	echo "output valid" >> output_valid 
fi
if [ $rootstatus -eq 0 ]; then 
	echo "Result bad"
	echo "output bad" >> output_bad
fi
#remove analysis.log since it is displayed inside the log file
rm analysis.log

#Remove temporary content
for f in ${inputfiles[@]}; do
  basein=`basename $f` 
  rm $WD/$basein
done
for s in ${selectors[@]}; do
  basein=`basename $s` 
  rm $WD/$basein
done
rm config

rm -rf *.d *.so
echo "Done"

