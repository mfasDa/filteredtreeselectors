//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jan 30 06:15:28 2015 by ROOT version 5.34/08
// from TTree JetEvent/A tree with jet information
// found on file: root_archive.zip#AnalysisResults.root
//////////////////////////////////////////////////////////

#ifndef SelectorJetStructure_h
#define SelectorJetStructure_h

#include <vector>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.
#include "AliReducedJetEvent.h"

// Fixed size dimensions of array or collections stored in the TTree if any.

class TH2;

//class AliReducedJetInfo;


class HistoList{
public:
  HistoList() : fHistograms() {}
  ~HistoList() {}

  void Insert(double ptmin, double rmin, double rmax, TH2 *histo){
    std::vector<HistoContent>::iterator it = FindBin(ptmin, rmin, rmax);
    if(it != fHistograms.end()) it->SetHisto(histo);
    else
      fHistograms.push_back(HistoContent(ptmin, rmin, rmax, histo));
  }
  TH2 * FindHistogram(double ptmin, double rmin, double rmax){
    TH2 *result = NULL;
    std::vector<HistoContent>::iterator it = FindBin(ptmin, rmin, rmax);
    if(it != fHistograms.end()) result = it->GetHisto();
    return result;
  }
private:
  class HistoContent{
  public:
    HistoContent():
      fPtMin(0),
      fRMin(0),
      fRMax(0),
      fHistogram(NULL)
    {}
    HistoContent(double ptmin, double rmin, double rmax, TH2 *histo):
      fPtMin(ptmin),
      fRMin(rmin),
      fRMax(rmax),
      fHistogram(histo)
    {}
    ~HistoContent() {}

    double IsEqual(double ptmin, double rmin, double rmax) const {
      return (fPtMin == ptmin && fRMin == rmin && fRMax == rmax);
    }
    TH2 *GetHisto() const { return fHistogram; }
    void SetHisto(TH2 *hist) { fHistogram = hist; }

  private:
    double        fPtMin;
    double        fRMin;
    double        fRMax;
    TH2           *fHistogram;
  };
  std::vector<HistoContent>::iterator FindBin(double ptmin, double rmin, double rmax){
    std::vector<HistoContent>::iterator result = fHistograms.end();
    for(std::vector<HistoContent>::iterator it = fHistograms.begin(); it != fHistograms.end(); ++it){
      if(it->IsEqual(ptmin, rmin, rmax)){
        result = it;
        break;
      }
    }
    return result;
  }
  std::vector<HistoContent>     fHistograms;
};

class SelectorJetStructure : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   HighPtTracks::AliReducedJetEvent *JetEvent;

   HistoList     *fContribHistoMapAll;
   HistoList     *fContribHistoMapCharged;
   HistoList     *fPtHistoMapAll;
   HistoList     *fPtHistoMapCharged;

   // List of branches
   TBranch        *b_JetEvent;   //!

   SelectorJetStructure(TTree * /*tree*/ =0) : fChain(0), fContribHistoMapAll(NULL), fContribHistoMapCharged(NULL), fPtHistoMapAll(NULL), fPtHistoMapCharged(NULL) { }
   virtual ~SelectorJetStructure() { }
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

   void InspectJet(HighPtTracks::AliReducedJetInfo *jet, double ptmin, double radmin, double radmax, bool charged, double weight);

   ClassDef(SelectorJetStructure,0);
};

#endif

#ifdef SelectorJetStructure_cxx
void SelectorJetStructure::Init(TTree *tree)
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

Bool_t SelectorJetStructure::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef SelectorJetStructure_cxx
