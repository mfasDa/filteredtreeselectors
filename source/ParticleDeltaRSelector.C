#define ParticleDeltaRSelector_cxx
// The class definition in ParticleDeltaRSelector.h has been generated automatically
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
// Root > T->Process("ParticleDeltaRSelector.C")
// Root > T->Process("ParticleDeltaRSelector.C","some options")
// Root > T->Process("ParticleDeltaRSelector.C+")
//

#include <iostream>

#include "ParticleDeltaRSelector.h"
#include <TDatabasePDG.h>
#include <TH2.h>
#include <TMath.h>
#include <TParticlePDG.h>
#include <TStyle.h>
#include <TLorentzVector.h>

#include "AliReducedJetInfo.h"
#include "AliReducedMatchedTrack.h"
#include "AliReducedJetConstituent.h"
#include "AliReducedJetParticle.h"
#include "AliEMCalHistoContainer.h"


void ParticleDeltaRSelector::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void ParticleDeltaRSelector::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).
   std::cout << "Creating histos on Slave" << std::endl;
   TString option = GetOption();

   fHistos = new EMCalTriggerPtAnalysis::AliEMCalHistoContainer("deltaRhistos");
   fHistos->ReleaseOwner();            // fOutput will take care of histograms
   fHistos->CreateTH1("hPtHard", "Pt of the hard interaction", 3000, 0., 300);
   fHistos->CreateTH1("hNTrials", "Number of trials", 1, 0.5, 1.5);
   fHistos->CreateTProfile("hCrossSection", "Cross section", 1, 0.5, 1.5);
   fHistos->CreateTH1("hPtTrackLeading", "Pt of the leading track in jet", 200, 0., 200);
   fHistos->CreateTH1("hPtTrackLeadingRec", "Pt of the reconstructed leading track in jet", 200, 0., 200);
   fHistos->CreateTH1("hPtTrackLeadingNoRec", "Pt of the reconstructed leading track in jet", 200, 0., 200);
   fHistos->CreateTH2("hPtJetPtLeading", "Jet pt vs. leading track pt", 600, 0., 300, 200, 0., 100.);
   fHistos->CreateTH2("hPtJetDrLeading", "Jet pt vs. dr of the leading track to the main jet axis", 600, 0., 300, 100, 0., 0.5);
   fHistos->CreateTH2("hPtTrackPtNeighbor", "Track pt vs. closest neighbor track pt", 200, 0., 100., 200, 0., 100);
   fHistos->CreateTH2("hPtTrackDrNeighbor", "Track pt vs. distance to the closest neighbor", 200, 0., 100, 100, 0., 0.5);

   // Build jet pt dependent histograms
   for(int ipt = 0; ipt <= 200; ipt += 20){
      // Leading vs. subleading
      fHistos->CreateTH2(Form("hPtLeadingVsSubleading_jetpt%d_%d", ipt, ipt+20),
            Form("pt of leading vs subleading charged contributor for jets with pt within [%d,%d] GeV/c", ipt, ipt+20),
            200,0.,100.,200,0.,100.);
      fHistos->CreateTH2(Form("hDrLeadingSubleading_jetpt%d_%d", ipt, ipt+20),
            Form("Dr of leading vs subleading charged contributor for jets with pt within [%d,%d] GeV/c", ipt, ipt+20),
            200,0.,100.,100,0.,0.5);
      // track vs. neighbor
      fHistos->CreateTH2(Form("hPtTrackNeighbor_jet%d_%d", ipt, ipt+20),
            Form("pt of track vs nearest neighbor for jets with pt within [%d,%d] GeV/c", ipt, ipt+20),
            200, 0., 100., 200, 0., 100.);
      fHistos->CreateTH2(Form("hDrTrackNeighbor_jet%d_%d", ipt, ipt+20),
            Form("Dr of leading vs subleading charged contributor for jets with pt within [%d,%d] GeV/c", ipt, ipt+20),
            200,0.,100.,100,0.,0.5);
      // track vs. high-pt neighbor
      fHistos->CreateTH2(Form("hPtTrackNeighborHighPt_jet%d_%d", ipt, ipt+20),
            Form("pt of track vs nearest neighbor for jets with pt within [%d,%d] GeV/c", ipt, ipt+20),
            200, 0., 100., 200, 0., 100.);
      fHistos->CreateTH2(Form("hDrTrackNeighborHighPt_jet%d_%d", ipt, ipt+20),
            Form("Dr of leading vs subleading charged contributor for jets with pt within [%d,%d] GeV/c", ipt, ipt+20),
            200,0.,100.,100,0.,0.5);
      // distance to main jet axis for particles with depending on jet pt and trackPt
      fHistos->CreateTH2(Form("hDrTrackMainJetAxis_jet%d_%d", ipt, ipt+20),
            Form("Dr of particle to the main jet axis for jets with pt within [%d,%d] GeV/c", ipt, ipt+20),
            200,0.,100.,100,0.,0.5);
   }

   // Add outputhistos to histlist
   for(TIter histiter = TIter(fHistos->GetListOfHistograms()).Begin(); histiter != TIter::End(); ++histiter)
      fOutput->Add(*histiter);
   std::cout << "Finished creating histograms" << std::endl;
}

