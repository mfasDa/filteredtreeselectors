#define SelectorParticleRdist_cxx
// The class definition in SelectorParticleRdist.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("SelectorParticleRdist.C")
// Root > T->Process("SelectorParticleRdist.C","some options")
// Root > T->Process("SelectorParticleRdist.C+")
//
#include <string>

#include "SelectorParticleRdist.h"
#include <TH2.h>
#include <TObjArray.h>
#include <TStyle.h>
#include <TLorentzVector.h>

#include "AliReducedJetInfo.h"
#include "AliReducedJetConstituent.h"
#include "AliReducedJetParticle.h"


void SelectorParticleRdist::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void SelectorParticleRdist::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

   hRgen = new TH1D("Rgenerated", "2-particle R-distribution at generation level; R; Entries", 200, 0., 1.);
   hRrec = new TH1D("Rreconstructed", "2-particle R-distribution at reconstruction level; R; Entries", 200, 0., 1.);
   hRrecPartial = new TH1D("RreconstructedPartial", "2-particle R-distribution at reconstruction level (only one of the particle reconstructed); R; Entries", 200, 0., 1.);
   hRnorec = new TH1D("RNonreconstructed", "2-particle R-distribution at reconstruction level (non of the two particles reconstructed); R; Entries", 200, 0., 1.);

   fOutput->Add(hRgen);
   fOutput->Add(hRrec);
   fOutput->Add(hRrecPartial);
   fOutput->Add(hRnorec);
}

Bool_t SelectorParticleRdist::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either SelectorParticleRdist::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.
   GetEntry(entry);

   double weight = JetEvent->GetCrossSection()/static_cast<double>(JetEvent->GetNumberOfTrials());

   for(TIter jetIter = TIter(JetEvent->GetListOfJets()).Begin(); jetIter != TIter::End(); ++jetIter){
   	HighPtTracks::AliReducedJetInfo *recjet = dynamic_cast<HighPtTracks::AliReducedJetInfo *>(*jetIter);
   	TObjArray *recParticles = recjet->GetListOfMatchedParticles();
   	for(int ipart = 0; ipart < recParticles->GetEntries()-1; ipart++){ // Only correlation with particles with ID > larger than own ID to avoid double counting
   		HighPtTracks::AliReducedJetParticle *mainpart = dynamic_cast<HighPtTracks::AliReducedJetParticle *>(recParticles->At(ipart));
   		TLorentzVector mainkine;
   		mainpart->FillLorentzVector(mainkine);
   		for(int jpart = ipart + 1; jpart < recParticles->GetEntries(); ++jpart){
   			HighPtTracks::AliReducedJetParticle *assocpart = dynamic_cast<HighPtTracks::AliReducedJetParticle *>(recParticles->At(jpart));
   			TLorentzVector assockine;
   			assocpart->FillLorentzVector(assockine);

   			double dr = mainkine.DeltaR(assockine);
   			hRgen->Fill(dr, weight);
   			if(mainpart->IsReconstructed() && assocpart->IsReconstructed())
   				hRrec->Fill(dr, weight);
   			else{
   				if(mainpart->IsReconstructed() || assocpart->IsReconstructed())
   					hRrecPartial->Fill(dr, weight);
   				else
   					hRnorec->Fill(dr, weight);
   			}
   		}
   	}
   }

   return kTRUE;
}

void SelectorParticleRdist::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void SelectorParticleRdist::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

	std::string histnames[] = {"Rgenerated", "Rreconstructed", "RreconstructedPartial", "RNonreconstructed"};
	TFile *out = new TFile("ParticleRdist.root", "RECREATE");
	out->cd();
	for(std::string *histiter = histnames; histiter < histnames + sizeof(histnames)/sizeof(std::string); ++histiter)
		fOutput->FindObject(histiter->c_str())->Write();
	delete out;
}
