#define SelectorJetStructure_cxx
// The class definition in SelectorJetStructure.h has been generated automatically
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
// Root > T->Process("SelectorJetStructure.C")
// Root > T->Process("SelectorJetStructure.C","some options")
// Root > T->Process("SelectorJetStructure.C+")
//

#include "SelectorJetStructure.h"
#include <TArrayD.h>
#include <TH2.h>
#include <THnSparse.h>
#include <TParticlePDG.h>
#include <TStyle.h>
#include <TLorentzVector.h>

#include "AliReducedJetInfo.h"
#include "AliReducedJetConstituent.h"
#include "AliReducedJetParticle.h"

using namespace HighPtTracks;


void SelectorJetStructure::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void SelectorJetStructure::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

   fContribHistoMapAll = new HistoList;
   fContribHistoMapCharged = new HistoList;
   fPtHistoMapAll = new HistoList;
   fPtHistoMapCharged = new HistoList;

   double pttrackmin[] = {0., 0.5, 1., 2., 5., 10., 20., 50.};
   double rlim[] = {0, 0.05, 0.1, 0.2, 0.3, 0.4};

   TH2 *result = NULL;
   for(double *ptiter = pttrackmin; ptiter < pttrackmin + sizeof(pttrackmin)/sizeof(double); ++ptiter){
     for(int irad = 0; irad < 5; irad++){
       result = new TH2D(Form("hContribAllPt%03dRadmin%03dRadmax%03d", int((*ptiter) *10.), int(rlim[irad]*100), int(rlim[irad+1]*100)),
           Form("All Particles with pt > %1f GeV/c within radii %.2f and %.2f", *ptiter,rlim[irad],rlim[irad+1]), 100, 0., 500., 101, -0.5 , 100.5);
       fContribHistoMapAll->Insert(*ptiter, rlim[irad], rlim[irad+1], result);
       fOutput->Add(result);

       result = new TH2D(Form("hContribChargedPt%03dRadmin%03dRadmax%03d", int((*ptiter) *10.), int(rlim[irad]*100), int(rlim[irad+1]*100)),
           Form("Charged Particles with pt > %1f GeV/c within radii %.2f and %.2f", *ptiter,rlim[irad],rlim[irad+1]), 100, 0., 500., 101, -0.5 , 100.5);
       fContribHistoMapCharged->Insert(*ptiter, rlim[irad], rlim[irad+1], result);
       fOutput->Add(result);

       result = new TH2D(Form("hSumPtAllPt%03dRadmin%03dRadmax%03d", int((*ptiter) *10.), int(rlim[irad]*100), int(rlim[irad+1]*100)),
           Form("Sum pt of all Particles with pt > %1f GeV/c within radii %.2f and %.2f", *ptiter,rlim[irad],rlim[irad+1]), 100, 0., 500., 500., 0. , 500);
       fPtHistoMapAll->Insert(*ptiter, rlim[irad], rlim[irad+1], result);
       fOutput->Add(result);

       result = new TH2D(Form("hSumPtChargedPt%03dRadmin%03dRadmax%03d", int((*ptiter) *10.), int(rlim[irad]*100), int(rlim[irad+1]*100)),
           Form("Sum pt of charged Particles with pt > %1f GeV/c within radii %.2f and %.2f", *ptiter,rlim[irad],rlim[irad+1]), 100, 0., 500., 500., 0. , 500);
       fPtHistoMapCharged->Insert(*ptiter, rlim[irad], rlim[irad+1], result);
       fOutput->Add(result);
     }
   }
}

Bool_t SelectorJetStructure::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either SelectorJetStructure::GetEntry() or TBranch::GetEntry()
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

   double pttrackmin[] = {0., 0.5, 1., 2., 5., 10., 20., 50.};
   double rlim[] = {0, 0.05, 0.1, 0.2, 0.3, 0.4};

   double weight = JetEvent->GetCrossSection()/static_cast<double>(JetEvent->GetNumberOfTrials());

   for(double *ptiter = pttrackmin; ptiter < pttrackmin + sizeof(pttrackmin)/sizeof(double); ++ptiter){
     for(int irad = 0; irad < 5; irad++){
       TIter jetIter(JetEvent->GetListOfJets());
       AliReducedJetInfo *myjet(NULL);
       while((myjet = dynamic_cast<AliReducedJetInfo *>(jetIter()))){
         InspectJet(myjet, *ptiter, rlim[irad], rlim[irad+1], true, weight);
         InspectJet(myjet, *ptiter, rlim[irad], rlim[irad+1], false, weight);
       }
     }
   }
   return kTRUE;
}

void SelectorJetStructure::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void SelectorJetStructure::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

  TList *lcontrib = new TList;
  TList *lsumpt = new TList;

  TList *lcall = new TList; lcall->SetName("all");
  TList *lccharged = new TList; lccharged->SetName("charged");
  lcontrib->Add(lcall); lcontrib->Add(lccharged);
  TList *lsall = new TList; lsall->SetName("all");
  TList *lscharged = new TList; lscharged->SetName("charged");
  lsumpt->Add(lsall); lsumpt->Add(lscharged);


  TIter histIter(fOutput);
  TObject *histo;
  while((histo = histIter())){
    TString histname(histo->GetName());
    if(histname.Contains("hContrib")){
      if(histname.Contains("Charged")) lccharged->Add(histo);
      else lcall->Add(histo);
    }
    else if(histname.Contains("hContrib")){
      if(histname.Contains("Charged")) lscharged->Add(histo);
      else lsall->Add(histo);
    }
  }

  TFile *jetstructure = new TFile("jetstructure.root", "RECREATE");
  jetstructure->cd();
  lcontrib->Write("ncontrib", TObject::kSingleKey);
  lsumpt->Write("sumpt", TObject::kSingleKey);
  delete jetstructure;
}

void SelectorJetStructure::InspectJet(AliReducedJetInfo* jet, double ptmin, double radmin, double radmax, bool charged, double weight) {
  int nconst = 0;
  double sumpt = 0.;
  TIter constIter(jet->GetListOfConstituents());
  AliReducedJetConstituent *jconst = NULL;

  TLorentzVector jetvec;
  jet->FillLorentzVector(jetvec);
  while((jconst = dynamic_cast<AliReducedJetConstituent *>(constIter()))){
    TLorentzVector ptvec;
    jconst->FillLorentzVector(ptvec);
    if(TMath::Abs(ptvec.Eta()) > 0.8) continue;
    if(TMath::Abs(ptvec.Pt()) < ptmin) continue;
    if(charged){
      if(!jconst->GetPDGParticle()->Charge()) continue;
    }
    double r = TMath::Abs(jetvec.DeltaR(ptvec));
    if(r < radmin || r > radmax) continue;
    nconst++;
    sumpt += TMath::Abs(ptvec.Pt());
  }
  TH2 *htofill(NULL), *htofillpt(NULL);
  if(charged){
    htofill = fContribHistoMapCharged->FindHistogram(ptmin, radmin, radmax);
    htofillpt = fPtHistoMapCharged->FindHistogram(ptmin, radmin, radmax);
  } else {
    htofill = fContribHistoMapAll->FindHistogram(ptmin, radmin, radmax);
    htofillpt = fPtHistoMapAll->FindHistogram(ptmin, radmin, radmax);
  }
  if(htofill) htofill->Fill(TMath::Abs(jetvec.Pt()), nconst, weight);
  if(htofillpt) htofill->Fill(TMath::Abs(jetvec.Pt()), sumpt, weight);
}
