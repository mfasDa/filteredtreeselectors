#! /bin/bash

. /usr/share/Modules/init/bash

module use /project/projectdirs/alice/mfasel/modulefiles
module load mfasel/root/v5-34-25

SANDBOX=$1
mytask=$SGE_TASK_ID
let "mytask--"
WD=$SANDBOX/job$mytask
echo Using working dir $WD
cd $WD

SOURCELOCATION=/project/projectdirs/alice/mfasel/RAATrigger/trackInJetRecoEff/source
inputfiles=(runAnalysisLocal.C MakeChain.C ParticlesInJetSelector.C  ParticlesInJetSelector.h)

for f in ${inputfiles[@]}; do cp $SOURCELOCATION/$f $WD/; done

root -b -q 'runAnalysisLocal.C("files.txt", "ParticlesInJetSelector.C")'

for f in ${inputfiles[@]}; do
  basein=`basename $f` 
  rm $WD/$basein
done
rm -rf *.d *.so
echo "Done"

