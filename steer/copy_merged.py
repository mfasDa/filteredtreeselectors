#! /usr/bin/env python 

import os, sys, shutil

def main():
    inputpath = sys.argv[1]
    outputpath = sys.argv[2]
    rootfile = sys.argv[3]
    
    directories = os.listdir(inputpath)
    for od in directories:
        outputdir = os.path.join(ouputpath, od)
        os.makedirs(outputdir)
        shutil.copy(os.path.join(inputpath, dir, rootfile), outputdir)
        
if __name__ == "__main__":
    main