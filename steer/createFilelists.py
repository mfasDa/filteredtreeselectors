#! /usr/bin/env python 

import os, sys
from commands import getstatusoutput

def FindFiles(basedir, archivename):
    filestr = getstatusoutput("find %s --name %s" %(basedir, archivename))
    listoffiles = filestr.split("\n") 
    resultlist = []
    for f in listoffiles:
        f = f.lstrip()
        f = f.rstrip()
        resultlist.append(f)
    return resultlist
    
def main(basedir, pthardbin, outputpath):
    print "Basedir:     %s" %(basedir)
    print "Outputdir:   %s" %(outputdir)
    print "Pt-hard bin: %d" %(pthardbin)
    listoffiles = []
    for run in os.listdir(basedir):
        if not run.isdigit():
            continue
        if not os.path.isdir("%s/%s" %(basedir,run)):
            continue
        
        runbase = "%s/%s/%02d" %(basedir, run, pthardbin)
        foundfiles = FindFiles(runbase, "root_archive.zip")
        
        outputstream = open("%s/filelist_bin%d.txt" %(outputpath, pthardbin))
        for foundfile in foundfiles:
            outputstream.write("%s#AnalysisResults.root\n" %(foundfile))
        outputstream.close()


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2], sys.argv[3])