#! /usr/bin/env python

import os,sys
from getopt import getopt
from threading import Thread
from commands import getstatusoutput as shellInteract

class TransferStream(Thread):
    
    def __init__(self, alienbase, outputbase, outputfile):
        Thread.__init__(self)
        self.__filestotransfer = []
        self.__alienbase = alienbase
        self.__outputbase = outputbase
        self.__outputfile = outputfile
	
    def AddFile(self, myfile):
        self.__filestotransfer.append(myfile)
	
    def run(self):
        # Copy all files of my thread
        for myfile in self.__filestotransfer:
            outputdir = "%s/%s" %(self.__outputbase, self.__ExtractSubfolder(myfile, self.__alienbase, self.__outputfile))
            if os.path.exists("%s/%s" %(outputdir, self.__outputfile)):
                continue
            #print "Outputdir: %s" %(outputdir)
            if not os.path.exists(outputdir):
                os.makedirs(outputdir, 0755)
            self.__alien_copy_file("alien://%s" %(myfile), "%s/%s" %(outputdir, self.__outputfile))

    def __alien_copy_file(self, source, target):
        os.system("alien_cp %s %s &> /dev/null" %( source, target))

    def __ExtractSubfolder(self, alienfilename, alienbase, outputfile):
        mybase = alienbase.rstrip('/')
        result = alienfilename.replace(alienbase, "")
        result = result.replace(outputfile,"")
        # remove starting and tailing "/"
        result = result.lstrip('/')
        result = result.rstrip('/')
        return result
  

def alien_find(basedir, pattern):
    print "alien_find %s %s" %(basedir, pattern)
    files = shellInteract("alien_find %s %s" %(basedir, pattern))[1]
    filelist = files.split("\n")
    #print files
    result = []
    for found in filelist:
        #print found
        if not basedir in found:
            continue
        result.append(found)
    return result

def doTransfer(argc, argv):
    alienbase=argv[1]
    outputbase=argv[2]
    outputfile = "root_archive.zip"
    nstream = 5
    if argc > 3:
        optionals,ars = getopt(argv[3:], "r:n:")
        for o,a in optionals:
            if o == "-r":
                outputfile = str(a)
            if o == "-n":
                nstream = int(a)
 
    files = alien_find(alienbase, "*/%s" %(outputfile))
    
    # Start worker threads
    workers = []
    for i in range(0,nstream):
        workers.append(TransferStream(alienbase, outputbase, outputfile))

    # distriubte files to the workers
    currentworker = 0
    for myfile in files:
        workers[currentworker].AddFile(myfile)
        # find next worker
        if currentworker == nstream-1:
            currentworker = 0
        else:
            currentworker += 1

    # start workers
    for myworker in workers:
        myworker.start()
	
    # stop all workers when finished
    for myworker in workers:
        myworker.join()

if __name__ == "__main__":
    doTransfer(len(sys.argv), sys.argv)
