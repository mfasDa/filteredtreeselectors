#! /usr/bin/env python 

import os,sys,shutil
import json as jsonhandler

class ModuleEnvironment(object):
    
    def __init__(self):
        self.__modulepath = []
        self.__modules = []
        
    def AddModule(self, mymod):
        self.__modules.append(mymod)
        
    def GetListOfModules(self):
        return self.__modules
    
    def AddModulePath(self, mypath):
        self.__modulepath.append(mypath)
    
    def GetListOfModulepaths(self):
        return self.__modulepath

class ConfigParser(object):
    
    def __init__(self, inputconf):
        self.__configuration = jsonhandler.load(inputconf)

    def GetValue(key):
        return self.__configuration[key]
    
def RunJob(selector, modenv):
    """
    Create light executable, run job
    """
    execwriter = open("executable.sh", "w")
    execwriter.write("#! /bin/bash\n")
    execwriter.write("/usr/share/Modules/init/bash\n")
    for mypath in modenv.GetListOfModulePaths():
        execwriter.write("module use %s\n" %(mypath))
    for mymod in modenv.GetListOfModules():
        execwriter.write("module load %s\n" %(mymod)) 
    execwriter.write("root -b -q \'runAnalysisLocal.C(\"files.txt\", \"%s\")\' &> analysis.log" %(selector))
    execwriter.close()
    os.chmod("execultable.sh", 0755)
    os.system("executable.sh")
    os.remove("executable.sh")
    
    
def CopyFiles(inputdir, filelist):
    for inputfile in filelist:
        shutil.copy(os.path.join(inputdir, inputfile), os.getcwd())
        
def Cleanup(filelist):
    for l in filelist:
        os.remove(l)

def __main__():
    configuration = ConfigParser(sys.argv[1])
    print "Job configuration: " %(sys.argv[1])
    jobid = os.environ["JOB_ID"]
    taskid = os.environ["SGE_TASK_ID"]
    
    # find workdir
    workdir = "%s/job%d" %(configuration.GetValue("sandbox"), taskid -1)
    os.chdir(workdir)
    
    # copy files to sandbox
    skeleton = ["runAnalysisLocal.C", "MakeChain.C", "LoadLibs.C"]
    selectorfiles = ["%s.C" %(configuration.GetValue("selector")),  "%s.h" %(configuration.GetValue("selector"))]
    CopyFiles("%s/steer" %(configuration.GetValue("sourcedir")), skeleton)
    CopyFiles("%s/source" %(configuration.GetValue("sourcedir")), configuration.GetValue("selector"))
    
    # prepare modules
    modenv = ModuleEnvironment()
    for mypath in configuration.GetValue("modulepath"):
        modenv.AddModulePath(mypath)
    for mymod in configuration.GetValue("module"):
        modenv.AddModule(mymod)
        
    # everything prepared - run
    RunJob(configuration.GetValue("selector"), modenv)
    
    # cleanup after done
    listforcleanup = []
    for entry in skeleton:
        listforcleanup.append(entry)
    for entry in selectorfiles:
        listforcleanup.append(entry)
    for entry in [".d", ".so"]:
        listforcleanup.append("%s%s" %(configuration.GetValue("selector"), entry))
    Cleanup(listforcleanup)
    
    # We are done!
    
if __name__ == "__main__":
    main()