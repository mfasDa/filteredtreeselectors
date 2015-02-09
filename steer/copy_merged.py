#! /usr/bin/env python 

import os, sys, shutil

def main():
    inputpath = sys.argv[1]
    outputpath = sys.argv[2]
    rootfile = sys.argv[3]
    
    if "~" in inputhpath:
        inputpath = inputpath.replace("~", os.path.expanduser("~"))
    if "~" in outputpath:
        outputpath = outputpath.replace("~", os.path.expanduser("~"))
    
    directories = os.listdir(os.readlink(inputpath))
    for od in directories:
        outputdir = os.path.join(ouputpath, od)
        if not os.path.exists(outputdir):
            os.makedirs(outputdir, 0755)
        shutil.copy(os.path.join(inputpath, dir, rootfile), outputdir)
        
if __name__ == "__main__":
    main