Bool_t ParticleDeltaRSelector::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either ParticleDeltaRSelector::GetEntry() or TBranch::GetEntry()
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
   std::cout << "Reading next entry" << std::endl;
   GetEntry(entry);

   if(!JetEvent){
      std::cout << "Failed reading jet event" << std::endl;
   }

   fHistos->FillTH1("hPtHard", JetEvent->GetPtHard());
   fHistos->FillTH1("hNTrials", 1., JetEvent->GetNumberOfTrials());
   fHistos->FillProfile("hCrossSection", 1., JetEvent->GetCrossSection());

   // Iterate over reconstructed jets
   std::cout << "Iterating over jets" << std::endl;
   HighPtTracks::AliReducedJetInfo *recjet(NULL);
   HighPtTracks::AliReducedJetParticle *leadingPart(NULL), *subleadingpart(NULL), *neighbor(NULL), *neighborHighPt(NULL);
   for(TIter jetIter(JetEvent->GetListOfJets()); jetIter != TIter::End(); ++jetIter){
      std::cout << "Next jet" << std::endl;
      recjet = static_cast<HighPtTracks::AliReducedJetInfo *>(*jetIter);
      // request at least 2 charged contributors
      int ncontrib(0);
      for(TIter contIter = TIter(recjet->GetListOfConstituents()); contIter != TIter::End(); ++contIter){
         HighPtTracks::AliReducedJetConstituent *cont = static_cast<HighPtTracks::AliReducedJetConstituent *>(*contIter);
         if(!cont->GetPDGParticle()->Charge()) continue;
         ncontrib++;
      }
      if(ncontrib < 2) continue;
      std::cout << "Jet selected" << std::endl;

      // Get reconstructed jet kinematics
      TLorentzVector jetvec;
      recjet->FillLorentzVector(jetvec);
      double xmin, xmax;
      GetJetPtHistLimits(TMath::Abs(jetvec.Pt()), xmin, xmax);

      leadingPart = GetLeadingParticle(recjet);
      if(leadingPart){
         std::cout << "Found leading track" << std::endl;
         TLorentzVector leadingvec;
         leadingPart->FillLorentzVector(leadingvec);
         // Fill pt spectrum of leading particles
         fHistos->FillTH1("hPtTrackLeading", TMath::Abs(leadingvec.Pt()));
         if(leadingPart->GetNumberOfMatchedTracks())
            fHistos->FillTH1("hPtTrackLeadingRec", TMath::Abs(leadingvec.Pt()));
         else
            fHistos->FillTH1("hPtTrackLeadingNoRec", TMath::Abs(leadingvec.Pt()));
         // correlation plots for leading particles vs. jet observables
         fHistos->FillTH2("hPtJetPtLeading", TMath::Abs(jetvec.Pt()), TMath::Abs(leadingvec.Pt()));
         fHistos->FillTH2("hPtJetDrLeading", TMath::Abs(jetvec.Pt()), TMath::Abs(jetvec.DrEtaPhi(leadingvec)));

         // Get subleading track
         subleadingpart = GetSubleadingParticle(recjet);
         if(subleadingpart){
            std::cout << "subleading particle found" << std::endl;
            TLorentzVector subleadingvec;
            subleadingpart->FillLorentzVector(subleadingvec);
            fHistos->FillTH2(Form("hPtLeadingVsSubleading_jetpt%d_%d", int(xmin), int(xmax)), TMath::Abs(leadingvec.Pt()), TMath::Abs(subleadingvec.Pt()));
            fHistos->FillTH2(Form("hDrLeadingSubleading_jetpt%d_%d", int(xmin), int(xmax)), TMath::Abs(leadingvec.Pt()), TMath::Abs(leadingvec.DrEtaPhi(subleadingvec)));
            std::cout << "Finished subleading particle" << std::endl;
         }
         std::cout << "Finished leading particle" << std::endl;
      }

      // Iterate over all particles, select those with pt above 30 GeV/c
      // and find nearest neighbor
      for(TIter partIter = TIter(recjet->GetListOfMatchedParticles()).Begin(); partIter != TIter::End(); ++partIter){
         std::cout << "Iterate over high-pt particles" << std::endl;
         leadingPart = static_cast<HighPtTracks::AliReducedJetParticle *>(*partIter);
         if(!TDatabasePDG::Instance()->GetParticle(leadingPart->GetPdgCode())->Charge()) continue;
         TLorentzVector partvec;
         leadingPart->FillLorentzVector(partvec);
         if(TMath::Abs(partvec.Eta()) > 0.8) continue;
         fHistos->FillTH2(Form("hDrTrackMainJetAxis_jet%d_%d", xmin, xmax), partvec.Pt(), jetvec.DrEtaPhi(partvec));
         if(TMath::Abs(partvec.Pt()) < 30.) continue;
         std::cout << "High-pt particle found" << std::endl;
         neighbor = GetNearestNeighbor(leadingPart, recjet);
         if(neighbor){
            std::cout << "Neighbor found" << std::endl;
            TLorentzVector neighborvec;
            neighbor->FillLorentzVector(neighborvec);
            fHistos->FillTH2(Form("hPtTrackNeighbor_jet%d_%d", xmin, xmax), TMath::Abs(partvec.Pt()), TMath::Abs(neighborvec.Pt()));
            fHistos->FillTH2(Form("hDrTrackNeighbor_jet%d_%d", xmin, xmax), TMath::Abs(partvec.Pt()), TMath::Abs(partvec.DrEtaPhi(neighborvec)));
            std::cout << "Finished neighbor" << std::endl;
         }

         // min. neighbor pt = 30 GeV/c
         neighborHighPt = GetNearestNeighbor(leadingPart, recjet, 30.);
         if(neighborHighPt){
            std::cout << "High-pt neighbor found" << std::endl;
            TLorentzVector neighborvec;
            neighborHighPt->FillLorentzVector(neighborvec);
            fHistos->FillTH2(Form("hPtTrackNeighborHighPt_jet%d_%d", xmin, xmax), TMath::Abs(partvec.Pt()), TMath::Abs(neighborvec.Pt()));
            fHistos->FillTH2(Form("hDrTrackNeighborHighPt_jet%d_%d", xmin, xmax), TMath::Abs(partvec.Pt()), TMath::Abs(partvec.DrEtaPhi(neighborvec)));
            std::cout << "Finished high-pt neighbor" << std::endl;
         }
         std::cout << "Finished high-pt particle" << std::endl;
      }
      std::cout << "Finished jet" <<std::endl;
   }

   return kTRUE;
}

