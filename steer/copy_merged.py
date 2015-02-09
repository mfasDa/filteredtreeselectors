#! /usr/bin/env python 

import os, sys, shutil

def main():
    inputpath = os.path.abspath(sys.argv[1])
    outputpath = os.path.abspath(sys.argv[2])
    rootfile = sys.argv[3]
    
    if "~" in inputpath:
        inputpath = inputpath.replace("~", os.path.expanduser("~"))
    if "~" in outputpath:
        outputpath = outputpath.replace("~", os.path.expanduser("~"))
        
    print "Inputpath:   %s" %(inputpath)
    print "Outputpath:  %s" %(outputpath)
    print "ROOT file:   %s" %(rootfile)
    
    directories = os.listdir(inputpath)
    for od in directories:
        outputdir = os.path.join(outputpath, od)
        if not os.path.exists(outputdir):
            os.makedirs(outputdir, 0755)
        shutil.copy(os.path.join(inputpath, od, rootfile), outputdir)
        
if __name__ == "__main__":
    main()