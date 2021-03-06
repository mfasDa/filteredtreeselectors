#define TrackPtSelector_cxx
// The class definition in TrackPtSelector.h has been generated automatically
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
// Root > T->Process("TrackPtSelector.C")
// Root > T->Process("TrackPtSelector.C","some options")
// Root > T->Process("TrackPtSelector.C+")
//

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <TArrayD.h>
#include <TH2.h>
#include <TMath.h>
#include <TStyle.h>

#include "TrackPtSelector.h"
#include "THistManager.h"
#include "AliReducedPatchInfo.h"
#include "AliReducedPatchContainer.h"
#include "AliReducedEmcalCluster.h"
#include "AliReducedReconstructedTrack.h"


void TrackPtSelector::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void TrackPtSelector::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

   fHistos = new THistManager("TrackPtHistos");

   TArrayD ptBinning;
   GetDefaultPtBinning(ptBinning);

   const int kNtriggers = 5;
   std::string triggernames[kNtriggers] = {"MinBias", "EMCGHigh", "EMCGLow", "EMCJHigh", "EMCJLow"};
   for(std::string *trigger = triggernames; trigger < triggernames + kNtriggers; ++trigger){
   	std::cout << "Creating histogram for trigger " << *trigger << std::endl;
   	fHistos->CreateTH1(Form("hEvents%s", trigger->c_str()), Form("Event count for trigger class %s", trigger->c_str()), 1, 0.5, 1.5);
   	fHistos->CreateTH1(Form("hEnergy%s", trigger->c_str()), Form("Cluster energy for trigger class %s", trigger->c_str()), ptBinning);
   	fHistos->CreateTH1(Form("hPt%s", trigger->c_str()), Form("Track pt for trigger class %s", trigger->c_str()), ptBinning);
   	if(strcmp(trigger->c_str(), "MinBias"))
   		fHistos->CreateTH1(Form("hPatchAmplitude%s", trigger->c_str()), Form("Patch amplitude for %s patches", trigger->c_str()), 1000, 0., 1000.);
   }

   fOutput->Add(fHistos);
}

Bool_t TrackPtSelector::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either TrackPtSelector::GetEntry() or TBranch::GetEntry()
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

	enum{
		kDefaultCuts = 0,
		kHybridCuts = 1
	};

	if(TMath::Abs(ReducedEvent->GetVertexZ()) > 10) return kFALSE;

	std::vector<std::string> triggerstrings;
	if(ReducedEvent->IsMinBias()) triggerstrings.push_back("MinBias");
	if(ReducedEvent->IsJetHighFromString()) triggerstrings.push_back("EMCJHigh");
	if(ReducedEvent->IsJetLowFromString()) triggerstrings.push_back("EMCJLow");
	if(ReducedEvent->IsGammaHighFromString()) triggerstrings.push_back("EMCGHigh");
	if(ReducedEvent->IsGammaLowFromString()) triggerstrings.push_back("EMCGLow");
	if(!triggerstrings.size()) return kFALSE; 		// event not selected

	for(std::vector<std::string>::iterator trigger = triggerstrings.begin(); trigger != triggerstrings.end(); ++trigger)
		fHistos->FillTH1(Form("hEvents%s", trigger->c_str()), 1);

	HighPtTracks::AliReducedPatchContainer *triggerpatches = ReducedEvent->GetPatchContainer();
	for(TIter patchIter = TIter(triggerpatches->GetTriggerPatches(kFALSE, HighPtTracks::AliReducedPatchContainer::kEMCJetHigh)).Begin(); patchIter != TIter::End(); ++patchIter){
		HighPtTracks::AliReducedPatchInfo *patch = dynamic_cast<HighPtTracks::AliReducedPatchInfo *>(*patchIter);
		if(patch) fHistos->FillTH1("hPatchAmplitudeEMCJHigh", patch->GetAmplitude());
	}
	for(TIter patchIter = TIter(triggerpatches->GetTriggerPatches(kFALSE, HighPtTracks::AliReducedPatchContainer::kEMCJetLow)).Begin(); patchIter != TIter::End(); ++patchIter){
		HighPtTracks::AliReducedPatchInfo *patch = dynamic_cast<HighPtTracks::AliReducedPatchInfo *>(*patchIter);
		if(patch) fHistos->FillTH1("hPatchAmplitudeEMCJLow", patch->GetAmplitude());
	}
	for(TIter patchIter = TIter(triggerpatches->GetTriggerPatches(kFALSE, HighPtTracks::AliReducedPatchContainer::kEMCGammaHigh)).Begin(); patchIter != TIter::End(); ++patchIter){
		HighPtTracks::AliReducedPatchInfo *patch = dynamic_cast<HighPtTracks::AliReducedPatchInfo *>(*patchIter);
		if(patch) fHistos->FillTH1("hPatchAmplitudeEMCGHigh", patch->GetAmplitude());
	}
	for(TIter patchIter = TIter(triggerpatches->GetTriggerPatches(kFALSE, HighPtTracks::AliReducedPatchContainer::kEMCGammaLow)).Begin(); patchIter != TIter::End(); ++patchIter){
		HighPtTracks::AliReducedPatchInfo *patch = dynamic_cast<HighPtTracks::AliReducedPatchInfo *>(*patchIter);
		if(patch) fHistos->FillTH1("hPatchAmplitudeEMCGLow", patch->GetAmplitude());
	}

	//std::cout << "Number of clusters: " << ReducedEvent->GetClusterContainer()->GetEntries() << std::endl;
	for(TIter clustIter = TIter(ReducedEvent->GetClusterContainer()).Begin(); clustIter != TIter::End(); ++clustIter){
		HighPtTracks::AliReducedEmcalCluster *clust = dynamic_cast<HighPtTracks::AliReducedEmcalCluster *>(*clustIter);
		for(std::vector<std::string>::iterator trgit = triggerstrings.begin(); trgit != triggerstrings.end(); ++trgit)
			fHistos->FillTH1(Form("hEnergy%s", trgit->c_str()), clust->GetClusterEnergy());
	}

	//std::cout << "Number of clusters: " << ReducedEvent->GetClusterContainer()->GetEntries() << std::endl;
	for(TIter trackIter = TIter(ReducedEvent->GetTrackContainer()).Begin(); trackIter != TIter::End(); ++trackIter){
		HighPtTracks::AliReducedReconstructedTrack *track = dynamic_cast<HighPtTracks::AliReducedReconstructedTrack *>(*trackIter);
		if(TMath::Abs(track->Eta()) > 0.8) continue;
		if(track->TestTrackCuts(kDefaultCuts)) continue;
		for(std::vector<std::string>::iterator trgit = triggerstrings.begin(); trgit != triggerstrings.end(); ++trgit)
			fHistos->FillTH1(Form("hPt%s", trgit->c_str()), track->Pt());
	}
   return kTRUE;
}