void ParticleDeltaRSelector::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void ParticleDeltaRSelector::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

   std::cout << "Writing results to DeltaRHistos.root" << std::endl;
   TFile *output = new TFile("DeltaRHistos.root", "RECREATE");
   for(TIter contentIter = TIter(fOutput).Begin(); contentIter != TIter::End(); ++contentIter){
      if((*contentIter)->InheritsFrom("TCollection"))
         (*contentIter)->Write((*contentIter)->GetName(), TObject::kSingleKey);
      else
         (*contentIter)->Write();
   }
   std::cout << "Finished writing output file" << std::endl;
   delete output;
}

HighPtTracks::AliReducedJetParticle* ParticleDeltaRSelector::GetLeadingParticle(const HighPtTracks::AliReducedJetInfo *recjet) const {
   std::cout << "Getting leading particle for jet " << recjet << std::endl;
   HighPtTracks::AliReducedJetParticle *jettrack = NULL,
         *tmptrack = NULL;
   double leadingPt(0.);
   std::cout << "Iterate over list " << recjet->GetListOfMatchedParticles() << std::endl;
   for(TIter partIter = TIter(recjet->GetListOfMatchedParticles()).Begin(); partIter != TIter::End(); ++partIter){
      std::cout << (*partIter)->IsA()->GetName() << std::endl;
      tmptrack = static_cast<HighPtTracks::AliReducedJetParticle *>(*partIter);
      if(!TDatabasePDG::Instance()->GetParticle(tmptrack->GetPdgCode())->Charge()) continue;		// select only charged particles
      TLorentzVector partvec;
      tmptrack->FillLorentzVector(partvec);
      if(TMath::Abs(partvec.Eta()) > 0.8) continue;
      if(TMath::Abs(partvec.Pt()) >= leadingPt){
         jettrack = tmptrack;
         leadingPt = TMath::Abs(partvec.Pt());
      }
   }
   std::cout << "Getting leading particle done" << std::endl;
   return jettrack;
}

