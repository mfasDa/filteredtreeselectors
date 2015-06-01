#! /usr/bin/env python
import os, sys, commands, getopt

if __name__ == "__main__":
    sys.path.append("%s:%s" %(os.getenv("PYTHONPATH"), os.path.basename(sys.argv[0])))
    #os.setenv("PYTHONPATH", "%s:%s" %(os.getenv("PYTHONPATH"), os.path.basename(sys.argv[0])))

from copyDerivedData import doTransfer

class RunStruct(object):
    
    def __init__(self, run, path, pthardbin = -1):
        self.__run = run
        self.__path = path
        self.__pthardbin = pthardbin

    def GetRun(self):
        return self.__run

    def GetPath(self):
        return self.__path

    def GetPtHardBin(self):
        return self.__pthardbin

def alien_ls(inputpath):
    return commands.getstatusoutput("alien_ls %s" %(inputpath))[1].split("\n")

def FindRuns(inputdir,trainID, recpass = None):
    runlist = alien_ls(inputdir)
    datalist = []
    for r in runlist:
        print "Doing %s" %r
        if recpass and "pthard" in recpass:
            pthardbins = alien_ls("%s/%s" %(inputdir, r))
            for ib in pthardbins:
                passdir = "%s/%s/%s/PWGJE/Jets_EMC_pPb" %(inputdir, r, ib)
                #print "Passdir %s" %passdir
                trainlist = alien_ls(passdir)
                for train in trainlist:
                    #print "train %s" %(train)
                    if train == trainID:
                        print "requested train ID %s found for run %s in pt-hard bin %s" %(trainID, r, ib)
                        datalist.append(RunStruct(r, "%s/%s" %(passdir, trainID), ib))
        else:
            passdir = "%s/%s/%s/PWGJE/Jets_EMC_pPb" %(inputdir, r, recpass) if recpass else "%s/%s/PWGJE/Jets_EMC_pPb" %(inputdir, r)
            print "Passdir %s" %passdir
            trainlist = alien_ls(passdir)
            for train in trainlist:
                print "train %s" %(train)
                if train == trainID:
                    print "requested train ID %s found for run %s" %(trainID, r)
                    datalist.append(RunStruct(r, "%s/%s" %(passdir, trainID)))
    return datalist

def RunTransfer(inputdir, outputbase, trainID, recpass = "", rootfile = ""):
    print "Copying derived data from train %s from folder %s into %s" %(trainID, inputdir, outputbase)
    if not os.path.exists(outputbase):
        os.makedirs(outputbase, 0755)
    runlist = FindRuns(inputdir, trainID, recpass if len(recpass) else None)
    for r in runlist:
        targetdir = os.path.join(outputbase, r.GetRun())
        if "pthard" in recpass:
            targetdir = os.path.join(targetdir, r.GetPtHardBin())
        if not os.path.exists(targetdir):
            os.makedirs(targetdir, 0755)

        argv = [os.path.basename(sys.argv[0]),r.GetPath(), targetdir]
        if len(rootfile):
            argv.append("-r")
            argv.append(rootfile)
        doTransfer(len(argv), argv)

if __name__ == "__main__":
    rpass = None
    rootfile = None
    opt,arg = getopt.getopt(sys.argv[4:],"r:p:")
    for o,a in opt:
        if o == "-r":
            rootfile = str(a)
        elif o == "-p":
            print "Pass %s" %str(a)
            rpass = str(a)
    if not rpass and not rootfile:
        RunTransfer(sys.argv[1], sys.argv[2], sys.argv[3])
    elif rpass and not rootfile:
        RunTransfer(sys.argv[1], sys.argv[2], sys.argv[3], recpass = rpass)
    elif rootfile and not rpass:
        RunTransfer(sys.argv[1], sys.argv[2], sys.argv[3], rootfile = rootfile)
    else:
        RunTransfer(sys.argv[1], sys.argv[2], sys.argv[3], recpass = rpass, rootfile = rootfile)