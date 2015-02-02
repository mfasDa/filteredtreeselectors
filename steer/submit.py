#! /usr/bin/env python

import os, sys, shutil
from getopt import getopt
from CGIHTTPServer import executable

class Jobparams(object):

    def __init__(self):
        self.__executable = ""
        self.__globalsandbox = ""
        self.__nchunk = 0
        self.__chunksize = 0

    def SetExecutable(self, exe):
        self.__executable = exe

    def SetGlobalSandbox(self, sand):
        self.__globalsandbox = sand
        if not os.path.exists(self.__globalsandbox):
            os.makedirs(self.__globalsandbox, 0755)

    def SetNChunk(self, nch):
        self.__nchunk = nch

    def SetChunksize(self, nin):
        self.__chunksize = nin

    def GetExecutable(self):
        return self.__executable

    def GetGlobalSandbox(self):
        return self.__globalsandbox

    def GetNChunk(self):
        return self.__nchunk

    def GetChunksize(self):
        return self.__chunksize

def split(filelist, jobparams):
    chunksize = jobparams.GetChunksize()
    currentchunk = 0
    ncurrent = 0
    infile = open(filelist, 'r')
    fbuffer= []
    for inf in infile:
        fbuffer.append(inf.rstrip())
        ncurrent += 1
        if ncurrent == chunksize:
            chunkdir = "%s/job%d" %(jobparams.GetGlobalSandbox(), currentchunk)
            os.makedirs(chunkdir, 0755)
            chunkfile = open("%s/files.txt" %(chunkdir), 'w')
            for entry in fbuffer:
                chunkfile.write("%s\n" %(entry))
            chunkfile.close()
            fbuffer = []
            currentchunk += 1
            ncurrent = 0
    if ncurrent != 0:
        # last chunk
        chunkdir = "%s/job%d" %(jobparams.GetGlobalSandbox(), currentchunk)
        os.makedirs(chunkdir, 0755)
        chunkfile = open("%s/files.txt" %(chunkdir), 'w')
        for entry in fbuffer:
            chunkfile.write("%s\n" %(entry))
        chunkfile.close()
        currentchunk += 1
    jobparams.SetNChunk(currentchunk)

def CreateSubjobParams(jobparams, subjobOut):
    jobParamsNew = Jobparams()
    jobParamsNew.SetGlobalSandbox("%s/%s" %(jobparams.GetGlobalSandbox(), subjobOut))
    jobParamsNew.SetExecutable(jobparams.GetExecutable())
    jobParamsNew.SetNChunk(jobparams.GetNChunk())
    jobParamsNew.SetChunksize(jobparams.GetChunksize())
    return jobParamsNew

def CreateConfig(subjobparams, configfile):
    for i in range(0, subjobparams.GetNChunk()):
        taskdir = os.path.join(subjobparams.GetGlobalSandbox(), "job%d" %(i))
        if not os.path.exists(taskdir):
            os.makedirs(taskdir, 0755)
        shutil.copy(configfile, os.path.join(taskdir, "config"))

def main():
    opt,arg = getopt(sys.argv[1:], "i:o:c:s:m:")
    sourcedir = os.path.abspath(os.path.dirname(sys.argv[0]))
    sourcedir = sourcedir.replace("/steer","")

    jobparams = Jobparams()
    jobparams.SetExecutable(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "run.sh"))
    inputbase = ""
    chunksize = 20
    selector = ""
    configfile = ""
    for o,a in opt:
        if o == "-o":
            jobparams.SetGlobalSandbox(a)
        elif o == "-c":
            chunksize = int(a)
        elif o == "-i":
            inputbase = a
        elif o == "-s":
            selector = a
        elif o == "-m":
            configfile = os.path.abspath(a)

    jobparams.SetChunksize(chunksize)

    inputfiles = os.listdir(inputbase)
    for myfile in inputfiles:
        mybase = myfile.replace("filelist_","")
        mybase = mybase.replace(".txt", "");

        subjobParams = CreateSubjobParams(jobparams, mybase)

        split("%s/%s" %(inputbase, myfile), subjobParams )
        CreateConfig(subjobParams, configfile)
        submitcommand = "qsub -l gscratchio=1,projectio=1 -t 1:%d -wd %s %s %s %s %s" %(subjobParams.GetNChunk(), subjobParams.GetGlobalSandbox(), subjobParams.GetExecutable(), subjobParams.GetGlobalSandbox(), selector, sourcedir)
        print "submit command: %s" %(submitcommand)
        os.system(submitcommand)

if __name__ == "__main__":
    main()

