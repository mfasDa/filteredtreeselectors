//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jan 30 06:15:28 2015 by ROOT version 5.34/08
// from TTree JetEvent/A tree with jet information
// found on file: root_archive.zip#AnalysisResults.root
//////////////////////////////////////////////////////////

#ifndef ParticleDeltaRSelector_h
#define ParticleDeltaRSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.
#include "AliReducedJetEvent.h"

// Fixed size dimensions of array or collections stored in the TTree if any.

namespace EMCalTriggerPtAnalysis{
class AliEMCalHistoContainer;
}

namespace HighPtTracks{
class AliReducedJetParticle;
}

class ParticleDeltaRSelector : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   HighPtTracks::AliReducedJetEvent *JetEvent;

   // List of branches
   TBranch        *b_JetEvent;   //!

   // Histogram handling
   EMCalTriggerPtAnalysis::AliEMCalHistoContainer 	*fHistos;

   ParticleDeltaRSelector(TTree * /*tree*/ =0) : fChain(0), fHistos(0) { }
   virtual ~ParticleDeltaRSelector() { }
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

private:
   HighPtTracks::AliReducedJetParticle *GetLeadingParticle(const HighPtTracks::AliReducedJetInfo *recjet) const;
   HighPtTracks::AliReducedJetParticle *GetSubleadingParticle(const HighPtTracks::AliReducedJetInfo *recjet) const;
   HighPtTracks::AliReducedJetParticle *GetNearestNeighbor(const HighPtTracks::AliReducedJetParticle *inputparticle, HighPtTracks::AliReducedJetInfo *recjet, double minpt = 0) const;
   void GetJetPtHistLimits(double jetpt, double &xmin, double &xmax) const;

   ClassDef(ParticleDeltaRSelector,0);
};

#endif

#ifdef ParticleDeltaRSelector_cxx
void ParticleDeltaRSelector::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   JetEvent = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("JetEvent", &JetEvent, &b_JetEvent);
}

Bool_t ParticleDeltaRSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef ParticleDeltaRSelector_cxx
