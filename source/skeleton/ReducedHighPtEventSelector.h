//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri May 22 16:46:14 2015 by ROOT version 5.34/28
// from TTree ReducedEvent/A simple reduced event
// found on file: LHC13b/000195483/0010/root_archive.zip#ReducedHighPtEvent.root
//////////////////////////////////////////////////////////

#ifndef ReducedHighPtEventSelector_h
#define ReducedHighPtEventSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.
#include "/home/markus/alisoft/aliphysics/mfaselDev/src/PWGJE/EMCALJetTasks/Tracks/AliReducedHighPtEvent.h"

// Fixed size dimensions of array or collections stored in the TTree if any.

class ReducedHighPtEventSelector : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   HighPtTracks::AliReducedHighPtEvent *ReducedEvent;

   // List of branches
   TBranch        *b_ReducedEvent;   //!

   ReducedHighPtEventSelector(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~ReducedHighPtEventSelector() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(ReducedHighPtEventSelector,0);
};

#endif

#ifdef ReducedHighPtEventSelector_cxx
void ReducedHighPtEventSelector::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   ReducedEvent = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("ReducedEvent", &ReducedEvent, &b_ReducedEvent);
}

Bool_t ReducedHighPtEventSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef ReducedHighPtEventSelector_cxx
