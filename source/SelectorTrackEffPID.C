#define SelectorTrackEffPID_cxx
// The class definition in SelectorTrackEffPID.h has been generated automatically
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
// Root > T->Process("SelectorTrackEffPID.C")
// Root > T->Process("SelectorTrackEffPID.C","some options")
// Root > T->Process("SelectorTrackEffPID.C+")
//
#include <map>
#include <string>

#include "SelectorTrackEffPID.h"
#include <TH2.h>
#include <TStyle.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TParticlePDG.h>
#include <TProfile.h>
#include <TPDGCode.h>
#include <TString.h>

#include "AliReducedJetInfo.h"
#include "AliReducedJetConstituent.h"
#include "AliReducedJetParticle.h"


void SelectorTrackEffPID::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void SelectorTrackEffPID::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

   // We store:
   // 	jet pt
   // 	Track pt
   // 	Track pid (electrons - 0, pions - 1, kaons - 2, protons - 3, others - 4);
   // 	Number of charged jet constituents
   // 	Distance to the main jet axis


   fGen = new TrackHistogram("generated", "Information at generator level");
   fRec = new TrackHistogram("reconstructed", "Information at reconstruction level");

   TArrayD ptbinning;
   CreateDefaultPtBinning(ptbinning);
   fGen->SetPtBinninng(ptbinning);
   fRec->SetPtBinninng(ptbinning);

   fOutput->Add(fGen->GetHistogram());
   fOutput->Add(fRec->GetHistogram());

   fCrossSection = new TProfile("CrossSection", "CrossSection", 11, -0.5, 10.5);
   fTrials = new TH1D("Trials", "Trials", 11, -0.5, 10.5);
   fOutput->Add(fCrossSection);
   fOutput->Add(fTrials);
}

Bool_t SelectorTrackEffPID::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either SelectorTrackEffPID::GetEntry() or TBranch::GetEntry()
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
   fCrossSection->Fill(JetEvent->GetPtHard(), JetEvent->GetCrossSection());
   fTrials->Fill(JetEvent->GetPtHard(), JetEvent->GetNumberOfTrials());

   HighPtTracks::AliReducedJetInfo *recjet(NULL);
   HighPtTracks::AliReducedJetParticle *jetParticle(NULL);
   int ncontrib;
   for(TIter jetIter = TIter(JetEvent->GetListOfJets()).Begin(); jetIter != TIter::End(); ++jetIter){
   	recjet = dynamic_cast<HighPtTracks::AliReducedJetInfo *>(*jetIter);
   	if(!recjet) continue;
   	TLorentzVector jetkine;
   	recjet->FillLorentzVector(jetkine);
   	if(TMath::Abs(jetkine.Eta()) > 0.5) continue;
   	ncontrib = GetChargedContributors(recjet);

   	for(TIter partIter = TIter(recjet->GetListOfMatchedParticles()).Begin(); partIter != TIter::End(); ++partIter){
   		jetParticle = static_cast<HighPtTracks::AliReducedJetParticle *>(*partIter);
   		FillParticleHistos(*jetParticle, TMath::Abs(jetkine.Pt()), ncontrib, weight);
   	}
   }

   return kTRUE;
}

void SelectorTrackEffPID::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void SelectorTrackEffPID::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
   std::string outputObjects[] = {"generated","reconstructed","CrossSection","Trials"};
   TFile *outFile = new TFile("trackingEfficiency.root", "RECREATE");
   outFile->cd();
   for(std::string *oiter = outputObjects; oiter < outputObjects + sizeof(outputObjects)/sizeof(std::string); ++oiter)
   	fOutput->FindObject(oiter->c_str())->Write();
   delete outFile;
}

unsigned int SelectorTrackEffPID::GetParticleType(const HighPtTracks::AliReducedJetParticle& recparticle) const {
	// Find species, definitions:
	// 0 - electrons
	// 1 - pions
	// 2 - kaons
	// 3 - protons
	// 4 - other
	unsigned int abspdg = TMath::Abs(recparticle.GetPdgCode());
	unsigned int particles[] = {TMath::Abs(kElectron), TMath::Abs(kPiPlus), TMath::Abs(kKPlus), TMath::Abs(kProton)};
	unsigned int result = 4;
	for(unsigned int part = 0; part < sizeof(particles)/sizeof(unsigned int); part++){
		if(particles[part] == abspdg){
			result = part;
			break;
		}
	}
	return result;
}

