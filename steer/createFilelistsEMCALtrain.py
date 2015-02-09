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

def CreateListForPtHardBin(basedir, outputpath):
    print "Basedir:     %s" %(basedir)
    print "Outputdir:   %s" %(outputpath)
    listoffiles = FindFiles(basedir, "root_archive.zip")

    if not os.path.exists(outputpath):
        os.makedirs(outputpath, 0755)
    outputstream = open(os.path.join(outputpath, "filelist_bin%d.txt" %(pthardbin)), 'w')
    for foundfile in sorted(listoffiles):
        outputstream.write("%s#AnalysisResults.root\n" %(foundfile))
    outputstream.close()
    
def main(basedir, outputpath):
    for ibin in range(1, 11):
        CreateListForPtHardBin(os.path.join(os.path.abspath(basedir), "bin%d", ibin),  outputpath)

if __name__ == "__main__":
    basedir = sys.argv[1]
    outputpath = os.path.join(basedir, "treelists")
    if len(sys.argv) > 2:
        outputpath = sys.argv[2]
    main(sys.argv[1], outputpath)