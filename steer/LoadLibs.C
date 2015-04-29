/*
 * LoadLibs.C
 *
 *  Created on: Jan 30, 2015
 *      Author: markus
 */
#if !defined __CINT__ || defined __MAKECINT__
#include <TSystem.h>
#endif

void LoadLibs(){
        gSystem->Load("libPWGEMCAL");
        gSystem->Load("libCGAL");
        gSystem->Load("libfastjet");
        gSystem->Load("libsiscone");
        gSystem->Load("libsiscone_spherical");
        gSystem->Load("libfastjetplugins");
        gSystem->Load("libfastjettools");
        gSystem->Load("libfastjetcontribfragile");
        gSystem->Load("libPWGJEEMCALJetTasks");

        // Add include paths (fastjet and ALICE_PHYSICS):
        gSystem->AddIncludePath(Form("-I%s/include", gSystem->Getenv("ALICE_PHYSICS")));
        gSystem->AddIncludePath(Form("-I%s/include", gSystem->Getenv("FASTJET")));
}
