#! /usr/bin/env python

import os, sys, shutil
from getopt import getopt
from commands import getstatusoutput

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
    
class MergeParams(object):
    
    def __init__(self):
        self.__holdid = 0
        self.__basedir = ""
        self.__jobname = ""
        
    def SetHoldID(self, holdID):
        self.__holdid = holdID
        
    def GetHoldID(self):
        return self.__holdid
    
    def SetJobBasedir(self, jobbase):
        self.__basedir = jobbase
        
    def GetJobBasedir(self):
        return self.__basedir
    
    def SetJobName(self, name):
        self.__jobname = name
        
    def GetJobName(self):
        return self.__jobname
        

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
    joblist = []
    for myfile in inputfiles:
        mybase = myfile.replace("filelist_","")
        mybase = mybase.replace(".txt", "");

        subjobParams = CreateSubjobParams(jobparams, mybase)

        split("%s/%s" %(inputbase, myfile), subjobParams )
        CreateConfig(subjobParams, configfile)
        submitcommand = "qsub -l gscratchio=1,projectio=1 -t 1:%d -o %s/joboutput.log -e %s/joberror.log -wd %s %s %s %s %s" %(subjobParams.GetNChunk(), subjobParams.GetGlobalSandbox(), subjobParams.GetGlobalSandbox(), subjobParams.GetGlobalSandbox(), subjobParams.GetExecutable(), subjobParams.GetGlobalSandbox(), selector, sourcedir)
        print "submit command: %s" %(submitcommand)
        jobsubmitterout = getstatusoutput(submitcommand)[1]
        jobsubmitterout = jobsubmitterout.replace("Your job-array ","").replace(" has been submitted","")
        jobidstring = jobsubmitterout.split(" ")[0]
        jobids[mybase] = int(jobidstring.split(".")[0])
        mergeparams = MergeParams();
        mergeparams.SetHoldID(int(jobidstring.split(".")[0]))
        mergeparams.SetJobBasedir(subjobParams.GetGlobalSandbox())
        joblist.append(mergeparams)
        print "extracted job ID %d" %(mergeparams.GetHoldID())
        
    # submit merger, waiting for the jobs to finish
    mergeexecutable="%s/steer/mergeAuto.sh" %(sourcedir)
    for mergeparams in joblist:
        submitcommand = "qsub -l gscratchio=1,projectio=1 -hold_jid %d -o %s/mergeoutput.log -e %s/mergeerror.log -wd %s %s %s" %(mergeparams.GetHoldID(), mergeparams.GetJobBasedir(), mergeparams.GetJobBasedir(), mergeparams.GetJobBasedir(), mergeexecutable, mergeparams.GetJobBasedir()) 
        getstatusoutput(submitcommand)
        
if __name__ == "__main__":
    main()