void SelectorTrackEffPID::MakeLinearBinning(TArrayD& array, int nbins, double min, double max) const {
	//
	// Create linear binning
	//
	array.Set(nbins+1);
	double stepsize = (max - min)/double(nbins);
	for(int i = 0; i < nbins+1; i++) array[i] = min + i * stepsize;
}

unsigned int SelectorTrackEffPID::GetChargedContributors(const HighPtTracks::AliReducedJetInfo* recjet) const {
	//
	// Get the number of charged jet contributors
	//
	unsigned int ncontrib = 0;
	for(TIter contIter = TIter(recjet->GetListOfConstituents()).Begin(); contIter != TIter::End(); ++contIter){
		HighPtTracks::AliReducedJetConstituent *myconst = static_cast<HighPtTracks::AliReducedJetConstituent *>(*contIter);
		if(!myconst->GetPDGParticle()->Charge()) continue;
		ncontrib++;
	}
	return ncontrib;
}

void SelectorTrackEffPID::FillParticleHistos(const HighPtTracks::AliReducedJetParticle& part, double jetPt, unsigned int ncontrib, double weight) {
	//
	// Fill particle histo as defined
	// if reconstructed particle is found, fill also reconstructed particle histo
	//
	TLorentzVector kine;
	part.FillLorentzVector(kine);
	if(TMath::Abs(kine.Eta()) > 0.8) return;
	double content[5] = {jetPt, TMath::Abs(kine.Pt()), GetParticleType(part), static_cast<double>(ncontrib), part.GetDistanceToJetMainAxis()};
	fGen->Fill(content, weight);
	if(part.IsReconstructed()) fRec->Fill(content, weight);
}

void SelectorTrackEffPID::CreateDefaultPtBinning(TArrayD &binning) const{
  /*
   * Creating the default pt binning.
   *
   * @param binning: Array where to store the results.
   */
  std::vector<double> mybinning;
  std::map<double,double> definitions;
  definitions.insert(std::pair<double,double>(2.5, 0.1));
  definitions.insert(std::pair<double,double>(7., 0.25));
  definitions.insert(std::pair<double,double>(15., 0.5));
  definitions.insert(std::pair<double,double>(25., 1.));
  definitions.insert(std::pair<double,double>(40., 2.5));
  definitions.insert(std::pair<double,double>(50., 5.));
  definitions.insert(std::pair<double,double>(100., 10.));
  double currentval = 0;
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

void TrackHistogram::Create() {
	int nbins[5];
	TString axisNames[5] = {"jetpt", "trackpt", "pid", "ncontrib", "radius"},
			axisTitles[5] = {"p_{t,jet} (GeV/c)", "p_{t,track,gen} (GeV/c)", "particle type", "n_{contrib, charged}", "r"};
	for(int ib = 0; ib < 5; ++ib) nbins[ib] = fBinnings[ib].GetSize() - 1;
	fHistogram = new THnSparseD(GetName(), GetTitle(), 5, nbins);
	for(int ib = 0; ib < 5; ++ib) DefineAxis(fHistogram->GetAxis(ib), axisNames[ib], axisTitles[ib], fBinnings[ib]);
	fHistogram->Sumw2();
}

void TrackHistogram::DefineAxis(TAxis* axis, TString name, TString title, const TArrayD& binning) const {
	//
	// Set name, title and binning of the axis
	//
	axis->SetNameTitle(name.Data(), title.Data());
	axis->Set(binning.GetSize()-1, binning.GetArray());
}

void TrackHistogram::MakeLinearBinning(TArrayD& array, int nbins, double min, double max) const {
	//
	// Create linear binning
	//
	array.Set(nbins+1);
	double stepsize = (max - min)/double(nbins);
	for(int i = 0; i < nbins+1; i++) array[i] = min + i * stepsize;
}
