#define SelectorPtClusters_cxx
// The class definition in SelectorPtClusters.h has been generated automatically
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
// Root > T->Process("SelectorPtClusters.C")
// Root > T->Process("SelectorPtClusters.C","some options")
// Root > T->Process("SelectorPtClusters.C+")
//

#include "SelectorPtClusters.h"
#include <TDatabasePDG.h>
#include <TH2.h>
#include <TParticlePDG.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TLorentzVector.h>

#include "AliReducedJetInfo.h"
#include "AliReducedJetConstituent.h"
#include "AliReducedJetParticle.h"

using namespace HighPtTracks;

void SelectorPtClusters::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void SelectorPtClusters::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

   TArrayD ptbinning; MakeLinearBinning(ptbinning, 40, 0., 200.);
   TArrayD trackPtBinning; MakeLinearBinning(trackPtBinning, 100, 0., 100.);
   TArrayD nclustersBinning; MakeLinearBinning(nclustersBinning, 161, -0.5, 161.5);
   TArrayD ncontribBinning; MakeLinearBinning(ncontribBinning, 101, -0.5, 100.5);
   TArrayD radiusBinning; MakeLinearBinning(radiusBinning, 10, 0., 0.5);
   TArrayD ptresbinning; MakeLinearBinning(ptresbinning, 100, -1, 1);

   int nbinsClusters[5] = {40, 100, 101, 10, 161},
       nbinsPtRes[5] = {40, 100, 101, 10, 100};

   hClusterHist = new THnSparseD("hClusterHist", "THnSparse for cluster related studies", 5, nbinsClusters);
   DefineAxis(hClusterHist->GetAxis(0), "jetpt", "p_{t, jet} (GeV/c)", ptbinning);
   DefineAxis(hClusterHist->GetAxis(1), "trackpt", "p_{t, track} (GeV/c)", trackPtBinning);
   DefineAxis(hClusterHist->GetAxis(2), "ncontrib", "number of charged contibutors", ncontribBinning);
   DefineAxis(hClusterHist->GetAxis(3), "radius", "jet radius", radiusBinning);
   DefineAxis(hClusterHist->GetAxis(4), "nclusters", "Number of TPC clusters", nclustersBinning);
   hClusterHist->Sumw2();
   hPtResHist = new THnSparseD("hPtResHist", "THnSparse for pt resolution", 5, nbinsPtRes);
   DefineAxis(hPtResHist->GetAxis(0), "jetpt", "p_{t, jet} (GeV/c)", ptbinning);
   DefineAxis(hPtResHist->GetAxis(1), "trackpt", "p_{t, track} (GeV/c)", trackPtBinning);
   DefineAxis(hPtResHist->GetAxis(2), "ncontrib", "number of charged contibutors", ncontribBinning);
   DefineAxis(hPtResHist->GetAxis(3), "radius", "jet radius", radiusBinning);
   DefineAxis(hPtResHist->GetAxis(4), "ptresolution", "(p_{t,rec} - p_{t,gen})/(p_{t,gen})", ptresbinning);
   hPtResHist->Sumw2();


   fCrossSection = new TProfile("CrossSection", "CrossSection", 11, -0.5, 10.5);
   fTrials = new TH1D("Trials", "Trials", 11, -0.5, 10.5);
   fOutput->Add(hClusterHist);
   fOutput->Add(hPtResHist);
   fOutput->Add(fCrossSection);
   fOutput->Add(fTrials);
}

Bool_t SelectorPtClusters::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either SelectorPtClusters::GetEntry() or TBranch::GetEntry()
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

   for(TIter jetIter = TIter(JetEvent->GetListOfJets()).Begin(); jetIter != TIter::End(); ++jetIter){
     AliReducedJetInfo *recjet = static_cast<AliReducedJetInfo *>(*jetIter);
     int ncharged = GetChargedContributors(recjet);
     TLorentzVector jetvector;
     recjet->FillLorentzVector(jetvector);
     if(TMath::Abs(jetvector.Eta()) > 0.5) continue;
     for(TIter trackIter = TIter(recjet->GetListOfMatchedParticles()).Begin(); trackIter != TIter::End(); ++trackIter){
       AliReducedJetParticle *mtrack = static_cast<AliReducedJetParticle *>(*trackIter);
       TLorentzVector partvector;
       mtrack->FillLorentzVector(partvector);
       if(TMath::Abs(partvector.Eta())>0.8) continue;

       double clustercontent[] = {TMath::Abs(jetvector.Pt()), TMath::Abs(partvector.Pt()),
           ncharged, mtrack->GetDistanceToJetMainAxis(), mtrack->GetNumberOfClustersTPC()},
               ptrescontent[] = {TMath::Abs(jetvector.Pt()), TMath::Abs(partvector.Pt()),
           ncharged, mtrack->GetDistanceToJetMainAxis(), -mtrack->GetDeltaPt()/TMath::Abs(partvector.Pt())};

       hClusterHist->Fill(clustercontent, weight);
       hPtResHist->Fill(ptrescontent, weight);
     }
   }

   return kTRUE;
}

void SelectorPtClusters::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void SelectorPtClusters::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

   hClusterHist = dynamic_cast<THnSparseD *>(fOutput->FindObject("hClusterHist"));
   hPtResHist = dynamic_cast<THnSparseD *>(fOutput->FindObject("hPtResHist"));

   TFile *out = new TFile("jettracking.root", "RECREATE");
   out->cd();
   hClusterHist->Write();
   hPtResHist->Write();
   fOutput->FindObject("CrossSection")->Write();
   fOutput->FindObject("Trials")->Write();
   delete out;
}

void SelectorPtClusters::MakeLinearBinning(TArrayD& array, int nbins, double min, double max) const {
  array.Set(nbins+1);
  double stepsize = (max - min)/double(nbins);
  for(int i = 0; i < nbins+1; i++) array[i] = min + i * stepsize;
}

int SelectorPtClusters::GetChargedContributors(const HighPtTracks::AliReducedJetInfo* recjet) const {
  int ncontrib = 0;
  for(TIter contIter = TIter(recjet->GetListOfConstituents()).Begin(); contIter != TIter::End(); ++contIter){
    AliReducedJetConstituent *myconst = static_cast<AliReducedJetConstituent *>(*contIter);
    if(!myconst->GetPDGParticle()->Charge()) continue;
    ncontrib++;
  }
  return ncontrib;
}

void SelectorPtClusters::DefineAxis(TAxis* axis, TString name, TString title, const TArrayD& binning) const {
  axis->SetNameTitle(name.Data(), title.Data());
  axis->Set(binning.GetSize()-1, binning.GetArray());
}