void TrackPtSelector::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void TrackPtSelector::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
	std::cout << "In terminate" << std::endl;
	fHistos = dynamic_cast<THistManager *>(fOutput->FindObject("TrackPtHistos"));
	if(!fHistos){
		std::cout << "Histogram manager not found" << std::endl;
	}

	TFile *result = new TFile("trackpt.root", "RECREATE");
	result->cd();
	std::cout << fHistos->GetListOfHistograms()->GetEntries() << " histograms found" << std::endl;
	for(TIter histIter = TIter(fHistos->GetListOfHistograms()).Begin(); histIter != TIter::End(); ++histIter){
		std::cout << "Writing histogram " << (*histIter)->GetName() << std::endl;
		(*histIter)->Write();
	}
	result->Close();
	delete result;
}

void TrackPtSelector::GetDefaultPtBinning(TArrayD& binning) const {
  std::vector<double> mybinning;
  std::map<double,double> definitions;
  definitions.insert(std::pair<double, double>(1, 0.05));
  definitions.insert(std::pair<double, double>(2, 0.1));
  definitions.insert(std::pair<double, double>(4, 0.2));
  definitions.insert(std::pair<double, double>(7, 0.5));
  definitions.insert(std::pair<double, double>(16, 1));
  definitions.insert(std::pair<double, double>(36, 2));
  definitions.insert(std::pair<double, double>(40, 4));
  definitions.insert(std::pair<double, double>(50, 5));
  definitions.insert(std::pair<double, double>(100, 10));
  definitions.insert(std::pair<double, double>(200, 20));
  double currentval = 0.;
  mybinning.push_back(currentval);
  for(std::map<double,double>::iterator id = definitions.begin(); id != definitions.end(); ++id){
    double limit = id->first, binwidth = id->second;
    while(currentval < limit){
      currentval += binwidth;
      mybinning.push_back(currentval);
    }
  }
  binning.Set(mybinning.size());
  int ib = 0;
  for(std::vector<double>::iterator it = mybinning.begin(); it != mybinning.end(); ++it)
    binning[ib++] = *it;
}
