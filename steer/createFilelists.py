#! /usr/bin/env python 

import os, sys
from commands import getstatusoutput

def FindFiles(basedir, archivename):
    print "Finding all %s in %s" %(archivename, basedir)
    filestr = getstatusoutput("find %s -name %s" %(basedir, archivename))[1]
    listoffiles = filestr.split("\n") 
    resultlist = []
    for f in listoffiles:
        f = f.lstrip()
        f = f.rstrip()
        resultlist.append(f)
    return resultlist

def CreateListForPtHardBin(basedir, pthardbin, outputpath):
    print "Basedir:     %s" %(basedir)
    print "Outputdir:   %s" %(outputpath)
    print "Pt-hard bin: %d" %(pthardbin)
    listoffiles = []
    for run in os.listdir(basedir):
        if not run.isdigit():
            continue
        if not os.path.isdir("%s/%s" %(basedir,run)):
            continue
        
        runbase = "%s/%s/%d" %(basedir, run, pthardbin)
        foundfiles = FindFiles(runbase, "root_archive.zip")
        for foundfile in foundfiles:
            listoffiles.append(foundfile)
        
    if not os.path.exists(outputpath):
        os.makedirs(outputpath, 0755)
    outputstream = open(os.path.join(outputpath, "filelist_bin%d.txt" %(pthardbin)), 'w')
    for foundfile in sorted(listoffiles):
        outputstream.write("%s#AnalysisResults.root\n" %(foundfile))
    outputstream.close()
    
def main(basedir, outputpath):
    for ibin in range(1, 11):
        CreateListForPtHardBin(basedir, ibin, outputpath)

if __name__ == "__main__":
    basedir = sys.argv[1]
    outputpath = os.path.join(basedir, "treelists")
    if len(sys.argv) > 2:
        outputpath = sys.argv[2]
    main(sys.argv[1], outputpath)