#! /usr/bin/env python 
import os
from commands import getstatusoutput

def FindAllRootFiles(inputdir):
    return getstatusoutput("find $PWD -name *.root")[1].split("\n")

def ListRootFiles(allinputs):
    result = []
    for infile in allinputs:
        myfile = os.path.basename(infile)
        if not infile in result:
            result.append(infile)
    return result

def MergeFiles(rootfile, listoffiles):
    filestomerge = []
    for myfile in listoffiles:
        if rootfile in myfile:
            filestomerge.append(myfile)
            
    submitcommand = "hadd %s" %(rootfile)
    for myfile in filestomerge:
        submitcommand += " %s" %(myfile)
    os.system(submitcommand)

def main():
    inputdir = sys.argv[1]
    os.chdir(inputdir)
    os.system(". /usr/share/Modules/init/bash")
    os.system("module use /cvmfs/alice.cern.ch/x86_64-2.6-gnu-4.1.2/Modules/modulefiles")
    os.system("module load AliEn/v2-19-217")    
    os.system("module load AliRoot/v5-03-Rev-30d")
    
    listall = FildAllRootfile(inputdir)
    listoutputs = ListRootFiles(listall)
    
    for myoutput in listoutputs:
        print "Merging %s" %(myoutput)
        MergeFiles(myoutput, listall)


if __name__ == "__main__":
    main()