HighPtTracks::AliReducedJetParticle* ParticleDeltaRSelector::GetSubleadingParticle(const HighPtTracks::AliReducedJetInfo* recjet) const {
   std::cout << "Getting subleading particle" << std::endl;
   HighPtTracks::AliReducedJetParticle *leadingtrack = NULL,
         *subleadingtrack = NULL,
         *tmptrack = NULL;
   double leadingPt(0);
   for(TIter partIter = TIter(recjet->GetListOfMatchedParticles()).Begin(); partIter != TIter::End(); ++partIter){
      tmptrack = static_cast<HighPtTracks::AliReducedJetParticle *>(*partIter);
      if(!TDatabasePDG::Instance()->GetParticle(tmptrack->GetPdgCode())->Charge()) continue;		// select only charged particles
      TLorentzVector partvec;
      tmptrack->FillLorentzVector(partvec);
      if(TMath::Abs(partvec.Eta()) > 0.8) continue;
      if(TMath::Abs(partvec.Pt()) >= leadingPt){
         subleadingtrack = leadingtrack;
         leadingtrack = tmptrack;
         leadingPt = TMath::Abs(partvec.Pt());
      }
   }
   std::cout << "Getting subleading particle done" << std::endl;
   return subleadingtrack;
}

HighPtTracks::AliReducedJetParticle* ParticleDeltaRSelector::GetNearestNeighbor(const HighPtTracks::AliReducedJetParticle* inputparticle,
      HighPtTracks::AliReducedJetInfo *recjet, double minpt) const {
   std::cout << "Getting nearest neighbor"<<std::endl;
   HighPtTracks::AliReducedJetParticle *testpart(NULL), *selected(NULL);
   double mindistance = 1000.;
   TLorentzVector partvec;
   inputparticle->FillLorentzVector(partvec);
   for(TIter partIter = TIter(recjet->GetListOfMatchedParticles()).Begin(); partIter != TIter::End(); ++partIter){
      testpart = static_cast<HighPtTracks::AliReducedJetParticle *>(*partIter);
      if(testpart == inputparticle) continue;
      if(!TDatabasePDG::Instance()->GetParticle(testpart->GetPdgCode())->Charge()) continue;
      TLorentzVector testvec;
      testpart->FillLorentzVector(testvec);
      if(TMath::Abs(testvec.Pt()) < minpt) continue;
      if(TMath::Abs(testvec.Eta()) > 0.8) continue;
      double dr = partvec.DrEtaPhi(testvec);
      if(dr < mindistance){
         selected = testpart;
         mindistance = dr;
      }
   }
   std::cout << "Getting nearest neighbor done"<<std::endl;
   return selected;
}

void ParticleDeltaRSelector::GetJetPtHistLimits(double jetpt, double& xmin, double& xmax) const {
   for(double ipt = 0; ipt < 200; ipt += 20){
      if(jetpt >= ipt && jetpt < ipt +20){
         xmin = ipt;
         xmax = ipt + 20;
         break;
      }
   }
}
